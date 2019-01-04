#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptPARTICLE_HEMISPHERE_SHAPE_DESC : public ScriptObject<ScriptPARTICLE_HEMISPHERE_SHAPE_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleHemisphereShapeOptions")

		static MonoObject* box(const PARTICLE_HEMISPHERE_SHAPE_DESC& value);
		static PARTICLE_HEMISPHERE_SHAPE_DESC unbox(MonoObject* value);

	private:
		ScriptPARTICLE_HEMISPHERE_SHAPE_DESC(MonoObject* managedInstance);

	};
}
