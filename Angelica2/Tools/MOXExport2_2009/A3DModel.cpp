#include "A3DModel.h"
#include "A3DTypes.h"
#include "A3DFuncs.h"

A3DModel::A3DModel() : A3DFrame()
{
}

A3DModel::~A3DModel()
{
}

bool A3DModel::AddFrame(A3DFrame ** ppFrame)
{
	A3DFrame * pFrame;

	pFrame = new A3DFrame();
	if( !pFrame->Init(m_pDevice) )
		return false;

	pFrame->SetTM(IdentityMatrix());

	*ppFrame = pFrame;
	return true;
}

bool A3DModel::Init(IDirect3DDevice8 * pDevice)
{
	return A3DFrame::Init(pDevice);
}

bool A3DModel::Release()
{
	return A3DFrame::Release();
}

bool A3DModel::Render()
{
	return A3DFrame::Render();
}

bool A3DModel::TickAnimation()
{
	return A3DFrame::TickAnimation();
}

bool A3DModel::Save(A3DFile * pFileToSave)
{
	//We should save the action definition list here;
	if( pFileToSave->IsBinary() )
	{

	}	
	else
	{
		char	szLineBuffer[2048];
		int		nChildFrameCount;
		DWORD	dwWriteLength;

		sprintf(szLineBuffer, "MODEL: %s\n{", GetName());
		pFileToSave->WriteLine(szLineBuffer);

		//save my child frames;
		sprintf(szLineBuffer, "CHILDFRAME: %d", 1);
		pFileToSave->WriteLine(szLineBuffer);
						 	  
		nChildFrameCount = 1;
		if( nChildFrameCount )
		{
			pFileToSave->WriteLine("{");

			// For now we have to do a fake mox file name, because we have 
			// no mox manager here;
			char szMoxName[MAX_PATH];
			sprintf(szMoxName, "%s.mox", GetName());
			sprintf(szLineBuffer, "%s", szMoxName);
			pFileToSave->WriteLine(szLineBuffer);
			
			pFileToSave->WriteLine("}");
		}
			
		//Save my child models;
		// No child models;
		sprintf(szLineBuffer, "CHILDMODEL: %d", 0);
		pFileToSave->WriteLine(szLineBuffer);

		// No actions;
		sprintf(szLineBuffer, "ACTIONS: %d", 0);
		pFileToSave->WriteLine(szLineBuffer);

		// No group actions;
		sprintf(szLineBuffer, "GROUPACTIONS: %d", 0);
		pFileToSave->WriteLine(szLineBuffer);

		// <== GROUPACTIONS: %s { 
		pFileToSave->WriteLine("}");			

		// No gfx event;
		sprintf(szLineBuffer, "GFXEVENT: %d", 0);
		pFileToSave->WriteLine(szLineBuffer);

		
		// No sfx event;
		sprintf(szLineBuffer, "SFXEVENT: %d", 0);
		pFileToSave->WriteLine(szLineBuffer);
		
		pFileToSave->WriteLine("USER_PROPERTY DO NOT EDIT");
		//Attached is a user defined property buffer, but maybe the property has not be saved yet;
		BYTE pPropertiesBuffer[1024];
		ZeroMemory(pPropertiesBuffer, 1024);
		pFileToSave->Write(pPropertiesBuffer, A3DMODEL_PROPERTY_SIZE, &dwWriteLength);
	}
	return true;
}

bool A3DModel::Load(A3DFile * pFileToLoad)
{
	return true;
}
