#include "BsScriptSkeleton.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "../../../bsf/Source/Foundation/bsfCore/Animation/BsSkeleton.h"
#include "BsScriptSkeletonBoneInfoEx.generated.h"
#include "../../SBansheeEngine/Extensions/BsSkeletonEx.h"

namespace bs
{
	ScriptSkeleton::ScriptSkeleton(MonoObject* managedInstance, const SPtr<Skeleton>& value)
		:ScriptObject(managedInstance), mInternal(value)
	{
	}

	void ScriptSkeleton::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_getNumBones", (void*)&ScriptSkeleton::Internal_getNumBones);
		metaData.scriptClass->addInternalCall("Internal_getBoneInfo", (void*)&ScriptSkeleton::Internal_getBoneInfo);

	}

	MonoObject* ScriptSkeleton::create(const SPtr<Skeleton>& value)
	{
		if(value == nullptr) return nullptr; 

		bool dummy = false;
		void* ctorParams[1] = { &dummy };

		MonoObject* managedInstance = metaData.scriptClass->createInstance("bool", ctorParams);
		new (bs_alloc<ScriptSkeleton>()) ScriptSkeleton(managedInstance, value);
		return managedInstance;
	}
	uint32_t ScriptSkeleton::Internal_getNumBones(ScriptSkeleton* thisPtr)
	{
		uint32_t tmp__output;
		tmp__output = thisPtr->getInternal()->getNumBones();

		uint32_t __output;
		__output = tmp__output;

		return __output;
	}

	void ScriptSkeleton::Internal_getBoneInfo(ScriptSkeleton* thisPtr, int32_t boneIdx, __SkeletonBoneInfoExInterop* __output)
	{
		SkeletonBoneInfoEx tmp__output;
		tmp__output = SkeletonEx::getBoneInfo(thisPtr->getInternal(), boneIdx);

		__SkeletonBoneInfoExInterop interop__output;
		interop__output = ScriptSkeletonBoneInfoEx::toInterop(tmp__output);
		MonoUtil::valueCopy(__output, &interop__output, ScriptSkeletonBoneInfoEx::getMetaData()->scriptClass->_getInternalClass());
	}
}
