using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Animation
	 *  @{
	 */

	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct KeyframeRef
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static KeyframeRef Default()
		{
			KeyframeRef value = new KeyframeRef();
			value.curveIdx = 0;
			value.keyIdx = 0;

			return value;
		}

		public KeyframeRef(int curveIdx, int keyIdx)
		{
			this.curveIdx = curveIdx;
			this.keyIdx = keyIdx;
		}

		public int curveIdx;
		public int keyIdx;
	}

	/** @} */
}
