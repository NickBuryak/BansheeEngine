#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsPhysicsCommon.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsPhysicsCommon.h"
#include "BsScriptContactPoint.generated.h"

namespace bs
{
	struct __CollisionDataInterop
	{
		MonoArray* collider;
		MonoArray* contactPoints;
	};

	class BS_SCR_BE_EXPORT ScriptCollisionData : public ScriptObject<ScriptCollisionData>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "CollisionData")

		static MonoObject* box(const __CollisionDataInterop& value);
		static __CollisionDataInterop unbox(MonoObject* value);
		static CollisionData fromInterop(const __CollisionDataInterop& value);
		static __CollisionDataInterop toInterop(const CollisionData& value);

	private:
		ScriptCollisionData(MonoObject* managedInstance);

	};
}
