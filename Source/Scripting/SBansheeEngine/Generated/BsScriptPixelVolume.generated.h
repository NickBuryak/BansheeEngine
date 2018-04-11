#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Image/BsPixelVolume.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptPixelVolume : public ScriptObject<ScriptPixelVolume>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "PixelVolume")

		static MonoObject* box(const PixelVolume& value);
		static PixelVolume unbox(MonoObject* value);

	private:
		ScriptPixelVolume(MonoObject* managedInstance);

	};
}
