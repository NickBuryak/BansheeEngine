using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Specifies a burst of particles that occurs at a certain time point.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ParticleBurst
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static ParticleBurst Default()
		{
			ParticleBurst value = new ParticleBurst();
			value.time = 0f;
			value.count = new FloatDistribution(0f);
			value.cycles = 1;
			value.interval = 1f;

			return value;
		}

		public ParticleBurst(float time, FloatDistribution count, uint cycles = 1, float interval = 1f)
		{
			this.time = time;
			this.count = count;
			this.cycles = cycles;
			this.interval = interval;
		}

		/// <summary>Time at which to trigger the burst, in seconds.</summary>
		public float time;
		/// <summary>Number of particles to emit when the burst triggers.</summary>
		public FloatDistribution count;
		/// <summary>
		/// Determines how many times to trigger the burst. If 0 the burst will trigger infinitely. Use <see cref="interval"/> to 
		/// to control the time between each cycle.
		/// </summary>
		public uint cycles;
		/// <summary>Controls how much time needs to pass before triggering another burst cycle, in seconds.</summary>
		public float interval;
	}

	/** @} */
}
