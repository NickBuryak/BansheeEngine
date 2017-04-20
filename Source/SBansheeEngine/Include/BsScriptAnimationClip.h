//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsAnimationClip.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for AnimationClip. */
	class BS_SCR_BE_EXPORT ScriptAnimationClip : public TScriptResource<ScriptAnimationClip, AnimationClip>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "AnimationClip")

		/**	Creates an empty, uninitialized managed instance of the resource interop object. */
		static MonoObject* createInstance();

	private:
		friend class ScriptResourceManager;
		friend class BuiltinResourceTypes;

		ScriptAnimationClip(MonoObject* instance, const HAnimationClip& animationClip);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance);
		static MonoObject* internal_GetAnimationCurves(ScriptAnimationClip* thisPtr);
		static void internal_SetAnimationCurves(ScriptAnimationClip* thisPtr, MonoObject* curves);
		static MonoArray* internal_GetAnimationEvents(ScriptAnimationClip* thisPtr);
		static void internal_SetAnimationEvents(ScriptAnimationClip* thisPtr, MonoArray* events);
		static MonoObject* internal_GetRootMotion(ScriptAnimationClip* thisPtr);
		static float internal_GetLength(ScriptAnimationClip* thisPtr);
		static UINT32 internal_GetSampleRate(ScriptAnimationClip* thisPtr);
		static void internal_SetSampleRate(ScriptAnimationClip* thisPtr, UINT32 sampleRate);
	};

	/**	Interop class between C++ & CLR for AnimationEvent. */
	class BS_SCR_BE_EXPORT ScriptAnimationEvent : public ScriptObject<ScriptAnimationEvent>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "AnimationEvent")

		/** Converts managed animation event its native counterpart. */
		static AnimationEvent toNative(MonoObject* object);

		/** Converts native animation event to its managed counterpart. */
		static MonoObject* toManaged(const AnimationEvent& event);
	private:
		ScriptAnimationEvent(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoField* sNameField;
		static MonoField* sTimeField;
	};

	/** @} */
}