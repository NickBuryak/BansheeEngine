#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"

namespace bs
{
	class CBone;

	class BS_SCR_BE_EXPORT ScriptCBone : public TScriptComponent<ScriptCBone, CBone>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Bone")

		ScriptCBone(MonoObject* managedInstance, const GameObjectHandle<CBone>& value);

	private:
		static void Internal_setBoneName(ScriptCBone* thisPtr, MonoString* name);
		static MonoString* Internal_getBoneName(ScriptCBone* thisPtr);
	};
}
