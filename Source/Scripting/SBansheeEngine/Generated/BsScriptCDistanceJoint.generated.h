#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"
#include "BsScriptCJoint.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsJoint.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsDistanceJoint.h"

namespace bs
{
	class CDistanceJoint;

	class BS_SCR_BE_EXPORT ScriptCDistanceJoint : public TScriptComponent<ScriptCDistanceJoint, CDistanceJoint, ScriptCJointBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "DistanceJoint")

		ScriptCDistanceJoint(MonoObject* managedInstance, const GameObjectHandle<CDistanceJoint>& value);

	private:
		static float Internal_getDistance(ScriptCDistanceJoint* thisPtr);
		static float Internal_getMinDistance(ScriptCDistanceJoint* thisPtr);
		static void Internal_setMinDistance(ScriptCDistanceJoint* thisPtr, float value);
		static float Internal_getMaxDistance(ScriptCDistanceJoint* thisPtr);
		static void Internal_setMaxDistance(ScriptCDistanceJoint* thisPtr, float value);
		static float Internal_getTolerance(ScriptCDistanceJoint* thisPtr);
		static void Internal_setTolerance(ScriptCDistanceJoint* thisPtr, float value);
		static void Internal_getSpring(ScriptCDistanceJoint* thisPtr, Spring* __output);
		static void Internal_setSpring(ScriptCDistanceJoint* thisPtr, Spring* value);
		static void Internal_setFlag(ScriptCDistanceJoint* thisPtr, DistanceJointFlag flag, bool enabled);
		static bool Internal_hasFlag(ScriptCDistanceJoint* thisPtr, DistanceJointFlag flag);
	};
}
