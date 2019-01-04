﻿//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Flags that are used to define properties of a single field in a managed object.
    /// </summary>
    public enum SerializableFieldAttributes // Note: Must match C++ enum ScriptFieldFlags
    {
        /// <summary>
        /// Field will be automatically serialized.
        /// </summary>
        Serializable  = 1 << 0,

        /// <summary>
        /// Field will be visible in the default inspector.
        /// </summary>
        Inspectable   = 1 << 1,

        /// <summary>
        /// Integer or floating point field with min/max range.
        /// </summary>
        Ranged        = 1 << 2,

        /// <summary>
        /// Integer or floating point field with a minimum increment/decrement step.
        /// </summary>
        Stepped       = 1 << 3,

        /// <summary>
        /// Field can be animated through the animation window.
        /// </summary>
        Animable      = 1 << 4,

        /// <summary>
        /// Integer field rendered as a layer selection dropdown.
        /// </summary>
        LayerMask     = 1 << 5,

        /// <summary>
        /// Field containing a reference type being passed by copy instead of by reference.
        /// </summary>
        PassByCopy    = 1 << 6,

        /// <summary>
        /// Field containing a reference type that should never be null.
        /// </summary>
        NotNull       = 1 << 7,

        /// <summary>
        /// Field represents a property that wraps a native object. Getters and setters of such a property issue calls into
        /// native code to update the native object.
        /// </summary>
        NativeWrapper = 1 << 8,

        /// <summary>
        /// When a field changes those changes need to be applied to the parent object by calling the field setter. Only
        /// applicable to properties containing reference types.
        /// </summary>
        ApplyOnDirty = 1 << 9
    }

    /// <summary>
    /// Allows you to access meta-data about field in an object. Similar to Reflection but simpler and faster.
    /// </summary>
    public class SerializableField : ScriptObject
    {
        private SerializableObject parent;
        private SerializableProperty.FieldType type;
        private int flags;
        private Type internalType;
        private string name;

        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        /// <param name="parent">Object that conains the field.</param>
        /// <param name="name">Name of the field.</param>
        /// <param name="flags">Flags that control whether the field is inspectable or serializable.</param>
        /// <param name="internalType">Internal C# type of the field.</param>
        private SerializableField(SerializableObject parent, string name, int flags, Type internalType)
        {
            this.parent = parent;
            this.name = name;
            this.flags = flags;
            this.type = SerializableProperty.DetermineFieldType(internalType);
            this.internalType = internalType;
        }

        public SerializableFieldAttributes Flags
        {
            get { return (SerializableFieldAttributes) flags; }
        }

        /// <summary>
        /// Returns the type of data contained in the field.
        /// </summary>
        public SerializableProperty.FieldType Type
        {
            get { return type; }
        }

        /// <summary>
        /// Returns the actual type of the object contained in the field.
        /// </summary>
        public Type InternalType
        {
            get { return internalType; }
        }

        /// <summary>
        /// Returns the name of the field.
        /// </summary>
        public string Name
        {
            get { return name; }
        }

        /// <summary>
        /// Returns the requested style of the field, that may be used for controlling how the field is displayed and how
        /// is its input filtered.
        /// </summary>
        public SerializableFieldStyle Style
        {
            get
            {
                SerializableFieldStyle style;
                Internal_GetStyle(mCachedPtr, out style);
                return style;
            }
        }

        /// <summary>
        /// Returns a serializable property for the field. 
        /// </summary>
        /// <returns>Serializable property that allows you to manipulate contents of the field.</returns>
        public SerializableProperty GetProperty()
        {
            SerializableProperty.Getter getter = () =>
            {
                object parentObject = parent.GetReferencedObject();
                
                if (parentObject != null)
                    return Internal_GetValue(mCachedPtr, parentObject);
                else
                    return null;
            };

            SerializableProperty.Setter setter = (object value) =>
            {
                object parentObject = parent.GetReferencedObject();

                if (parentObject != null)
                {
                    Internal_SetValue(mCachedPtr, parentObject, value);

                    // If value type we cannot just modify the parent object because it's just a copy
                    if (parentObject.GetType().IsValueType && parent.parentProperty != null)
                        parent.parentProperty.SetValue(parentObject);
                }
            };

            SerializableProperty newProperty = Internal_CreateProperty(mCachedPtr);
            newProperty.Construct(type, internalType, getter, setter);

            return newProperty;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateProperty(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_GetValue(IntPtr nativeInstance, object instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, object instance, object value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetStyle(IntPtr nativeInstance, out SerializableFieldStyle style);
    }

    /// <summary>
    /// Contains information about a style of a serializable field.
    /// </summary>
    public struct SerializableFieldStyle // Note: Must match C++ struct SerializableMemberStyle
    {
        /// <summary>
        /// True if the range of the field is limited, false if unlimited.
        /// </summary>
        public bool HasRange;
        /// <summary>
        /// Returns the lower bound of the range. Only relevant if <see cref="HasRange"/> is true.
        /// </summary>
        public float RangeMin;
        /// <summary>
        /// Returns the upper bound of the range. Only relevant if <see cref="HasRange"/> is true.
        /// </summary>
        public float RangeMax;
        /// <summary>
        /// True if the field value can only be incremented in specific increments.
        /// </summary>
        public bool HasStep;
        /// <summary>
        /// Minimum increment the field value can be increment/decremented by. Only relevant if <see cref="HasStep"/> is true.
        /// </summary>
        public float StepIncrement;
        /// <summary>
        /// If true, number fields will be displayed as sliders instead of regular input boxes.
        /// </summary>
        public bool DisplayAsSlider; 
        
        /// <summary>
        /// If true, 64-bit fields will be displayed as a layer mask drop down menu.
        /// </summary>
        public bool DisplayAsLayerMask;
    }

    /** @} */
}
