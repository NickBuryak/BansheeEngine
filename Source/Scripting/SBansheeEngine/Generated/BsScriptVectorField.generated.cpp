#include "BsScriptVectorField.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsVectorField.h"
#include "BsScriptResourceManager.h"
#include "Wrappers/BsScriptRRefBase.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsVectorField.h"
#include "Wrappers/BsScriptVector.h"
#include "BsScriptVECTOR_FIELD_DESC.generated.h"

namespace bs
{
	ScriptVectorField::ScriptVectorField(MonoObject* managedInstance, const ResourceHandle<VectorField>& value)
		:TScriptResource(managedInstance, value)
	{
	}

	void ScriptVectorField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetRef", (void*)&ScriptVectorField::Internal_getRef);
		metaData.scriptClass->addInternalCall("Internal_create", (void*)&ScriptVectorField::Internal_create);

	}

	 MonoObject*ScriptVectorField::createInstance()
	{
		bool dummy = false;
		void* ctorParams[1] = { &dummy };

		return metaData.scriptClass->createInstance("bool", ctorParams);
	}
	MonoObject* ScriptVectorField::Internal_getRef(ScriptVectorField* thisPtr)
	{
		return thisPtr->getRRef();
	}

	void ScriptVectorField::Internal_create(MonoObject* managedInstance, __VECTOR_FIELD_DESCInterop* desc, MonoArray* values)
	{
		VECTOR_FIELD_DESC tmpdesc;
		tmpdesc = ScriptVECTOR_FIELD_DESC::fromInterop(*desc);
		Vector<Vector3> vecvalues;
		if(values != nullptr)
		{
			ScriptArray arrayvalues(values);
			vecvalues.resize(arrayvalues.size());
			for(int i = 0; i < (int)arrayvalues.size(); i++)
			{
				vecvalues[i] = arrayvalues.get<Vector3>(i);
			}
		}
		ResourceHandle<VectorField> instance = VectorField::create(tmpdesc, vecvalues);
		ScriptResourceManager::instance().createBuiltinScriptResource(instance, managedInstance);
	}
}
