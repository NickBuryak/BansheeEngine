#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleSystem.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleSystem.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"
#include "Math/BsVector3.h"

namespace bs
{
	struct ParticleGpuSimulationSettings;

	class BS_SCR_BE_EXPORT ScriptParticleGpuSimulationSettings : public ScriptObject<ScriptParticleGpuSimulationSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleGpuSimulationSettings")

		ScriptParticleGpuSimulationSettings(MonoObject* managedInstance, const SPtr<ParticleGpuSimulationSettings>& value);

		SPtr<ParticleGpuSimulationSettings> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<ParticleGpuSimulationSettings>& value);

	private:
		SPtr<ParticleGpuSimulationSettings> mInternal;

		static MonoObject* Internal_getvectorField(ScriptParticleGpuSimulationSettings* thisPtr);
		static void Internal_setvectorField(ScriptParticleGpuSimulationSettings* thisPtr, MonoObject* value);
		static MonoObject* Internal_getcolorOverLifetime(ScriptParticleGpuSimulationSettings* thisPtr);
		static void Internal_setcolorOverLifetime(ScriptParticleGpuSimulationSettings* thisPtr, MonoObject* value);
		static MonoObject* Internal_getsizeScaleOverLifetime(ScriptParticleGpuSimulationSettings* thisPtr);
		static void Internal_setsizeScaleOverLifetime(ScriptParticleGpuSimulationSettings* thisPtr, MonoObject* value);
		static void Internal_getacceleration(ScriptParticleGpuSimulationSettings* thisPtr, Vector3* __output);
		static void Internal_setacceleration(ScriptParticleGpuSimulationSettings* thisPtr, Vector3* value);
		static float Internal_getdrag(ScriptParticleGpuSimulationSettings* thisPtr);
		static void Internal_setdrag(ScriptParticleGpuSimulationSettings* thisPtr, float value);
		static MonoObject* Internal_getdepthCollision(ScriptParticleGpuSimulationSettings* thisPtr);
		static void Internal_setdepthCollision(ScriptParticleGpuSimulationSettings* thisPtr, MonoObject* value);
	};
}
