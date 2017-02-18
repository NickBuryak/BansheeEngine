//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptManagedComponent.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptObjectManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptSceneObject.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedComponent.h"
#include "BsSceneObject.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptManagedComponent::ScriptManagedComponent(MonoObject* instance)
		:ScriptObject(instance), mTypeMissing(false)
	{
		assert(instance != nullptr);

		MonoUtil::getClassName(instance, mNamespace, mType);
	}

	void ScriptManagedComponent::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Invoke", &ScriptManagedComponent::internal_invoke);
	}

	void ScriptManagedComponent::internal_invoke(ScriptManagedComponent* nativeInstance, MonoString* name)
	{
		HManagedComponent comp = nativeInstance->mComponent;
		if (checkIfDestroyed(nativeInstance->mComponent))
			return;

		MonoObject* compObj = comp->getManagedInstance();
		MonoClass* compClass = comp->getClass();

		bool found = false;
		String methodName = MonoUtil::monoToString(name);
		while (compClass != nullptr)
		{
			MonoMethod* method = compClass->getMethod(methodName);
			if (method != nullptr)
			{
				method->invoke(compObj, nullptr);
				found = true;
				break;
			}

			// Search for methods on base class if there is one
			MonoClass* baseClass = compClass->getBaseClass();
			if (baseClass != metaData.scriptClass)
				compClass = baseClass;
			else
				break;
		}

		if (!found)
		{
			LOGWRN("Method invoke failed. Cannot find method \"" + methodName + "\" on component of type \"" +
				   compClass->getTypeName() + "\".");
		}
	}

	MonoObject* ScriptManagedComponent::_createManagedInstance(bool construct)
	{
		SPtr<ManagedSerializableObjectInfo> currentObjInfo = nullptr;

		// See if this type even still exists
		if (!ScriptAssemblyManager::instance().getSerializableObjectInfo(mNamespace, mType, currentObjInfo))
		{
			mTypeMissing = true;
			return ScriptAssemblyManager::instance().getMissingComponentClass()->createInstance(true);
		}

		mTypeMissing = false;
		return currentObjInfo->mMonoClass->createInstance(construct);
	}

	ScriptObjectBackup ScriptManagedComponent::beginRefresh()
	{
		ScriptGameObjectBase::beginRefresh();

		HManagedComponent managedComponent = static_object_cast<ManagedComponent>(mComponent);
		ScriptObjectBackup backupData;

		// It's possible that managed component is destroyed but a reference to it
		// is still kept. Don't backup such components.
		if (!managedComponent.isDestroyed(true))
			backupData.data = managedComponent->backup(true);

		return backupData;
	}

	void ScriptManagedComponent::endRefresh(const ScriptObjectBackup& backupData)
	{
		HManagedComponent managedComponent = static_object_cast<ManagedComponent>(mComponent);

		ComponentBackupData componentBackup = any_cast<ComponentBackupData>(backupData.data);
		managedComponent->restore(mManagedInstance, componentBackup, mTypeMissing);

		ScriptGameObjectBase::endRefresh(backupData);
	}

	void ScriptManagedComponent::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		// It's possible that managed component is destroyed but a reference to it
		// is still kept during assembly refresh. Such components shouldn't be restored
		// so we delete them.
		if (!mRefreshInProgress || mComponent.isDestroyed(true))
			ScriptGameObjectManager::instance().destroyScriptComponent(this);
	}
}