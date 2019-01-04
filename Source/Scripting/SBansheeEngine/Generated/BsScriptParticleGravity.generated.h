#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEvolver.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"

namespace bs
{
	class ParticleGravity;

	class BS_SCR_BE_EXPORT ScriptParticleGravity : public ScriptObject<ScriptParticleGravity, ScriptParticleEvolverBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleGravity")

		ScriptParticleGravity(MonoObject* managedInstance, const SPtr<ParticleGravity>& value);

		SPtr<ParticleGravity> getInternal() const;
		static MonoObject* create(const SPtr<ParticleGravity>& value);

	private:
		static void Internal_setOptions(ScriptParticleGravity* thisPtr, PARTICLE_GRAVITY_DESC* options);
		static void Internal_getOptions(ScriptParticleGravity* thisPtr, PARTICLE_GRAVITY_DESC* __output);
		static void Internal_create(MonoObject* managedInstance, PARTICLE_GRAVITY_DESC* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
