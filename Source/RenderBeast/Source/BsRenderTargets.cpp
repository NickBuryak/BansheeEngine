//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsRenderTargets.h"
#include "BsRenderTexturePool.h"
#include "BsViewport.h"
#include "BsRenderAPI.h"
#include "BsTextureManager.h"
#include "BsRendererUtility.h"

namespace bs { namespace ct
{
	RenderTargets::RenderTargets(const RENDERER_VIEW_TARGET_DESC& view, bool hdr)
		:mViewTarget(view), mHDR(hdr), mWidth(view.targetWidth), mHeight(view.targetHeight)
	{
		// Note: Consider customizable HDR format via options? e.g. smaller PF_FLOAT_R11G11B10 or larger 32-bit format
		mSceneColorFormat = hdr ? PF_FLOAT16_RGBA : PF_R8G8B8A8;
		mAlbedoFormat = PF_R8G8B8A8; // Note: Also consider customizable format (e.g. 16-bit float?)
		mNormalFormat = PF_UNORM_R10G10B10A2; // Note: Also consider customizable format (e.g. 16-bit float?)
	}

	SPtr<RenderTargets> RenderTargets::create(const RENDERER_VIEW_TARGET_DESC& view, bool hdr)
	{
		return bs_shared_ptr<RenderTargets>(new (bs_alloc<RenderTargets>()) RenderTargets(view, hdr));
	}

	void RenderTargets::allocate()
	{
		RenderTexturePool& texPool = RenderTexturePool::instance();

		UINT32 width = mViewTarget.viewRect.width;
		UINT32 height = mViewTarget.viewRect.height;

		// Note: Albedo is allocated as SRGB, meaning when reading from textures during depth pass we decode from sRGB into linear,
		// then back into sRGB when writing to albedo, and back to linear when reading from albedo during light pass. This /might/ have
		// a performance impact. In which case we could just use a higher precision albedo buffer, which can then store linear color
		// directly (storing linear in 8bit buffer causes too much detail to be lost in the blacks).
		SPtr<PooledRenderTexture> newColorRT = texPool.get(POOLED_RENDER_TEXTURE_DESC::create2D(mSceneColorFormat, width, 
			height, TU_RENDERTARGET | TU_LOADSTORE, mViewTarget.numSamples, false));
		SPtr<PooledRenderTexture> newAlbedoRT = texPool.get(POOLED_RENDER_TEXTURE_DESC::create2D(mAlbedoFormat, width, 
			height, TU_RENDERTARGET, mViewTarget.numSamples, true));
		SPtr<PooledRenderTexture> newNormalRT = texPool.get(POOLED_RENDER_TEXTURE_DESC::create2D(mNormalFormat, width, 
			height, TU_RENDERTARGET, mViewTarget.numSamples, false));
		SPtr<PooledRenderTexture> newDepthRT = texPool.get(POOLED_RENDER_TEXTURE_DESC::create2D(PF_D32_S8X24, width, height, 
			TU_DEPTHSTENCIL, mViewTarget.numSamples, false));

		bool rebuildTargets = newColorRT != mSceneColorTex || newAlbedoRT != mAlbedoTex || newNormalRT != mNormalTex || newDepthRT != mDepthTex;

		mSceneColorTex = newColorRT;
		mAlbedoTex = newAlbedoRT;
		mNormalTex = newNormalRT;
		mDepthTex = newDepthRT;

		if (mGBufferRT == nullptr || mSceneColorRT == nullptr || rebuildTargets)
		{
			RENDER_TEXTURE_DESC gbufferDesc;
			gbufferDesc.colorSurfaces[0].texture = mSceneColorTex->texture;
			gbufferDesc.colorSurfaces[0].face = 0;
			gbufferDesc.colorSurfaces[0].numFaces = 1;
			gbufferDesc.colorSurfaces[0].mipLevel = 0;

			gbufferDesc.colorSurfaces[1].texture = mAlbedoTex->texture;
			gbufferDesc.colorSurfaces[1].face = 0;
			gbufferDesc.colorSurfaces[1].numFaces = 1;
			gbufferDesc.colorSurfaces[1].mipLevel = 0;

			gbufferDesc.colorSurfaces[2].texture = mNormalTex->texture;
			gbufferDesc.colorSurfaces[2].face = 0;
			gbufferDesc.colorSurfaces[2].numFaces = 1;
			gbufferDesc.colorSurfaces[2].mipLevel = 0;

			gbufferDesc.depthStencilSurface.texture = mDepthTex->texture;
			gbufferDesc.depthStencilSurface.face = 0;
			gbufferDesc.depthStencilSurface.mipLevel = 0;

			mGBufferRT = RenderTexture::create(gbufferDesc);

			RENDER_TEXTURE_DESC sceneColorDesc;
			sceneColorDesc.colorSurfaces[0].texture = mSceneColorTex->texture;
			sceneColorDesc.colorSurfaces[0].face = 0;
			sceneColorDesc.colorSurfaces[0].numFaces = 1;
			sceneColorDesc.colorSurfaces[0].mipLevel = 0;

			sceneColorDesc.depthStencilSurface.texture = mDepthTex->texture;
			sceneColorDesc.depthStencilSurface.face = 0;
			sceneColorDesc.depthStencilSurface.numFaces = 1;
			sceneColorDesc.depthStencilSurface.mipLevel = 0;

			mSceneColorRT = TextureManager::instance().createRenderTexture(sceneColorDesc);
		}
	}

	void RenderTargets::release()
	{
		RenderAPI& rapi = RenderAPI::instance();
		rapi.setRenderTarget(nullptr);

		RenderTexturePool& texPool = RenderTexturePool::instance();

		texPool.release(mSceneColorTex);
		texPool.release(mAlbedoTex);
		texPool.release(mNormalTex);
		texPool.release(mDepthTex);
	}

	void RenderTargets::bindGBuffer()
	{
		RenderAPI& rapi = RenderAPI::instance();
		rapi.setRenderTarget(mGBufferRT);

		Rect2 area(0.0f, 0.0f, 1.0f, 1.0f);
		rapi.setViewport(area);

		// Clear scene color, depth, stencil according to user defined values
		UINT32 clearFlags = mViewTarget.clearFlags;
		if (clearFlags != 0)
		{
			RenderAPI::instance().clearViewport(clearFlags, mViewTarget.clearColor,
												mViewTarget.clearDepthValue, mViewTarget.clearStencilValue, 0x01);
		}

		// Clear all others
		RenderAPI::instance().clearViewport(FBT_COLOR, Color::ZERO, 1.0f, 0, 0xFF & ~0x01);
	}

	void RenderTargets::bindSceneColor(bool readOnlyDepthStencil)
	{
		RenderAPI& rapi = RenderAPI::instance();
		rapi.setRenderTarget(mSceneColorRT, readOnlyDepthStencil, RT_COLOR0 | RT_DEPTH);

		Rect2 area(0.0f, 0.0f, 1.0f, 1.0f);
		rapi.setViewport(area);
	}

	SPtr<Texture> RenderTargets::getSceneColor() const
	{
		return mSceneColorTex->texture;
	}

	SPtr<Texture> RenderTargets::getTextureA() const
	{
		return mAlbedoTex->texture;
	}

	SPtr<Texture> RenderTargets::getTextureB() const
	{
		return mNormalTex->texture;
	}

	SPtr<Texture> RenderTargets::getTextureDepth() const
	{
		return mDepthTex->texture;
	}
}}