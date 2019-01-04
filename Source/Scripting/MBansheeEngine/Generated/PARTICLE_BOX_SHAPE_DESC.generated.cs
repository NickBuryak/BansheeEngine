using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Particles
	 *  @{
	 */

	/// <summary>Information describing a ParticleEmitterBoxShape.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct ParticleBoxShapeOptions
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static ParticleBoxShapeOptions Default()
		{
			ParticleBoxShapeOptions value = new ParticleBoxShapeOptions();
			value.type = ParticleEmitterBoxType.Volume;
			value.extents = new Vector3();

			return value;
		}

		/// <summary>Determines from which portion of the box should particles be emitted from.</summary>
		public ParticleEmitterBoxType type;
		/// <summary>Extends of the box.</summary>
		public Vector3 extents;
	}

	/** @} */
}
