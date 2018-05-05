#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	struct ShadowSettings;

	class BS_SCR_BE_EXPORT ScriptShadowSettings : public ScriptObject<ScriptShadowSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ShadowSettings")

		ScriptShadowSettings(MonoObject* managedInstance, const SPtr<ShadowSettings>& value);

		SPtr<ShadowSettings> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<ShadowSettings>& value);

	private:
		SPtr<ShadowSettings> mInternal;

		static void Internal_ShadowSettings(MonoObject* managedInstance);
		static float Internal_getdirectionalShadowDistance(ScriptShadowSettings* thisPtr);
		static void Internal_setdirectionalShadowDistance(ScriptShadowSettings* thisPtr, float value);
		static uint32_t Internal_getnumCascades(ScriptShadowSettings* thisPtr);
		static void Internal_setnumCascades(ScriptShadowSettings* thisPtr, uint32_t value);
		static float Internal_getcascadeDistributionExponent(ScriptShadowSettings* thisPtr);
		static void Internal_setcascadeDistributionExponent(ScriptShadowSettings* thisPtr, float value);
		static uint32_t Internal_getshadowFilteringQuality(ScriptShadowSettings* thisPtr);
		static void Internal_setshadowFilteringQuality(ScriptShadowSettings* thisPtr, uint32_t value);
	};
}
