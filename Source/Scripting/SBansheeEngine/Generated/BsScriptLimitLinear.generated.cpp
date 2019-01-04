#include "BsScriptLimitLinear.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../bsf/Source/Foundation/bsfCore/Physics/BsJoint.h"
#include "BsScriptSpring.generated.h"

namespace bs
{
	ScriptLimitLinear::ScriptLimitLinear(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptLimitLinear::initRuntimeData()
	{ }

	MonoObject*ScriptLimitLinear::box(const __LimitLinearInterop& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	__LimitLinearInterop ScriptLimitLinear::unbox(MonoObject* value)
	{
		return *(__LimitLinearInterop*)MonoUtil::unbox(value);
	}

	LimitLinear ScriptLimitLinear::fromInterop(const __LimitLinearInterop& value)
	{
		LimitLinear output;
		output.extent = value.extent;
		output.contactDist = value.contactDist;
		output.restitution = value.restitution;
		output.spring = value.spring;

		return output;
	}

	__LimitLinearInterop ScriptLimitLinear::toInterop(const LimitLinear& value)
	{
		__LimitLinearInterop output;
		output.extent = value.extent;
		output.contactDist = value.contactDist;
		output.restitution = value.restitution;
		output.spring = value.spring;

		return output;
	}

}
