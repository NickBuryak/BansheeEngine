#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimationUtility.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimationUtility.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimationUtility.h"

namespace bs
{
	struct __TangentRefInterop
	{
		KeyframeRef keyframeRef;
		TangentType type;
	};

	class BS_SCR_BE_EXPORT ScriptTangentRef : public ScriptObject<ScriptTangentRef>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "TangentRef")

		static MonoObject* box(const __TangentRefInterop& value);
		static __TangentRefInterop unbox(MonoObject* value);
		static TangentRef fromInterop(const __TangentRefInterop& value);
		static __TangentRefInterop toInterop(const TangentRef& value);

	private:
		ScriptTangentRef(MonoObject* managedInstance);

	};
}
