
/*----------------------------------------------------------------------

	Common header of APhysXEngine header files...

	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXFLUIDEMITTERATTACHRBCONNECTOR_H_
#define _APHYSXFLUIDEMITTERATTACHRBCONNECTOR_H_


class APHYSX_DLL_EXPORT APhysXFluidEmitterAttachRBConnector: public APhysXObjectConnector
{

public:

	APhysXFluidEmitterAttachRBConnector() 
	{
		m_pNxFluid = NULL;
		m_pNxFluidEmitter = NULL;
	}

	virtual ~APhysXFluidEmitterAttachRBConnector() {}


protected:

	virtual bool CreateConnector(const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc);
	virtual void ReleaseConnector();

protected:

	NxFluid* m_pNxFluid;
	NxFluidEmitter* m_pNxFluidEmitter;

};

#endif
