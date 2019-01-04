#include "BsScriptLimitAngularRange.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsJoint.h"
#include "BsScriptSpring.generated.h"

namespace bs
{
	ScriptLimitAngularRange::ScriptLimitAngularRange(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptLimitAngularRange::initRuntimeData()
	{ }

	MonoObject*ScriptLimitAngularRange::box(const __LimitAngularRangeInterop& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	__LimitAngularRangeInterop ScriptLimitAngularRange::unbox(MonoObject* value)
	{
		return *(__LimitAngularRangeInterop*)MonoUtil::unbox(value);
	}

	LimitAngularRange ScriptLimitAngularRange::fromInterop(const __LimitAngularRangeInterop& value)
	{
		LimitAngularRange output;
		output.lower = value.lower;
		output.upper = value.upper;
		output.contactDist = value.contactDist;
		output.restitution = value.restitution;
		output.spring = value.spring;

		return output;
	}

	__LimitAngularRangeInterop ScriptLimitAngularRange::toInterop(const LimitAngularRange& value)
	{
		__LimitAngularRangeInterop output;
		output.lower = value.lower;
		output.upper = value.upper;
		output.contactDist = value.contactDist;
		output.restitution = value.restitution;
		output.spring = value.spring;

		return output;
	}

}
