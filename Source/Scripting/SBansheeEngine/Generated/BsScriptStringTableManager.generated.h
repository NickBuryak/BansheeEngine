#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Localization/BsStringTable.h"

namespace bs
{
	class StringTableManager;

	class BS_SCR_BE_EXPORT ScriptStringTableManager : public ScriptObject<ScriptStringTableManager>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "StringTables")

		ScriptStringTableManager(MonoObject* managedInstance);

	private:
		static void Internal_setActiveLanguage(Language language);
		static Language Internal_getActiveLanguage();
		static MonoObject* Internal_getTable(uint32_t id);
		static void Internal_removeTable(uint32_t id);
		static void Internal_setTable(uint32_t id, MonoObject* table);
	};
}
