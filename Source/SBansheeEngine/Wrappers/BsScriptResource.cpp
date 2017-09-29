//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/BsScriptResource.h"
#include "BsScriptResourceManager.h"
#include "Resources/BsResource.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptResourceBase::ScriptResourceBase(MonoObject* instance)
		:PersistentScriptObjectBase(instance), mRefreshInProgress(false)
	{ }

	ScriptObjectBackup ScriptResourceBase::beginRefresh()
	{
		mRefreshInProgress = true;

		return PersistentScriptObjectBase::beginRefresh();
	}

	void ScriptResourceBase::endRefresh(const ScriptObjectBackup& backupData)
	{
		mRefreshInProgress = false;

		PersistentScriptObjectBase::endRefresh(backupData);
	}

	void ScriptResourceBase::destroy()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptResource::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetName", (void*)&ScriptResource::internal_getName);
		metaData.scriptClass->addInternalCall("Internal_GetUUID", (void*)&ScriptResource::internal_getUUID);
		metaData.scriptClass->addInternalCall("Internal_Release", (void*)&ScriptResource::internal_release);
	}

	MonoString* ScriptResource::internal_getName(ScriptResourceBase* nativeInstance)
	{
		return MonoUtil::wstringToMono(nativeInstance->getGenericHandle()->getName());
	}

	MonoString* ScriptResource::internal_getUUID(ScriptResourceBase* nativeInstance)
	{
		return MonoUtil::stringToMono(nativeInstance->getGenericHandle().getUUID());
	}

	void ScriptResource::internal_release(ScriptResourceBase* nativeInstance)
	{
		nativeInstance->getGenericHandle().release();
	}
}