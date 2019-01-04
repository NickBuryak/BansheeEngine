using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Audio
	 *  @{
	 */

	/// <summary>
	/// Represents a listener that hears audio sources. For spatial audio the volume and pitch of played audio is determined 
	/// by the distance, orientation and velocity differences between the source and the listener.
	/// </summary>
	[ShowInInspector]
	public partial class AudioListener : Component
	{
		private AudioListener(bool __dummy0) { }
		protected AudioListener() { }

	}

	/** @} */
}
