#include "BsScriptControllerControllerCollision.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsScriptGameObjectManager.h"
#include "Wrappers/BsScriptVector.h"
#include "BsScriptCCharacterController.generated.h"
#include "Wrappers/BsScriptVector.h"

namespace bs
{
	ScriptControllerControllerCollision::ScriptControllerControllerCollision(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptControllerControllerCollision::initRuntimeData()
	{ }

	MonoObject*ScriptControllerControllerCollision::box(const __ControllerControllerCollisionInterop& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	__ControllerControllerCollisionInterop ScriptControllerControllerCollision::unbox(MonoObject* value)
	{
		return *(__ControllerControllerCollisionInterop*)MonoUtil::unbox(value);
	}

	ControllerControllerCollision ScriptControllerControllerCollision::fromInterop(const __ControllerControllerCollisionInterop& value)
	{
		ControllerControllerCollision output;
		GameObjectHandle<CCharacterController> tmpcontroller;
		ScriptCCharacterController* scriptcontroller;
		scriptcontroller = ScriptCCharacterController::toNative(value.controller);
		if(scriptcontroller != nullptr)
			tmpcontroller = scriptcontroller->getHandle();
		output.controller = tmpcontroller;
		output.position = value.position;
		output.normal = value.normal;
		output.motionDir = value.motionDir;
		output.motionAmount = value.motionAmount;

		return output;
	}

	__ControllerControllerCollisionInterop ScriptControllerControllerCollision::toInterop(const ControllerControllerCollision& value)
	{
		__ControllerControllerCollisionInterop output;
		ScriptComponentBase* scriptcontroller = nullptr;
		if(value.controller)
				scriptcontroller = ScriptGameObjectManager::instance().getBuiltinScriptComponent(value.controller);
		MonoObject* tmpcontroller;
		if(scriptcontroller != nullptr)
			tmpcontroller = scriptcontroller->getManagedInstance();
		else
			tmpcontroller = nullptr;
		output.controller = tmpcontroller;
		output.position = value.position;
		output.normal = value.normal;
		output.motionDir = value.motionDir;
		output.motionAmount = value.motionAmount;

		return output;
	}

}
