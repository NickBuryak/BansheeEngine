//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/GUI/BsScriptGUITextureField.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsGUITextureField.h"
#include "GUI/BsGUIOptions.h"
#include "GUI/BsGUIContent.h"
#include "BsScriptResourceManager.h"
#include "Resources/BsResources.h"

#include "Generated/BsScriptGUIContent.generated.h"
#include "Generated/BsScriptTexture.generated.h"
#include "Wrappers/BsScriptRRefBase.h"

using namespace std::placeholders;

namespace bs
{
	ScriptGUITextureField::OnChangedThunkDef ScriptGUITextureField::onChangedThunk;

	ScriptGUITextureField::ScriptGUITextureField(MonoObject* instance, GUITextureField* resourceField)
		:TScriptGUIElement(instance, resourceField)
	{

	}

	void ScriptGUITextureField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", (void*)&ScriptGUITextureField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", (void*)&ScriptGUITextureField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", (void*)&ScriptGUITextureField::internal_setValue);
		metaData.scriptClass->addInternalCall("Internal_GetValueRef", (void*)&ScriptGUITextureField::internal_getValueRef);
		metaData.scriptClass->addInternalCall("Internal_SetValueRef", (void*)&ScriptGUITextureField::internal_setValueRef);
		metaData.scriptClass->addInternalCall("Internal_SetTint", (void*)&ScriptGUITextureField::internal_setTint);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("Internal_DoOnChanged", 1)->getThunk();
	}

	void ScriptGUITextureField::internal_createInstance(MonoObject* instance, __GUIContentInterop* title, UINT32 titleWidth,
		MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		ScriptArray scriptArray(guiOptions);
		UINT32 arrayLen = scriptArray.size();
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(scriptArray.get<GUIOption>(i));

		String styleName = MonoUtil::monoToString(style);

		GUITextureField* guiTextureField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent = ScriptGUIContent::fromInterop(*title);
			guiTextureField = GUITextureField::create(nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiTextureField = GUITextureField::create(options, styleName);
		}

		auto nativeInstance = new (bs_alloc<ScriptGUITextureField>()) ScriptGUITextureField(instance, guiTextureField);
		guiTextureField->onValueChanged.connect(std::bind(&ScriptGUITextureField::onChanged, nativeInstance, _1));
	}

	void ScriptGUITextureField::internal_getValue(ScriptGUITextureField* nativeInstance, MonoObject** output)
	{
		GUITextureField* textureField = static_cast<GUITextureField*>(nativeInstance->getGUIElement());

		HTexture resource = textureField->getValue();
		if(resource)
		{
			const ResourceLoadFlags loadFlags = ResourceLoadFlag::Default | ResourceLoadFlag::KeepSourceData;
			resource = static_resource_cast<Texture>(Resources::instance().loadFromUUID(resource.getUUID(), false, loadFlags));
		}

		MonoUtil::referenceCopy(output, nativeToManagedResource(resource));
	}

	void ScriptGUITextureField::internal_setValue(ScriptGUITextureField* nativeInstance, MonoObject* value)
	{
		GUITextureField* textureField = static_cast<GUITextureField*>(nativeInstance->getGUIElement());

		if (value == nullptr)
			textureField->setValue(HTexture());
		else
		{
			ScriptTexture* scriptTexture = ScriptTexture::toNative(value);
			textureField->setValue(static_resource_cast<Texture>(scriptTexture->getGenericHandle()));
		}
	}

	void ScriptGUITextureField::internal_getValueRef(ScriptGUITextureField* nativeInstance, MonoObject** output)
	{
		GUITextureField* textureField = static_cast<GUITextureField*>(nativeInstance->getGUIElement());

		HTexture resource = textureField->getValue();
		ScriptRRefBase* scriptRRef = ScriptResourceManager::instance().getScriptRRef(resource);

		if(scriptRRef)
			MonoUtil::referenceCopy(output, scriptRRef->getManagedInstance());
		else
			MonoUtil::referenceCopy(output, nullptr);
	}

	void ScriptGUITextureField::internal_setValueRef(ScriptGUITextureField* nativeInstance, MonoObject* value)
	{
		GUITextureField* textureField = static_cast<GUITextureField*>(nativeInstance->getGUIElement());

		if (value == nullptr)
			textureField->setValue(HTexture());
		else
		{
			ScriptRRefBase* scriptRRef = ScriptRRefBase::toNative(value);
			textureField->setValue(static_resource_cast<Texture>(scriptRRef->getHandle()));
		}
	}

	void ScriptGUITextureField::internal_setTint(ScriptGUITextureField* nativeInstance, Color* color)
	{
		GUITextureField* textureField = (GUITextureField*)nativeInstance->getGUIElement();
		textureField->setTint(*color);
	}

	void ScriptGUITextureField::onChanged(const HTexture& newHandle)
	{
		ScriptRRefBase* scriptRRef = ScriptResourceManager::instance().getScriptRRef(newHandle);

		MonoObject* managedObj = nullptr;
		if(scriptRRef)
			managedObj = scriptRRef->getManagedInstance();

		MonoUtil::invokeThunk(onChangedThunk, getManagedInstance(), managedObj);
	}

	MonoObject* ScriptGUITextureField::nativeToManagedResource(const HTexture& instance)
	{
		if (instance == nullptr)
			return nullptr;

		ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(instance, true);
		return scriptResource->getManagedInstance();
	}
}