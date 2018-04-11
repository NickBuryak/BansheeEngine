#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	class MorphShapes;

	class BS_SCR_BE_EXPORT ScriptMorphShapes : public ScriptObject<ScriptMorphShapes>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "MorphShapes")

		ScriptMorphShapes(MonoObject* managedInstance, const SPtr<MorphShapes>& value);

		SPtr<MorphShapes> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<MorphShapes>& value);

	private:
		SPtr<MorphShapes> mInternal;

		static MonoArray* Internal_getChannels(ScriptMorphShapes* thisPtr);
	};
}
