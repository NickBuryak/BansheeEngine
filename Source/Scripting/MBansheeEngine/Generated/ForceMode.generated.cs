using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>Type of force or torque that can be applied to a rigidbody.</summary>
	public enum ForceMode
	{
		/// <summary>Value applied is a force.</summary>
		Force = 0,
		/// <summary>Value applied is an impulse (a direct change in its linear or angular momentum).</summary>
		Impulse = 1,
		/// <summary>Value applied is velocity.</summary>
		Velocity = 2,
		/// <summary>Value applied is accelearation.</summary>
		Acceleration = 3
	}

	/** @} */
}
