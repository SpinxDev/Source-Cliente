/*----------------------------------------------------------------------
	
	Common header of APhysXEngine header files...
	
	Created by He Wenfeng, Jan.2, 2008

----------------------------------------------------------------------*/

#ifndef _APHYSXCLOTHATTACHRBCONNECTOR_H_
#define _APHYSXCLOTHATTACHRBCONNECTOR_H_


class APHYSX_DLL_EXPORT APhysXClothAttachRBConnector: public APhysXObjectConnector
{

public:

	APhysXClothAttachRBConnector() 
	{
		m_pNxCloth = NULL;
		// m_pAttachedShape = NULL;
	}

	virtual ~APhysXClothAttachRBConnector() {}



protected:

	virtual bool CreateConnector(const APhysXObjectConnectorInstanceDesc& apxObjConnectorInstanceDesc);
	virtual void ReleaseConnector();

protected:

	NxCloth* m_pNxCloth;

	//??? it seems no need to keep the pointers of attached shape
	// NxShape* m_pAttachedShape;
	
};



#endif