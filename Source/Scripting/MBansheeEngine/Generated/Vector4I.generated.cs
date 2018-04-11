using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Math
	 *  @{
	 */

	/// <summary>A four dimensional vector with integer coordinates.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct Vector4I
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static Vector4I Default()
		{
			Vector4I value = new Vector4I();
			value.x = 0;
			value.y = 0;
			value.z = 0;
			value.w = 0;

			return value;
		}

		public Vector4I(int x, int y, int z, int w)
		{
			this.x = x;
			this.y = y;
			this.z = z;
			this.w = w;
		}

		public int x;
		public int y;
		public int z;
		public int w;
	}

	/** @} */
}
