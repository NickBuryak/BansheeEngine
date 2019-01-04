using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Determines how to sort particles before rendering.</summary>
	public enum ParticleSortMode
	{
		/// <summary>Do not sort the particles.</summary>
		None = 0,
		/// <summary>Sort by distance from the camera, furthest to nearest.</summary>
		Distance = 1,
		/// <summary>Sort by age, oldest to youngest.</summary>
		OldToYoung = 2,
		/// <summary>Sort by age, youngest to oldest.</summary>
		YoungToOld = 3
	}

	/** @} */
}
