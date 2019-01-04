#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEmitterShape.generated.h"

namespace bs
{
	class ParticleEmitterRectShape;
	struct __PARTICLE_RECT_SHAPE_DESCInterop;

	class BS_SCR_BE_EXPORT ScriptParticleEmitterRectShape : public ScriptObject<ScriptParticleEmitterRectShape, ScriptParticleEmitterShapeBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleEmitterRectShape")

		ScriptParticleEmitterRectShape(MonoObject* managedInstance, const SPtr<ParticleEmitterRectShape>& value);

		SPtr<ParticleEmitterRectShape> getInternal() const;
		static MonoObject* create(const SPtr<ParticleEmitterRectShape>& value);

	private:
		static void Internal_setOptions(ScriptParticleEmitterRectShape* thisPtr, __PARTICLE_RECT_SHAPE_DESCInterop* options);
		static void Internal_getOptions(ScriptParticleEmitterRectShape* thisPtr, __PARTICLE_RECT_SHAPE_DESCInterop* __output);
		static void Internal_create(MonoObject* managedInstance, __PARTICLE_RECT_SHAPE_DESCInterop* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
