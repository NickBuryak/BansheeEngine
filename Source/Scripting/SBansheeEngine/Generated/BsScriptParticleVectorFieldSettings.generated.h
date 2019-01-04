#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "Math/BsVector3.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleDistribution.h"
#include "Math/BsQuaternion.h"

namespace bs
{
	struct ParticleVectorFieldSettings;

	class BS_SCR_BE_EXPORT ScriptParticleVectorFieldSettings : public ScriptObject<ScriptParticleVectorFieldSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleVectorFieldSettings")

		ScriptParticleVectorFieldSettings(MonoObject* managedInstance, const SPtr<ParticleVectorFieldSettings>& value);

		SPtr<ParticleVectorFieldSettings> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<ParticleVectorFieldSettings>& value);

	private:
		SPtr<ParticleVectorFieldSettings> mInternal;

		static MonoObject* Internal_getvectorField(ScriptParticleVectorFieldSettings* thisPtr);
		static void Internal_setvectorField(ScriptParticleVectorFieldSettings* thisPtr, MonoObject* value);
		static float Internal_getintensity(ScriptParticleVectorFieldSettings* thisPtr);
		static void Internal_setintensity(ScriptParticleVectorFieldSettings* thisPtr, float value);
		static float Internal_gettightness(ScriptParticleVectorFieldSettings* thisPtr);
		static void Internal_settightness(ScriptParticleVectorFieldSettings* thisPtr, float value);
		static void Internal_getscale(ScriptParticleVectorFieldSettings* thisPtr, Vector3* __output);
		static void Internal_setscale(ScriptParticleVectorFieldSettings* thisPtr, Vector3* value);
		static void Internal_getoffset(ScriptParticleVectorFieldSettings* thisPtr, Vector3* __output);
		static void Internal_setoffset(ScriptParticleVectorFieldSettings* thisPtr, Vector3* value);
		static void Internal_getrotation(ScriptParticleVectorFieldSettings* thisPtr, Quaternion* __output);
		static void Internal_setrotation(ScriptParticleVectorFieldSettings* thisPtr, Quaternion* value);
		static MonoObject* Internal_getrotationRate(ScriptParticleVectorFieldSettings* thisPtr);
		static void Internal_setrotationRate(ScriptParticleVectorFieldSettings* thisPtr, MonoObject* value);
		static bool Internal_gettilingX(ScriptParticleVectorFieldSettings* thisPtr);
		static void Internal_settilingX(ScriptParticleVectorFieldSettings* thisPtr, bool value);
		static bool Internal_gettilingY(ScriptParticleVectorFieldSettings* thisPtr);
		static void Internal_settilingY(ScriptParticleVectorFieldSettings* thisPtr, bool value);
		static bool Internal_gettilingZ(ScriptParticleVectorFieldSettings* thisPtr);
		static void Internal_settilingZ(ScriptParticleVectorFieldSettings* thisPtr, bool value);
	};
}
