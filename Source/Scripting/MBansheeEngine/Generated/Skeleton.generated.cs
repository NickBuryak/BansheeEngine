using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Animation
	 *  @{
	 */

	/// <summary>Contains information about bones required for skeletal animation.</summary>
	[ShowInInspector]
	public partial class Skeleton : ScriptObject
	{
		private Skeleton(bool __dummy0) { }
		protected Skeleton() { }

		/// <summary>Returns the total number of bones in the skeleton.</summary>
		[ShowInInspector]
		[NativeWrapper]
		public uint NumBones
		{
			get { return Internal_getNumBones(mCachedPtr); }
		}

		/// <summary>Returns information about a bone at the provided index.</summary>
		/// <param name="boneIdx">Index of the bone to retrieve information for.</param>
		/// <returns>Information about the bone at the specified index.</returns>
		public BoneInfo GetBoneInfo(int boneIdx)
		{
			BoneInfo temp;
			Internal_getBoneInfo(mCachedPtr, boneIdx, out temp);
			return temp;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern uint Internal_getNumBones(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_getBoneInfo(IntPtr thisPtr, int boneIdx, out BoneInfo __output);
	}

	/** @} */
}
