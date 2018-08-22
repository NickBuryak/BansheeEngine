using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>
	/// Descriptor that describes a simple sprite sheet animation. The parent texture is split into a grid of  <paramref 
	/// name="numRows"/> x <paramref name="numColumns"/>, each representing one frame of the animation. Every frame is of 
	/// equal size. Frames are sequentially evaluated starting from the top-most row, iterating over all columns in a row and 
	/// then moving to next row, up to <paramref name="count"/> frames. Frames in rows/colums past <paramref name="count"/>. 
	/// <paramref name="fps"/> frames are evaluated every second, allowing you to control animation speed.
	/// </summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct SpriteSheetGridAnimation
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static SpriteSheetGridAnimation Default()
		{
			SpriteSheetGridAnimation value = new SpriteSheetGridAnimation();
			value.numRows = 1;
			value.numColumns = 1;
			value.count = 1;
			value.fps = 8;

			return value;
		}

		public SpriteSheetGridAnimation(uint numRows, uint numColumns, uint count, uint fps)
		{
			this.numRows = numRows;
			this.numColumns = numColumns;
			this.count = count;
			this.fps = fps;
		}

		/// <summary>
		/// Number of rows to divide the parent's texture area. Determines height of the individual frame (depends on  parent 
		/// texture size).
		/// </summary>
		public uint numRows;
		/// <summary>
		/// Number of columns to divide the parent's texture area. Determines column of the individual frame (depends on  parent 
		/// texture size).
		/// </summary>
		public uint numColumns;
		/// <summary>
		/// Number of frames in the animation. Must be less or equal than <see cref="numRows"/> * <see cref="numColumns"/>.
		/// </summary>
		public uint count;
		/// <summary>How many frames to evaluate each second. Determines the animation speed.</summary>
		public uint fps;
	}

	/** @} */
}
