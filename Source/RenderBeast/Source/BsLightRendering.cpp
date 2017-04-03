//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsLightRendering.h"
#include "BsMaterial.h"
#include "BsShader.h"
#include "BsRenderBeast.h"
#include "BsRenderTargets.h"
#include "BsGpuParams.h"
#include "BsGpuParamsSet.h"
#include "BsGpuBuffer.h"
#include "BsLight.h"
#include "BsRendererUtility.h"

namespace bs { namespace ct
{
	static const UINT32 BUFFER_INCREMENT = 16 * sizeof(LightData);

	TiledLightingParamDef gTiledLightingParamDef;

	RendererLight::RendererLight(Light* light)
		:mInternal(light)
	{ }

	void RendererLight::getParameters(LightData& output) const
	{
		Radian spotAngle = Math::clamp(mInternal->getSpotAngle() * 0.5f, Degree(0), Degree(89));
		Radian spotFalloffAngle = Math::clamp(mInternal->getSpotFalloffAngle() * 0.5f, Degree(0), (Degree)spotAngle);
		Color color = mInternal->getColor();

		output.position = mInternal->getPosition();
		output.attRadius = mInternal->getBounds().getRadius();
		output.srcRadius = mInternal->getSourceRadius();
		output.direction = mInternal->getRotation().zAxis();
		output.luminance = mInternal->getLuminance();
		output.spotAngles.x = spotAngle.valueRadians();
		output.spotAngles.y = Math::cos(output.spotAngles.x);
		output.spotAngles.z = 1.0f / std::max(Math::cos(spotFalloffAngle) - output.spotAngles.y, 0.001f);
		output.attRadiusSqrdInv = 1.0f / (output.attRadius * output.attRadius);
		output.color = Vector3(color.r, color.g, color.b);

		// If directional lights, convert angular radius in degrees to radians
		if (mInternal->getType() == LightType::Directional)
			output.srcRadius *= Math::DEG2RAD;

		// Create position for fake attenuation for area spot lights (with disc center)
		if (mInternal->getType() == LightType::Spot)
			output.shiftedLightPosition = output.position - output.direction * (output.srcRadius / Math::tan(spotAngle * 0.5f));
		else
			output.shiftedLightPosition = output.position;
	}

	GPULightData::GPULightData()
		:mNumLights{}
	{ }

	void GPULightData::setLights(const Vector<LightData>& lightData, UINT32 numDirLights, UINT32 numRadialLights,
									UINT32 numSpotLights)
	{
		mNumLights[0] = numDirLights;
		mNumLights[1] = numRadialLights;
		mNumLights[2] = numSpotLights;

		Vector3I lightOffsets;
		lightOffsets[0] = numDirLights;
		lightOffsets[1] = lightOffsets[0] + numRadialLights;
		lightOffsets[2] = lightOffsets[1] + numSpotLights;

		UINT32 totalNumLights = (UINT32)lightOffsets[2];

		UINT32 size = totalNumLights * sizeof(LightData);
		UINT32 curBufferSize;

		if (mLightBuffer != nullptr)
			curBufferSize = mLightBuffer->getSize();
		else
			curBufferSize = 0;

		if (size > curBufferSize || curBufferSize == 0)
		{
			// Allocate at least one block even if no lights, to avoid issues with null buffers
			UINT32 bufferSize = std::max(1, Math::ceilToInt(size / (float)BUFFER_INCREMENT)) * BUFFER_INCREMENT;

			GPU_BUFFER_DESC bufferDesc;
			bufferDesc.type = GBT_STRUCTURED;
			bufferDesc.elementCount = bufferSize / sizeof(LightData);
			bufferDesc.elementSize = sizeof(LightData);
			bufferDesc.format = BF_UNKNOWN;

			mLightBuffer = GpuBuffer::create(bufferDesc);
		}

		if (size > 0)
			mLightBuffer->writeData(0, size, lightData.data(), BWT_DISCARD);
	}

	const UINT32 TiledDeferredLighting::TILE_SIZE = 16;

	TiledDeferredLighting::TiledDeferredLighting(const SPtr<Material>& material, const SPtr<GpuParamsSet>& paramsSet,
													UINT32 sampleCount)
		:mSampleCount(sampleCount), mMaterial(material), mParamsSet(paramsSet), mLightOffsets()
	{
		SPtr<GpuParams> params = mParamsSet->getGpuParams();

		auto& texParams = mMaterial->getShader()->getTextureParams();
		for (auto& entry : texParams)
		{
			if (entry.second.rendererSemantic == RPS_GBufferA)
				params->getTextureParam(GPT_COMPUTE_PROGRAM, entry.second.name, mGBufferA);
			else if (entry.second.rendererSemantic == RPS_GBufferB)
				params->getTextureParam(GPT_COMPUTE_PROGRAM, entry.second.name, mGBufferB);
			else if (entry.second.rendererSemantic == RPS_GBufferC)
				params->getTextureParam(GPT_COMPUTE_PROGRAM, entry.second.name, mGBufferC);
			else if (entry.second.rendererSemantic == RPS_GBufferDepth)
				params->getTextureParam(GPT_COMPUTE_PROGRAM, entry.second.name, mGBufferDepth);
		}

		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gLights", mLightBufferParam);

		if (params->hasLoadStoreTexture(GPT_COMPUTE_PROGRAM, "gOutput"))
			params->getLoadStoreTextureParam(GPT_COMPUTE_PROGRAM, "gOutput", mOutputTextureParam);

		if (params->hasBuffer(GPT_COMPUTE_PROGRAM, "gOutput"))
			params->getBufferParam(GPT_COMPUTE_PROGRAM, "gOutput", mOutputBufferParam);

		mParamBuffer = gTiledLightingParamDef.createBuffer();
		mParamsSet->setParamBlockBuffer("Params", mParamBuffer, true);
	}

	void TiledDeferredLighting::execute(const SPtr<RenderTargets>& renderTargets, const SPtr<GpuParamBlockBuffer>& perCamera,
		bool noLighting)
	{
		Vector2I framebufferSize;
		framebufferSize[0] = renderTargets->getWidth();
		framebufferSize[1] = renderTargets->getHeight();
		gTiledLightingParamDef.gFramebufferSize.set(mParamBuffer, framebufferSize);

		if (noLighting)
		{
			Vector3I lightOffsets;
			lightOffsets[0] = 0;
			lightOffsets[1] = 0;
			lightOffsets[2] = 0;

			gTiledLightingParamDef.gLightOffsets.set(mParamBuffer, lightOffsets);
		}
		else
		{
			gTiledLightingParamDef.gLightOffsets.set(mParamBuffer, mLightOffsets);
		}
		mParamBuffer->flushToGPU();

		mGBufferA.set(renderTargets->getGBufferA());
		mGBufferB.set(renderTargets->getGBufferB());
		mGBufferC.set(renderTargets->getGBufferC());
		mGBufferDepth.set(renderTargets->getSceneDepth());

		mParamsSet->setParamBlockBuffer("PerCamera", perCamera, true);

		if (mSampleCount > 1)
		{
			SPtr<GpuBuffer> lightAccumulation = renderTargets->getLightAccumulationBuffer();
			mOutputBufferParam.set(lightAccumulation);
		}
		else
		{
			SPtr<Texture> lightAccumulation = renderTargets->getLightAccumulation();
			mOutputTextureParam.set(lightAccumulation);
		}

		UINT32 width = renderTargets->getWidth();
		UINT32 height = renderTargets->getHeight();

		UINT32 numTilesX = (UINT32)Math::ceilToInt(width / (float)TILE_SIZE);
		UINT32 numTilesY = (UINT32)Math::ceilToInt(height / (float)TILE_SIZE);

		gRendererUtility().setComputePass(mMaterial, 0);
		gRendererUtility().setPassParams(mParamsSet);

		RenderAPI::instance().dispatchCompute(numTilesX, numTilesY);
	}

	void TiledDeferredLighting::setLights(const GPULightData& lightData)
	{
		mLightBufferParam.set(lightData.getLightBuffer());

		mLightOffsets[0] = lightData.getNumDirLights();
		mLightOffsets[1] = mLightOffsets[0] + lightData.getNumRadialLights();
		mLightOffsets[2] = mLightOffsets[1] + lightData.getNumSpotLights();
	}

	template<int MSAA_COUNT>
	TTiledDeferredLightingMat<MSAA_COUNT>::TTiledDeferredLightingMat()
		:mInternal(mMaterial, mParamsSet, MSAA_COUNT)
	{

	}

	template<int MSAA_COUNT>
	void TTiledDeferredLightingMat<MSAA_COUNT>::_initDefines(ShaderDefines& defines)
	{
		defines.set("TILE_SIZE", TiledDeferredLighting::TILE_SIZE);
		defines.set("MSAA_COUNT", MSAA_COUNT);
	}

	template<int MSAA_COUNT>
	void TTiledDeferredLightingMat<MSAA_COUNT>::execute(const SPtr<RenderTargets>& gbuffer,
		const SPtr<GpuParamBlockBuffer>& perCamera, bool noLighting)
	{
		mInternal.execute(gbuffer, perCamera, noLighting);
	}

	template<int MSAA_COUNT>
	void TTiledDeferredLightingMat<MSAA_COUNT>::setLights(const GPULightData& lightData)
	{
		mInternal.setLights(lightData);
	}

	TiledDeferredLightingMaterials::TiledDeferredLightingMaterials()
	{
		mInstances[0] = bs_new<TTiledDeferredLightingMat<1>>();
		mInstances[1] = bs_new<TTiledDeferredLightingMat<2>>();
		mInstances[2] = bs_new<TTiledDeferredLightingMat<4>>();
		mInstances[3] = bs_new<TTiledDeferredLightingMat<8>>();
	}

	TiledDeferredLightingMaterials::~TiledDeferredLightingMaterials()
	{
		for (UINT32 i = 0; i < 4; i++)
			bs_delete(mInstances[i]);
	}

	ITiledDeferredLightingMat* TiledDeferredLightingMaterials::get(UINT32 msaa)
	{
		if (msaa == 1)
			return mInstances[0];
		else if (msaa == 2)
			return mInstances[1];
		else if (msaa == 4)
			return mInstances[2];
		else
			return mInstances[3];
	}

	FlatFramebufferToTextureParamDef gFlatFramebufferToTextureParamDef;

	FlatFramebufferToTextureMat::FlatFramebufferToTextureMat()
	{
		SPtr<GpuParams> params = mParamsSet->getGpuParams();
		params->getBufferParam(GPT_FRAGMENT_PROGRAM, "gInput", mInputParam);

		mParamBuffer = gTiledLightingParamDef.createBuffer();
		mParamsSet->setParamBlockBuffer("Params", mParamBuffer, true);
	}

	void FlatFramebufferToTextureMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void FlatFramebufferToTextureMat::execute(const SPtr<GpuBuffer>& flatFramebuffer, const SPtr<Texture>& target)
	{
		const TextureProperties& props = target->getProperties();

		Vector2I framebufferSize;
		framebufferSize[0] = props.getWidth();
		framebufferSize[1] = props.getHeight();
		gFlatFramebufferToTextureParamDef.gFramebufferSize.set(mParamBuffer, framebufferSize);

		gFlatFramebufferToTextureParamDef.gSampleCount.set(mParamBuffer, props.getNumSamples());

		mParamBuffer->flushToGPU();

		mInputParam.set(flatFramebuffer);

		gRendererUtility().setPass(mMaterial, 0);
		gRendererUtility().setPassParams(mParamsSet);

		Rect2 area(0.0f, 0.0f, (float)props.getWidth(), (float)props.getHeight());
		gRendererUtility().drawScreenQuad(area);
	}
}}