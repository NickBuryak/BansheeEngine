#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEmitterShape.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"

namespace bs
{
	class ParticleEmitterSphereShape;

	class BS_SCR_BE_EXPORT ScriptParticleEmitterSphereShape : public ScriptObject<ScriptParticleEmitterSphereShape, ScriptParticleEmitterShapeBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleEmitterSphereShape")

		ScriptParticleEmitterSphereShape(MonoObject* managedInstance, const SPtr<ParticleEmitterSphereShape>& value);

		SPtr<ParticleEmitterSphereShape> getInternal() const;
		static MonoObject* create(const SPtr<ParticleEmitterSphereShape>& value);

	private:
		static void Internal_setOptions(ScriptParticleEmitterSphereShape* thisPtr, PARTICLE_SPHERE_SHAPE_DESC* options);
		static void Internal_getOptions(ScriptParticleEmitterSphereShape* thisPtr, PARTICLE_SPHERE_SHAPE_DESC* __output);
		static void Internal_create(MonoObject* managedInstance, PARTICLE_SPHERE_SHAPE_DESC* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
