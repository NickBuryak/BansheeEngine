#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "Math/BsVector2.h"

namespace bs
{
	struct __PARTICLE_RECT_SHAPE_DESCInterop
	{
		Vector2 extents;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_RECT_SHAPE_DESC : public ScriptObject<ScriptPARTICLE_RECT_SHAPE_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleRectShapeOptions")

		static MonoObject* box(const __PARTICLE_RECT_SHAPE_DESCInterop& value);
		static __PARTICLE_RECT_SHAPE_DESCInterop unbox(MonoObject* value);
		static PARTICLE_RECT_SHAPE_DESC fromInterop(const __PARTICLE_RECT_SHAPE_DESCInterop& value);
		static __PARTICLE_RECT_SHAPE_DESCInterop toInterop(const PARTICLE_RECT_SHAPE_DESC& value);

	private:
		ScriptPARTICLE_RECT_SHAPE_DESC(MonoObject* managedInstance);

	};
}
