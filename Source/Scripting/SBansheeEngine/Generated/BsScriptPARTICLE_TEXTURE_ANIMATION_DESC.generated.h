#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptPARTICLE_TEXTURE_ANIMATION_DESC : public ScriptObject<ScriptPARTICLE_TEXTURE_ANIMATION_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleTextureAnimationOptions")

		static MonoObject* box(const PARTICLE_TEXTURE_ANIMATION_DESC& value);
		static PARTICLE_TEXTURE_ANIMATION_DESC unbox(MonoObject* value);

	private:
		ScriptPARTICLE_TEXTURE_ANIMATION_DESC(MonoObject* managedInstance);

	};
}
