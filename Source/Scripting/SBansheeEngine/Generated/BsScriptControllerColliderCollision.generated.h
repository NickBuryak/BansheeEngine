#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsCharacterController.h"
#include "Math/BsVector3.h"

namespace bs
{
	struct __ControllerColliderCollisionInterop
	{
		MonoObject* collider;
		uint32_t triangleIndex;
		Vector3 position;
		Vector3 normal;
		Vector3 motionDir;
		float motionAmount;
	};

	class BS_SCR_BE_EXPORT ScriptControllerColliderCollision : public ScriptObject<ScriptControllerColliderCollision>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ControllerColliderCollision")

		static MonoObject* box(const __ControllerColliderCollisionInterop& value);
		static __ControllerColliderCollisionInterop unbox(MonoObject* value);
		static ControllerColliderCollision fromInterop(const __ControllerColliderCollisionInterop& value);
		static __ControllerColliderCollisionInterop toInterop(const ControllerColliderCollision& value);

	private:
		ScriptControllerColliderCollision(MonoObject* managedInstance);

	};
}
