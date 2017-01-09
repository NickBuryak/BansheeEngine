//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsLightRendering.h"
#include "BsMaterial.h"
#include "BsShader.h"
#include "BsRenderBeast.h"
#include "BsRenderTargets.h"
#include "BsGpuParams.h"
#include "BsGpuParamsSet.h"
#include "BsLight.h"
#include "BsRendererUtility.h"

namespace bs { namespace ct
{
	PerLightParamDef gPerLightParamDef;

	LightRenderingParams::LightRenderingParams(const SPtr<Material>& material, const SPtr<GpuParamsSet>& paramsSet)
		:mMaterial(material), mParamsSet(paramsSet)
	{
		SPtr<GpuParams> params = mParamsSet->getGpuParams();

		auto& texParams = material->getShader()->getTextureParams();
		for (auto& entry : texParams)
		{
			if (entry.second.rendererSemantic == RPS_GBufferA)
				params->getTextureParam(GPT_FRAGMENT_PROGRAM, entry.second.name, mGBufferA);
			else if (entry.second.rendererSemantic == RPS_GBufferB)
				params->getTextureParam(GPT_FRAGMENT_PROGRAM, entry.second.name, mGBufferB);
			else if (entry.second.rendererSemantic == RPS_GBufferDepth)
				params->getTextureParam(GPT_FRAGMENT_PROGRAM, entry.second.name, mGBufferDepth);
		}

		mParamBuffer = gPerLightParamDef.createBuffer();
		mParamsSet->setParamBlockBuffer("PerLight", mParamBuffer, true);
	}

	void LightRenderingParams::setStaticParameters(const SPtr<RenderTargets>& gbuffer, 
		const SPtr<GpuParamBlockBuffer>& perCamera)
	{
		mGBufferA.set(gbuffer->getTextureA());
		mGBufferB.set(gbuffer->getTextureB());
		mGBufferDepth.set(gbuffer->getTextureDepth());

		mParamsSet->setParamBlockBuffer("PerCamera", perCamera, true);

		gRendererUtility().setPassParams(mParamsSet);
	}

	void LightRenderingParams::setParameters(const Light* light)
	{
		// Note: I could just copy the data directly to the parameter buffer if I ensured the parameter
		// layout matches

		Vector4 positionAndType = (Vector4)light->getPosition();

		switch (light->getType())
		{
		case LightType::Directional:
			positionAndType.w = 0;
			break;
		case LightType::Point:
			positionAndType.w = 0.3f;
			break;
		case LightType::Spot:
			positionAndType.w = 0.8f;
			break;
		}

		gPerLightParamDef.gLightPositionAndType.set(mParamBuffer, positionAndType);
			
		Vector4 colorAndIntensity;
		colorAndIntensity.x = light->getColor().r;
		colorAndIntensity.y = light->getColor().g;
		colorAndIntensity.z = light->getColor().b;
		colorAndIntensity.w = light->getIntensity();

		gPerLightParamDef.gLightColorAndIntensity.set(mParamBuffer, colorAndIntensity);

		Radian spotAngle = Math::clamp(light->getSpotAngle() * 0.5f, Degree(1), Degree(90));
		Radian spotFalloffAngle = Math::clamp(light->getSpotFalloffAngle() * 0.5f, Degree(1), (Degree)spotAngle);

		Vector4 spotAnglesAndInvSqrdRadius;
		spotAnglesAndInvSqrdRadius.x = spotAngle.valueRadians();
		spotAnglesAndInvSqrdRadius.y = Math::cos(spotAnglesAndInvSqrdRadius.x);
		spotAnglesAndInvSqrdRadius.z = 1.0f / (Math::cos(spotFalloffAngle) - spotAnglesAndInvSqrdRadius.y);
		spotAnglesAndInvSqrdRadius.w = 1.0f / (light->getBounds().getRadius() * light->getBounds().getRadius());

		gPerLightParamDef.gLightSpotAnglesAndSqrdInvRadius.set(mParamBuffer, spotAnglesAndInvSqrdRadius);

		gPerLightParamDef.gLightDirection.set(mParamBuffer, -light->getRotation().zAxis());

		Vector4 lightGeometry;
		lightGeometry.x = light->getType() == LightType::Spot ? (float)Light::LIGHT_CONE_NUM_SIDES : 0;
		lightGeometry.y = (float)Light::LIGHT_CONE_NUM_SLICES;
		lightGeometry.z = light->getBounds().getRadius();

		float coneRadius = Math::sin(spotAngle) * light->getRange();
		lightGeometry.w = coneRadius;

		gPerLightParamDef.gLightGeometry.set(mParamBuffer, lightGeometry);

		Matrix4 transform = Matrix4::TRS(light->getPosition(), light->getRotation(), Vector3::ONE);
		gPerLightParamDef.gMatConeTransform.set(mParamBuffer, transform);

		mParamBuffer->flushToGPU();
	}

	const SPtr<GpuParamBlockBuffer>& LightRenderingParams::getBuffer() const
	{
		return mParamBuffer;
	}
	
	DirectionalLightMat::DirectionalLightMat()
		:mParams(mMaterial, mParamsSet)
	{
		
	}

	void DirectionalLightMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void DirectionalLightMat::bind(const SPtr<RenderTargets>& gbuffer,
		const SPtr<GpuParamBlockBuffer>& perCamera)
	{
		RendererUtility::instance().setPass(mMaterial, 0);
		mParams.setStaticParameters(gbuffer, perCamera);
	}

	void DirectionalLightMat::setPerLightParams(const Light* light)
	{
		mParams.setParameters(light);
	}

	PointLightInMat::PointLightInMat()
		:mParams(mMaterial, mParamsSet)
	{

	}

	void PointLightInMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void PointLightInMat::bind(const SPtr<RenderTargets>& gbuffer, 
		const SPtr<GpuParamBlockBuffer>& perCamera)
	{
		RendererUtility::instance().setPass(mMaterial, 0);
		mParams.setStaticParameters(gbuffer, perCamera);
	}

	void PointLightInMat::setPerLightParams(const Light* light)
	{
		mParams.setParameters(light);
	}

	PointLightOutMat::PointLightOutMat()
		:mParams(mMaterial, mParamsSet)
	{
		
	}

	void PointLightOutMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void PointLightOutMat::bind(const SPtr<RenderTargets>& gbuffer,
		const SPtr<GpuParamBlockBuffer>& perCamera)
	{
		RendererUtility::instance().setPass(mMaterial, 0);
		mParams.setStaticParameters(gbuffer, perCamera);
	}

	void PointLightOutMat::setPerLightParams(const Light* light)
	{
		mParams.setParameters(light);
	}
}}