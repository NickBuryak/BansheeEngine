#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"
#include "BsScriptCJoint.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsSliderJoint.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsJoint.h"

namespace bs
{
	struct __LimitLinearRangeInterop;
	class CSliderJoint;

	class BS_SCR_BE_EXPORT ScriptCSliderJoint : public TScriptComponent<ScriptCSliderJoint, CSliderJoint, ScriptCJointBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SliderJoint")

		ScriptCSliderJoint(MonoObject* managedInstance, const GameObjectHandle<CSliderJoint>& value);

	private:
		static float Internal_getPosition(ScriptCSliderJoint* thisPtr);
		static float Internal_getSpeed(ScriptCSliderJoint* thisPtr);
		static void Internal_getLimit(ScriptCSliderJoint* thisPtr, __LimitLinearRangeInterop* __output);
		static void Internal_setLimit(ScriptCSliderJoint* thisPtr, __LimitLinearRangeInterop* limit);
		static void Internal_setFlag(ScriptCSliderJoint* thisPtr, SliderJointFlag flag, bool enabled);
		static bool Internal_hasFlag(ScriptCSliderJoint* thisPtr, SliderJointFlag flag);
	};
}
