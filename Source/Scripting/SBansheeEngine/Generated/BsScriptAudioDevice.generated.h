#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Audio/BsAudio.h"

namespace bs
{
	struct __AudioDeviceInterop
	{
		MonoString* name;
	};

	class BS_SCR_BE_EXPORT ScriptAudioDevice : public ScriptObject<ScriptAudioDevice>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "AudioDevice")

		static MonoObject* box(const __AudioDeviceInterop& value);
		static __AudioDeviceInterop unbox(MonoObject* value);
		static AudioDevice fromInterop(const __AudioDeviceInterop& value);
		static __AudioDeviceInterop toInterop(const AudioDevice& value);

	private:
		ScriptAudioDevice(MonoObject* managedInstance);

	};
}
