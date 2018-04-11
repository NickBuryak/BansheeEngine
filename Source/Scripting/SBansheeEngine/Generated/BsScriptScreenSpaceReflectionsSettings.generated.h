#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	struct ScreenSpaceReflectionsSettings;

	class BS_SCR_BE_EXPORT ScriptScreenSpaceReflectionsSettings : public ScriptObject<ScriptScreenSpaceReflectionsSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ScreenSpaceReflectionsSettings")

		ScriptScreenSpaceReflectionsSettings(MonoObject* managedInstance, const SPtr<ScreenSpaceReflectionsSettings>& value);

		SPtr<ScreenSpaceReflectionsSettings> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<ScreenSpaceReflectionsSettings>& value);

	private:
		SPtr<ScreenSpaceReflectionsSettings> mInternal;

		static void Internal_ScreenSpaceReflectionsSettings(MonoObject* managedInstance);
		static bool Internal_getenabled(ScriptScreenSpaceReflectionsSettings* thisPtr);
		static void Internal_setenabled(ScriptScreenSpaceReflectionsSettings* thisPtr, bool value);
		static uint32_t Internal_getquality(ScriptScreenSpaceReflectionsSettings* thisPtr);
		static void Internal_setquality(ScriptScreenSpaceReflectionsSettings* thisPtr, uint32_t value);
		static float Internal_getintensity(ScriptScreenSpaceReflectionsSettings* thisPtr);
		static void Internal_setintensity(ScriptScreenSpaceReflectionsSettings* thisPtr, float value);
		static float Internal_getmaxRoughness(ScriptScreenSpaceReflectionsSettings* thisPtr);
		static void Internal_setmaxRoughness(ScriptScreenSpaceReflectionsSettings* thisPtr, float value);
	};
}
