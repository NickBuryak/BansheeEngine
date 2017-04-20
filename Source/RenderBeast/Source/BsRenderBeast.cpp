//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsRenderBeast.h"
#include "BsCCamera.h"
#include "BsCRenderable.h"
#include "BsMaterial.h"
#include "BsMesh.h"
#include "BsPass.h"
#include "BsSamplerState.h"
#include "BsCoreApplication.h"
#include "BsViewport.h"
#include "BsRenderTarget.h"
#include "BsRenderQueue.h"
#include "BsCoreThread.h"
#include "BsGpuParams.h"
#include "BsProfilerCPU.h"
#include "BsProfilerGPU.h"
#include "BsShader.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsTime.h"
#include "BsRenderableElement.h"
#include "BsCoreObjectManager.h"
#include "BsRenderBeastOptions.h"
#include "BsSamplerOverrides.h"
#include "BsLight.h"
#include "BsGpuResourcePool.h"
#include "BsRenderTargets.h"
#include "BsRendererUtility.h"
#include "BsAnimationManager.h"
#include "BsSkeleton.h"
#include "BsGpuBuffer.h"
#include "BsGpuParamsSet.h"
#include "BsRendererExtension.h"
#include "BsLightProbeCache.h"
#include "BsReflectionProbe.h"
#include "BsIBLUtility.h"
#include "BsMeshData.h"
#include "BsLightGrid.h"
#include "BsSkybox.h"

using namespace std::placeholders;

namespace bs { namespace ct
{
	// Limited by max number of array elements in texture for DX11 hardware
	constexpr UINT32 MaxReflectionCubemaps = 2048 / 6;

	RenderBeast::RenderBeast()
	{
		mOptions = bs_shared_ptr_new<RenderBeastOptions>();
	}

	const StringID& RenderBeast::getName() const
	{
		static StringID name = "RenderBeast";
		return name;
	}

	void RenderBeast::initialize()
	{
		Renderer::initialize();

		gCoreThread().queueCommand(std::bind(&RenderBeast::initializeCore, this), CTQF_InternalQueue);
	}

	void RenderBeast::destroy()
	{
		Renderer::destroy();

		gCoreThread().queueCommand(std::bind(&RenderBeast::destroyCore, this));
		gCoreThread().submit(true);
	}

	void RenderBeast::initializeCore()
	{
		RendererUtility::startUp();

		mCoreOptions = bs_shared_ptr_new<RenderBeastOptions>();
		mObjectRenderer = bs_new<ObjectRenderer>();

		mDefaultMaterial = bs_new<DefaultMaterial>();
		mSkyboxMat = bs_new<SkyboxMat<false>>();
		mSkyboxSolidColorMat = bs_new<SkyboxMat<true>>();
		mFlatFramebufferToTextureMat = bs_new<FlatFramebufferToTextureMat>();

		mTiledDeferredLightingMats = bs_new<TiledDeferredLightingMaterials>();
		mTileDeferredImageBasedLightingMats = bs_new<TiledDeferredImageBasedLightingMaterials>();

		mPreintegratedEnvBRDF = TiledDeferredImageBasedLighting::generatePreintegratedEnvBRDF();
		mGPULightData = bs_new<GPULightData>();
		mGPUReflProbeData = bs_new<GPUReflProbeData>();
		mLightGrid = bs_new<LightGrid>();

		GpuResourcePool::startUp();
		PostProcessing::startUp();
	}

	void RenderBeast::destroyCore()
	{
		if (mObjectRenderer != nullptr)
			bs_delete(mObjectRenderer);

		for (auto& entry : mRenderables)
			bs_delete(entry);

		for (auto& entry : mCameras)
			bs_delete(entry.second);

		mRenderTargets.clear();
		mCameras.clear();
		mRenderables.clear();
		mRenderableVisibility.clear();

		mReflCubemapArrayTex = nullptr;
		mSkyboxTexture = nullptr;
		mSkyboxFilteredReflections = nullptr;
		mSkyboxIrradiance = nullptr;

		PostProcessing::shutDown();
		GpuResourcePool::shutDown();

		bs_delete(mDefaultMaterial);
		bs_delete(mSkyboxMat);
		bs_delete(mSkyboxSolidColorMat);
		bs_delete(mGPULightData);
		bs_delete(mGPUReflProbeData);
		bs_delete(mLightGrid);
		bs_delete(mFlatFramebufferToTextureMat);
		bs_delete(mTiledDeferredLightingMats);
		bs_delete(mTileDeferredImageBasedLightingMats);

		mPreintegratedEnvBRDF = nullptr;

		RendererUtility::shutDown();

		assert(mSamplerOverrides.empty());
	}

	void RenderBeast::notifyRenderableAdded(Renderable* renderable)
	{
		UINT32 renderableId = (UINT32)mRenderables.size();

		renderable->setRendererId(renderableId);

		mRenderables.push_back(bs_new<RendererObject>());
		mRenderableCullInfos.push_back(CullInfo(renderable->getBounds(), renderable->getLayer()));
		mRenderableVisibility.push_back(false);

		RendererObject* rendererObject = mRenderables.back();
		rendererObject->renderable = renderable;
		rendererObject->updatePerObjectBuffer();

		SPtr<Mesh> mesh = renderable->getMesh();
		if (mesh != nullptr)
		{
			const MeshProperties& meshProps = mesh->getProperties();
			SPtr<VertexDeclaration> vertexDecl = mesh->getVertexData()->vertexDeclaration;

			for (UINT32 i = 0; i < meshProps.getNumSubMeshes(); i++)
			{
				rendererObject->elements.push_back(BeastRenderableElement());
				BeastRenderableElement& renElement = rendererObject->elements.back();

				renElement.mesh = mesh;
				renElement.subMesh = meshProps.getSubMesh(i);
				renElement.renderableId = renderableId;
				renElement.animType = renderable->getAnimType();
				renElement.animationId = renderable->getAnimationId();
				renElement.morphShapeVersion = 0;
				renElement.morphShapeBuffer = renderable->getMorphShapeBuffer();
				renElement.boneMatrixBuffer = renderable->getBoneMatrixBuffer();
				renElement.morphVertexDeclaration = renderable->getMorphVertexDeclaration();

				renElement.material = renderable->getMaterial(i);
				if (renElement.material == nullptr)
					renElement.material = renderable->getMaterial(0);

				if (renElement.material != nullptr && renElement.material->getShader() == nullptr)
					renElement.material = nullptr;

				// If no material use the default material
				if (renElement.material == nullptr)
					renElement.material = mDefaultMaterial->getMaterial();

				// Determine which technique to use
				static StringID techniqueIDLookup[4] = { StringID::NONE, RTag_Skinned, RTag_Morph, RTag_SkinnedMorph };
				static_assert((UINT32)RenderableAnimType::Count == 4, "RenderableAnimType is expected to have four sequential entries.");
				
				UINT32 techniqueIdx = -1;
				RenderableAnimType animType = renderable->getAnimType();
				if(animType != RenderableAnimType::None)
					techniqueIdx = renElement.material->findTechnique(techniqueIDLookup[(int)animType]);

				if (techniqueIdx == (UINT32)-1)
					techniqueIdx = renElement.material->getDefaultTechnique();

				renElement.techniqueIdx = techniqueIdx;

				// Validate mesh <-> shader vertex bindings
				if (renElement.material != nullptr)
				{
					UINT32 numPasses = renElement.material->getNumPasses(techniqueIdx);
					for (UINT32 j = 0; j < numPasses; j++)
					{
						SPtr<Pass> pass = renElement.material->getPass(j, techniqueIdx);

						SPtr<VertexDeclaration> shaderDecl = pass->getVertexProgram()->getInputDeclaration();
						if (!vertexDecl->isCompatible(shaderDecl))
						{
							Vector<VertexElement> missingElements = vertexDecl->getMissingElements(shaderDecl);

							// If using morph shapes ignore POSITION1 and NORMAL1 missing since we assign them from within the renderer
							if(animType == RenderableAnimType::Morph || animType == RenderableAnimType::SkinnedMorph)
							{
								auto removeIter = std::remove_if(missingElements.begin(), missingElements.end(), [](const VertexElement& x)
								{
									return (x.getSemantic() == VES_POSITION && x.getSemanticIdx() == 1) ||
										(x.getSemantic() == VES_NORMAL && x.getSemanticIdx() == 1);
								});

								missingElements.erase(removeIter, missingElements.end());
							}

							if (!missingElements.empty())
							{
								StringStream wrnStream;
								wrnStream << "Provided mesh is missing required vertex attributes to render with the provided shader. Missing elements: " << std::endl;

								for (auto& entry : missingElements)
									wrnStream << "\t" << toString(entry.getSemantic()) << entry.getSemanticIdx() << std::endl;

								LOGWRN(wrnStream.str());
								break;
							}
						}
					}
				}

				// Generate or assigned renderer specific data for the material
				renElement.params = renElement.material->createParamsSet(techniqueIdx);
				renElement.material->updateParamsSet(renElement.params, true);

				// Generate or assign sampler state overrides
				SamplerOverrideKey samplerKey(renElement.material, techniqueIdx);
				auto iterFind = mSamplerOverrides.find(samplerKey);
				if (iterFind != mSamplerOverrides.end())
				{
					renElement.samplerOverrides = iterFind->second;
					iterFind->second->refCount++;
				}
				else
				{
					SPtr<Shader> shader = renElement.material->getShader();
					MaterialSamplerOverrides* samplerOverrides = SamplerOverrideUtility::generateSamplerOverrides(shader,
						renElement.material->_getInternalParams(), renElement.params, mCoreOptions);

					mSamplerOverrides[samplerKey] = samplerOverrides;

					renElement.samplerOverrides = samplerOverrides;
					samplerOverrides->refCount++;
				}

				mObjectRenderer->initElement(*rendererObject, renElement);
			}
		}
	}

	void RenderBeast::notifyRenderableRemoved(Renderable* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();
		Renderable* lastRenerable = mRenderables.back()->renderable;
		UINT32 lastRenderableId = lastRenerable->getRendererId();

		RendererObject* rendererObject = mRenderables[renderableId];
		Vector<BeastRenderableElement>& elements = rendererObject->elements;
		for (auto& element : elements)
		{
			SamplerOverrideKey samplerKey(element.material, element.techniqueIdx);

			auto iterFind = mSamplerOverrides.find(samplerKey);
			assert(iterFind != mSamplerOverrides.end());

			MaterialSamplerOverrides* samplerOverrides = iterFind->second;
			samplerOverrides->refCount--;
			if (samplerOverrides->refCount == 0)
			{
				SamplerOverrideUtility::destroySamplerOverrides(samplerOverrides);
				mSamplerOverrides.erase(iterFind);
			}

			element.samplerOverrides = nullptr;
		}

		if (renderableId != lastRenderableId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mRenderables[renderableId], mRenderables[lastRenderableId]);
			std::swap(mRenderableCullInfos[renderableId], mRenderableCullInfos[lastRenderableId]);

			lastRenerable->setRendererId(renderableId);

			for (auto& element : elements)
				element.renderableId = renderableId;
		}

		// Last element is the one we want to erase
		mRenderables.erase(mRenderables.end() - 1);
		mRenderableCullInfos.erase(mRenderableCullInfos.end() - 1);
		mRenderableVisibility.erase(mRenderableVisibility.end() - 1);

		bs_delete(rendererObject);
	}

	void RenderBeast::notifyRenderableUpdated(Renderable* renderable)
	{
		UINT32 renderableId = renderable->getRendererId();

		mRenderables[renderableId]->updatePerObjectBuffer();
		mRenderableCullInfos[renderableId].bounds = renderable->getBounds();
	}

	void RenderBeast::notifyLightAdded(Light* light)
	{
		if (light->getType() == LightType::Directional)
		{
			UINT32 lightId = (UINT32)mDirectionalLights.size();
			light->setRendererId(lightId);

			mDirectionalLights.push_back(RendererLight(light));
		}
		else
		{
			if (light->getType() == LightType::Radial)
			{
				UINT32 lightId = (UINT32)mRadialLights.size();
				light->setRendererId(lightId);

				mRadialLights.push_back(RendererLight(light));
				mPointLightWorldBounds.push_back(light->getBounds());
			}
			else // Spot
			{
				UINT32 lightId = (UINT32)mSpotLights.size();
				light->setRendererId(lightId);

				mSpotLights.push_back(RendererLight(light));
				mSpotLightWorldBounds.push_back(light->getBounds());
			}
		}
	}

	void RenderBeast::notifyLightUpdated(Light* light)
	{
		UINT32 lightId = light->getRendererId();

		if (light->getType() == LightType::Radial)
			mPointLightWorldBounds[lightId] = light->getBounds();
		else if(light->getType() == LightType::Spot)
			mSpotLightWorldBounds[lightId] = light->getBounds();
	}

	void RenderBeast::notifyLightRemoved(Light* light)
	{
		UINT32 lightId = light->getRendererId();
		if (light->getType() == LightType::Directional)
		{
			Light* lastLight = mDirectionalLights.back().getInternal();
			UINT32 lastLightId = lastLight->getRendererId();

			if (lightId != lastLightId)
			{
				// Swap current last element with the one we want to erase
				std::swap(mDirectionalLights[lightId], mDirectionalLights[lastLightId]);
				lastLight->setRendererId(lightId);
			}

			// Last element is the one we want to erase
			mDirectionalLights.erase(mDirectionalLights.end() - 1);
		}
		else
		{
			if (light->getType() == LightType::Radial)
			{
				Light* lastLight = mRadialLights.back().getInternal();
				UINT32 lastLightId = lastLight->getRendererId();

				if (lightId != lastLightId)
				{
					// Swap current last element with the one we want to erase
					std::swap(mRadialLights[lightId], mRadialLights[lastLightId]);
					std::swap(mPointLightWorldBounds[lightId], mPointLightWorldBounds[lastLightId]);

					lastLight->setRendererId(lightId);
				}

				// Last element is the one we want to erase
				mRadialLights.erase(mRadialLights.end() - 1);
				mPointLightWorldBounds.erase(mPointLightWorldBounds.end() - 1);
			}
			else // Spot
			{
				Light* lastLight = mSpotLights.back().getInternal();
				UINT32 lastLightId = lastLight->getRendererId();

				if (lightId != lastLightId)
				{
					// Swap current last element with the one we want to erase
					std::swap(mSpotLights[lightId], mSpotLights[lastLightId]);
					std::swap(mSpotLightWorldBounds[lightId], mSpotLightWorldBounds[lastLightId]);

					lastLight->setRendererId(lightId);
				}

				// Last element is the one we want to erase
				mSpotLights.erase(mSpotLights.end() - 1);
				mSpotLightWorldBounds.erase(mSpotLightWorldBounds.end() - 1);
			}
		}
	}

	void RenderBeast::notifyCameraAdded(const Camera* camera)
	{
		RendererCamera* renCamera = updateCameraData(camera);
		renCamera->updatePerViewBuffer();
	}

	void RenderBeast::notifyCameraUpdated(const Camera* camera, UINT32 updateFlag)
	{
		RendererCamera* rendererCam;
		if((updateFlag & (UINT32)CameraDirtyFlag::Everything) != 0)
		{
			rendererCam = updateCameraData(camera);
		}
		else if((updateFlag & (UINT32)CameraDirtyFlag::PostProcess) != 0)
		{
			rendererCam = mCameras[camera];

			rendererCam->setPostProcessSettings(camera->getPostProcessSettings());
		}
		else // Transform
		{
			rendererCam = mCameras[camera];

			rendererCam->setTransform(
				camera->getPosition(),
				camera->getForward(),
				camera->getViewMatrix(),
				camera->getProjectionMatrixRS(),
				camera->getWorldFrustum());
		}

		rendererCam->updatePerViewBuffer();
	}

	void RenderBeast::notifyCameraRemoved(const Camera* camera)
	{
		updateCameraData(camera, true);
	}

	void RenderBeast::notifyReflectionProbeAdded(ReflectionProbe* probe)
	{
		UINT32 probeId = (UINT32)mReflProbes.size();
		probe->setRendererId(probeId);

		mReflProbes.push_back(RendererReflectionProbe(probe));
		RendererReflectionProbe& probeInfo = mReflProbes.back();

		mReflProbeWorldBounds.push_back(probe->getBounds());

		// Find a spot in cubemap array
		UINT32 numArrayEntries = (UINT32)mCubemapArrayUsedSlots.size();
		for(UINT32 i = 0; i < numArrayEntries; i++)
		{
			if(!mCubemapArrayUsedSlots[i])
			{
				probeInfo.arrayIdx = i;
				mCubemapArrayUsedSlots[i] = true;
				break;
			}
		}

		// No empty slot was found
		if (probeInfo.arrayIdx == -1)
		{
			probeInfo.arrayIdx = numArrayEntries;
			mCubemapArrayUsedSlots.push_back(true);
		}

		if(probeInfo.arrayIdx > MaxReflectionCubemaps)
		{
			LOGERR("Reached the maximum number of allowed reflection probe cubemaps at once. "
				"Ignoring reflection probe data.");
		}
	}

	void RenderBeast::notifyReflectionProbeUpdated(ReflectionProbe* probe)
	{
		// Should only get called if transform changes, any other major changes and ReflProbeInfo entry gets rebuild
		UINT32 probeId = probe->getRendererId();
		mReflProbeWorldBounds[probeId] = probe->getBounds();

		RendererReflectionProbe& probeInfo = mReflProbes[probeId];
		probeInfo.arrayDirty = true;

		LightProbeCache::instance().notifyDirty(probe->getUUID());
		probeInfo.textureDirty = true;
	}

	void RenderBeast::notifyReflectionProbeRemoved(ReflectionProbe* probe)
	{
		UINT32 probeId = probe->getRendererId();
		UINT32 arrayIdx = mReflProbes[probeId].arrayIdx;

		ReflectionProbe* lastProbe = mReflProbes.back().probe;
		UINT32 lastProbeId = lastProbe->getRendererId();

		if (probeId != lastProbeId)
		{
			// Swap current last element with the one we want to erase
			std::swap(mReflProbes[probeId], mReflProbes[lastProbeId]);
			std::swap(mReflProbeWorldBounds[probeId], mReflProbeWorldBounds[lastProbeId]);

			probe->setRendererId(probeId);
		}

		// Last element is the one we want to erase
		mRadialLights.erase(mRadialLights.end() - 1);
		mPointLightWorldBounds.erase(mPointLightWorldBounds.end() - 1);

		if (arrayIdx != -1)
			mCubemapArrayUsedSlots[arrayIdx] = false;

		LightProbeCache::instance().unloadCachedTexture(probe->getUUID());
	}

	void RenderBeast::notifySkyboxAdded(Skybox* skybox)
	{
		mSkybox = skybox;

		SPtr<Texture> skyTex = skybox->getTexture();
		if (skyTex != nullptr && skyTex->getProperties().getTextureType() == TEX_TYPE_CUBE_MAP)
			mSkyboxTexture = skyTex;

		mSkyboxFilteredReflections = nullptr;
		mSkyboxIrradiance = nullptr;
	}

	void RenderBeast::notifySkyboxTextureChanged(Skybox* skybox)
	{
		LightProbeCache::instance().notifyDirty(skybox->getUUID());

		if (mSkybox == skybox)
		{
			mSkyboxTexture = skybox->getTexture();
			mSkyboxFilteredReflections = nullptr;
			mSkyboxIrradiance = nullptr;
		}
	}

	void RenderBeast::notifySkyboxRemoved(Skybox* skybox)
	{
		LightProbeCache::instance().unloadCachedTexture(skybox->getUUID());

		if (mSkybox == skybox)
			mSkyboxTexture = nullptr;
	}

	SPtr<PostProcessSettings> RenderBeast::createPostProcessSettings() const
	{
		return bs_shared_ptr_new<StandardPostProcessSettings>();
	}

	RendererCamera* RenderBeast::updateCameraData(const Camera* camera, bool forceRemove)
	{
		RendererCamera* output;

		SPtr<RenderTarget> renderTarget = camera->getViewport()->getTarget();

		auto iterFind = mCameras.find(camera);
		if(forceRemove)
		{
			if(iterFind != mCameras.end())
			{
				bs_delete(iterFind->second);
				mCameras.erase(iterFind);
			}

			renderTarget = nullptr;
			output = nullptr;
		}
		else
		{
			SPtr<Viewport> viewport = camera->getViewport();
			RENDERER_VIEW_DESC viewDesc;

			viewDesc.target.clearFlags = 0;
			if (viewport->getRequiresColorClear())
				viewDesc.target.clearFlags |= FBT_COLOR;

			if (viewport->getRequiresDepthClear())
				viewDesc.target.clearFlags |= FBT_DEPTH;

			if (viewport->getRequiresStencilClear())
				viewDesc.target.clearFlags |= FBT_STENCIL;

			viewDesc.target.clearColor = viewport->getClearColor();
			viewDesc.target.clearDepthValue = viewport->getClearDepthValue();
			viewDesc.target.clearStencilValue = viewport->getClearStencilValue();

			viewDesc.target.target = viewport->getTarget();
			viewDesc.target.nrmViewRect = viewport->getNormArea();
			viewDesc.target.viewRect = Rect2I(
				viewport->getX(),
				viewport->getY(),
				(UINT32)viewport->getWidth(),
				(UINT32)viewport->getHeight());

			if (viewDesc.target.target != nullptr)
			{
				viewDesc.target.targetWidth = viewDesc.target.target->getProperties().getWidth();
				viewDesc.target.targetHeight = viewDesc.target.target->getProperties().getHeight();
			}
			else
			{
				viewDesc.target.targetWidth = 0;
				viewDesc.target.targetHeight = 0;
			}

			viewDesc.target.numSamples = camera->getMSAACount();

			viewDesc.isOverlay = camera->getFlags().isSet(CameraFlag::Overlay);
			viewDesc.isHDR = camera->getFlags().isSet(CameraFlag::HDR);
			viewDesc.noLighting = camera->getFlags().isSet(CameraFlag::NoLighting);
			viewDesc.triggerCallbacks = true;
			viewDesc.runPostProcessing = true;
			viewDesc.renderingReflections = false;

			viewDesc.cullFrustum = camera->getWorldFrustum();
			viewDesc.visibleLayers = camera->getLayers();
			viewDesc.nearPlane = camera->getNearClipDistance();
			viewDesc.farPlane = camera->getFarClipDistance();
			viewDesc.flipView = false;

			viewDesc.viewOrigin = camera->getPosition();
			viewDesc.viewDirection = camera->getForward();
			viewDesc.projTransform = camera->getProjectionMatrixRS();
			viewDesc.viewTransform = camera->getViewMatrix();

			viewDesc.stateReduction = mCoreOptions->stateReductionMode;
			viewDesc.sceneCamera = camera;

			if (iterFind != mCameras.end())
			{
				output = iterFind->second;
				output->setView(viewDesc);
			}
			else
			{
				output = bs_new<RendererCamera>(viewDesc);
				mCameras[camera] = output;
			}

			output->setPostProcessSettings(camera->getPostProcessSettings());
		}

		// Remove from render target list
		int rtChanged = 0; // 0 - No RT, 1 - RT found, 2 - RT changed
		for (auto iterTarget = mRenderTargets.begin(); iterTarget != mRenderTargets.end(); ++iterTarget)
		{
			RendererRenderTarget& target = *iterTarget;
			for (auto iterCam = target.cameras.begin(); iterCam != target.cameras.end(); ++iterCam)
			{
				if (camera == *iterCam)
				{
					if (renderTarget != target.target)
					{
						target.cameras.erase(iterCam);
						rtChanged = 2;

					}
					else
						rtChanged = 1;

					break;
				}
			}

			if (target.cameras.empty())
			{
				mRenderTargets.erase(iterTarget);
				break;
			}
		}

		// Register in render target list
		if (renderTarget != nullptr && (rtChanged == 0 || rtChanged == 2))
		{
			auto findIter = std::find_if(mRenderTargets.begin(), mRenderTargets.end(),
				[&](const RendererRenderTarget& x) { return x.target == renderTarget; });

			if (findIter != mRenderTargets.end())
			{
				findIter->cameras.push_back(camera);
			}
			else
			{
				mRenderTargets.push_back(RendererRenderTarget());
				RendererRenderTarget& renderTargetData = mRenderTargets.back();

				renderTargetData.target = renderTarget;
				renderTargetData.cameras.push_back(camera);
			}

			// Sort render targets based on priority
			auto cameraComparer = [&](const Camera* a, const Camera* b) { return a->getPriority() > b->getPriority(); };
			auto renderTargetInfoComparer = [&](const RendererRenderTarget& a, const RendererRenderTarget& b)
			{ return a.target->getProperties().getPriority() > b.target->getProperties().getPriority(); };
			std::sort(begin(mRenderTargets), end(mRenderTargets), renderTargetInfoComparer);

			for (auto& camerasPerTarget : mRenderTargets)
			{
				Vector<const Camera*>& cameras = camerasPerTarget.cameras;

				std::sort(begin(cameras), end(cameras), cameraComparer);
			}
		}

		return output;
	}

	void RenderBeast::setOptions(const SPtr<RendererOptions>& options)
	{
		mOptions = std::static_pointer_cast<RenderBeastOptions>(options);
		mOptionsDirty = true;
	}

	SPtr<RendererOptions> RenderBeast::getOptions() const
	{
		return mOptions;
	}

	void RenderBeast::syncOptions(const RenderBeastOptions& options)
	{
		bool filteringChanged = mCoreOptions->filtering != options.filtering;
		if (options.filtering == RenderBeastFiltering::Anisotropic)
			filteringChanged |= mCoreOptions->anisotropyMax != options.anisotropyMax;

		if (filteringChanged)
			refreshSamplerOverrides(true);

		*mCoreOptions = options;

		for (auto& entry : mCameras)
		{
			RendererCamera* rendererCam = entry.second;
			rendererCam->setStateReductionMode(mCoreOptions->stateReductionMode);
		}
	}

	void RenderBeast::renderAll() 
	{
		// Sync all dirty sim thread CoreObject data to core thread
		CoreObjectManager::instance().syncToCore();

		if (mOptionsDirty)
		{
			gCoreThread().queueCommand(std::bind(&RenderBeast::syncOptions, this, *mOptions));
			mOptionsDirty = false;
		}

		gCoreThread().queueCommand(std::bind(&RenderBeast::renderAllCore, this, gTime().getTime(), gTime().getFrameDelta()));
	}

	void RenderBeast::renderAllCore(float time, float delta)
	{
		THROW_IF_NOT_CORE_THREAD;

		gProfilerGPU().beginFrame();
		gProfilerCPU().beginSample("renderAllCore");

		// Note: I'm iterating over all sampler states every frame. If this ends up being a performance
		// issue consider handling this internally in ct::Material which can only do it when sampler states
		// are actually modified after sync
		refreshSamplerOverrides();

		// Update global per-frame hardware buffers
		mObjectRenderer->setParamFrameParams(time);

		// Retrieve animation data
		AnimationManager::instance().waitUntilComplete();
		const RendererAnimationData& animData = AnimationManager::instance().getRendererData();
		
		FrameInfo frameInfo(delta, animData);

		// Update reflection probes
		updateLightProbes(frameInfo);

		// Gather all views
		Vector<RendererCamera*> views;
		for (auto& rtInfo : mRenderTargets)
		{
			SPtr<RenderTarget> target = rtInfo.target;
			Vector<const Camera*>& cameras = rtInfo.cameras;

			UINT32 numCameras = (UINT32)cameras.size();
			for (UINT32 i = 0; i < numCameras; i++)
			{
				RendererCamera* viewInfo = mCameras[cameras[i]];
				views.push_back(viewInfo);
			}
		}

		// Render everything
		renderViews(views.data(), (UINT32)views.size(), frameInfo);

		gProfilerGPU().endFrame();

		// Present render targets with back buffers
		for (auto& rtInfo : mRenderTargets)
		{
			if(rtInfo.target->getProperties().isWindow())
				RenderAPI::instance().swapBuffers(rtInfo.target);
		}

		gProfilerCPU().endSample("renderAllCore");
	}

	void RenderBeast::renderViews(RendererCamera** views, UINT32 numViews, const FrameInfo& frameInfo)
	{
		// Generate render queues per camera
		mRenderableVisibility.assign(mRenderableVisibility.size(), false);

		for(UINT32 i = 0; i < numViews; i++)
			views[i]->determineVisible(mRenderables, mRenderableCullInfos, &mRenderableVisibility);

		// Generate a list of lights and their GPU buffers
		UINT32 numDirLights = (UINT32)mDirectionalLights.size();
		for (UINT32 i = 0; i < numDirLights; i++)
		{
			mLightDataTemp.push_back(LightData());
			mDirectionalLights[i].getParameters(mLightDataTemp.back());
		}

		UINT32 numRadialLights = (UINT32)mRadialLights.size();
		UINT32 numVisibleRadialLights = 0;
		mLightVisibilityTemp.resize(numRadialLights, false);
		for (UINT32 i = 0; i < numViews; i++)
			views[i]->calculateVisibility(mPointLightWorldBounds, mLightVisibilityTemp);

		for(UINT32 i = 0; i < numRadialLights; i++)
		{
			if (!mLightVisibilityTemp[i])
				continue;

			mLightDataTemp.push_back(LightData());
			mRadialLights[i].getParameters(mLightDataTemp.back());
			numVisibleRadialLights++;
		}

		UINT32 numSpotLights = (UINT32)mSpotLights.size();
		UINT32 numVisibleSpotLights = 0;
		mLightVisibilityTemp.resize(numSpotLights, false);
		for (UINT32 i = 0; i < numViews; i++)
			views[i]->calculateVisibility(mSpotLightWorldBounds, mLightVisibilityTemp);

		for (UINT32 i = 0; i < numSpotLights; i++)
		{
			if (!mLightVisibilityTemp[i])
				continue;

			mLightDataTemp.push_back(LightData());
			mSpotLights[i].getParameters(mLightDataTemp.back());
			numVisibleSpotLights++;
		}

		mGPULightData->setLights(mLightDataTemp, numDirLights, numVisibleRadialLights, numVisibleSpotLights);

		mLightDataTemp.clear();
		mLightVisibilityTemp.clear();

		// Gemerate reflection probes and their GPU buffers
		UINT32 numProbes = (UINT32)mReflProbes.size();

		mReflProbeVisibilityTemp.resize(numProbes, false);
		for (UINT32 i = 0; i < numViews; i++)
			views[i]->calculateVisibility(mReflProbeWorldBounds, mReflProbeVisibilityTemp);

		for(UINT32 i = 0; i < numProbes; i++)
		{
			if (!mReflProbeVisibilityTemp[i])
				continue;

			mReflProbeDataTemp.push_back(ReflProbeData());
			mReflProbes[i].getParameters(mReflProbeDataTemp.back());
		}

		// Sort probes so bigger ones get accessed first, this way we overlay smaller ones on top of biggers ones when
		// rendering
		auto sorter = [](const ReflProbeData& lhs, const ReflProbeData& rhs)
		{
			return rhs.radius < lhs.radius;
		};

		std::sort(mReflProbeDataTemp.begin(), mReflProbeDataTemp.end(), sorter);

		mGPUReflProbeData->setProbes(mReflProbeDataTemp, numProbes);

		mReflProbeDataTemp.clear();
		mReflProbeVisibilityTemp.clear();

		// Update various buffers required by each renderable
		UINT32 numRenderables = (UINT32)mRenderables.size();
		for (UINT32 i = 0; i < numRenderables; i++)
		{
			if (!mRenderableVisibility[i])
				continue;

			// Note: Before uploading bone matrices perhaps check if they has actually been changed since last frame
			mRenderables[i]->renderable->updateAnimationBuffers(frameInfo.animData);

			// Note: Could this step be moved in notifyRenderableUpdated, so it only triggers when material actually gets
			// changed? Although it shouldn't matter much because if the internal versions keeping track of dirty params.
			for (auto& element : mRenderables[i]->elements)
				element.material->updateParamsSet(element.params);

			mRenderables[i]->perObjectParamBuffer->flushToGPU();
		}

		for (UINT32 i = 0; i < numViews; i++)
		{
			if (views[i]->isOverlay())
				renderOverlay(views[i]);
			else
				renderView(views[i], frameInfo.timeDelta);
		}
	}

	void RenderBeast::renderView(RendererCamera* viewInfo, float frameDelta)
	{
		gProfilerCPU().beginSample("Render");

		const Camera* sceneCamera = viewInfo->getSceneCamera();

		SPtr<GpuParamBlockBuffer> perCameraBuffer = viewInfo->getPerViewBuffer();
		perCameraBuffer->flushToGPU();

		Matrix4 viewProj = viewInfo->getViewProjMatrix();
		UINT32 numSamples = viewInfo->getNumSamples();

		viewInfo->beginRendering(true);

		// Prepare light grid required for transparent object rendering
		mLightGrid->updateGrid(*viewInfo, *mGPULightData, *mGPUReflProbeData, viewInfo->renderWithNoLighting());

		SPtr<GpuParamBlockBuffer> gridParams;
		SPtr<GpuBuffer> gridLightOffsetsAndSize, gridLightIndices;
		SPtr<GpuBuffer> gridProbeOffsetsAndSize, gridProbeIndices;
		mLightGrid->getOutputs(gridLightOffsetsAndSize, gridLightIndices, gridProbeOffsetsAndSize, gridProbeIndices, 
			gridParams);

		// Prepare image based material and its param buffer
		ITiledDeferredImageBasedLightingMat* imageBasedLightingMat =
			mTileDeferredImageBasedLightingMats->get(numSamples);

		imageBasedLightingMat->setReflectionProbes(*mGPUReflProbeData, mReflCubemapArrayTex, viewInfo->isRenderingReflections());

		float skyBrightness = 1.0f;
		if (mSkybox != nullptr)
			skyBrightness = mSkybox->getBrightness();

		imageBasedLightingMat->setSky(mSkyboxFilteredReflections, mSkyboxIrradiance, skyBrightness);

		// Assign camera and per-call data to all relevant renderables
		const VisibilityInfo& visibility = viewInfo->getVisibilityMasks();
		UINT32 numRenderables = (UINT32)mRenderables.size();
		SPtr<GpuParamBlockBuffer> reflParamBuffer = imageBasedLightingMat->getReflectionsParamBuffer();
		SPtr<SamplerState> reflSamplerState = imageBasedLightingMat->getReflectionsSamplerState();
		for (UINT32 i = 0; i < numRenderables; i++)
		{
			if (!visibility.renderables[i])
				continue;

			RendererObject* rendererObject = mRenderables[i];
			rendererObject->updatePerCallBuffer(viewProj);

			for (auto& element : mRenderables[i]->elements)
			{
				if (element.perCameraBindingIdx != -1)
					element.params->setParamBlockBuffer(element.perCameraBindingIdx, perCameraBuffer, true);

				// Everything below is required only for forward rendering (ATM only used for transparent objects)
				// Note: It would be nice to be able to set this once and keep it, only updating if the buffers actually
				// change (e.g. when growing). Although technically the internal systems should be smart enough to
				// avoid updates unless objects actually changed.
				if (element.gridParamsBindingIdx != -1)
					element.params->setParamBlockBuffer(element.gridParamsBindingIdx, gridParams, true);

				element.gridLightOffsetsAndSizeParam.set(gridLightOffsetsAndSize);
				element.gridLightIndicesParam.set(gridLightIndices);
				element.lightsBufferParam.set(mGPULightData->getLightBuffer());

				// Image based lighting params
				ImageBasedLightingParams& iblParams = element.imageBasedParams;
				if (iblParams.reflProbeParamsBindingIdx != -1)
					element.params->setParamBlockBuffer(iblParams.reflProbeParamsBindingIdx, reflParamBuffer);

				element.gridProbeOffsetsAndSizeParam.set(gridProbeOffsetsAndSize);

				iblParams.reflectionProbeIndicesParam.set(gridProbeIndices);
				iblParams.reflectionProbesParam.set(mGPUReflProbeData->getProbeBuffer());

				iblParams.skyReflectionsTexParam.set(mSkyboxFilteredReflections);
				iblParams.skyIrradianceTexParam.set(mSkyboxIrradiance);

				iblParams.reflectionProbeCubemapsTexParam.set(mReflCubemapArrayTex);
				iblParams.preintegratedEnvBRDFParam.set(mPreintegratedEnvBRDF);

				iblParams.reflectionProbeCubemapsSampParam.set(reflSamplerState);
				iblParams.skyReflectionsSampParam.set(reflSamplerState);
			}
		}

		SPtr<RenderTargets> renderTargets = viewInfo->getRenderTargets();
		renderTargets->allocate(RTT_GBuffer);
		renderTargets->bindGBuffer();

		// Trigger pre-base-pass callbacks
		auto iterRenderCallback = mCallbacks.begin();

		if (viewInfo->checkTriggerCallbacks())
		{
			while (iterRenderCallback != mCallbacks.end())
			{
				RendererExtension* extension = *iterRenderCallback;
				if (extension->getLocation() != RenderLocation::PreBasePass)
					break;

				if (extension->check(*sceneCamera))
					extension->render(*sceneCamera);

				++iterRenderCallback;
			}
		}

		// Render base pass
		const Vector<RenderQueueElement>& opaqueElements = viewInfo->getOpaqueQueue()->getSortedElements();
		for (auto iter = opaqueElements.begin(); iter != opaqueElements.end(); ++iter)
		{
			BeastRenderableElement* renderElem = static_cast<BeastRenderableElement*>(iter->renderElem);
			renderElement(*renderElem, iter->passIdx, iter->applyPass, viewProj);
		}

		// Trigger post-base-pass callbacks
		if (viewInfo->checkTriggerCallbacks())
		{
			while (iterRenderCallback != mCallbacks.end())
			{
				RendererExtension* extension = *iterRenderCallback;
				if (extension->getLocation() != RenderLocation::PostBasePass)
					break;

				if (extension->check(*sceneCamera))
					extension->render(*sceneCamera);

				++iterRenderCallback;
			}
		}

		RenderAPI& rapi = RenderAPI::instance();
		rapi.setRenderTarget(nullptr);

		// Render light pass into light accumulation buffer
		ITiledDeferredLightingMat* lightingMat = mTiledDeferredLightingMats->get(numSamples);

		renderTargets->allocate(RTT_LightAccumulation);

		lightingMat->setLights(*mGPULightData);
		lightingMat->execute(renderTargets, perCameraBuffer, viewInfo->renderWithNoLighting());

		renderTargets->allocate(RTT_SceneColor);

		// Render image based lighting and add it with light accumulation, output to scene color
		// Note: Image based lighting is split from direct lighting in order to reduce load on GPU shared memory. The
		// image based shader ends up re-doing a lot of calculations and it could be beneficial to profile and see if
		// both methods can be squeezed into the same shader.
		imageBasedLightingMat->execute(renderTargets, perCameraBuffer, mPreintegratedEnvBRDF);

		renderTargets->release(RTT_LightAccumulation);
		renderTargets->release(RTT_GBuffer);

		bool usingFlattenedFB = numSamples > 1;

		renderTargets->bindSceneColor(true);

		// If we're using flattened framebuffer for MSAA we need to copy its contents to the MSAA scene texture before
		// continuing
		if(usingFlattenedFB)
		{
			mFlatFramebufferToTextureMat->execute(renderTargets->getSceneColorBuffer(), 
												  renderTargets->getSceneColor());
		}

		// Render skybox (if any)
		if (mSkyboxTexture != nullptr)
		{
			mSkyboxMat->bind(perCameraBuffer);
			mSkyboxMat->setParams(mSkyboxTexture, Color::White);
		}
		else
		{
			Color clearColor = viewInfo->getClearColor();

			mSkyboxSolidColorMat->bind(perCameraBuffer);
			mSkyboxSolidColorMat->setParams(nullptr, clearColor);
		}

		SPtr<Mesh> mesh = gRendererUtility().getSkyBoxMesh();
		gRendererUtility().draw(mesh, mesh->getProperties().getSubMesh(0));

		renderTargets->bindSceneColor(false);

		// Render transparent objects
		const Vector<RenderQueueElement>& transparentElements = viewInfo->getTransparentQueue()->getSortedElements();
		for (auto iter = transparentElements.begin(); iter != transparentElements.end(); ++iter)
		{
			BeastRenderableElement* renderElem = static_cast<BeastRenderableElement*>(iter->renderElem);
			renderElement(*renderElem, iter->passIdx, iter->applyPass, viewProj);
		}

		// Trigger post-light-pass callbacks
		if (viewInfo->checkTriggerCallbacks())
		{
			while (iterRenderCallback != mCallbacks.end())
			{
				RendererExtension* extension = *iterRenderCallback;
				if (extension->getLocation() != RenderLocation::PostLightPass)
					break;

				if (extension->check(*sceneCamera))
					extension->render(*sceneCamera);

				++iterRenderCallback;
			}
		}

		// Post-processing and final resolve
		Rect2 viewportArea = viewInfo->getViewportRect();

		if (viewInfo->checkRunPostProcessing())
		{
			// If using MSAA, resolve into non-MSAA texture before post-processing
			if(numSamples > 1)
			{
				rapi.setRenderTarget(renderTargets->getResolvedSceneColorRT());
				rapi.setViewport(viewportArea);

				SPtr<Texture> sceneColor = renderTargets->getSceneColor();
				gRendererUtility().blit(sceneColor, Rect2I::EMPTY, viewInfo->getFlipView());
			}

			// Post-processing code also takes care of writting to the final output target
			PostProcessing::instance().postProcess(viewInfo, renderTargets->getResolvedSceneColor(), frameDelta);
		}
		else
		{
			// Just copy from scene color to output if no post-processing
			SPtr<RenderTarget> target = viewInfo->getFinalTarget();

			rapi.setRenderTarget(target);
			rapi.setViewport(viewportArea);

			SPtr<Texture> sceneColor = renderTargets->getSceneColor();
			gRendererUtility().blit(sceneColor, Rect2I::EMPTY, viewInfo->getFlipView());
		}

		renderTargets->release(RTT_SceneColor);

		// Trigger overlay callbacks
		if (viewInfo->checkTriggerCallbacks())
		{
			while (iterRenderCallback != mCallbacks.end())
			{
				RendererExtension* extension = *iterRenderCallback;
				if (extension->getLocation() != RenderLocation::Overlay)
					break;

				if (extension->check(*sceneCamera))
					extension->render(*sceneCamera);

				++iterRenderCallback;
			}
		}

		viewInfo->endRendering();

		gProfilerCPU().endSample("Render");
	}

	void RenderBeast::renderOverlay(RendererCamera* viewInfo)
	{
		gProfilerCPU().beginSample("RenderOverlay");

		viewInfo->getPerViewBuffer()->flushToGPU();
		viewInfo->beginRendering(false);

		const Camera* camera = viewInfo->getSceneCamera();
		SPtr<RenderTarget> target = viewInfo->getFinalTarget();
		SPtr<Viewport> viewport = camera->getViewport();

		UINT32 clearBuffers = 0;
		if (viewport->getRequiresColorClear())
			clearBuffers |= FBT_COLOR;

		if (viewport->getRequiresDepthClear())
			clearBuffers |= FBT_DEPTH;

		if (viewport->getRequiresStencilClear())
			clearBuffers |= FBT_STENCIL;

		if (clearBuffers != 0)
		{
			RenderAPI::instance().setRenderTarget(target);
			RenderAPI::instance().clearViewport(clearBuffers, viewport->getClearColor(),
				viewport->getClearDepthValue(), viewport->getClearStencilValue());
		}
		else
			RenderAPI::instance().setRenderTarget(target, false, RT_COLOR0);

		RenderAPI::instance().setViewport(viewport->getNormArea());

		// Trigger overlay callbacks
		auto iterRenderCallback = mCallbacks.begin();
		while (iterRenderCallback != mCallbacks.end())
		{
			RendererExtension* extension = *iterRenderCallback;
			if (extension->getLocation() != RenderLocation::Overlay)
			{
				++iterRenderCallback;
				continue;
			}

			if (extension->check(*camera))
				extension->render(*camera);

			++iterRenderCallback;
		}

		viewInfo->endRendering();

		gProfilerCPU().endSample("RenderOverlay");
	}
	
	void RenderBeast::renderElement(const BeastRenderableElement& element, UINT32 passIdx, bool bindPass, 
									const Matrix4& viewProj)
	{
		SPtr<Material> material = element.material;

		if (bindPass)
			gRendererUtility().setPass(material, passIdx, element.techniqueIdx);

		gRendererUtility().setPassParams(element.params, passIdx);

		if(element.morphVertexDeclaration == nullptr)
			gRendererUtility().draw(element.mesh, element.subMesh);
		else
			gRendererUtility().drawMorph(element.mesh, element.subMesh, element.morphShapeBuffer, 
				element.morphVertexDeclaration);
	}

	void RenderBeast::updateLightProbes(const FrameInfo& frameInfo)
	{
		UINT32 numProbes = (UINT32)mReflProbes.size();

		bs_frame_mark();
		{		
			UINT32 currentCubeArraySize = 0;

			if(mReflCubemapArrayTex != nullptr)
				mReflCubemapArrayTex->getProperties().getNumArraySlices();

			bool forceArrayUpdate = false;
			if(mReflCubemapArrayTex == nullptr || (currentCubeArraySize < numProbes && currentCubeArraySize != MaxReflectionCubemaps))
			{
				TEXTURE_DESC cubeMapDesc;
				cubeMapDesc.type = TEX_TYPE_CUBE_MAP;
				cubeMapDesc.format = PF_FLOAT_R11G11B10;
				cubeMapDesc.width = IBLUtility::REFLECTION_CUBEMAP_SIZE;
				cubeMapDesc.height = IBLUtility::REFLECTION_CUBEMAP_SIZE;
				cubeMapDesc.numMips = PixelUtil::getMaxMipmaps(cubeMapDesc.width, cubeMapDesc.height, 1, cubeMapDesc.format);
				cubeMapDesc.numArraySlices = std::min(MaxReflectionCubemaps, numProbes + 4); // Keep a few empty entries

				mReflCubemapArrayTex = Texture::create(cubeMapDesc);

				forceArrayUpdate = true;
			}

			auto& cubemapArrayProps = mReflCubemapArrayTex->getProperties();

			TEXTURE_DESC cubemapDesc;
			cubemapDesc.type = TEX_TYPE_CUBE_MAP;
			cubemapDesc.format = PF_FLOAT_R11G11B10;
			cubemapDesc.width = IBLUtility::REFLECTION_CUBEMAP_SIZE;
			cubemapDesc.height = IBLUtility::REFLECTION_CUBEMAP_SIZE;
			cubemapDesc.numMips = PixelUtil::getMaxMipmaps(cubemapDesc.width, cubemapDesc.height, 1, cubemapDesc.format);
			cubemapDesc.usage = TU_STATIC | TU_RENDERTARGET;

			SPtr<Texture> scratchCubemap;
			if (numProbes > 0)
				scratchCubemap = Texture::create(cubemapDesc);

			FrameQueue<UINT32> emptySlots;
			for (UINT32 i = 0; i < numProbes; i++)
			{
				RendererReflectionProbe& probeInfo = mReflProbes[i];

				if (probeInfo.arrayIdx > MaxReflectionCubemaps)
					continue;

				if (probeInfo.texture == nullptr)
					probeInfo.texture = LightProbeCache::instance().getCachedRadianceTexture(probeInfo.probe->getUUID());

				if (probeInfo.texture == nullptr || probeInfo.textureDirty)
				{
					probeInfo.texture = Texture::create(cubemapDesc);

					if (!probeInfo.customTexture)
					{
						captureSceneCubeMap(probeInfo.texture, probeInfo.probe->getPosition(), true, frameInfo);
					}
					else
					{
						SPtr<Texture> customTexture = probeInfo.probe->getCustomTexture();
						IBLUtility::scaleCubemap(customTexture, 0, probeInfo.texture, 0);
					}

					IBLUtility::filterCubemapForSpecular(probeInfo.texture, scratchCubemap);
					LightProbeCache::instance().setCachedRadianceTexture(probeInfo.probe->getUUID(), probeInfo.texture);
				}

				probeInfo.textureDirty = false;

				if(probeInfo.arrayDirty || forceArrayUpdate)
				{
					auto& srcProps = probeInfo.texture->getProperties();
					bool isValid = srcProps.getWidth() == IBLUtility::REFLECTION_CUBEMAP_SIZE && 
						srcProps.getHeight() == IBLUtility::REFLECTION_CUBEMAP_SIZE &&
						srcProps.getNumMipmaps() == cubemapArrayProps.getNumMipmaps() &&
						srcProps.getTextureType() == TEX_TYPE_CUBE_MAP;

					if(!isValid)
					{
						if (!probeInfo.errorFlagged)
						{
							String errMsg = StringUtil::format("Cubemap texture invalid to use as a reflection cubemap. " 
								"Check texture size (must be {0}x{0}) and mip-map count", 
								IBLUtility::REFLECTION_CUBEMAP_SIZE);

							LOGERR(errMsg);
							probeInfo.errorFlagged = true;
						}
					}
					else
					{
						for(UINT32 face = 0; face < 6; face++)
							for(UINT32 mip = 0; mip <= srcProps.getNumMipmaps(); mip++)
								probeInfo.texture->copy(mReflCubemapArrayTex, face, mip, probeInfo.arrayIdx * 6 + face, mip);
					}

					probeInfo.arrayDirty = false;
				}

				// Note: Consider pruning the reflection cubemap array if empty slot count becomes too high
			}

			// Get skybox image-based lighting textures if needed/available
			if (mSkybox != nullptr && mSkyboxTexture != nullptr)
			{
				// If haven't assigned them already, do it now
				if (mSkyboxFilteredReflections == nullptr)
				{
					if (!LightProbeCache::instance().isRadianceDirty(mSkybox->getUUID()))
						mSkyboxFilteredReflections = LightProbeCache::instance().getCachedRadianceTexture(mSkybox->getUUID());
					else
					{
						mSkyboxFilteredReflections = Texture::create(cubemapDesc);

						IBLUtility::scaleCubemap(mSkyboxTexture, 0, mSkyboxFilteredReflections, 0);
						IBLUtility::filterCubemapForSpecular(mSkyboxFilteredReflections, scratchCubemap);
						LightProbeCache::instance().setCachedRadianceTexture(mSkybox->getUUID(), mSkyboxFilteredReflections);
					}
				}

				if(mSkyboxIrradiance == nullptr)
				{
					if (!LightProbeCache::instance().isIrradianceDirty(mSkybox->getUUID()))
						mSkyboxIrradiance = LightProbeCache::instance().getCachedIrradianceTexture(mSkybox->getUUID());
					else
					{
						TEXTURE_DESC irradianceCubemapDesc;
						irradianceCubemapDesc.type = TEX_TYPE_CUBE_MAP;
						irradianceCubemapDesc.format = PF_FLOAT_R11G11B10;
						irradianceCubemapDesc.width = IBLUtility::IRRADIANCE_CUBEMAP_SIZE;
						irradianceCubemapDesc.height = IBLUtility::IRRADIANCE_CUBEMAP_SIZE;
						irradianceCubemapDesc.numMips = 0;
						irradianceCubemapDesc.usage = TU_STATIC | TU_RENDERTARGET;

						mSkyboxIrradiance = Texture::create(irradianceCubemapDesc);

						IBLUtility::filterCubemapForIrradiance(mSkyboxFilteredReflections, mSkyboxIrradiance);
						LightProbeCache::instance().setCachedIrradianceTexture(mSkybox->getUUID(), mSkyboxFilteredReflections);
					}
				}
			}
			else
			{
				mSkyboxFilteredReflections = nullptr;
				mSkyboxIrradiance = nullptr;
			}

		}
		bs_frame_clear();
	}

	void RenderBeast::captureSceneCubeMap(const SPtr<Texture>& cubemap, const Vector3& position, bool hdr, const FrameInfo& frameInfo)
	{
		auto& texProps = cubemap->getProperties();

		Matrix4 projTransform = Matrix4::projectionPerspective(Degree(90.0f), 1.0f, 0.05f, 1000.0f);
		ConvexVolume localFrustum(projTransform);
		RenderAPI::instance().convertProjectionMatrix(projTransform, projTransform);

		RENDERER_VIEW_DESC viewDesc;
		viewDesc.target.clearFlags = FBT_COLOR | FBT_DEPTH;
		viewDesc.target.clearColor = Color::Black;
		viewDesc.target.clearDepthValue = 1.0f;
		viewDesc.target.clearStencilValue = 0;

		viewDesc.target.nrmViewRect = Rect2(0, 0, 1.0f, 1.0f);
		viewDesc.target.viewRect = Rect2I(0, 0, texProps.getWidth(), texProps.getHeight());
		viewDesc.target.targetWidth = texProps.getWidth();
		viewDesc.target.targetHeight = texProps.getHeight();
		viewDesc.target.numSamples = 1;

		viewDesc.isOverlay = false;
		viewDesc.isHDR = hdr;
		viewDesc.noLighting = false;
		viewDesc.triggerCallbacks = false;
		viewDesc.runPostProcessing = false;
		viewDesc.renderingReflections = true;

		viewDesc.visibleLayers = 0xFFFFFFFFFFFFFFFF;
		viewDesc.nearPlane = 0.5f;
		viewDesc.farPlane = 1000.0f;
		viewDesc.flipView = RenderAPI::instance().getAPIInfo().isFlagSet(RenderAPIFeatureFlag::UVYAxisUp);

		viewDesc.viewOrigin = position;
		viewDesc.projTransform = projTransform;

		viewDesc.stateReduction = mCoreOptions->stateReductionMode;
		viewDesc.sceneCamera = nullptr;

		Matrix4 viewOffsetMat = Matrix4::translation(-position);

		RendererCamera views[6];
		for(UINT32 i = 0; i < 6; i++)
		{
			// Calculate view matrix
			Matrix3 viewRotationMat;
			Vector3 forward;

			Vector3 up = Vector3::UNIT_Y;

			switch (i)
			{
			case CF_PositiveX:
				forward = Vector3::UNIT_X;
				break;
			case CF_NegativeX:
				forward = -Vector3::UNIT_X;
				break;
			case CF_PositiveY:
				forward = Vector3::UNIT_Y;
				up = -Vector3::UNIT_Z;
				break;
			case CF_NegativeY:
				forward = Vector3::UNIT_X;
				up = Vector3::UNIT_Z;
				break;
			case CF_PositiveZ:
				forward = Vector3::UNIT_Z;
				break;
			case CF_NegativeZ:
				forward = -Vector3::UNIT_Z;
				break;
			}

			Vector3 right = Vector3::cross(up, forward);
			viewRotationMat = Matrix3(right, up, forward);

			viewDesc.viewDirection = forward;
			viewDesc.viewTransform = Matrix4(viewRotationMat) * viewOffsetMat;

			// Calculate world frustum for culling
			const Vector<Plane>& frustumPlanes = localFrustum.getPlanes();
			Matrix4 worldMatrix = viewDesc.viewTransform.transpose();

			Vector<Plane> worldPlanes(frustumPlanes.size());
			UINT32 j = 0;
			for (auto& plane : frustumPlanes)
			{
				worldPlanes[j] = worldMatrix.multiplyAffine(plane);
				j++;
			}

			viewDesc.cullFrustum = ConvexVolume(worldPlanes);

			// Set up face render target
			RENDER_TEXTURE_DESC cubeFaceRTDesc;
			cubeFaceRTDesc.colorSurfaces[0].texture = cubemap;
			cubeFaceRTDesc.colorSurfaces[0].face = i;
			cubeFaceRTDesc.colorSurfaces[0].numFaces = 1;
			
			viewDesc.target.target = RenderTexture::create(cubeFaceRTDesc);

			views[i].setView(viewDesc);
			views[i].updatePerViewBuffer();
			views[i].determineVisible(mRenderables, mRenderableCullInfos);
		}

		RendererCamera* viewPtrs[] = { &views[0], &views[1], &views[2], &views[3], &views[4], &views[5] };
		renderViews(viewPtrs, 6, frameInfo);
	}

	void RenderBeast::refreshSamplerOverrides(bool force)
	{
		bool anyDirty = false;
		for (auto& entry : mSamplerOverrides)
		{
			SPtr<MaterialParams> materialParams = entry.first.material->_getInternalParams();

			MaterialSamplerOverrides* materialOverrides = entry.second;
			for(UINT32 i = 0; i < materialOverrides->numOverrides; i++)
			{
				SamplerOverride& override = materialOverrides->overrides[i];
				const MaterialParamsBase::ParamData* materialParamData = materialParams->getParamData(override.paramIdx);

				SPtr<SamplerState> samplerState;
				materialParams->getSamplerState(*materialParamData, samplerState);

				UINT64 hash = 0;
				if (samplerState != nullptr)
					hash = samplerState->getProperties().getHash();

				if (hash != override.originalStateHash || force)
				{
					if (samplerState != nullptr)
						override.state = SamplerOverrideUtility::generateSamplerOverride(samplerState, mCoreOptions);
					else
						override.state = SamplerOverrideUtility::generateSamplerOverride(SamplerState::getDefault(), mCoreOptions);

					override.originalStateHash = override.state->getProperties().getHash();
					materialOverrides->isDirty = true;
				}

				// Dirty flag can also be set externally, so check here even though we assign it above
				if (materialOverrides->isDirty)
					anyDirty = true;
			}
		}

		// Early exit if possible
		if (!anyDirty)
			return;

		UINT32 numRenderables = (UINT32)mRenderables.size();
		for (UINT32 i = 0; i < numRenderables; i++)
		{
			for(auto& element : mRenderables[i]->elements)
			{
				MaterialSamplerOverrides* overrides = element.samplerOverrides;
				if(overrides != nullptr && overrides->isDirty)
				{
					UINT32 numPasses = element.material->getNumPasses();
					for(UINT32 j = 0; j < numPasses; j++)
					{
						SPtr<GpuParams> params = element.params->getGpuParams(j);

						const UINT32 numStages = 6;
						for (UINT32 k = 0; k < numStages; k++)
						{
							GpuProgramType type = (GpuProgramType)k;

							SPtr<GpuParamDesc> paramDesc = params->getParamDesc(type);
							if (paramDesc == nullptr)
								continue;

							for (auto& samplerDesc : paramDesc->samplers)
							{
								UINT32 set = samplerDesc.second.set;
								UINT32 slot = samplerDesc.second.slot;

								UINT32 overrideIndex = overrides->passes[j].stateOverrides[set][slot];
								if (overrideIndex == (UINT32)-1)
									continue;

								params->setSamplerState(set, slot, overrides->overrides[overrideIndex].state);
							}
						}
					}
				}
			}
		}

		for (auto& entry : mSamplerOverrides)
			entry.second->isDirty = false;
	}
}}