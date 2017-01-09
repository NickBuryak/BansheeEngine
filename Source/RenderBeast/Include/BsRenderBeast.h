//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRenderer.h"
#include "BsBounds.h"
#include "BsSamplerOverrides.h"
#include "BsRendererMaterial.h"
#include "BsLightRendering.h"
#include "BsObjectRendering.h"
#include "BsPostProcessing.h"
#include "BsRendererCamera.h"
#include "BsRendererObject.h"

namespace bs 
{ 
	struct RendererAnimationData;

	namespace ct
	{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	/** Semantics that may be used for signaling the renderer for what is a certain shader parameter used for. */
	static StringID RPS_GBufferA = "GBufferA";
	static StringID RPS_GBufferB = "GBufferB";
	static StringID RPS_GBufferDepth = "GBufferDepth";
	static StringID RPS_BoneMatrices = "BoneMatrices";

	/**
	 * Default renderer for Banshee. Performs frustum culling, sorting and renders objects in custom ways determine by
	 * renderable handlers.
	 *
	 * @note	Sim thread unless otherwise noted.
	 */
	class RenderBeast : public Renderer
	{
		/** Renderer information specific to a single frame. */
		struct RendererFrame
		{
			RendererFrame(float delta, const RendererAnimationData& animData);

			float delta;
			const RendererAnimationData& animData;
		};

		/**	Renderer information specific to a single render target. */
		struct RendererRenderTarget
		{
			SPtr<RenderTarget> target;
			Vector<const Camera*> cameras;
		};

		/**	Renderer information specific to a single light. */
		struct RendererLight
		{
			Light* internal;
		};

		/** Renderer information for a single material. */
		struct RendererMaterial
		{
			Vector<SPtr<GpuParamsSet>> params;
			UINT32 matVersion;
		};

	public:
		RenderBeast();
		~RenderBeast() { }

		/** @copydoc Renderer::getName */
		const StringID& getName() const override;

		/** @copydoc Renderer::renderAll */
		void renderAll() override;

		/**	Sets options used for controlling the rendering. */
		void setOptions(const SPtr<CoreRendererOptions>& options) override;

		/**	Returns current set of options used for controlling the rendering. */
		SPtr<CoreRendererOptions> getOptions() const override;

		/** @copydoc Renderer::initialize */
		void initialize() override;

		/** @copydoc Renderer::destroy */
		void destroy() override;

		/** @copydoc Renderer::createPostProcessSettings */
		SPtr<PostProcessSettings> createPostProcessSettings() const override;

	private:
		/** @copydoc Renderer::notifyCameraAdded */
		void notifyCameraAdded(const Camera* camera) override;

		/** @copydoc Renderer::notifyCameraUpdated */
		void notifyCameraUpdated(const Camera* camera, UINT32 updateFlag) override;

		/** @copydocRenderer::notifyCameraRemoved */
		void notifyCameraRemoved(const Camera* camera) override;

		/** @copydoc Renderer::notifyLightAdded */
		void notifyLightAdded(Light* light) override;

		/** @copydoc Renderer::notifyLightUpdated */
		void notifyLightUpdated(Light* light) override;

		/** @copydoc Renderer::notifyLightRemoved */
		void notifyLightRemoved(Light* light) override;

		/** @copydoc Renderer::notifyRenderableAdded */
		void notifyRenderableAdded(Renderable* renderable) override;

		/** @copydoc Renderer::notifyRenderableUpdated */
		void notifyRenderableUpdated(Renderable* renderable) override;

		/** @copydoc Renderer::notifyRenderableRemoved */
		void notifyRenderableRemoved(Renderable* renderable) override;

		/** 
		 * Updates (or adds) renderer specific data for the specified camera. Should be called whenever camera properties
		 * change. 
		 *
		 * @param[in]	camera		Camera whose data to update.
		 * @param[in]	forceRemove	If true, the camera data will be removed instead of updated.
		 * @return					Renderer camera object that represents the camera. Null if camera was removed.
		 */
		RendererCamera* updateCameraData(const Camera* camera, bool forceRemove = false);

		/**
		 * Updates the render options on the core thread.
		 *
		 * @note	Core thread only.
		 */
		void syncOptions(const RenderBeastOptions& options);

		/**
		 * Performs rendering over all camera proxies.
		 *
		 * @param[in]	time	Current frame time in milliseconds.
		 * @param[in]	delta	Time elapsed since the last frame.
		 *
		 * @note	Core thread only.
		 */
		void renderAllCore(float time, float delta);

		/**
		 * Renders all objects visible by the provided camera.
		 *
		 * @param[in]	frameInfo	Renderer information specific to this frame.
		 * @param[in]	rtInfo		Render target information containing the camera to render.
		 * @param[in]	camIdx		Index of the camera to render.
		 * 					
		 * @note	Core thread only.
		 */
		void render(const RendererFrame& frameInfo, RendererRenderTarget& rtInfo, UINT32 camIdx);

		/**
		 * Renders all overlay callbacks attached to the provided camera.
		 *
		 * @param[in]	frameInfo	Renderer information specific to this frame.
		 * @param[in]	rtInfo		Render target information containing the camera to render.
		 * @param[in]	camIdx		Index of the camera to render.
		 * 					
		 * @note	Core thread only.
		 */
		void renderOverlay(const RendererFrame& frameInfo, RendererRenderTarget& rtInfo, UINT32 camIdx);

		/** 
		 * Renders a single element of a renderable object. 
		 *
		 * @param[in]	element		Element to render.
		 * @param[in]	passIdx		Index of the material pass to render the element with.
		 * @param[in]	bindPass	If true the material pass will be bound for rendering, if false it is assumed it is
		 *							already bound.
		 * @param[in]	frameInfo	Renderer information specific to this frame.
		 * @param[in]	viewProj	View projection matrix of the camera the element is being rendered with.
		 */
		void renderElement(const BeastRenderableElement& element, UINT32 passIdx, bool bindPass, 
			const RendererFrame& frameInfo, const Matrix4& viewProj);

		/**	Creates data used by the renderer on the core thread. */
		void initializeCore();

		/**	Destroys data used by the renderer on the core thread. */
		void destroyCore();

		/**
		 * Checks all sampler overrides in case material sampler states changed, and updates them.
		 *
		 * @param[in]	force	If true, all sampler overrides will be updated, regardless of a change in the material
		 *						was detected or not.
		 */
		void refreshSamplerOverrides(bool force = false);

		// Core thread only fields
		Vector<RendererRenderTarget> mRenderTargets;
		UnorderedMap<const Camera*, RendererCamera*> mCameras;
		UnorderedMap<SamplerOverrideKey, MaterialSamplerOverrides*> mSamplerOverrides;

		Vector<RendererObject*> mRenderables;
		Vector<Bounds> mWorldBounds;
		Vector<bool> mVisibility; // Transient

		Vector<RendererLight> mDirectionalLights;
		Vector<RendererLight> mPointLights;
		Vector<Sphere> mLightWorldBounds;

		SPtr<RenderBeastOptions> mCoreOptions;

		DefaultMaterial* mDefaultMaterial;
		PointLightInMat* mPointLightInMat;
		PointLightOutMat* mPointLightOutMat;
		DirectionalLightMat* mDirLightMat;
		SkyboxMat* mSkyboxMat;

		ObjectRenderer* mObjectRenderer;

		// Sim thread only fields
		SPtr<RenderBeastOptions> mOptions;
		bool mOptionsDirty;
	};

	/** @} */
}}