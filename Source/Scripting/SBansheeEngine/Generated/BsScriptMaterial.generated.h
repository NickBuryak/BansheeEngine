#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptResource.h"

namespace bs
{
	class Material;
	class MaterialEx;

	class BS_SCR_BE_EXPORT ScriptMaterial : public TScriptResource<ScriptMaterial, Material>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Material")

		ScriptMaterial(MonoObject* managedInstance, const ResourceHandle<Material>& value);

		static MonoObject* createInstance();

	private:
		static void Internal_setShader(ScriptMaterial* thisPtr, MonoObject* shader);
		static MonoObject* Internal_clone(ScriptMaterial* thisPtr);
		static MonoObject* Internal_getShader(ScriptMaterial* thisPtr);
		static void Internal_setFloat(ScriptMaterial* thisPtr, MonoString* name, float value, uint32_t arrayIdx);
		static void Internal_setColor(ScriptMaterial* thisPtr, MonoString* name, Color* value, uint32_t arrayIdx);
		static void Internal_setVec2(ScriptMaterial* thisPtr, MonoString* name, Vector2* value, uint32_t arrayIdx);
		static void Internal_setVec3(ScriptMaterial* thisPtr, MonoString* name, Vector3* value, uint32_t arrayIdx);
		static void Internal_setVec4(ScriptMaterial* thisPtr, MonoString* name, Vector4* value, uint32_t arrayIdx);
		static void Internal_setMat3(ScriptMaterial* thisPtr, MonoString* name, Matrix3* value, uint32_t arrayIdx);
		static void Internal_setMat4(ScriptMaterial* thisPtr, MonoString* name, Matrix4* value, uint32_t arrayIdx);
		static float Internal_getFloat(ScriptMaterial* thisPtr, MonoString* name, uint32_t arrayIdx);
		static void Internal_getColor(ScriptMaterial* thisPtr, MonoString* name, uint32_t arrayIdx, Color* __output);
		static void Internal_getVec2(ScriptMaterial* thisPtr, MonoString* name, uint32_t arrayIdx, Vector2* __output);
		static void Internal_getVec3(ScriptMaterial* thisPtr, MonoString* name, uint32_t arrayIdx, Vector3* __output);
		static void Internal_getVec4(ScriptMaterial* thisPtr, MonoString* name, uint32_t arrayIdx, Vector4* __output);
		static void Internal_getMat3(ScriptMaterial* thisPtr, MonoString* name, uint32_t arrayIdx, Matrix3* __output);
		static void Internal_getMat4(ScriptMaterial* thisPtr, MonoString* name, uint32_t arrayIdx, Matrix4* __output);
		static void Internal_create(MonoObject* managedInstance);
		static void Internal_create0(MonoObject* managedInstance, MonoObject* shader);
		static void Internal_setTexture(ScriptMaterial* thisPtr, MonoString* name, MonoObject* value, uint32_t mipLevel, uint32_t numMipLevels, uint32_t arraySlice, uint32_t numArraySlices);
		static MonoObject* Internal_getTexture(ScriptMaterial* thisPtr, MonoString* name);
	};
}
