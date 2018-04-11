#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"
#include "BsScriptCCollider.generated.h"

namespace bs
{
	class CPlaneCollider;

	class BS_SCR_BE_EXPORT ScriptCPlaneCollider : public TScriptComponent<ScriptCPlaneCollider, CPlaneCollider, ScriptCColliderBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "PlaneCollider")

		ScriptCPlaneCollider(MonoObject* managedInstance, const GameObjectHandle<CPlaneCollider>& value);

	private:
		static void Internal_setNormal(ScriptCPlaneCollider* thisPtr, Vector3* normal);
		static void Internal_getNormal(ScriptCPlaneCollider* thisPtr, Vector3* __output);
		static void Internal_setDistance(ScriptCPlaneCollider* thisPtr, float distance);
		static float Internal_getDistance(ScriptCPlaneCollider* thisPtr);
	};
}
