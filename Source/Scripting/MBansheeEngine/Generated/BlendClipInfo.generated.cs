using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Animation
	 *  @{
	 */

	/// <summary>Represents an animation clip used in 1D blending. Each clip has a position on the number line.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct BlendClipInfo
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static BlendClipInfo Default()
		{
			BlendClipInfo value = new BlendClipInfo();
			value.clip = null;
			value.position = 0f;

			return value;
		}

		public RRef<AnimationClip> clip;
		public float position;
	}

	/** @} */
}
