#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptResource.h"
#include "Math/BsVector3.h"

namespace bs
{
	class VectorField;
	struct __VECTOR_FIELD_DESCInterop;

	class BS_SCR_BE_EXPORT ScriptVectorField : public TScriptResource<ScriptVectorField, VectorField>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "VectorField")

		ScriptVectorField(MonoObject* managedInstance, const ResourceHandle<VectorField>& value);

		static MonoObject* createInstance();

	private:
		static MonoObject* Internal_getRef(ScriptVectorField* thisPtr);

		static void Internal_create(MonoObject* managedInstance, __VECTOR_FIELD_DESCInterop* desc, MonoArray* values);
	};
}
