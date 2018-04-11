using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Specifies axes that the D6 joint can constrain motion on.</summary>
	public enum D6JointAxis
	{
		/// <summary>Movement on the X axis.</summary>
		X = 0,
		/// <summary>Movement on the Y axis.</summary>
		Y = 1,
		/// <summary>Movement on the Z axis.</summary>
		Z = 2,
		/// <summary>Rotation around the X axis.</summary>
		Twist = 3,
		/// <summary>Rotation around the Y axis.</summary>
		SwingY = 4,
		/// <summary>Rotation around the Z axis.</summary>
		SwingZ = 5,
		Count = 6
	}

	/** @} */
}
