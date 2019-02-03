﻿//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Inspector
     *  @{
     */

    /// <summary>
    /// Default implementation of the inspector used when no specified inspector is provided for the type. Inspector 
    /// displays GUI for all the inspectable fields in the object.
    /// </summary>
    internal sealed class GenericInspector : Inspector
    {
        private bool isEmpty = true;
        private List<InspectableField> inspectableFields = new List<InspectableField>();

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            if (InspectedObject == null)
                LoadResource();

            if (InspectedObject != null)
            {
                SerializableObject serializableObject = new SerializableObject(InspectedObject.GetType(), InspectedObject);
                inspectableFields = InspectableField.CreateFields(serializableObject, this, "", 0, Layout);
                isEmpty = inspectableFields.Count == 0;

                base.SetVisible(!isEmpty);
            }
        }

        /// <inheritdoc/>
        protected internal override InspectableState Refresh()
        {
            InspectableState state = InspectableState.NotModified;

            int currentIndex = 0;
            foreach (var field in inspectableFields)
            {
                state |= field.Refresh(currentIndex);
                currentIndex += field.GetNumLayoutElements();
            }

            return state;
        }

        /// <inheritdoc/>
        internal override void SetVisible(bool visible)
        {
            base.SetVisible(!isEmpty && visible);
        }
    }

    /** @} */
}
