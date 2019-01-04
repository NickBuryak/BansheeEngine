#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEvolver.generated.h"

namespace bs
{
	struct __PARTICLE_ROTATION_DESCInterop;
	class ParticleRotation;

	class BS_SCR_BE_EXPORT ScriptParticleRotation : public ScriptObject<ScriptParticleRotation, ScriptParticleEvolverBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleRotation")

		ScriptParticleRotation(MonoObject* managedInstance, const SPtr<ParticleRotation>& value);

		SPtr<ParticleRotation> getInternal() const;
		static MonoObject* create(const SPtr<ParticleRotation>& value);

	private:
		static void Internal_setOptions(ScriptParticleRotation* thisPtr, __PARTICLE_ROTATION_DESCInterop* options);
		static void Internal_getOptions(ScriptParticleRotation* thisPtr, __PARTICLE_ROTATION_DESCInterop* __output);
		static void Internal_create(MonoObject* managedInstance, __PARTICLE_ROTATION_DESCInterop* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
