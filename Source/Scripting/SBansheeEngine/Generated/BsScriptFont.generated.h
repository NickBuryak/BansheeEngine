#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptResource.h"

namespace bs
{
	class Font;

	class BS_SCR_BE_EXPORT ScriptFont : public TScriptResource<ScriptFont, Font>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Font")

		ScriptFont(MonoObject* managedInstance, const ResourceHandle<Font>& value);

		static MonoObject* createInstance();

	private:
		static MonoObject* Internal_getRef(ScriptFont* thisPtr);

		static MonoObject* Internal_getBitmap(ScriptFont* thisPtr, uint32_t size);
		static int32_t Internal_getClosestSize(ScriptFont* thisPtr, uint32_t size);
	};
}
