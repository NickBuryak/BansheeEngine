#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"

namespace bs
{
	struct __PARTICLE_COLOR_DESCInterop
	{
		MonoObject* color;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_COLOR_DESC : public ScriptObject<ScriptPARTICLE_COLOR_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleColorOptions")

		static MonoObject* box(const __PARTICLE_COLOR_DESCInterop& value);
		static __PARTICLE_COLOR_DESCInterop unbox(MonoObject* value);
		static PARTICLE_COLOR_DESC fromInterop(const __PARTICLE_COLOR_DESCInterop& value);
		static __PARTICLE_COLOR_DESCInterop toInterop(const PARTICLE_COLOR_DESC& value);

	private:
		ScriptPARTICLE_COLOR_DESC(MonoObject* managedInstance);

	};
}
