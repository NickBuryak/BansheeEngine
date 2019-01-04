#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"

namespace bs
{
	struct __PARTICLE_ROTATION_DESCInterop
	{
		MonoObject* rotation;
		MonoObject* rotation3D;
		bool use3DRotation;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_ROTATION_DESC : public ScriptObject<ScriptPARTICLE_ROTATION_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleRotationOptions")

		static MonoObject* box(const __PARTICLE_ROTATION_DESCInterop& value);
		static __PARTICLE_ROTATION_DESCInterop unbox(MonoObject* value);
		static PARTICLE_ROTATION_DESC fromInterop(const __PARTICLE_ROTATION_DESCInterop& value);
		static __PARTICLE_ROTATION_DESCInterop toInterop(const PARTICLE_ROTATION_DESC& value);

	private:
		ScriptPARTICLE_ROTATION_DESC(MonoObject* managedInstance);

	};
}
