#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEvolver.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"

namespace bs
{
	class ParticleTextureAnimation;

	class BS_SCR_BE_EXPORT ScriptParticleTextureAnimation : public ScriptObject<ScriptParticleTextureAnimation, ScriptParticleEvolverBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleTextureAnimation")

		ScriptParticleTextureAnimation(MonoObject* managedInstance, const SPtr<ParticleTextureAnimation>& value);

		SPtr<ParticleTextureAnimation> getInternal() const;
		static MonoObject* create(const SPtr<ParticleTextureAnimation>& value);

	private:
		static void Internal_setOptions(ScriptParticleTextureAnimation* thisPtr, PARTICLE_TEXTURE_ANIMATION_DESC* options);
		static void Internal_getOptions(ScriptParticleTextureAnimation* thisPtr, PARTICLE_TEXTURE_ANIMATION_DESC* __output);
		static void Internal_create(MonoObject* managedInstance, PARTICLE_TEXTURE_ANIMATION_DESC* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
