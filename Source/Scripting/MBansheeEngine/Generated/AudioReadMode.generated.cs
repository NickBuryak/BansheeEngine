using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Audio
	 *  @{
	 */

	/// <summary>Modes that determine how and when is audio data read.</summary>
	public enum AudioReadMode
	{
		/// <summary>Entire audio clip will be loaded and decompressed. Uses most memory but has lowest CPU impact.</summary>
		LoadDecompressed = 0,
		/// <summary>
		/// Entire audio clip will be loaded, but will be decompressed while playing. Uses medium amount of memory and has the 
		/// highest CPU impact.
		/// </summary>
		LoadCompressed = 1,
		/// <summary>
		/// Audio will be slowly streamed from the disk, and decompressed if needed. Uses very little memory, and has either low 
		/// or high CPU impact depending if the audio is in a compressed format. Since data is streamed from the disk, read 
		/// speeds could also be a bottleneck.
		/// </summary>
		Stream = 2
	}

	/** @} */
}
