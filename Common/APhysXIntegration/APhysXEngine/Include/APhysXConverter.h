/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/
#ifndef _APHYSXCONVERTER_H_
#define _APHYSXCONVERTER_H_


class APHYSX_DLL_EXPORT APhysXConverter
{

public:
	
	//--------------------------------------------------------------------------------
	inline static void A2N_Vector3(const A3DVECTOR3& av3, NxVec3& nv3)
	{
		nv3.set(av3.x, av3.y, av3.z);
	}

	inline static NxVec3 A2N_Vector3(const A3DVECTOR3& av3)
	{
		return NxVec3(av3.x, av3.y, av3.z);
	}

	inline static void N2A_Vector3(const NxVec3& nv3, A3DVECTOR3& av3)
	{
		av3.Set(nv3.x, nv3.y, nv3.z);
	}

	inline static A3DVECTOR3 N2A_Vector3(const NxVec3& nv3)
	{
		return A3DVECTOR3(nv3.x, nv3.y, nv3.z);
	}

	//--------------------------------------------------------------------------------
	/*
		Be aware that
			
			1. Angelica SDK uses row vectors and right matrix multiply which are consistent with D3D.
			
			2. PhysX SDK uses the standard maths conventions which deploy column vectors and left matrix multiply.

			3. Angelica SDK uses left-hand reference frame, while PhysX SDK uses right-hand one. But Physics simulation
			   can be run in a mirror mode, thus, we need not transform between the frame. ???? Are you sure of this????	

		So, here we should convert them each other according to above rules.
	
	*/

	//--------------------------------------------------------------------------------
	// 3 * 3 Matrix conversion...
	inline static void A2N_Matrix33(const A3DMATRIX3& am33, NxMat33& nm33)
	{
		nm33.setColumnMajor(am33.m[0]);
	}
	
	inline static void N2A_Matrix33(const NxMat33& nm33, A3DMATRIX3& am33)
	{
		nm33.getColumnMajor(am33.m[0]);
	}

	//--------------------------------------------------------------------------------
	// note here: PhysX SDK only provide NxMat34 which is corresponding to 4*4 matrix
	// 4 * 4 (3 * 4) Matrix conversion...
	inline static void A2N_Matrix44(const A3DMATRIX4& am44, NxMat34& nm34)
	{
		nm34.setColumnMajor44(am44.m[0]);

	}

	inline static void N2A_Matrix44(const NxMat34& nm34, A3DMATRIX4& am44)
	{
		nm34.getColumnMajor44(am44.m[0]);
	}

	//--------------------------------------------------------------------------------
	// quaternion conversion...
	inline static void A2N_Quat(const A3DQUATERNION& aq, NxQuat& nq)
	{
		nq.setXYZW(aq.x, aq.y, aq.z, aq.w);
	}
	
	inline static void N2A_Quat(const NxQuat& nq, A3DQUATERNION& aq)
	{
		aq.Set(nq.x, nq.y, nq.z, nq.w);
	}


	//--------------------------------------------------------------------------------
	// aabb conversion...
	
	inline static void A2N_AABB(const A3DAABB& aAABB, NxBounds3& nAABB)
	{
		A3DAABB aabb(aAABB);
		aabb.CompleteMinsMaxs();
		nAABB.set(A2N_Vector3(aabb.Mins), A2N_Vector3(aabb.Maxs));
	}

	inline static void N2A_AABB(const NxBounds3& nAABB, A3DAABB& aAABB)
	{
		N2A_Vector3(nAABB.min, aAABB.Mins);
		N2A_Vector3(nAABB.max, aAABB.Maxs);

		aAABB.CompleteCenterExts();
	}


};


#endif