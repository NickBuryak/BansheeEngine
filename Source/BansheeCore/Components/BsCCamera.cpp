//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Components/BsCCamera.h"
#include "RTTI/BsCCameraRTTI.h"
#include "Scene/BsSceneObject.h"
#include "Scene/BsSceneManager.h"
#include "BsCoreApplication.h"

namespace bs 
{
	CCamera::CCamera()
	{
		setFlag(ComponentFlag::AlwaysRun, true);
		setName("Camera");
	}

	CCamera::CCamera(const HSceneObject& parent, SPtr<RenderTarget> target, float left, float top, float width, float height)
		: Component(parent), mTarget(target), mLeft(left), mTop(top), mWidth(width), mHeight(height)
	{
		if(mTarget == nullptr)
			mTarget = CoreApplication::instance().getPrimaryWindow();

		setFlag(ComponentFlag::AlwaysRun, true);
		setName("Camera");
	}

	CCamera::~CCamera()
	{
		mInternal->destroy();
	}

	ConvexVolume CCamera::getWorldFrustum() const
	{
		const Vector<Plane>& frustumPlanes = getFrustum().getPlanes();
		Matrix4 worldMatrix = SO()->getWorldMatrix();

		Vector<Plane> worldPlanes(frustumPlanes.size());
		UINT32 i = 0;
		for (auto& plane : frustumPlanes)
		{
			worldPlanes[i] = worldMatrix.multiplyAffine(plane);
			i++;
		}

		return ConvexVolume(worldPlanes);
	}

	void CCamera::updateView() const
	{
		mInternal->_updateState(*SO());
	}

	void CCamera::setMain(bool main)
	{
		mInternal->setMain(main);

		gSceneManager()._notifyMainCameraStateChanged(mInternal);
	}

	void CCamera::update() 
	{

	}

	void CCamera::onInitialized()
	{
		// If mInternal already exists this means this object was deserialized,
		// so all we need to do is initialize it.
		if (mInternal != nullptr)
			mInternal->initialize();
		else
		{
			mInternal = Camera::create(mTarget, mLeft, mTop, mWidth, mHeight);
			mTarget = nullptr;
		}

		gSceneManager()._bindActor(mInternal, SO());
	}

	void CCamera::onDestroyed()
	{
		gSceneManager()._unbindActor(mInternal);
	}

	RTTITypeBase* CCamera::getRTTIStatic()
	{
		return CCameraRTTI::instance();
	}

	RTTITypeBase* CCamera::getRTTI() const
	{
		return CCamera::getRTTIStatic();
	}
} 
