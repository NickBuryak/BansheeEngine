//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRendererMaterial.h"
#include "BsParamBlocks.h"

namespace bs { namespace ct
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(PerLightParamDef)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightPositionAndType)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightColorAndIntensity)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightSpotAnglesAndSqrdInvRadius)
		BS_PARAM_BLOCK_ENTRY(Vector3, gLightDirection)
		BS_PARAM_BLOCK_ENTRY(Vector4, gLightGeometry)
		BS_PARAM_BLOCK_ENTRY(Matrix4, gMatConeTransform)
	BS_PARAM_BLOCK_END

	extern PerLightParamDef gPerLightParamDef;

	/** Manipulates parameters used in various light rendering shaders. */
	class LightRenderingParams
	{
	public:
		LightRenderingParams(const SPtr<Material>& lightMaterial, const SPtr<GpuParamsSet>& paramsSet);

		/** Updates parameters that are common for all lights. */
		void setStaticParameters(const SPtr<RenderTargets>& gbuffer,
			const SPtr<GpuParamBlockBuffer>& perCamera);

		/** Updates data in the parameter buffer from the data in the provided light. */
		void setParameters(const Light* light);

		/** Returns the internal parameter buffer that can be bound to the pipeline. */
		const SPtr<GpuParamBlockBuffer>& getBuffer() const;
	private:
		SPtr<Material> mMaterial;
		SPtr<GpuParamsSet> mParamsSet;

		GpuParamTexture mGBufferA;
		GpuParamTexture mGBufferB;
		GpuParamTexture mGBufferDepth;
		SPtr<GpuParamBlockBuffer> mParamBuffer;
	};

	/** Shader that renders directional light sources during deferred rendering light pass. */
	class DirectionalLightMat : public RendererMaterial<DirectionalLightMat>
	{
		RMAT_DEF("DeferredDirectionalLightPass.bsl");

	public:
		DirectionalLightMat();

		/** Binds the material for rendering and sets up any global parameters. */
		void bind(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBuffer>& perCamera);

		/** Updates the per-light buffers used by the material. */
		void setPerLightParams(const Light* light);
	private:
		LightRenderingParams mParams;
	};

	/** 
	 * Shader that renders point (radial & spot) light sources during deferred rendering light pass. Used when the camera
	 * is inside the point light geometry.
	 */
	class PointLightInMat : public RendererMaterial<PointLightInMat>
	{
		RMAT_DEF("DeferredPointLightPassIn.bsl");

	public:
		PointLightInMat();

		/** Binds the material for rendering and sets up any global parameters. */
		void bind(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBuffer>& perCamera);

		/** Updates the per-light buffers used by the material. */
		void setPerLightParams(const Light* light);
	private:
		LightRenderingParams mParams;
	};

	/** 
	 * Shader that renders point (radial & spot) light sources during deferred rendering light pass. Used when the camera
	 * is outside the point light geometry.
	 */
	class PointLightOutMat : public RendererMaterial<PointLightOutMat>
	{
		RMAT_DEF("DeferredPointLightPassOut.bsl");

	public:
		PointLightOutMat();

		/** Binds the material for rendering and sets up any global parameters. */
		void bind(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBuffer>& perCamera);

		/** Updates the per-light buffers used by the material. */
		void setPerLightParams(const Light* light);
	private:
		LightRenderingParams mParams;
	};

	/** @} */
}}