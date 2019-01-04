using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Render target is a frame buffer or a texture that the render system renders the scene to.</summary>
	[ShowInInspector]
	public partial class RenderTarget : ScriptObject
	{
		private RenderTarget(bool __dummy0) { }
		protected RenderTarget() { }

		/// <summary>Width of the render target, in pixels.</summary>
		[ShowInInspector]
		[NativeWrapper]
		public uint Width
		{
			get { return Internal_getWidth(mCachedPtr); }
		}

		/// <summary>Height of the render target, in pixels.</summary>
		[ShowInInspector]
		[NativeWrapper]
		public uint Height
		{
			get { return Internal_getHeight(mCachedPtr); }
		}

		/// <summary>True if pixels written to the render target will be gamma corrected.</summary>
		[ShowInInspector]
		[NativeWrapper]
		public bool GammaCorrection
		{
			get { return Internal_getGammaCorrection(mCachedPtr); }
		}

		/// <summary>
		/// Controls in what order is the render target rendered to compared to other render targets. Targets with higher  
		/// priority will be rendered before ones with lower priority.
		/// </summary>
		[ShowInInspector]
		[NativeWrapper]
		public int Priority
		{
			get { return Internal_getPriority(mCachedPtr); }
			set { Internal_setPriority(mCachedPtr, value); }
		}

		/// <summary>Controls how many samples are used for multisampling. (0 or 1 if multisampling is not used).</summary>
		[ShowInInspector]
		[NativeWrapper]
		public uint SampleCount
		{
			get { return Internal_getSampleCount(mCachedPtr); }
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern uint Internal_getWidth(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern uint Internal_getHeight(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern bool Internal_getGammaCorrection(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int Internal_getPriority(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setPriority(IntPtr thisPtr, int priority);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern uint Internal_getSampleCount(IntPtr thisPtr);
	}

	/** @} */
}
