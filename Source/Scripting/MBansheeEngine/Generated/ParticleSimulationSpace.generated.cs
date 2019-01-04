using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Space in which to spawn/transform particles.</summary>
	public enum ParticleSimulationSpace
	{
		/// <summary>
		/// Particles will always remain local to their transform parent. This means if the transform parent moves so will all 
		/// the particles.
		/// </summary>
		Local = 0,
		/// <summary>
		/// Particles will be placed in world space. This means they will spawn at the location of the transform parent, but are 
		/// no longer affected by its transform after spawn (e.g. smoke rising from a moving train).
		/// </summary>
		World = 1
	}

	/** @} */
}
