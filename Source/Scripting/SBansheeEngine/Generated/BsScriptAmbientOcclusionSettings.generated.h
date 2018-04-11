#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	struct AmbientOcclusionSettings;

	class BS_SCR_BE_EXPORT ScriptAmbientOcclusionSettings : public ScriptObject<ScriptAmbientOcclusionSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "AmbientOcclusionSettings")

		ScriptAmbientOcclusionSettings(MonoObject* managedInstance, const SPtr<AmbientOcclusionSettings>& value);

		SPtr<AmbientOcclusionSettings> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<AmbientOcclusionSettings>& value);

	private:
		SPtr<AmbientOcclusionSettings> mInternal;

		static void Internal_AmbientOcclusionSettings(MonoObject* managedInstance);
		static bool Internal_getenabled(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setenabled(ScriptAmbientOcclusionSettings* thisPtr, bool value);
		static float Internal_getradius(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setradius(ScriptAmbientOcclusionSettings* thisPtr, float value);
		static float Internal_getbias(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setbias(ScriptAmbientOcclusionSettings* thisPtr, float value);
		static float Internal_getfadeDistance(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setfadeDistance(ScriptAmbientOcclusionSettings* thisPtr, float value);
		static float Internal_getfadeRange(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setfadeRange(ScriptAmbientOcclusionSettings* thisPtr, float value);
		static float Internal_getintensity(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setintensity(ScriptAmbientOcclusionSettings* thisPtr, float value);
		static float Internal_getpower(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setpower(ScriptAmbientOcclusionSettings* thisPtr, float value);
		static uint32_t Internal_getquality(ScriptAmbientOcclusionSettings* thisPtr);
		static void Internal_setquality(ScriptAmbientOcclusionSettings* thisPtr, uint32_t value);
	};
}
