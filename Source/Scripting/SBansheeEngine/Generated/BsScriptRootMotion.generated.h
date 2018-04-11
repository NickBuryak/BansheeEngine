#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimationCurve.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimationCurve.h"

namespace bs
{
	struct RootMotion;
	class RootMotionEx;

	class BS_SCR_BE_EXPORT ScriptRootMotion : public ScriptObject<ScriptRootMotion>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "RootMotion")

		ScriptRootMotion(MonoObject* managedInstance, const SPtr<RootMotion>& value);

		SPtr<RootMotion> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<RootMotion>& value);

	private:
		SPtr<RootMotion> mInternal;

		static MonoObject* Internal_getPositionCurves(ScriptRootMotion* thisPtr);
		static MonoObject* Internal_getRotationCurves(ScriptRootMotion* thisPtr);
	};
}
