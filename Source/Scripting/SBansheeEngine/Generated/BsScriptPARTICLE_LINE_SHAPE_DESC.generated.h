#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"

namespace bs
{
	struct __PARTICLE_LINE_SHAPE_DESCInterop
	{
		float length;
		ParticleEmissionMode mode;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_LINE_SHAPE_DESC : public ScriptObject<ScriptPARTICLE_LINE_SHAPE_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleLineShapeOptions")

		static MonoObject* box(const __PARTICLE_LINE_SHAPE_DESCInterop& value);
		static __PARTICLE_LINE_SHAPE_DESCInterop unbox(MonoObject* value);
		static PARTICLE_LINE_SHAPE_DESC fromInterop(const __PARTICLE_LINE_SHAPE_DESCInterop& value);
		static __PARTICLE_LINE_SHAPE_DESCInterop toInterop(const PARTICLE_LINE_SHAPE_DESC& value);

	private:
		ScriptPARTICLE_LINE_SHAPE_DESC(MonoObject* managedInstance);

	};
}
