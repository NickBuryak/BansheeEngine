#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "Math/BsDegree.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"

namespace bs
{
	struct __PARTICLE_CIRCLE_SHAPE_DESCInterop
	{
		float radius;
		float thickness;
		Degree arc;
		ParticleEmissionMode mode;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_CIRCLE_SHAPE_DESC : public ScriptObject<ScriptPARTICLE_CIRCLE_SHAPE_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleCircleShapeOptions")

		static MonoObject* box(const __PARTICLE_CIRCLE_SHAPE_DESCInterop& value);
		static __PARTICLE_CIRCLE_SHAPE_DESCInterop unbox(MonoObject* value);
		static PARTICLE_CIRCLE_SHAPE_DESC fromInterop(const __PARTICLE_CIRCLE_SHAPE_DESCInterop& value);
		static __PARTICLE_CIRCLE_SHAPE_DESCInterop toInterop(const PARTICLE_CIRCLE_SHAPE_DESC& value);

	private:
		ScriptPARTICLE_CIRCLE_SHAPE_DESC(MonoObject* managedInstance);

	};
}
