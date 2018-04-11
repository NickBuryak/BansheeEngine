using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Rendering
	 *  @{
	 */

	/// <summary>Data about a sub-mesh range and the type of primitives contained in the range.</summary>
	[StructLayout(LayoutKind.Sequential), SerializeObject]
	public partial struct SubMesh
	{
		/// <summary>Initializes the struct with default values.</summary>
		public static SubMesh Default()
		{
			SubMesh value = new SubMesh();
			value.indexOffset = 0;
			value.indexCount = 0;
			value.drawOp = MeshTopology.TriangleList;

			return value;
		}

		public SubMesh(uint indexOffset, uint indexCount, MeshTopology drawOp)
		{
			this.indexOffset = indexOffset;
			this.indexCount = indexCount;
			this.drawOp = drawOp;
		}

		public uint indexOffset;
		public uint indexCount;
		public MeshTopology drawOp;
	}

	/** @} */
}
