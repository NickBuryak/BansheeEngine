#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptResource.h"
#include "../../../bsf/Source/Foundation/bsfCore/Utility/BsCommonTypes.h"
#include "../../../bsf/Source/Foundation/bsfCore/Mesh/BsMeshBase.h"
#include "../../../bsf/Source/Foundation/bsfCore/Utility/BsCommonTypes.h"
#include "../../../bsf/Source/Foundation/bsfCore/Renderer/BsRendererMeshData.h"

namespace bs
{
	class Mesh;
	class MeshEx;

	class BS_SCR_BE_EXPORT ScriptMesh : public TScriptResource<ScriptMesh, Mesh>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Mesh")

		ScriptMesh(MonoObject* managedInstance, const ResourceHandle<Mesh>& value);

		static MonoObject* createInstance();

	private:
		static MonoObject* Internal_getRef(ScriptMesh* thisPtr);

		static MonoObject* Internal_getSkeleton(ScriptMesh* thisPtr);
		static MonoObject* Internal_getMorphShapes(ScriptMesh* thisPtr);
		static void Internal_create(MonoObject* managedInstance, int32_t numVertices, int32_t numIndices, DrawOperationType topology, MeshUsage usage, VertexLayout vertex, IndexType index);
		static void Internal_create0(MonoObject* managedInstance, int32_t numVertices, int32_t numIndices, MonoArray* subMeshes, MeshUsage usage, VertexLayout vertex, IndexType index);
		static void Internal_create1(MonoObject* managedInstance, MonoObject* data, DrawOperationType topology, MeshUsage usage);
		static void Internal_create2(MonoObject* managedInstance, MonoObject* data, MonoArray* subMeshes, MeshUsage usage);
		static MonoArray* Internal_getSubMeshes(ScriptMesh* thisPtr);
		static uint32_t Internal_getSubMeshCount(ScriptMesh* thisPtr);
		static void Internal_getBounds(ScriptMesh* thisPtr, AABox* box, Sphere* sphere);
		static MonoObject* Internal_getMeshData(ScriptMesh* thisPtr);
		static void Internal_setMeshData(ScriptMesh* thisPtr, MonoObject* value);
	};
}
