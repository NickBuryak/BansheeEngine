using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/// <summary>Flags that describe an animation curve.</summary>
	public enum AnimationCurveFlags
	{
		/// <summary>
		/// If enabled, the curve was imported from an external file and not created within the engine. This will affect how are 
		/// animation results applied to scene objects (with imported animations it is assumed the curve is animating bones and 
		/// with in-engine curves it is assumed the curve is animating scene objects).
		/// </summary>
		ImportedCurve = 1,
		/// <summary>
		/// Signifies the curve is used to animate between different frames within a morph channel. In range [0, 1].
		/// </summary>
		MorphFrame = 2,
		/// <summary>Signifies the curve is used to adjust the weight of a morph channel. In range [0, 1].</summary>
		MorphWeight = 4
	}
}
