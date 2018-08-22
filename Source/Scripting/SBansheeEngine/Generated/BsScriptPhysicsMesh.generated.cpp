#include "BsScriptPhysicsMesh.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsPhysicsMesh.h"
#include "BsScriptResourceManager.h"
#include "Wrappers/BsScriptRRefBase.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsPhysicsMesh.h"
#include "BsScriptRendererMeshData.generated.h"
#include "../../SBansheeEngine/Extensions/BsPhysicsMeshEx.h"

namespace bs
{
	ScriptPhysicsMesh::ScriptPhysicsMesh(MonoObject* managedInstance, const ResourceHandle<PhysicsMesh>& value)
		:TScriptResource(managedInstance, value)
	{
	}

	void ScriptPhysicsMesh::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetRef", (void*)&ScriptPhysicsMesh::Internal_getRef);
		metaData.scriptClass->addInternalCall("Internal_getType", (void*)&ScriptPhysicsMesh::Internal_getType);
		metaData.scriptClass->addInternalCall("Internal_create", (void*)&ScriptPhysicsMesh::Internal_create);
		metaData.scriptClass->addInternalCall("Internal_getMeshData", (void*)&ScriptPhysicsMesh::Internal_getMeshData);

	}

	 MonoObject*ScriptPhysicsMesh::createInstance()
	{
		bool dummy = false;
		void* ctorParams[1] = { &dummy };

		return metaData.scriptClass->createInstance("bool", ctorParams);
	}
	MonoObject* ScriptPhysicsMesh::Internal_getRef(ScriptPhysicsMesh* thisPtr)
	{
		return thisPtr->getRRef();
	}

	PhysicsMeshType ScriptPhysicsMesh::Internal_getType(ScriptPhysicsMesh* thisPtr)
	{
		PhysicsMeshType tmp__output;
		tmp__output = thisPtr->getHandle()->getType();

		PhysicsMeshType __output;
		__output = tmp__output;

		return __output;
	}

	void ScriptPhysicsMesh::Internal_create(MonoObject* managedInstance, MonoObject* meshData, PhysicsMeshType type)
	{
		SPtr<RendererMeshData> tmpmeshData;
		ScriptRendererMeshData* scriptmeshData;
		scriptmeshData = ScriptRendererMeshData::toNative(meshData);
		tmpmeshData = scriptmeshData->getInternal();
		ResourceHandle<PhysicsMesh> instance = PhysicsMeshEx::create(tmpmeshData, type);
		ScriptResourceManager::instance().createBuiltinScriptResource(instance, managedInstance);
	}

	MonoObject* ScriptPhysicsMesh::Internal_getMeshData(ScriptPhysicsMesh* thisPtr)
	{
		SPtr<RendererMeshData> tmp__output;
		tmp__output = PhysicsMeshEx::getMeshData(thisPtr->getHandle());

		MonoObject* __output;
		__output = ScriptRendererMeshData::create(tmp__output);

		return __output;
	}
}
