#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimation.h"

namespace bs
{
	struct __Blend2DInfoInterop
	{
		MonoObject* topLeftClip;
		MonoObject* topRightClip;
		MonoObject* botLeftClip;
		MonoObject* botRightClip;
	};

	class BS_SCR_BE_EXPORT ScriptBlend2DInfo : public ScriptObject<ScriptBlend2DInfo>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Blend2DInfo")

		static MonoObject* box(const __Blend2DInfoInterop& value);
		static __Blend2DInfoInterop unbox(MonoObject* value);
		static Blend2DInfo fromInterop(const __Blend2DInfoInterop& value);
		static __Blend2DInfoInterop toInterop(const Blend2DInfo& value);

	private:
		ScriptBlend2DInfo(MonoObject* managedInstance);

	};
}
