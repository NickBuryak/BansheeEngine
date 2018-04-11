#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace bs
{
	struct TonemappingSettings;

	class BS_SCR_BE_EXPORT ScriptTonemappingSettings : public ScriptObject<ScriptTonemappingSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "TonemappingSettings")

		ScriptTonemappingSettings(MonoObject* managedInstance, const SPtr<TonemappingSettings>& value);

		SPtr<TonemappingSettings> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<TonemappingSettings>& value);

	private:
		SPtr<TonemappingSettings> mInternal;

		static void Internal_TonemappingSettings(MonoObject* managedInstance);
		static float Internal_getfilmicCurveShoulderStrength(ScriptTonemappingSettings* thisPtr);
		static void Internal_setfilmicCurveShoulderStrength(ScriptTonemappingSettings* thisPtr, float value);
		static float Internal_getfilmicCurveLinearStrength(ScriptTonemappingSettings* thisPtr);
		static void Internal_setfilmicCurveLinearStrength(ScriptTonemappingSettings* thisPtr, float value);
		static float Internal_getfilmicCurveLinearAngle(ScriptTonemappingSettings* thisPtr);
		static void Internal_setfilmicCurveLinearAngle(ScriptTonemappingSettings* thisPtr, float value);
		static float Internal_getfilmicCurveToeStrength(ScriptTonemappingSettings* thisPtr);
		static void Internal_setfilmicCurveToeStrength(ScriptTonemappingSettings* thisPtr, float value);
		static float Internal_getfilmicCurveToeNumerator(ScriptTonemappingSettings* thisPtr);
		static void Internal_setfilmicCurveToeNumerator(ScriptTonemappingSettings* thisPtr, float value);
		static float Internal_getfilmicCurveToeDenominator(ScriptTonemappingSettings* thisPtr);
		static void Internal_setfilmicCurveToeDenominator(ScriptTonemappingSettings* thisPtr, float value);
		static float Internal_getfilmicCurveLinearWhitePoint(ScriptTonemappingSettings* thisPtr);
		static void Internal_setfilmicCurveLinearWhitePoint(ScriptTonemappingSettings* thisPtr, float value);
	};
}
