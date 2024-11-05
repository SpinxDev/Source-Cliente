/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFLUIDEMITTERATTACHERDESC_H_
#define _APHYSXFLUIDEMITTERATTACHERDESC_H_

// define the local name...
// #define APX_FLUIDEMITTERATTACHER_FLUID_NAME			"FluidEmitterAttacher_Fluid"
// #define APX_FLUIDEMITTERATTACHER_ATTACHER_NAME		"FluidEmitterAttacher_EmitterAttacher"

class APHYSX_DLL_EXPORT APhysXFluidEmitterAttacherDesc:public APhysXAttacherDesc
{

	DeclareVersion(0xcc000001);
	DeclareObjType(APX_OBJTYPE_FLUIDEMITTER_ATTACHER);

public:

	APhysXFluidEmitterAttacherDesc() 
	{

	}

	virtual ~APhysXFluidEmitterAttacherDesc() { Release(); }


	//??? to do: revise to make sure we only add one fluid component...
	// we can only add one fluid object component...
	void AddFluidComponent(const APhysXObjectDesc* pFluidObjectDesc)
	{
		APHYSX_ASSERT(pFluidObjectDesc->IsFluid());

		if(pFluidObjectDesc->IsFluid())
			AddObjectComponent(pFluidObjectDesc, APX_ATTACHER_ATTACHED_FLUID_NAME);
	}

	bool AddAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const APhysXFluidEmitterParameters& fluidEmitterParameters)
	{
		return AddMultiAttacherComponent(pRBObjectDesc, fluidEmitterParameters, APX_ATTACHER_ATTACHER_NAME);
	}

	// if you want to add more than one emitter attacher, use this function.
	bool AddMultiAttacherComponent(const APhysXObjectDesc* pRBObjectDesc, const APhysXFluidEmitterParameters& fluidEmitterParameters, const char* szAttacherName);

	bool IsValid();

	//-----------------------------------------------------------------
	// replaced the following code by the macro DeclareCloneAPhysXObjectDesc
	// virtual APhysXObjectDesc* Clone() const;
	DeclareCloneAPhysXObjectDesc(APhysXFluidEmitterAttacherDesc)


	// access fluid parameters and emitter parameters directly...
	APhysXFluidParameters* GetFluidParameters();
	APhysXFluidEmitterParameters* GetFluidEmitterParameters(const char* szAttacherName = APX_ATTACHER_ATTACHER_NAME);


protected:


};




#endif

