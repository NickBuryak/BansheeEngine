#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEmitterShape.generated.h"

namespace bs
{
	class ParticleEmitterSkinnedMeshShape;
	struct __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop;

	class BS_SCR_BE_EXPORT ScriptParticleEmitterSkinnedMeshShape : public ScriptObject<ScriptParticleEmitterSkinnedMeshShape, ScriptParticleEmitterShapeBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleEmitterSkinnedMeshShape")

		ScriptParticleEmitterSkinnedMeshShape(MonoObject* managedInstance, const SPtr<ParticleEmitterSkinnedMeshShape>& value);

		SPtr<ParticleEmitterSkinnedMeshShape> getInternal() const;
		static MonoObject* create(const SPtr<ParticleEmitterSkinnedMeshShape>& value);

	private:
		static void Internal_setOptions(ScriptParticleEmitterSkinnedMeshShape* thisPtr, __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop* options);
		static void Internal_getOptions(ScriptParticleEmitterSkinnedMeshShape* thisPtr, __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop* __output);
		static void Internal_create(MonoObject* managedInstance, __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
