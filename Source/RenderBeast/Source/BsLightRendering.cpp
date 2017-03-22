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
#include "BsReflectionProbeSampling.h"

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
		output.radius = mInternal->getBounds().getRadius();
		output.direction = mInternal->getRotation().zAxis();
		output.intensity = mInternal->getIntensity();
		output.spotAngles.x = spotAngle.valueRadians();
		output.spotAngles.y = Math::cos(output.spotAngles.x);
		output.spotAngles.z = 1.0f / (Math::cos(spotFalloffAngle) - output.spotAngles.y);
		output.radiusSqrdInv = 1.0f / (output.radius * output.radius);
		output.color = Vector3(color.r, color.g, color.b);
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

		// Sky
		params->getTextureParam(GPT_COMPUTE_PROGRAM, "gSkyReflectionTex", mSkyReflectionsParam);
		params->getTextureParam(GPT_COMPUTE_PROGRAM, "gSkyIrradianceTex", mSkyIrradianceParam);

		// Reflections
		params->getTextureParam(GPT_COMPUTE_PROGRAM, "gReflProbeCubemaps", mReflectionProbeCubemapsParam);
		params->getTextureParam(GPT_COMPUTE_PROGRAM, "gPreintegratedEnvBRDF", mPreintegratedEnvBRDFParam);

		params->getBufferParam(GPT_COMPUTE_PROGRAM, "gReflectionProbes", mReflectionProbesParam);

		mReflectionsParamBuffer = gReflProbeParamsParamDef.createBuffer();
		mParamsSet->setParamBlockBuffer("ReflProbeParams", mReflectionsParamBuffer);

		SAMPLER_STATE_DESC reflSamplerDesc;
		reflSamplerDesc.magFilter = FO_LINEAR;
		reflSamplerDesc.minFilter = FO_LINEAR;
		reflSamplerDesc.mipFilter = FO_LINEAR;

		mReflectionSamplerState = SamplerState::create(reflSamplerDesc);

		params->setSamplerState(GPT_COMPUTE_PROGRAM, "gSkyReflectionSamp", mReflectionSamplerState);
		params->setSamplerState(GPT_COMPUTE_PROGRAM, "gReflProbeSamp", mReflectionSamplerState);
	}

	void TiledDeferredLighting::execute(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBuffer>& perCamera,
										const SPtr<Texture>& preintegratedGF, bool noLighting)
	{
		Vector2I framebufferSize;
		framebufferSize[0] = gbuffer->getWidth();
		framebufferSize[1] = gbuffer->getHeight();
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
		mReflectionsParamBuffer->flushToGPU();

		mGBufferA.set(gbuffer->getTextureA());
		mGBufferB.set(gbuffer->getTextureB());
		mGBufferC.set(gbuffer->getTextureC());
		mGBufferDepth.set(gbuffer->getTextureDepth());

		mPreintegratedEnvBRDFParam.set(preintegratedGF);

		mParamsSet->setParamBlockBuffer("PerCamera", perCamera, true);

		const RenderAPIInfo& rapiInfo = RenderAPI::instance().getAPIInfo();
		if (mSampleCount > 1 && !rapiInfo.isFlagSet(RenderAPIFeatureFlag::MSAAImageStores))
		{
			SPtr<GpuBuffer> sceneColorBuffer = gbuffer->getFlattenedSceneColorBuffer();
			mOutputBufferParam.set(sceneColorBuffer);
		}
		else
		{
			SPtr<Texture> sceneColorTex = gbuffer->getSceneColor();
			mOutputTextureParam.set(sceneColorTex);
		}

		UINT32 width = gbuffer->getWidth();
		UINT32 height = gbuffer->getHeight();

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

	void TiledDeferredLighting::setReflectionProbes(const GPUReflProbeData& probeData,
													const SPtr<Texture>& reflectionCubemaps)
	{
		mReflectionProbesParam.set(probeData.getProbeBuffer());
		mReflectionProbeCubemapsParam.set(reflectionCubemaps);

		gReflProbeParamsParamDef.gNumProbes.set(mReflectionsParamBuffer, probeData.getNumProbes());

		UINT32 numMips = 0;
		if (reflectionCubemaps != nullptr)
			numMips = reflectionCubemaps->getProperties().getNumMipmaps() + 1;

		gReflProbeParamsParamDef.gReflCubemapNumMips.set(mReflectionsParamBuffer, numMips);
	}

	void TiledDeferredLighting::setSky(const SPtr<Texture>& skyReflections, const SPtr<Texture>& skyIrradiance)
	{
		mSkyReflectionsParam.set(skyReflections);
		mSkyIrradianceParam.set(skyIrradiance);

		UINT32 skyReflectionsAvailable = 0;
		UINT32 numMips = 0;
		if (skyReflections != nullptr)
		{
			numMips = skyReflections->getProperties().getNumMipmaps() + 1;
			skyReflectionsAvailable = 1;
		}

		gReflProbeParamsParamDef.gSkyCubemapNumMips.set(mReflectionsParamBuffer, numMips);
		gReflProbeParamsParamDef.gSkyCubemapAvailable.set(mReflectionsParamBuffer, skyReflectionsAvailable);
	}

	// Reverse bits functions used for Hammersley sequence
	float reverseBits(UINT32 bits)
	{
		bits = (bits << 16u) | (bits >> 16u);
		bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
		bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
		bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
		bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

		return (float)(double(bits) / (double)0x100000000LL);
	}

	void hammersleySequence(UINT32 i, UINT32 count, float& e0, float& e1)
	{
		e0 = i / (float)count;
		e1 = reverseBits(i);
	}

	Vector3 sphericalToCartesian(float cosTheta, float sinTheta, float phi)
	{
		Vector3 output;
		output.x = sinTheta * cos(phi);
		output.y = sinTheta * sin(phi);
		output.z = cosTheta;

		return output;
	}

	// Generates an angle in spherical coordinates, importance sampled for the specified roughness based on some uniformly
	// distributed random variables in range [0, 1].
	void importanceSampleGGX(float e0, float e1, float roughness4, float& cosTheta, float& phi)
	{
		// See GGXImportanceSample.nb for derivation (essentially, take base GGX, normalize it, generate PDF, split PDF into
		// marginal probability for theta and conditional probability for phi. Plug those into the CDF, invert it.)				
		cosTheta = sqrt((1.0f - e0) / (1.0f + (roughness4 - 1.0f) * e0));
		phi = 2.0f * Math::PI * e1;
	}

	float calcMicrofacetShadowingSmithGGX(float roughness4, float NoV, float NoL)
	{
		// Note: See lighting shader for derivation. Includes microfacet BRDF divisor.
		float g1V = NoV + sqrt(NoV * (NoV - NoV * roughness4) + roughness4);
		float g1L = NoL + sqrt(NoL * (NoL - NoL * roughness4) + roughness4);
		return 1.0f / (g1V * g1L);
	}

	SPtr<Texture> TiledDeferredLighting::generatePreintegratedEnvBRDF()
	{
		TEXTURE_DESC desc;
		desc.type = TEX_TYPE_2D;
		desc.format = PF_FLOAT16_RG;
		desc.width = 128;
		desc.height = 32;

		SPtr<Texture> texture = Texture::create(desc);
		PixelData pixelData = texture->lock(GBL_WRITE_ONLY_DISCARD);

		for (UINT32 y = 0; y < desc.height; y++)
		{
			float roughness = (float)(y + 0.5f) / desc.height;
			float m = roughness * roughness;
			float m2 = m*m;

			for (UINT32 x = 0; x < desc.width; x++)
			{
				float NoV = (float)(x + 0.5f) / desc.width;

				Vector3 V;
				V.x = sqrt(1.0f - NoV * NoV); // sine
				V.y = 0.0f;
				V.z = NoV;

				// These are the two integrals resulting from the second part of the split-sum approximation. Described in
				// Epic's 2013 paper:
				//    http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
				float scale = 0.0f;
				float offset = 0.0f;

				// We use the same importance sampling function we use for reflection cube importance sampling, only we
				// sample G and F, instead of D factors of the microfactet BRDF. See GGXImportanceSample.nb for derivation.
				constexpr UINT32 NumSamples = 128;
				for (UINT32 i = 0; i < NumSamples; i++)
				{
					float e0, e1;
					hammersleySequence(i, NumSamples, e0, e1);

					float cosTheta, phi;
					importanceSampleGGX(e0, e1, m2, cosTheta, phi);

					float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
					Vector3 H = sphericalToCartesian(cosTheta, sinTheta, phi);
					Vector3 L = 2.0f * Vector3::dot(V, H) * H - V;

					float VoH = std::max(Vector3::dot(V, H), 0.0f);
					float NoL = std::max(L.z, 0.0f); // N assumed (0, 0, 1)
					float NoH = std::max(H.z, 0.0f); // N assumed (0, 0, 1)

					// Set second part of the split sum integral is split into two parts:
					//   F0*I[G * (1 - (1 - v.h)^5) * cos(theta)] + I[G * (1 - v.h)^5 * cos(theta)] (F0 * scale + bias)

					// We calculate the fresnel scale (1 - (1 - v.h)^5) and bias ((1 - v.h)^5) parts
					float fc = pow(1.0f - VoH, 5.0f);
					float fresnelScale = 1.0f - fc;
					float fresnelOffset = fc;

					// We calculate the G part
					float G = calcMicrofacetShadowingSmithGGX(m2, NoV, NoL);

					// When we factor out G and F, then divide D by PDF, this is what's left
					//float pdfFactor = 1.0f / NoH;

					// Note: This is based on PDF: D * NoH / (4 * VoH), but I'm not sure where does the (4 * VoH) factor
					// come from. I'm keeping it since it seems to look better.
					float pdfFactor = 4.0f * VoH / NoH;

					if (NoL > 0.0f)
					{
						scale += NoL * pdfFactor * G * fresnelScale;
						offset += NoL * pdfFactor * G * fresnelOffset;
					}
				}

				scale /= NumSamples;
				offset /= NumSamples;

				Color color;
				color.r = Math::clamp01(scale);
				color.g = Math::clamp01(offset);

				pixelData.setColorAt(color, x, y);
			}
		}

		texture->unlock();

		return texture;
	}

	template<int MSAA_COUNT, bool FixedReflColor>
	TTiledDeferredLightingMat<MSAA_COUNT, FixedReflColor>::TTiledDeferredLightingMat()
		:mInternal(mMaterial, mParamsSet, MSAA_COUNT)
	{

	}

	template<int MSAA_COUNT, bool FixedReflColor>
	void TTiledDeferredLightingMat<MSAA_COUNT, FixedReflColor>::_initDefines(ShaderDefines& defines)
	{
		defines.set("TILE_SIZE", TiledDeferredLighting::TILE_SIZE);
		defines.set("MSAA_COUNT", MSAA_COUNT);
		defines.set("FIXED_REFLECTION_COLOR", FixedReflColor);
	}

	template<int MSAA_COUNT, bool FixedReflColor>
	void TTiledDeferredLightingMat<MSAA_COUNT, FixedReflColor>::execute(const SPtr<RenderTargets>& gbuffer,
		const SPtr<GpuParamBlockBuffer>& perCamera, const SPtr<Texture>& preintegratedGF, bool noLighting)
	{
		mInternal.execute(gbuffer, perCamera, preintegratedGF, noLighting);
	}

	template<int MSAA_COUNT, bool FixedReflColor>
	void TTiledDeferredLightingMat<MSAA_COUNT, FixedReflColor>::setLights(const GPULightData& lightData)
	{
		mInternal.setLights(lightData);
	}

	template<int MSAA_COUNT, bool FixedReflColor>
	void TTiledDeferredLightingMat<MSAA_COUNT, FixedReflColor>::setReflectionProbes(const GPUReflProbeData& probeData, 
		const SPtr<Texture>& reflectionCubemaps)
	{
		mInternal.setReflectionProbes(probeData, reflectionCubemaps);
	}

	template<int MSAA_COUNT, bool FixedReflColor>
	void TTiledDeferredLightingMat<MSAA_COUNT, FixedReflColor>::setSky(const SPtr<Texture>& skyReflections, 
		const SPtr<Texture>& skyIrradiance)
	{
		mInternal.setSky(skyReflections, skyIrradiance);
	}

	TiledDeferredLightingMaterials::TiledDeferredLightingMaterials()
	{
		mInstances[0] = bs_new<TTiledDeferredLightingMat<1, false>>();
		mInstances[1] = bs_new<TTiledDeferredLightingMat<2, false>>();
		mInstances[2] = bs_new<TTiledDeferredLightingMat<4, false>>();
		mInstances[3] = bs_new<TTiledDeferredLightingMat<8, false>>();

		mInstances[4] = bs_new<TTiledDeferredLightingMat<1, true>>();
		mInstances[5] = bs_new<TTiledDeferredLightingMat<2, true>>();
		mInstances[6] = bs_new<TTiledDeferredLightingMat<4, true>>();
		mInstances[7] = bs_new<TTiledDeferredLightingMat<8, true>>();
	}

	TiledDeferredLightingMaterials::~TiledDeferredLightingMaterials()
	{
		for (UINT32 i = 0; i < 8; i++)
			bs_delete(mInstances[i]);
	}

	ITiledDeferredLightingMat* TiledDeferredLightingMaterials::get(UINT32 msaa, bool fixedReflColor)
	{
		if (!fixedReflColor)
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
		else
		{
			if (msaa == 1)
				return mInstances[4];
			else if (msaa == 2)
				return mInstances[5];
			else if (msaa == 4)
				return mInstances[6];
			else
				return mInstances[7];
		}
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