#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"

namespace bs
{
	struct __PARTICLE_VELOCITY_DESCInterop
	{
		MonoObject* velocity;
		bool worldSpace;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_VELOCITY_DESC : public ScriptObject<ScriptPARTICLE_VELOCITY_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleVelocityOptions")

		static MonoObject* box(const __PARTICLE_VELOCITY_DESCInterop& value);
		static __PARTICLE_VELOCITY_DESCInterop unbox(MonoObject* value);
		static PARTICLE_VELOCITY_DESC fromInterop(const __PARTICLE_VELOCITY_DESCInterop& value);
		static __PARTICLE_VELOCITY_DESCInterop toInterop(const PARTICLE_VELOCITY_DESC& value);

	private:
		ScriptPARTICLE_VELOCITY_DESC(MonoObject* managedInstance);

	};
}
