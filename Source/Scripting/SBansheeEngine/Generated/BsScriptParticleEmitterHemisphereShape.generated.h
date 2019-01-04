#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEmitterShape.generated.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"

namespace bs
{
	class ParticleEmitterHemisphereShape;

	class BS_SCR_BE_EXPORT ScriptParticleEmitterHemisphereShape : public ScriptObject<ScriptParticleEmitterHemisphereShape, ScriptParticleEmitterShapeBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleEmitterHemisphereShape")

		ScriptParticleEmitterHemisphereShape(MonoObject* managedInstance, const SPtr<ParticleEmitterHemisphereShape>& value);

		SPtr<ParticleEmitterHemisphereShape> getInternal() const;
		static MonoObject* create(const SPtr<ParticleEmitterHemisphereShape>& value);

	private:
		static void Internal_setOptions(ScriptParticleEmitterHemisphereShape* thisPtr, PARTICLE_HEMISPHERE_SHAPE_DESC* options);
		static void Internal_getOptions(ScriptParticleEmitterHemisphereShape* thisPtr, PARTICLE_HEMISPHERE_SHAPE_DESC* __output);
		static void Internal_create(MonoObject* managedInstance, PARTICLE_HEMISPHERE_SHAPE_DESC* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
