#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "Math/BsVector3.h"

namespace bs
{
	struct __PARTICLE_BOX_SHAPE_DESCInterop
	{
		ParticleEmitterBoxType type;
		Vector3 extents;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_BOX_SHAPE_DESC : public ScriptObject<ScriptPARTICLE_BOX_SHAPE_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleBoxShapeOptions")

		static MonoObject* box(const __PARTICLE_BOX_SHAPE_DESCInterop& value);
		static __PARTICLE_BOX_SHAPE_DESCInterop unbox(MonoObject* value);
		static PARTICLE_BOX_SHAPE_DESC fromInterop(const __PARTICLE_BOX_SHAPE_DESCInterop& value);
		static __PARTICLE_BOX_SHAPE_DESCInterop toInterop(const PARTICLE_BOX_SHAPE_DESC& value);

	private:
		ScriptPARTICLE_BOX_SHAPE_DESC(MonoObject* managedInstance);

	};
}
