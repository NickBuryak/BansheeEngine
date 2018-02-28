//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "Physics/BsCollider.h"
#include "Math/BsVector3.h"
#include "Math/BsQuaternion.h"

namespace bs
{
	/** @addtogroup Physics
	 *  @{
	 */

	/** A collider with sphere geometry. */
	class BS_CORE_EXPORT SphereCollider : public Collider
	{
	public:
		SphereCollider();

		/** Determines the radius of the sphere geometry. */
		virtual void setRadius(float radius) = 0;

		/** @copydoc setRadius */
		virtual float getRadius() const = 0;

		/**
		 * Creates a new sphere collider.
		 * 
		 * @param[in]	radius		Radius of the sphere geometry.
		 * @param[in]	position	Position of the collider.
		 * @param[in]	rotation	Rotation of the collider.
		 */
		static SPtr<SphereCollider> create(float radius = 0.0f, const Vector3& position = Vector3::ZERO, 
			const Quaternion& rotation = Quaternion::IDENTITY);
	};

	/** @} */
}