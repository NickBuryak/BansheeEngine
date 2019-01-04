using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Controls how are particle positions on a shape chosen.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ParticleEmissionMode
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static ParticleEmissionMode Default()
		{
			ParticleEmissionMode value = new ParticleEmissionMode();
			value.type = ParticleEmissionModeType.Random;
			value.speed = 1f;
			value.interval = 0f;

			return value;
		}

		/// <summary>Type that determines general behaviour.</summary>
		public ParticleEmissionModeType type;
		/// <summary>
		/// Speed along which particle generation should move around the shape, relevant for Loop and PingPing emission  modes.
		/// </summary>
		public float speed;
		/// <summary>
		/// Determines the minimum interval allowed between the generated particles. 0 specifies the particles can be  generated 
		/// anywhere on the shape.
		/// </summary>
		public float interval;
	}

	/** @} */
}
