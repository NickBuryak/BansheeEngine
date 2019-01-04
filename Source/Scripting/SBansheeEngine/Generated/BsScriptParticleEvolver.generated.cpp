#include "BsScriptParticleEvolver.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"

namespace bs
{
	ScriptParticleEvolverBase::ScriptParticleEvolverBase(MonoObject* managedInstance)
		:ScriptObjectBase(managedInstance)
	 { }

	ScriptParticleEvolver::ScriptParticleEvolver(MonoObject* managedInstance, const SPtr<ParticleEvolver>& value)
		:ScriptObject(managedInstance)
	{
		mInternal = value;
	}

	SPtr<ParticleEvolver> ScriptParticleEvolver::getInternal() const 
	{
		return std::static_pointer_cast<ParticleEvolver>(mInternal);
	}

	void ScriptParticleEvolver::initRuntimeData()
	{

	}

	MonoObject* ScriptParticleEvolver::create(const SPtr<ParticleEvolver>& value)
	{
		if(value == nullptr) return nullptr; 

		bool dummy = false;
		void* ctorParams[1] = { &dummy };

		MonoObject* managedInstance = metaData.scriptClass->createInstance("bool", ctorParams);
		new (bs_alloc<ScriptParticleEvolver>()) ScriptParticleEvolver(managedInstance, value);
		return managedInstance;
	}
}
