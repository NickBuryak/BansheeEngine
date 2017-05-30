//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptComponent.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptObjectManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptSceneObject.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedComponent.h"
#include "BsSceneObject.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptComponentBase::ScriptComponentBase(MonoObject* instance)
		:ScriptGameObjectBase(instance)
	{ }

	void ScriptComponentBase::destroy()
	{
		mManagedInstance = nullptr;

		// It's possible that managed component is destroyed but a reference to it is still kept during assembly refresh. 
		// Such components shouldn't be restored so we delete them.

		HComponent component = getComponent();
		if (!mRefreshInProgress || component.isDestroyed(true))
			ScriptGameObjectManager::instance().destroyScriptComponent(this);
	}

	bool ScriptComponentBase::checkIfDestroyed(const GameObjectHandleBase& handle)
	{
		if (handle.isDestroyed())
		{
			LOGWRN("Trying to access a destroyed GameObject with instance ID: " + toString(handle.getInstanceId()));
			return true;
		}

		return false;
	}

	ScriptComponent::ScriptComponent(MonoObject* instance)
		:ScriptObject(instance)
	{ 
		assert(instance != nullptr);
	}

	void ScriptComponent::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_AddComponent", &ScriptComponent::internal_addComponent);
		metaData.scriptClass->addInternalCall("Internal_GetComponent", &ScriptComponent::internal_getComponent);
		metaData.scriptClass->addInternalCall("Internal_GetComponents", &ScriptComponent::internal_getComponents);
		metaData.scriptClass->addInternalCall("Internal_GetComponentsPerType", &ScriptComponent::internal_getComponentsPerType);
		metaData.scriptClass->addInternalCall("Internal_RemoveComponent", &ScriptComponent::internal_removeComponent);
		metaData.scriptClass->addInternalCall("Internal_GetSceneObject", &ScriptComponent::internal_getSceneObject);
		metaData.scriptClass->addInternalCall("Internal_GetNotifyFlags", &ScriptComponent::internal_getNotifyFlags);
		metaData.scriptClass->addInternalCall("Internal_SetNotifyFlags", &ScriptComponent::internal_setNotifyFlags);
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptComponent::internal_destroy);
	}

	MonoObject* ScriptComponent::internal_addComponent(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		if (checkIfDestroyed(so))
			return nullptr;

		ScriptAssemblyManager& sam = ScriptAssemblyManager::instance();

		MonoClass* managedComponent = sam.getManagedComponentClass();
		::MonoClass* requestedClass = MonoUtil::getClass(type);

		bool isManagedComponent = MonoUtil::isSubClassOf(requestedClass, managedComponent->_getInternalClass());
		if(isManagedComponent)
		{
			GameObjectHandle<ManagedComponent> mc = so->addComponent<ManagedComponent>(type);
			return mc->getManagedInstance();
		}
		else
		{
			BuiltinComponentInfo* info = sam.getBuiltinComponentInfo(type);
			if (info == nullptr)
				return nullptr;

			HComponent component = so->addComponent(info->typeId);
			ScriptComponentBase* scriptComponent = 
				ScriptGameObjectManager::instance().createBuiltinScriptComponent(component);

			return scriptComponent->getManagedInstance();
		}
	}

	MonoObject* ScriptComponent::internal_getComponent(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		if (checkIfDestroyed(so))
			return nullptr;

		::MonoClass* baseClass = MonoUtil::getClass(type);

		const Vector<HComponent>& mComponents = so->getComponents();
		for(auto& component : mComponents)
		{
			if(component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				MonoReflectionType* componentReflType = managedComponent->getRuntimeType();
				::MonoClass* componentClass = MonoUtil::getClass(componentReflType);
				
				if(MonoUtil::isSubClassOf(componentClass, baseClass))
				{
					return managedComponent->getManagedInstance();
				}
			}
			else
			{
				ScriptComponentBase* scriptComponent = ScriptGameObjectManager::instance().getBuiltinScriptComponent(component);
				return scriptComponent->getManagedInstance();
			}
		}

		return nullptr;
	}

	MonoArray* ScriptComponent::internal_getComponentsPerType(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		::MonoClass* baseClass = MonoUtil::getClass(type);
		Vector<MonoObject*> managedComponents;

		if (!checkIfDestroyed(so))
		{
			const Vector<HComponent>& mComponents = so->getComponents();
			for (auto& component : mComponents)
			{
				if (component->getTypeId() == TID_ManagedComponent)
				{
					GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

					MonoReflectionType* componentReflType = managedComponent->getRuntimeType();
					::MonoClass* componentClass = MonoUtil::getClass(componentReflType);

					if (MonoUtil::isSubClassOf(componentClass, baseClass))
						managedComponents.push_back(managedComponent->getManagedInstance());
				}
				else
				{
					ScriptComponentBase* scriptComponent = ScriptGameObjectManager::instance().getBuiltinScriptComponent(component);
					managedComponents.push_back(scriptComponent->getManagedInstance());
				}
			}
		}

		ScriptArray scriptArray(metaData.scriptClass->_getInternalClass(), (UINT32)managedComponents.size());
		for (UINT32 i = 0; i < (UINT32)managedComponents.size(); i++)
			scriptArray.set(i, managedComponents[i]);

		return scriptArray.getInternal();
	}

	MonoArray* ScriptComponent::internal_getComponents(MonoObject* parentSceneObject)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		Vector<MonoObject*> managedComponents;

		if (!checkIfDestroyed(so))
		{
			const Vector<HComponent>& mComponents = so->getComponents();
			for (auto& component : mComponents)
			{
				if (component->getTypeId() == TID_ManagedComponent)
				{
					GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

					managedComponents.push_back(managedComponent->getManagedInstance());
				}
				else
				{
					ScriptComponentBase* scriptComponent = ScriptGameObjectManager::instance().getBuiltinScriptComponent(component);
					if(scriptComponent != nullptr)
						managedComponents.push_back(scriptComponent->getManagedInstance());
				}
			}
		}

		ScriptArray scriptArray(metaData.scriptClass->_getInternalClass(), (UINT32)managedComponents.size());
		for(UINT32 i = 0; i < (UINT32)managedComponents.size(); i++)
			scriptArray.set(i, managedComponents[i]);

		return scriptArray.getInternal();
	}

	void ScriptComponent::internal_removeComponent(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		if (checkIfDestroyed(so))
			return;

		::MonoClass* baseClass = MonoUtil::getClass(type);

		const Vector<HComponent>& mComponents = so->getComponents();
		for(auto& component : mComponents)
		{
			if (component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				MonoReflectionType* componentReflType = managedComponent->getRuntimeType();
				::MonoClass* componentClass = MonoUtil::getClass(componentReflType);

				if (MonoUtil::isSubClassOf(componentClass, baseClass))
				{
					managedComponent->destroy();
					return;
				}
			}
			else
				component->destroy();
		}

		LOGWRN("Attempting to remove a component that doesn't exists on SceneObject \"" + so->getName() + "\"");
	}

	MonoObject* ScriptComponent::internal_getSceneObject(ScriptComponentBase* nativeInstance)
	{
		HComponent component = nativeInstance->getComponent();
		if (checkIfDestroyed(component))
			return nullptr;

		HSceneObject sceneObject = component->sceneObject();

		ScriptSceneObject* scriptSO = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(sceneObject);

		assert(scriptSO->getManagedInstance() != nullptr);
		return scriptSO->getManagedInstance();
	}

	TransformChangedFlags ScriptComponent::internal_getNotifyFlags(ScriptComponentBase* nativeInstance)
	{
		HComponent component = nativeInstance->getComponent();

		if (!checkIfDestroyed(component))
			return component->_getNotifyFlags();

		return TCF_None;
	}

	void ScriptComponent::internal_setNotifyFlags(ScriptComponentBase* nativeInstance, TransformChangedFlags flags)
	{
		HComponent component = nativeInstance->getComponent();

		if (!checkIfDestroyed(component))
			component->setNotifyFlags(flags);
	}
	
	void ScriptComponent::internal_destroy(ScriptComponentBase* nativeInstance, bool immediate)
	{
		HComponent component = nativeInstance->getComponent();

		if (!checkIfDestroyed(component))
			component->destroy(immediate);
	}
}