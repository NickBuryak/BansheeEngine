using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>References a subset of surfaces within a texture.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct TextureSurface
	{
		public TextureSurface(uint mipLevel = 0, uint numMipLevels = 1, uint face = 0, uint numFaces = 1)
		{
			this.mipLevel = mipLevel;
			this.numMipLevels = numMipLevels;
			this.face = face;
			this.numFaces = numFaces;
		}

		/// <summary>First mip level to reference.</summary>
		public uint mipLevel;
		/// <summary>Number of mip levels to reference. Must be greater than zero.</summary>
		public uint numMipLevels;
		/// <summary>
		/// First face to reference. Face can represent a single cubemap face, or a single array entry in a  texture array. If 
		/// cubemaps are laid out in a texture array then every six sequential faces represent a single array entry.
		/// </summary>
		public uint face;
		/// <summary>Number of faces to reference, if the texture has more than one.</summary>
		public uint numFaces;
	}

	/** @} */
}
