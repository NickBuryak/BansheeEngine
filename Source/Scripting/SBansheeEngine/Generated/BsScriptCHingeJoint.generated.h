#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"
#include "BsScriptCJoint.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsHingeJoint.h"
#include "Math/BsRadian.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsJoint.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsHingeJoint.h"

namespace bs
{
	class CHingeJoint;
	struct __LimitAngularRangeInterop;

	class BS_SCR_BE_EXPORT ScriptCHingeJoint : public TScriptComponent<ScriptCHingeJoint, CHingeJoint, ScriptCJointBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "HingeJoint")

		ScriptCHingeJoint(MonoObject* managedInstance, const GameObjectHandle<CHingeJoint>& value);

	private:
		static void Internal_getAngle(ScriptCHingeJoint* thisPtr, Radian* __output);
		static float Internal_getSpeed(ScriptCHingeJoint* thisPtr);
		static void Internal_getLimit(ScriptCHingeJoint* thisPtr, __LimitAngularRangeInterop* __output);
		static void Internal_setLimit(ScriptCHingeJoint* thisPtr, __LimitAngularRangeInterop* limit);
		static void Internal_getDrive(ScriptCHingeJoint* thisPtr, HingeJointDrive* __output);
		static void Internal_setDrive(ScriptCHingeJoint* thisPtr, HingeJointDrive* drive);
		static void Internal_setFlag(ScriptCHingeJoint* thisPtr, HingeJointFlag flag, bool enabled);
		static bool Internal_hasFlag(ScriptCHingeJoint* thisPtr, HingeJointFlag flag);
	};
}
