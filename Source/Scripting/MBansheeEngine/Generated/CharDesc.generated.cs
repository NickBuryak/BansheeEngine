using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup GUI_Engine
	 *  @{
	 */

	/// <summary>Describes a single character in a font of a specific size.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct CharDesc
	{
		/// <summary>Character ID, corresponding to a Unicode key.</summary>
		public uint charId;
		/// <summary>Index of the texture the character is located on.</summary>
		public uint page;
		/// <summary>Texture coordinates of the character in the page texture.</summary>
		public float uvX;
		/// <summary>Texture coordinates of the character in the page texture.</summary>
		public float uvY;
		/// <summary>Width/height of the character in texture coordinates.</summary>
		public float uvWidth;
		/// <summary>Width/height of the character in texture coordinates.</summary>
		public float uvHeight;
		/// <summary>Width/height of the character in pixels.</summary>
		public uint width;
		/// <summary>Width/height of the character in pixels.</summary>
		public uint height;
		/// <summary>Offset for the visible portion of the character in pixels.</summary>
		public int xOffset;
		/// <summary>Offset for the visible portion of the character in pixels.</summary>
		public int yOffset;
		/// <summary>Determines how much to advance the pen after writing this character, in pixels.</summary>
		public int xAdvance;
		/// <summary>Determines how much to advance the pen after writing this character, in pixels.</summary>
		public int yAdvance;
		/// <summary>
		/// Pairs that determine if certain character pairs should be closer or father together. for example "AV" combination.
		/// </summary>
		public KerningPair[] kerningPairs;
	}

	/** @} */
}
