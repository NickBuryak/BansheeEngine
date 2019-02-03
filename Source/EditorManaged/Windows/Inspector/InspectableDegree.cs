﻿//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2019 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspector
     *  @{
     */

    /// <summary>
    /// Displays GUI for a serializable property containing a value in degrees.
    /// </summary>
    [CustomInspector(typeof(Degree))]
    public class InspectableDegree : InspectableField
    {
        private GUIFloatField guiFloatField;
        private InspectableState state;
        private InspectableFieldStyleInfo style;

        /// <summary>
        /// Creates a new inspectable float GUI for the specified property.
        /// </summary>
        /// <param name="parent">Parent Inspector this field belongs to.</param>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="path">Full path to this property (includes name of this property and all parent properties).</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the field whose contents to display.</param>
        /// <param name="style">Information that can be used for customizing field rendering and behaviour.</param>
        public InspectableDegree(Inspector parent, string title, string path, int depth, InspectableFieldLayout layout,
            SerializableProperty property, InspectableFieldStyleInfo style)
            : base(parent, title, path, SerializableProperty.FieldType.Object, depth, layout, property)
        {
            this.style = style;
        }

        /// <inheritoc/>
        protected internal override void Initialize(int layoutIndex)
        {
            if (property != null)
            {
                guiFloatField = new GUIFloatField(new GUIContent(title));
                if (style != null)
                {
                    if (style.StepStyle != null && style.StepStyle.Step != 0)
                        guiFloatField.Step = style.StepStyle.Step;
                    if (style.RangeStyle != null)
                        guiFloatField.SetRange(style.RangeStyle.Min, style.RangeStyle.Max);
                }
                guiFloatField.OnChanged += OnFieldValueChanged;
                guiFloatField.OnConfirmed += OnFieldValueConfirm;
                guiFloatField.OnFocusLost += OnFieldValueConfirm;

                layout.AddElement(layoutIndex, guiFloatField);
            }
        }

        /// <inheritdoc/>
        public override InspectableState Refresh(int layoutIndex)
        {
            if (guiFloatField != null && !guiFloatField.HasInputFocus)
                guiFloatField.Value = property.GetValue<Degree>().Degrees;

            InspectableState oldState = state;
            if (state.HasFlag(InspectableState.Modified))
                state = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Triggered when the user inputs a new floating point value.
        /// </summary>
        /// <param name="newValue">New value of the float field.</param>
        private void OnFieldValueChanged(float newValue)
        {
            property.SetValue(new Degree(newValue));
            state |= InspectableState.ModifyInProgress;
        }

        /// <summary>
        /// Triggered when the user confirms input in the float field.
        /// </summary>
        private void OnFieldValueConfirm()
        {
            if (state.HasFlag(InspectableState.ModifyInProgress))
                state |= InspectableState.Modified;
        }
    }

    /** @} */
}
