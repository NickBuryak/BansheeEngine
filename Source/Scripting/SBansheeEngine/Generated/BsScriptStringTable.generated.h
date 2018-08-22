#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptResource.h"
#include "../../../bsf/Source/Foundation/bsfCore/Localization/BsStringTable.h"

namespace bs
{
	class StringTable;

	class BS_SCR_BE_EXPORT ScriptStringTable : public TScriptResource<ScriptStringTable, StringTable>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "StringTable")

		ScriptStringTable(MonoObject* managedInstance, const ResourceHandle<StringTable>& value);

		static MonoObject* createInstance();

	private:
		static MonoObject* Internal_getRef(ScriptStringTable* thisPtr);

		static bool Internal_contains(ScriptStringTable* thisPtr, MonoString* identifier);
		static uint32_t Internal_getNumStrings(ScriptStringTable* thisPtr);
		static MonoArray* Internal_getIdentifiers(ScriptStringTable* thisPtr);
		static void Internal_setString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString* value);
		static MonoString* Internal_getString(ScriptStringTable* thisPtr, MonoString* identifier, Language language);
		static void Internal_removeString(ScriptStringTable* thisPtr, MonoString* identifier);
		static void Internal_create(MonoObject* managedInstance);
	};
}
