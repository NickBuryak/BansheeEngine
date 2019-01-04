using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Structure used for initializing a ParticleGravity object.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ParticleGravityOptions
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static ParticleGravityOptions Default()
		{
			ParticleGravityOptions value = new ParticleGravityOptions();
			value.scale = 1f;

			return value;
		}

		/// <summary>Scale which to apply to the gravity value retrieved from the physics sub-system.</summary>
		public float scale;
	}

	/** @} */
}
