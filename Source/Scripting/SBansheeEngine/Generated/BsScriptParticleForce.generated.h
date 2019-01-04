#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEvolver.generated.h"

namespace bs
{
	class ParticleForce;
	struct __PARTICLE_FORCE_DESCInterop;

	class BS_SCR_BE_EXPORT ScriptParticleForce : public ScriptObject<ScriptParticleForce, ScriptParticleEvolverBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleForce")

		ScriptParticleForce(MonoObject* managedInstance, const SPtr<ParticleForce>& value);

		SPtr<ParticleForce> getInternal() const;
		static MonoObject* create(const SPtr<ParticleForce>& value);

	private:
		static void Internal_setOptions(ScriptParticleForce* thisPtr, __PARTICLE_FORCE_DESCInterop* options);
		static void Internal_getOptions(ScriptParticleForce* thisPtr, __PARTICLE_FORCE_DESCInterop* __output);
		static void Internal_create(MonoObject* managedInstance, __PARTICLE_FORCE_DESCInterop* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
