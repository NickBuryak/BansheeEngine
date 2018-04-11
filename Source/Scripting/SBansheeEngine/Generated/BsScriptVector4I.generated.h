#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfUtility/Math/BsVector4I.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptVector4I : public ScriptObject<ScriptVector4I>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Vector4I")

		static MonoObject* box(const Vector4I& value);
		static Vector4I unbox(MonoObject* value);

	private:
		ScriptVector4I(MonoObject* managedInstance);

	};
}
