#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	struct AutoExposureSettings;

	class BS_SCR_BE_EXPORT ScriptAutoExposureSettings : public ScriptObject<ScriptAutoExposureSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "AutoExposureSettings")

		ScriptAutoExposureSettings(MonoObject* managedInstance, const SPtr<AutoExposureSettings>& value);

		SPtr<AutoExposureSettings> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<AutoExposureSettings>& value);

	private:
		SPtr<AutoExposureSettings> mInternal;

		static void Internal_AutoExposureSettings(MonoObject* managedInstance);
		static float Internal_gethistogramLog2Min(ScriptAutoExposureSettings* thisPtr);
		static void Internal_sethistogramLog2Min(ScriptAutoExposureSettings* thisPtr, float value);
		static float Internal_gethistogramLog2Max(ScriptAutoExposureSettings* thisPtr);
		static void Internal_sethistogramLog2Max(ScriptAutoExposureSettings* thisPtr, float value);
		static float Internal_gethistogramPctLow(ScriptAutoExposureSettings* thisPtr);
		static void Internal_sethistogramPctLow(ScriptAutoExposureSettings* thisPtr, float value);
		static float Internal_gethistogramPctHigh(ScriptAutoExposureSettings* thisPtr);
		static void Internal_sethistogramPctHigh(ScriptAutoExposureSettings* thisPtr, float value);
		static float Internal_getminEyeAdaptation(ScriptAutoExposureSettings* thisPtr);
		static void Internal_setminEyeAdaptation(ScriptAutoExposureSettings* thisPtr, float value);
		static float Internal_getmaxEyeAdaptation(ScriptAutoExposureSettings* thisPtr);
		static void Internal_setmaxEyeAdaptation(ScriptAutoExposureSettings* thisPtr, float value);
		static float Internal_geteyeAdaptationSpeedUp(ScriptAutoExposureSettings* thisPtr);
		static void Internal_seteyeAdaptationSpeedUp(ScriptAutoExposureSettings* thisPtr, float value);
		static float Internal_geteyeAdaptationSpeedDown(ScriptAutoExposureSettings* thisPtr);
		static void Internal_seteyeAdaptationSpeedDown(ScriptAutoExposureSettings* thisPtr, float value);
	};
}
