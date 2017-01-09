//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanQueue.h"
#include "BsVulkanCommandBuffer.h"
#include "BsVulkanSwapChain.h"

namespace bs { namespace ct
{
	VulkanQueue::VulkanQueue(VulkanDevice& device, VkQueue queue, GpuQueueType type, UINT32 index)
		: mDevice(device), mQueue(queue), mType(type), mIndex(index), mLastCommandBuffer(nullptr)
		, mLastCBSemaphoreUsed(false), mNextSubmitIdx(1)
	{
		for (UINT32 i = 0; i < BS_MAX_UNIQUE_QUEUES; i++)
			mSubmitDstWaitMask[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}

	bool VulkanQueue::isExecuting() const
	{
		if (mLastCommandBuffer == nullptr)
			return false;

		return mLastCommandBuffer->isSubmitted();
	}

	void VulkanQueue::submit(VulkanCmdBuffer* cmdBuffer, VulkanSemaphore** waitSemaphores, UINT32 semaphoresCount)
	{
		cmdBuffer->allocateSemaphores();
		VulkanSemaphore* signalSemaphore = cmdBuffer->getIntraQueueSemaphore();

		VkCommandBuffer vkCmdBuffer = cmdBuffer->getHandle();
		VkSemaphore vkSemaphore = signalSemaphore->getHandle();

		prepareSemaphores(waitSemaphores, mSemaphoresTemp, semaphoresCount);
		
		VkSubmitInfo submitInfo;
		getSubmitInfo(&vkCmdBuffer, &vkSemaphore, mSemaphoresTemp, semaphoresCount, submitInfo);

		VkResult result = vkQueueSubmit(mQueue, 1, &submitInfo, cmdBuffer->getFence());
		assert(result == VK_SUCCESS);

		cmdBuffer->setIsSubmitted();
		mLastCommandBuffer = cmdBuffer;
		mLastCBSemaphoreUsed = false;

		mActiveSubmissions.push_back(SubmitInfo(cmdBuffer, mNextSubmitIdx++, semaphoresCount, 1));
		mActiveBuffers.push(cmdBuffer);
	}

	void VulkanQueue::queueSubmit(VulkanCmdBuffer* cmdBuffer, VulkanSemaphore** waitSemaphores, UINT32 semaphoresCount)
	{
		mQueuedBuffers.push_back(SubmitInfo(cmdBuffer, 0, semaphoresCount, 1));

		for (UINT32 i = 0; i < semaphoresCount; i++)
			mQueuedSemaphores.push_back(waitSemaphores[i]);
	}

	void VulkanQueue::submitQueued()
	{
		UINT32 numCBs = (UINT32)mQueuedBuffers.size();
		if (numCBs == 0)
			return;

		UINT32 totalNumWaitSemaphores = (UINT32)mQueuedSemaphores.size() + numCBs;

		UINT8* data = (UINT8*)bs_stack_alloc((sizeof(VkSubmitInfo) + sizeof(VkCommandBuffer) + sizeof(VkSemaphore)) * 
			numCBs + sizeof(VkSemaphore) * totalNumWaitSemaphores);
		UINT8* dataPtr = data;

		VkSubmitInfo* submitInfos = (VkSubmitInfo*)dataPtr;
		dataPtr += sizeof(VkSubmitInfo) * numCBs;

		VkCommandBuffer* commandBuffers = (VkCommandBuffer*)dataPtr;
		dataPtr += sizeof(VkCommandBuffer) * numCBs;

		VkSemaphore* signalSemaphores = (VkSemaphore*)dataPtr;
		dataPtr += sizeof(VkSemaphore) * numCBs;

		VkSemaphore* waitSemaphores = (VkSemaphore*)dataPtr;
		dataPtr += sizeof(VkSemaphore) * totalNumWaitSemaphores;

		UINT32 semaphoreIdx = 0;
		for(UINT32 i = 0; i < numCBs; i++)
		{
			const SubmitInfo& entry = mQueuedBuffers[i];

			entry.cmdBuffer->allocateSemaphores();
			VulkanSemaphore* signalSemaphore = entry.cmdBuffer->getIntraQueueSemaphore();

			commandBuffers[i] = entry.cmdBuffer->getHandle();
			signalSemaphores[i] = signalSemaphore->getHandle();

			UINT32 semaphoresCount = entry.numSemaphores;
			prepareSemaphores(mQueuedSemaphores.data() + semaphoreIdx, &waitSemaphores[semaphoreIdx], semaphoresCount);

			getSubmitInfo(&commandBuffers[i], &signalSemaphores[i], &waitSemaphores[semaphoreIdx], semaphoresCount, 
				submitInfos[i]);

			entry.cmdBuffer->setIsSubmitted();
			mLastCommandBuffer = entry.cmdBuffer; // Needs to be set because getSubmitInfo depends on it
			mLastCBSemaphoreUsed = false;

			mActiveBuffers.push(entry.cmdBuffer);
			semaphoreIdx += semaphoresCount;
		}

		VulkanCmdBuffer* lastCB = mQueuedBuffers[numCBs - 1].cmdBuffer;
		UINT32 totalNumSemaphores = semaphoreIdx;
		mActiveSubmissions.push_back(SubmitInfo(lastCB, mNextSubmitIdx++, totalNumSemaphores, numCBs));

		VkResult result = vkQueueSubmit(mQueue, numCBs, submitInfos, mLastCommandBuffer->getFence());
		assert(result == VK_SUCCESS);

		mQueuedBuffers.clear();
		mQueuedSemaphores.clear();

		bs_stack_free(data);
	}

	void VulkanQueue::getSubmitInfo(VkCommandBuffer* cmdBuffer, VkSemaphore* signalSemaphore, VkSemaphore* waitSemaphores,
									UINT32 semaphoresCount, VkSubmitInfo& submitInfo)
	{
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = cmdBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphore;
		submitInfo.waitSemaphoreCount = semaphoresCount;

		if (semaphoresCount > 0)
		{
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = mSubmitDstWaitMask;
		}
		else
		{
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
		}
	}

	void VulkanQueue::present(VulkanSwapChain* swapChain, VulkanSemaphore** waitSemaphores, UINT32 semaphoresCount)
	{
		UINT32 backBufferIdx;
		if (!swapChain->prepareForPresent(backBufferIdx))
			return; // Nothing to present (back buffer wasn't even acquired)

		prepareSemaphores(waitSemaphores, mSemaphoresTemp, semaphoresCount);

		VkSwapchainKHR vkSwapChain = swapChain->getHandle();
		VkPresentInfoKHR presentInfo;
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &vkSwapChain;
		presentInfo.pImageIndices = &backBufferIdx;
		presentInfo.pResults = nullptr;

		// Wait before presenting, if required
		if (semaphoresCount > 0)
		{
			presentInfo.pWaitSemaphores = mSemaphoresTemp;
			presentInfo.waitSemaphoreCount = semaphoresCount;
		}
		else
		{
			presentInfo.pWaitSemaphores = nullptr;
			presentInfo.waitSemaphoreCount = 0;
		}

		VkResult result = vkQueuePresentKHR(mQueue, &presentInfo);
		assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);

		mActiveSubmissions.push_back(SubmitInfo(nullptr, mNextSubmitIdx++, semaphoresCount, 0));
	}

	void VulkanQueue::waitIdle() const
	{
		VkResult result = vkQueueWaitIdle(mQueue);
		assert(result == VK_SUCCESS);
	}

	void VulkanQueue::refreshStates(bool queueEmpty)
	{
		UINT32 lastFinishedSubmission = 0;

		auto iter = mActiveSubmissions.begin();
		while (iter != mActiveSubmissions.end())
		{
			VulkanCmdBuffer* cmdBuffer = iter->cmdBuffer;
			if (cmdBuffer == nullptr)
			{
				++iter;
				continue;
			}

			if(!cmdBuffer->checkFenceStatus())
				break; // No chance of any later CBs of being done either
				
			lastFinishedSubmission = iter->submitIdx;
			++iter;
		}

		// If last submission was a present() call, it won't be freed until a command buffer after it is done. However on
		// shutdown there might not be a CB following it. So we instead check this special flag and free everything when its
		// true.
		if (queueEmpty)
			lastFinishedSubmission = mNextSubmitIdx - 1;

		iter = mActiveSubmissions.begin();
		while (iter != mActiveSubmissions.end())
		{
			if (iter->submitIdx > lastFinishedSubmission)
				break;

			for (UINT32 i = 0; i < iter->numSemaphores; i++)
			{
				VulkanSemaphore* semaphore = mActiveSemaphores.front();
				mActiveSemaphores.pop();

				semaphore->notifyDone(0, VulkanUseFlag::Read | VulkanUseFlag::Write);
			}

			for(UINT32 i = 0; i < iter->numCommandBuffers; i++)
			{
				VulkanCmdBuffer* cb = mActiveBuffers.front();
				mActiveBuffers.pop();

				cb->reset();
			}

			iter = mActiveSubmissions.erase(iter);
		}
	}

	void VulkanQueue::prepareSemaphores(VulkanSemaphore** inSemaphores, VkSemaphore* outSemaphores, UINT32& semaphoresCount)
	{
		UINT32 semaphoreIdx = 0;
		for (UINT32 i = 0; i < semaphoresCount; i++)
		{
			VulkanSemaphore* semaphore = inSemaphores[i];

			semaphore->notifyBound();
			semaphore->notifyUsed(0, 0, VulkanUseFlag::Read | VulkanUseFlag::Write);

			outSemaphores[semaphoreIdx++] = semaphore->getHandle();
			mActiveSemaphores.push(semaphore);
		}

		// Wait on previous CB, as we want execution to proceed in order
		if (mLastCommandBuffer != nullptr && mLastCommandBuffer->isSubmitted() && !mLastCBSemaphoreUsed)
		{
			VulkanSemaphore* prevSemaphore = mLastCommandBuffer->getIntraQueueSemaphore();

			prevSemaphore->notifyBound();
			prevSemaphore->notifyUsed(0, 0, VulkanUseFlag::Read | VulkanUseFlag::Write);

			outSemaphores[semaphoreIdx++] = prevSemaphore->getHandle();
			mActiveSemaphores.push(prevSemaphore);

			// This will prevent command buffers submitted after present() to use the same semaphore. This also means that
			// there will be no intra-queue dependencies between commands for on the other ends of a present call
			// (Meaning those queue submissions could execute concurrently).
			mLastCBSemaphoreUsed = true;
		}

		semaphoresCount = semaphoreIdx;
	}
}}