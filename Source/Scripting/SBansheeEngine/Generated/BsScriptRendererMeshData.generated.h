#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Renderer/BsRendererMeshData.h"
#include "../../../bsf/Source/Foundation/bsfCore/Mesh/BsMeshData.h"
#include "Math/BsVector3.h"
#include "../../../bsf/Source/Foundation/bsfCore/Utility/BsCommonTypes.h"
#include "Math/BsVector4.h"
#include "Math/BsVector2.h"
#include "Image/BsColor.h"

namespace bs
{
	class RendererMeshData;
	class MeshDataEx;

	class BS_SCR_BE_EXPORT ScriptRendererMeshData : public ScriptObject<ScriptRendererMeshData>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "MeshData")

		ScriptRendererMeshData(MonoObject* managedInstance, const SPtr<RendererMeshData>& value);

		SPtr<RendererMeshData> getInternal() const { return mInternal; }
		static MonoObject* create(const SPtr<RendererMeshData>& value);

	private:
		SPtr<RendererMeshData> mInternal;

		static void Internal_create(MonoObject* managedInstance, uint32_t numVertices, uint32_t numIndices, VertexLayout layout, IndexType indexType);
		static MonoArray* Internal_getPositions(ScriptRendererMeshData* thisPtr);
		static void Internal_setPositions(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static MonoArray* Internal_getNormals(ScriptRendererMeshData* thisPtr);
		static void Internal_setNormals(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static MonoArray* Internal_getTangents(ScriptRendererMeshData* thisPtr);
		static void Internal_setTangents(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static MonoArray* Internal_getColors(ScriptRendererMeshData* thisPtr);
		static void Internal_setColors(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static MonoArray* Internal_getUV0(ScriptRendererMeshData* thisPtr);
		static void Internal_setUV0(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static MonoArray* Internal_getUV1(ScriptRendererMeshData* thisPtr);
		static void Internal_setUV1(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static MonoArray* Internal_getBoneWeights(ScriptRendererMeshData* thisPtr);
		static void Internal_setBoneWeights(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static MonoArray* Internal_getIndices(ScriptRendererMeshData* thisPtr);
		static void Internal_setIndices(ScriptRendererMeshData* thisPtr, MonoArray* value);
		static int32_t Internal_getVertexCount(ScriptRendererMeshData* thisPtr);
		static int32_t Internal_getIndexCount(ScriptRendererMeshData* thisPtr);
	};
}
