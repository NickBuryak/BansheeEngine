#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptPARTICLE_GRAVITY_DESC : public ScriptObject<ScriptPARTICLE_GRAVITY_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleGravityOptions")

		static MonoObject* box(const PARTICLE_GRAVITY_DESC& value);
		static PARTICLE_GRAVITY_DESC unbox(MonoObject* value);

	private:
		ScriptPARTICLE_GRAVITY_DESC(MonoObject* managedInstance);

	};
}
