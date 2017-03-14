//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"

namespace bs
{
	/** @addtogroup Implementation
	 *  @{
	 */

	/** Base class for both core and sim thread implementations of a skybox. */
	class BS_CORE_EXPORT SkyboxBase
	{
	public:
        SkyboxBase();
		virtual ~SkyboxBase() { }
        
		/**	Checks whether the skybox should be used or not. */
		bool getIsActive() const { return mIsActive; }

		/**	Sets whether the skybox should be used or not. */
		void setIsActive(bool active) { mIsActive = active; _markCoreDirty(); }

		/** Returns an identifier that uniquely identifies the skybox. */
		const String& getUUID() const { return mUUID; }

		/** 
		 * Marks the simulation thread object as dirty and notifies the system its data should be synced with its core 
		 * thread counterpart. 
		 */
		virtual void _markCoreDirty() { }

	protected:
		String mUUID; /**< Identifier that uniquely identifies the skybox. */
		bool mIsActive; /**< Determines whether the skybox should be rendered or not. */
	};

	/** Templated base class for both core and sim thread implementations of a skybox. */
	template<bool Core>
	class BS_CORE_EXPORT TSkybox : public SkyboxBase
	{
		typedef typename TTextureType<Core>::Type TextureType;

	public:
        TSkybox();
		virtual ~TSkybox() { }

		/** 
		 * Assigns an environment map to use for sampling skybox radiance. Must be a cube-map texture, and should ideally
		 * contain HDR data.
		 */
		void setTexture(const TextureType& texture) { mTexture = texture; _markCoreDirty(); }

		/** Gets the texture assigned through setTexture(). */
		TextureType getTexture() const { return mTexture; }

	protected:
		TextureType mTexture;
	};

	/** @} */
	/** @addtogroup Renderer-Engine-Internal
	 *  @{
	 */

	namespace ct { class Skybox; }

	/** Allows you to specify an environment map to use for sampling radiance of the sky. */
	class BS_CORE_EXPORT Skybox : public IReflectable, public CoreObject, public TSkybox<false>
	{
	public:
		/**	Retrieves an implementation of the skybox usable only from the core thread. */
		SPtr<ct::Skybox> getCore() const;

		/** Creates a new skybox. */
		static SPtr<Skybox> create();

	protected:
        Skybox();

		/** @copydoc CoreObject::createCore */
		SPtr<ct::CoreObject> createCore() const override;

		/** @copydoc SkyboxBase::_markCoreDirty */
		void _markCoreDirty() override;

		/** @copydoc CoreObject::syncToCore */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SkyboxRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	namespace ct
	{
	/** Core thread usable version of a bs::Skybox */
	class BS_CORE_EXPORT Skybox : public CoreObject, public TSkybox<true>
	{
	public:
		~Skybox();

	protected:
		friend class bs::Skybox;

        Skybox();

		/** @copydoc CoreObject::initialize */
		void initialize() override;

		/** @copydoc CoreObject::syncToCore */
		void syncToCore(const CoreSyncData& data) override;
	};
	}

	/** @} */
}