using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>
	/// Contains definitions of GPU programs used for rendering, as well as a set of global parameters to control those  
	/// programs.
	/// </summary>
	[ShowInInspector]
	public partial class Shader : Resource
	{
		private Shader(bool __dummy0) { }
		protected Shader() { }

		/// <summary>Returns a reference wrapper for this resource.</summary>
		public RRef<Shader> Ref
		{
			get { return Internal_GetRef(mCachedPtr); }
		}

		/// <summary>Returns information about all parameters available in the shader.</summary>
		[ShowInInspector]
		[NativeWrapper]
		public ShaderParameter[] Parameters
		{
			get { return Internal_getParameters(mCachedPtr); }
		}

		/// <summary>Returns a reference wrapper for this resource.</summary>
		public static implicit operator RRef<Shader>(Shader x)
		{ return Internal_GetRef(x.mCachedPtr); }

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern RRef<Shader> Internal_GetRef(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern ShaderParameter[] Internal_getParameters(IntPtr thisPtr);
	}

	/** @} */
}
