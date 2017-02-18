//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRenderable.h"
#include "BsBounds.h"
#include "BsComponent.h"

namespace bs
{
	/** @addtogroup Components
	 *  @{
	 */

	/**
	 * @copydoc	Renderable
	 * 			
	 * Wraps a TRenderable as a Component.
	 */
	class BS_CORE_EXPORT CRenderable : public Component
	{
	public:
		/** @copydoc Renderable::setMesh */
		void setMesh(HMesh mesh);

		/** @copydoc Renderable::setMaterial */
		void setMaterial(UINT32 idx, HMaterial material) { mInternal->setMaterial(idx, material); }

		/** @copydoc Renderable::setMaterial */
		void setMaterial(HMaterial material) { mInternal->setMaterial(material); }

		/** @copydoc Renderable::setLayer */
		void setLayer(UINT64 layer) { mInternal->setLayer(layer); }

		/** @copydoc Renderable::getLayer */
		UINT64 getLayer() const { return mInternal->getLayer(); }

		/** @copydoc Renderable::getMesh */
		HMesh getMesh() const { return mInternal->getMesh(); }

		/** @copydoc Renderable::getMaterial */
		HMaterial getMaterial(UINT32 idx) const { return mInternal->getMaterial(idx); }

		/**	Gets world bounds of the mesh rendered by this object. */
		Bounds getBounds() const;

		/** @copydoc Component::calculateBounds */
		bool calculateBounds(Bounds& bounds) override;

		/** @name Internal 
		 *  @{
		 */

		/** Returns the internal renderable that is used for majority of operations by this component. */
		SPtr<Renderable> _getRenderable() const { return mInternal; }

		/** Registers an Animation component that will be used for animating the renderable's mesh. */
		void _registerAnimation(const HAnimation& animation);

		/** Removes the Animation component, making the renderable rendered as a static object. */
		void _unregisterAnimation();

		/** @} */

	private:
		mutable SPtr<Renderable> mInternal;
		HAnimation mAnimation;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		CRenderable(const HSceneObject& parent);

		/** @copydoc Component::onInitialized */
		void onInitialized() override;

		/** @copydoc Component::onDestroyed */
		void onDestroyed() override;

	public:
		/** @copydoc Component::update */
		void update() override;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CRenderableRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	protected:
		CRenderable(); // Serialization only
	};

	/** @} */
}