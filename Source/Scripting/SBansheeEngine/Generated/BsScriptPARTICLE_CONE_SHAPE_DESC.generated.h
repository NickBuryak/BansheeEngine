#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "Math/BsDegree.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"

namespace bs
{
	struct __PARTICLE_CONE_SHAPE_DESCInterop
	{
		ParticleEmitterConeType type;
		float radius;
		Degree angle;
		float length;
		float thickness;
		Degree arc;
		ParticleEmissionMode mode;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_CONE_SHAPE_DESC : public ScriptObject<ScriptPARTICLE_CONE_SHAPE_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleConeShapeOptions")

		static MonoObject* box(const __PARTICLE_CONE_SHAPE_DESCInterop& value);
		static __PARTICLE_CONE_SHAPE_DESCInterop unbox(MonoObject* value);
		static PARTICLE_CONE_SHAPE_DESC fromInterop(const __PARTICLE_CONE_SHAPE_DESCInterop& value);
		static __PARTICLE_CONE_SHAPE_DESCInterop toInterop(const PARTICLE_CONE_SHAPE_DESC& value);

	private:
		ScriptPARTICLE_CONE_SHAPE_DESC(MonoObject* managedInstance);

	};
}
