#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimation.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsAnimationClip.h"

namespace bs
{
	struct __BlendClipInfoInterop
	{
		MonoObject* clip;
		float position;
	};

	class BS_SCR_BE_EXPORT ScriptBlendClipInfo : public ScriptObject<ScriptBlendClipInfo>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "BlendClipInfo")

		static MonoObject* box(const __BlendClipInfoInterop& value);
		static __BlendClipInfoInterop unbox(MonoObject* value);
		static BlendClipInfo fromInterop(const __BlendClipInfoInterop& value);
		static __BlendClipInfoInterop toInterop(const BlendClipInfo& value);

	private:
		ScriptBlendClipInfo(MonoObject* managedInstance);

	};
}
