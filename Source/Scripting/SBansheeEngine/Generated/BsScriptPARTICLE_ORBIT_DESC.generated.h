#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"

namespace bs
{
	struct __PARTICLE_ORBIT_DESCInterop
	{
		MonoObject* center;
		MonoObject* velocity;
		MonoObject* radial;
		bool worldSpace;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_ORBIT_DESC : public ScriptObject<ScriptPARTICLE_ORBIT_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleOrbitOptions")

		static MonoObject* box(const __PARTICLE_ORBIT_DESCInterop& value);
		static __PARTICLE_ORBIT_DESCInterop unbox(MonoObject* value);
		static PARTICLE_ORBIT_DESC fromInterop(const __PARTICLE_ORBIT_DESCInterop& value);
		static __PARTICLE_ORBIT_DESCInterop toInterop(const PARTICLE_ORBIT_DESC& value);

	private:
		ScriptPARTICLE_ORBIT_DESC(MonoObject* managedInstance);

	};
}
