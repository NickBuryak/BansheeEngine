#include "BsScriptParticleEmitterHemisphereShape.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "BsScriptPARTICLE_HEMISPHERE_SHAPE_DESC.generated.h"
#include "BsScriptParticleEmitterHemisphereShape.generated.h"

namespace bs
{
	ScriptParticleEmitterHemisphereShape::ScriptParticleEmitterHemisphereShape(MonoObject* managedInstance, const SPtr<ParticleEmitterHemisphereShape>& value)
		:ScriptObject(managedInstance)
	{
		mInternal = value;
	}

	SPtr<ParticleEmitterHemisphereShape> ScriptParticleEmitterHemisphereShape::getInternal() const 
	{
		return std::static_pointer_cast<ParticleEmitterHemisphereShape>(mInternal);
	}

	void ScriptParticleEmitterHemisphereShape::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_setOptions", (void*)&ScriptParticleEmitterHemisphereShape::Internal_setOptions);
		metaData.scriptClass->addInternalCall("Internal_getOptions", (void*)&ScriptParticleEmitterHemisphereShape::Internal_getOptions);
		metaData.scriptClass->addInternalCall("Internal_create", (void*)&ScriptParticleEmitterHemisphereShape::Internal_create);
		metaData.scriptClass->addInternalCall("Internal_create0", (void*)&ScriptParticleEmitterHemisphereShape::Internal_create0);

	}

	MonoObject* ScriptParticleEmitterHemisphereShape::create(const SPtr<ParticleEmitterHemisphereShape>& value)
	{
		if(value == nullptr) return nullptr; 

		bool dummy = false;
		void* ctorParams[1] = { &dummy };

		MonoObject* managedInstance = metaData.scriptClass->createInstance("bool", ctorParams);
		new (bs_alloc<ScriptParticleEmitterHemisphereShape>()) ScriptParticleEmitterHemisphereShape(managedInstance, value);
		return managedInstance;
	}
	void ScriptParticleEmitterHemisphereShape::Internal_setOptions(ScriptParticleEmitterHemisphereShape* thisPtr, PARTICLE_HEMISPHERE_SHAPE_DESC* options)
	{
		thisPtr->getInternal()->setOptions(*options);
	}

	void ScriptParticleEmitterHemisphereShape::Internal_getOptions(ScriptParticleEmitterHemisphereShape* thisPtr, PARTICLE_HEMISPHERE_SHAPE_DESC* __output)
	{
		PARTICLE_HEMISPHERE_SHAPE_DESC tmp__output;
		tmp__output = thisPtr->getInternal()->getOptions();

		*__output = tmp__output;
	}

	void ScriptParticleEmitterHemisphereShape::Internal_create(MonoObject* managedInstance, PARTICLE_HEMISPHERE_SHAPE_DESC* desc)
	{
		SPtr<ParticleEmitterHemisphereShape> instance = ParticleEmitterHemisphereShape::create(*desc);
		new (bs_alloc<ScriptParticleEmitterHemisphereShape>())ScriptParticleEmitterHemisphereShape(managedInstance, instance);
	}

	void ScriptParticleEmitterHemisphereShape::Internal_create0(MonoObject* managedInstance)
	{
		SPtr<ParticleEmitterHemisphereShape> instance = ParticleEmitterHemisphereShape::create();
		new (bs_alloc<ScriptParticleEmitterHemisphereShape>())ScriptParticleEmitterHemisphereShape(managedInstance, instance);
	}
}
