//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRendererMaterial.h"
#include "BsParamBlocks.h"

namespace bs { namespace ct
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(ReflectionCubeDownsampleParamDef)
		BS_PARAM_BLOCK_ENTRY(int, gCubeFace)
	BS_PARAM_BLOCK_END

	extern ReflectionCubeDownsampleParamDef gReflectionCubeDownsampleParamDef;

	/** Performs filtering on cubemap faces in order to prepare them for importance sampling. */
	class ReflectionCubeDownsampleMat : public RendererMaterial<ReflectionCubeDownsampleMat>
	{
		RMAT_DEF("ReflectionCubeDownsample.bsl")

	public:
		ReflectionCubeDownsampleMat();

		/** Downsamples the provided texture face and outputs it to the provided target. */
		void execute(const SPtr<Texture>& source, UINT32 face, const TextureSurface& surface, 
					 const SPtr<RenderTarget>& target);

	private:
		SPtr<GpuParamBlockBuffer> mParamBuffer;
		GpuParamTexture mInputTexture;
	};

	BS_PARAM_BLOCK_BEGIN(ReflectionCubeImportanceSampleParamDef)
		BS_PARAM_BLOCK_ENTRY(int, gCubeFace)
		BS_PARAM_BLOCK_ENTRY(int, gMipLevel)
		BS_PARAM_BLOCK_ENTRY(int, gNumMips)
		BS_PARAM_BLOCK_ENTRY(float, gPrecomputedMipFactor)
	BS_PARAM_BLOCK_END

	extern ReflectionCubeImportanceSampleParamDef gReflectionCubeImportanceSampleParamDef;

	/** Performs importance sampling on cubemap faces in order for make them suitable for specular evaluation. */
	class ReflectionCubeImportanceSampleMat : public RendererMaterial<ReflectionCubeImportanceSampleMat>
	{
		RMAT_DEF("ReflectionCubeImportanceSample.bsl")

	public:
		ReflectionCubeImportanceSampleMat();

		/** Importance samples the provided texture face and outputs it to the provided target. */
		void execute(const SPtr<Texture>& source, UINT32 face, UINT32 mip, const SPtr<RenderTarget>& target);

	private:
		static const UINT32 NUM_SAMPLES;

		SPtr<GpuParamBlockBuffer> mParamBuffer;
		GpuParamTexture mInputTexture;
	};

	/** Vector representing spherical harmonic coefficients for 5 bands. */
	struct SHVector5
	{
		SHVector5()
			:coeffs()
		{ }

		float coeffs[25];
	};

	/** Vector representing spherical coefficients for 5 bands, separate for red, green and blue components. */
	struct SHVector5RGB
	{
		SHVector5 R, G, B;
	};

	/** Intermediate structure used for spherical coefficient calculation. Contains RGB coefficients and weight. */
	struct SHCoeffsAndWeight
	{
		SHVector5RGB coeffs;
		float weight;
	};

	BS_PARAM_BLOCK_BEGIN(IrradianceComputeSHParamDef)
		BS_PARAM_BLOCK_ENTRY(int, gCubeFace)
		BS_PARAM_BLOCK_ENTRY(int, gFaceSize)
		BS_PARAM_BLOCK_ENTRY(Vector2I, gDispatchSize)
	BS_PARAM_BLOCK_END

	extern IrradianceComputeSHParamDef gIrradianceComputeSHParamDef;

	/** Computes spherical harmonic coefficients from a radiance cubemap. */
	class IrradianceComputeSHMat : public RendererMaterial<IrradianceComputeSHMat>
	{
		RMAT_DEF("IrradianceComputeSH.bsl")

	public:
		IrradianceComputeSHMat();

		/** 
		 * Computes spherical harmonic coefficients from a radiance texture and outputs a buffer containing a list of
		 * coefficient sets (one set of coefficients for each thread group). Coefficients must be reduced and normalized
		 * by IrradianceReduceSHMat before use. Output buffer should be created by calling createOutputBuffer().
		 */
		void execute(const SPtr<Texture>& source, UINT32 face, const SPtr<GpuBuffer>& output);

		/** Creates a buffer of adequate size to be used as output for this material. */
		static SPtr<GpuBuffer> createOutputBuffer(const SPtr<Texture>& source, UINT32& numCoeffSets);

	private:
		static const UINT32 NUM_SAMPLES;

		SPtr<GpuParamBlockBuffer> mParamBuffer;
		GpuParamTexture mInputTexture;
		GpuParamBuffer mOutputBuffer;
	};

	BS_PARAM_BLOCK_BEGIN(IrradianceReduceSHParamDef)
		BS_PARAM_BLOCK_ENTRY(int, gNumEntries)
	BS_PARAM_BLOCK_END

	extern IrradianceReduceSHParamDef gIrradianceReduceSHParamDef;

	/** 
	 * Sums spherical harmonic coefficients calculated by each thread group of IrradianceComputeSHMat and outputs a single
	 * set of normalized coefficients. 
	 */
	class IrradianceReduceSHMat : public RendererMaterial<IrradianceReduceSHMat>
	{
		RMAT_DEF("IrradianceReduceSH.bsl")

	public:
		IrradianceReduceSHMat();

		/** 
		 * Sums spherical harmonic coefficients calculated by each thread group of IrradianceComputeSHMat and outputs a
		 * single set of normalized coefficients. Output buffer should be created by calling createOutputBuffer().
		 */
		void execute(const SPtr<GpuBuffer>& source, UINT32 numCoeffSets, const SPtr<GpuBuffer>& output);

		/** Creates a buffer of adequate size to be used as output for this material. */
		static SPtr<GpuBuffer> createOutputBuffer();

	private:
		static const UINT32 NUM_SAMPLES;

		SPtr<GpuParamBlockBuffer> mParamBuffer;
		GpuParamBuffer mInputBuffer;
		GpuParamBuffer mOutputBuffer;
	};

	BS_PARAM_BLOCK_BEGIN(IrradianceProjectSHParamDef)
		BS_PARAM_BLOCK_ENTRY(int, gCubeFace)
	BS_PARAM_BLOCK_END

	extern IrradianceProjectSHParamDef gIrradianceProjectSHParamDef;

	/** 
	 * Projects spherical harmonic coefficients calculated by IrradianceReduceSHMat and projects them onto faces of
	 * a cubemap.
	 */
	class IrradianceProjectSHMat : public RendererMaterial<IrradianceProjectSHMat>
	{
		RMAT_DEF("IrradianceProjectSH.bsl")

	public:
		IrradianceProjectSHMat();

		/** 
		 * Projects spherical harmonic coefficients calculated by IrradianceReduceSHMat and projects them onto faces of
		 * a cubemap.
		 */
		void execute(const SPtr<GpuBuffer>& shCoeffs, UINT32 face, const SPtr<RenderTarget>& target);

	private:
		static const UINT32 NUM_SAMPLES;

		SPtr<GpuParamBlockBuffer> mParamBuffer;
		GpuParamBuffer mInputBuffer;
	};

	/** Helper class that handles generation and processing of textures used for image based lighting. */
	class BS_EXPORT IBLUtility
	{
	public:
		/**
		 * Performs filtering on the cubemap, populating its mip-maps with filtered values that can be used for
		 * evaluating specular reflections.
		 * 
		 * @param[in, out]	cubemap		Cubemap to filter. Its mip level 0 will be read, filtered and written into
		 *								other mip levels.
		 * @param[in]		scratch		Temporary cubemap texture to use for the filtering process. Must match the size of
		 *								the source cubemap. Provide null to automatically create a scratch cubemap.
		 */
		static void filterCubemapForSpecular(const SPtr<Texture>& cubemap, const SPtr<Texture>& scratch);

		/**
		 * Performs filtering on the cubemap, populating the output cubemap with values that can be used for evaluating
		 * irradiance for use in diffuse lighting.
		 * 
		 * @param[in]		cubemap		Cubemap to filter. Its mip level 0 will be used as source.
		 * @param[in]		output		Output cubemap to store the irradiance data in.
		 */
		static void filterCubemapForIrradiance(const SPtr<Texture>& cubemap, const SPtr<Texture>& output);

		/**
		 * Scales a cubemap and outputs it in the destination texture, using hardware acceleration. If both textures are the
		 * same size, performs a copy instead.
		 *
		 * @param[in]   src				Source cubemap to scale.
		 * @param[in]   srcMip			Determines which mip level of the source texture to scale.
		 * @param[in]   dst				Desination texture to output the scaled data to. Must be usable as a render target.
		 * @param[in]   dstMip			Determines which mip level of the destination texture to scale.
		 */
		static void scaleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst, UINT32 dstMip);

		static const UINT32 REFLECTION_CUBEMAP_SIZE;
		static const UINT32 IRRADIANCE_CUBEMAP_SIZE;
	private:
		/** 
		 * Downsamples a cubemap using hardware bilinear filtering. 
		 * 
		 * @param[in]	src		Cubemap to downsample.
		 * @param[in]   srcMip	Determines which mip level of the source texture to downsample.
		 * @param[in]   dst		Desination texture to output the scaled data to. Must be usable as a render target.
		 * @param[in]   dstMip			Determines which mip level of the destination texture to scale.
		 */
		static void downsampleCubemap(const SPtr<Texture>& src, UINT32 srcMip, const SPtr<Texture>& dst, UINT32 dstMip);
	};

	/** @} */
}}
