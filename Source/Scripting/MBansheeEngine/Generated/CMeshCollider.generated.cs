using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
	/** @addtogroup Physics
	 *  @{
	 */

	/// <summary>A collider represented by an arbitrary mesh.</summary>
	[ShowInInspector]
	public partial class MeshCollider : Collider
	{
		private MeshCollider(bool __dummy0) { }
		protected MeshCollider() { }

		/// <summary>
		/// Sets a mesh that represents the collider geometry. This can be a generic triangle mesh, or and convex mesh.  Triangle 
		/// meshes are not supported as triggers, nor are they supported for colliders that are parts of a  non-kinematic 
		/// rigidbody.
		/// </summary>
		[ShowInInspector]
		[NativeWrapper]
		public RRef<PhysicsMesh> Mesh
		{
			get { return Internal_getMesh(mCachedPtr); }
			set { Internal_setMesh(mCachedPtr, value); }
		}

		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern void Internal_setMesh(IntPtr thisPtr, RRef<PhysicsMesh> mesh);
		[MethodImpl(MethodImplOptions.InternalCall)]
		private static extern RRef<PhysicsMesh> Internal_getMesh(IntPtr thisPtr);
	}

	/** @} */
}
