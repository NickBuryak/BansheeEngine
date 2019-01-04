#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptParticleEvolver.generated.h"
#include "Math/BsPlane.h"
#include "../../../bsf/Source/Foundation/bsfCore/Particles/BsParticleEvolver.h"

namespace bs
{
	class ParticleCollisions;

	class BS_SCR_BE_EXPORT ScriptParticleCollisions : public ScriptObject<ScriptParticleCollisions, ScriptParticleEvolverBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ParticleCollisions")

		ScriptParticleCollisions(MonoObject* managedInstance, const SPtr<ParticleCollisions>& value);

		SPtr<ParticleCollisions> getInternal() const;
		static MonoObject* create(const SPtr<ParticleCollisions>& value);

	private:
		static void Internal_setPlanes(ScriptParticleCollisions* thisPtr, MonoArray* planes);
		static MonoArray* Internal_getPlanes(ScriptParticleCollisions* thisPtr);
		static void Internal_setPlaneObjects(ScriptParticleCollisions* thisPtr, MonoArray* objects);
		static MonoArray* Internal_getPlaneObjects(ScriptParticleCollisions* thisPtr);
		static void Internal_setOptions(ScriptParticleCollisions* thisPtr, PARTICLE_COLLISIONS_DESC* options);
		static void Internal_getOptions(ScriptParticleCollisions* thisPtr, PARTICLE_COLLISIONS_DESC* __output);
		static void Internal_create(MonoObject* managedInstance, PARTICLE_COLLISIONS_DESC* desc);
		static void Internal_create0(MonoObject* managedInstance);
	};
}
