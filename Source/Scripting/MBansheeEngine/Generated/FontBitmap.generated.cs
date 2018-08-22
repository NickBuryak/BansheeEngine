using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup GUI_Engine
	 *  @{
	 */

	/// <summary>Contains textures and data about every character for a bitmap font of a specific size.</summary>
	public partial class FontBitmap : ScriptObject
	{
		private FontBitmap(bool __dummy0) { }
		protected FontBitmap() { }

		/// <summary>Font size for which the data is contained.</summary>
		public uint Size
		{
			get { return Internal_getsize(mCachedPtr); }
			set { Internal_setsize(mCachedPtr, value); }
		}

		/// <summary>Y offset to the baseline on which the characters are placed, in pixels.</summary>
		public int BaselineOffset
		{
			get { return Internal_getbaselineOffset(mCachedPtr); }
			set { Internal_setbaselineOffset(mCachedPtr, value); }
		}

		/// <summary>Height of a single line of the font, in pixels.</summary>
		public uint LineHeight
		{
			get { return Internal_getlineHeight(mCachedPtr); }
			set { Internal_setlineHeight(mCachedPtr, value); }
		}

		/// <summary>Character to use when data for a character is missing.</summary>
		public CharDesc MissingGlyph
		{
			get
			{
				CharDesc temp;
				Internal_getmissingGlyph(mCachedPtr, out temp);
				return temp;
			}
			set { Internal_setmissingGlyph(mCachedPtr, ref value); }
		}

		/// <summary>Width of a space in pixels.</summary>
		public uint SpaceWidth
		{
			get { return Internal_getspaceWidth(mCachedPtr); }
			set { Internal_setspaceWidth(mCachedPtr, value); }
		}

		/// <summary>Textures in which the character's pixels are stored.</summary>
		public RRef<Texture>[] TexturePages
		{
			get { return Internal_gettexturePages(mCachedPtr); }
			set { Internal_settexturePages(mCachedPtr, value); }
		}

		/// <summary>Returns a character description for the character with the specified Unicode key.</summary>
		public CharDesc GetCharDesc(uint charId)
		{
			CharDesc temp;
			Internal_getCharDesc(mCachedPtr, charId, out temp);
			return temp;
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_getCharDesc(IntPtr thisPtr, uint charId, out CharDesc __output);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern uint Internal_getsize(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setsize(IntPtr thisPtr, uint value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern int Internal_getbaselineOffset(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setbaselineOffset(IntPtr thisPtr, int value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern uint Internal_getlineHeight(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setlineHeight(IntPtr thisPtr, uint value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_getmissingGlyph(IntPtr thisPtr, out CharDesc __output);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setmissingGlyph(IntPtr thisPtr, ref CharDesc value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern uint Internal_getspaceWidth(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setspaceWidth(IntPtr thisPtr, uint value);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern RRef<Texture>[] Internal_gettexturePages(IntPtr thisPtr);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_settexturePages(IntPtr thisPtr, RRef<Texture>[] value);
	}

	/** @} */
}
