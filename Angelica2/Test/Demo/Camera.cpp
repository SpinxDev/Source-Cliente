/*
 * FILE: Camera.cpp
 *
 * DESCRIPTION: Camera control
 *
 * CREATED BY: Duyuxin, 2004/4/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Global.h"
#include "Camera.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define CAMERAFRAMERATE		1

enum
{
	CAMERAACT_PLAYTRACK	= 0,
	CAMERAACT_WAIT,
	CAMERAACT_PLAYACTION,
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

struct CAMERATRACK
{
	A3DVector3Track*	pPosTrack;		//	Position track
	A3DQuaternionTrack*	pRotTrack;		//	Rotation track
	A3DFloatTrack*		pSpeedTrack;	//	Speed track
};

struct CAMERAACTION
{
	int		iAction;	//	Action
	int		iData;		//	Action data
};

//	Camera tracks
APtrArray<CAMERATRACK*>	g_aCameraTracks;

AArray<A3DVECTOR3, A3DVECTOR3&>			g_aBuildPos;
AArray<A3DQUATERNION, A3DQUATERNION&>	g_aBuildRot;
AArray<CAMERAACTION, CAMERAACTION&>		g_aCameraActs;

//	Current track
static int	l_iCurTrack	= -1;
static int	l_iTrackTime = 0;
static int	l_iTotalTime = 0;
static int	l_iEndTime = 0;	

static int	l_iCurAction = -1;
static int	l_iWaitCnt = 0;

extern A3DSkinModel*	g_pSkinModel;		//	Player's Skin model

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

//	Create a camera track
static CAMERATRACK* _CreateCameraTrack(int iNumKey, float fFrameRate)
{
	//	Create a position track
	A3DVector3Track* pPosTrack = new A3DVector3Track;
	if (!pPosTrack || !pPosTrack->Create(iNumKey, fFrameRate, 1))
	{
		g_Log.Log("Failed to create position track");
		return NULL;
	}

	//	Create a rotation track
	A3DQuaternionTrack* pRotTrack = new A3DQuaternionTrack;
	if (!pRotTrack || !pRotTrack->Create(iNumKey, fFrameRate, 1))
	{
		g_Log.Log("Failed to create rotation track");
		return NULL;
	}

	pRotTrack->SetInterType(TRACKDATA_INTERTYPE_SLERP);

	//	Create a speed track
	A3DFloatTrack* pSpeedTrack = new A3DFloatTrack;
	if (!pSpeedTrack || !pSpeedTrack->Create(iNumKey, fFrameRate, 1))
	{
		g_Log.Log("Failed to create speed track");
		return NULL;
	}

	CAMERATRACK* pCameraTrack = new CAMERATRACK;
	if (!pCameraTrack)
		return NULL;

	pCameraTrack->pPosTrack		= pPosTrack;
	pCameraTrack->pRotTrack		= pRotTrack;
	pCameraTrack->pSpeedTrack	= pSpeedTrack;

	return pCameraTrack;
}

//	Release a camera track
static void _ReleaseCameraTrack(CAMERATRACK* pCameraTrack)
{
	if (!pCameraTrack)
		return;

	if (pCameraTrack->pPosTrack)
		pCameraTrack->pPosTrack->Release();

	if (pCameraTrack->pRotTrack)
		pCameraTrack->pRotTrack->Release();

	if (pCameraTrack->pSpeedTrack)
		pCameraTrack->pSpeedTrack->Release();

	delete pCameraTrack;
}

//	Parse a camera track in script file
static bool _ParseCameraTrack(AScriptFile* pFile)
{
	if (!pFile->MatchToken("{", false))
		return false;

	//	Get key number
	if (!pFile->MatchToken("KeyNum", false))
		return false;

	int iNumKey = pFile->GetNextTokenAsInt(false);

	//	Get frame rate
	if (!pFile->MatchToken("FrameRate", false))
		return false;

	int iFrameRate = pFile->GetNextTokenAsFloat(false);

	//	Create a new camera track
	CAMERATRACK* pTrack = _CreateCameraTrack(iNumKey, iFrameRate);
	if (!pTrack)
		return false;

	A3DVECTOR3* pPosTrack = pTrack->pPosTrack->GetKeyFrames();
	A3DQUATERNION* pRotTrack = pTrack->pRotTrack->GetKeyFrames();
	float* pSpeedTrack = pTrack->pSpeedTrack->GetKeyFrames();

	int iCount = 0;

	A3DMATRIX4 mat;
	mat.Identity();

	while (pFile->PeekNextToken(true))
	{
		if (!stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '}'
			break;
		}

		//	Get speed
		pSpeedTrack[iCount] = pFile->GetNextTokenAsFloat(true);

		//	Get position
		pPosTrack[iCount].x	= pFile->GetNextTokenAsFloat(true);
		pPosTrack[iCount].y	= pFile->GetNextTokenAsFloat(false);
		pPosTrack[iCount].z	= pFile->GetNextTokenAsFloat(false);

		//	Get rotation
		A3DVECTOR3 v;
		v.x = pFile->GetNextTokenAsFloat(true);
		v.y	= pFile->GetNextTokenAsFloat(false);
		v.z	= pFile->GetNextTokenAsFloat(false);
		v.Normalize();
		mat.SetRow(0, v);

		v.x = pFile->GetNextTokenAsFloat(true);
		v.y	= pFile->GetNextTokenAsFloat(false);
		v.z	= pFile->GetNextTokenAsFloat(false);
		v.Normalize();
		mat.SetRow(1, v);

		v.x = pFile->GetNextTokenAsFloat(true);
		v.y	= pFile->GetNextTokenAsFloat(false);
		v.z	= pFile->GetNextTokenAsFloat(false);
		v.Normalize();
		mat.SetRow(2, v);

		pRotTrack[iCount].ConvertFromMatrix(mat);

		iCount++;
	}

	int iEndTime = (int)((iNumKey - 1) * (1000.0f / iFrameRate));

	A3DVector3Track::SEGMENT* pPosSegment = pTrack->pPosTrack->GetSegments();
	pPosSegment->iStartTime		= 0;
	pPosSegment->iEndTime		= iEndTime;
	pPosSegment->iStartKey		= 0;
	pPosSegment->iEndKey		= iNumKey - 1;

	A3DQuaternionTrack::SEGMENT* pRotSegment = pTrack->pRotTrack->GetSegments();
	pRotSegment->iStartTime		= 0;
	pRotSegment->iEndTime		= iEndTime;
	pRotSegment->iStartKey		= 0;
	pRotSegment->iEndKey		= iNumKey - 1;

	A3DFloatTrack::SEGMENT* pSpeedSegment = pTrack->pSpeedTrack->GetSegments();
	pSpeedSegment->iStartTime	= 0;
	pSpeedSegment->iEndTime		= iEndTime;
	pSpeedSegment->iStartKey	= 0;
	pSpeedSegment->iEndKey		= iNumKey - 1;

	g_aCameraTracks.Add(pTrack);	

	return true;
}

//	Parse camera action in script file
static bool _ParseCameraAction(AScriptFile* pFile)
{
	if (!pFile->MatchToken("{", false))
		return false;

	CAMERAACTION Action;

	while (pFile->PeekNextToken(true))
	{
		if (!stricmp(pFile->m_szToken, "}"))
		{
			pFile->GetNextToken(true);	//	Skip '}'
			break;
		}

		//	Get action ID and data
		Action.iAction	= pFile->GetNextTokenAsInt(true);
		Action.iData	= pFile->GetNextTokenAsInt(false);

		g_aCameraActs.Add(Action);
	}

	return true;
}

//	Begin next action, return false if there isn't any action
static bool _GoToNextAction()
{
	l_iCurAction++;
	if (l_iCurAction >= g_aCameraActs.GetSize())
	{
		l_iCurAction = -1;
		return false;
	}

	const CAMERAACTION& Action = g_aCameraActs[l_iCurAction];

	if (Action.iAction == CAMERAACT_PLAYTRACK)
	{
		Camera_BeginTrack(Action.iData);
	}
	else if (Action.iAction == CAMERAACT_WAIT)
	{
		l_iWaitCnt = 0;
	}
	else if (Action.iAction == CAMERAACT_PLAYACTION)
	{
		g_pSkinModel->PlayActionByName("FUCKALL", 1.0f);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

bool Camera_Init(const char* szScriptFile)
{
	AScriptFile File;
	if (!File.Open(szScriptFile))
	{
		g_Log.Log("Failed to load camera script file");
		return false;
	}

	while (File.GetNextToken(true))
	{
		if (!stricmp(File.m_szToken, "Track"))
		{
			//	Get track ID
			int iTrackID = File.GetNextTokenAsInt(false);

			if (!_ParseCameraTrack(&File))
			{
				g_Log.Log("Failed to load camera track");
				return false;
			}
		}
		else if (!stricmp(File.m_szToken, "Action"))
		{
			if (!_ParseCameraAction(&File))
			{
				g_Log.Log("Failed to load camera action");
				return false;
			}
		}
	}
	
	File.Close();

	return true;
}

void Camera_Release()
{
	//	Release all camera tracks
	for (int i=0; i < g_aCameraTracks.GetSize(); i++)
		_ReleaseCameraTrack(g_aCameraTracks[i]);
	
	g_aCameraTracks.RemoveAll();
}

//	Add a key to camera track, return the key's index
int Camera_AddTrackKey(A3DCamera* pA3DCamera)
{
	A3DVECTOR3 vPos = pA3DCamera->GetPos();
	A3DVECTOR3 vDir = pA3DCamera->GetDir();
	A3DVECTOR3 vUp = pA3DCamera->GetUp();
	A3DVECTOR3 vRight = pA3DCamera->GetRight();

	A3DMATRIX4 mat;
	mat.Identity();

	mat.SetRow(0, vRight);
	mat.SetRow(1, vUp);
	mat.SetRow(2, vDir);

	A3DQUATERNION Quat(mat);

	g_aBuildPos.Add(vPos);
	g_aBuildRot.Add(Quat);

	return g_aBuildPos.GetSize() - 1;
}

bool Camera_SaveTrackToFile(const char* szFile)
{
	if (!g_aBuildPos.GetSize())
		return true;

	FILE* fp = fopen(szFile, "wt");
	if (!fp)
	{
		g_Log.Log("Failed to create camera track file %s", szFile);
		return false;
	}

	fprintf(fp, "Track\n{\n");

	fprintf(fp, "KeyNum %d\n", g_aBuildPos.GetSize());
	fprintf(fp, "FrameRate %d\n\n", CAMERAFRAMERATE);

	A3DMATRIX4 mat;
	
	for (int i=0; i < g_aBuildPos.GetSize(); i++)
	{
		const A3DVECTOR3& v = g_aBuildPos[i];
		const A3DQUATERNION& q = g_aBuildRot[i];
		q.ConvertToMatrix(mat);

		fprintf(fp, "0.1f\n");
		fprintf(fp, "%f, %f, %f\n", v.x, v.y, v.z);

		A3DVECTOR3 r = mat.GetRow(0);
		fprintf(fp, "%f, %f, %f\n", r.x, r.y, r.z);

		r = mat.GetRow(1);
		fprintf(fp, "%f, %f, %f\n", r.x, r.y, r.z);

		r = mat.GetRow(2);
		fprintf(fp, "%f, %f, %f\n\n", r.x, r.y, r.z);
	}

	fprintf(fp, "}\n");

	fclose(fp);

	return true;
}

void Camera_ResetTrack()
{
	g_aBuildPos.RemoveAll(false);
	g_aBuildRot.RemoveAll(false);
}

void Camera_BeginTrack(int iTrack)
{
	if (iTrack < 0 || iTrack >= g_aCameraTracks.GetSize())
		return;

	CAMERATRACK* pTrack = g_aCameraTracks[iTrack];
	A3DVector3Track::SEGMENT* pPosSegment = pTrack->pPosTrack->GetSegments();

	l_iCurTrack		= iTrack;
	l_iTrackTime	= 0;
	l_iEndTime		= pPosSegment->iEndTime;
	l_iTotalTime	= pPosSegment->iEndTime - pPosSegment->iStartTime + 1;
}

//	Return true if track is over
bool Camera_UpdateTrack(int iDeltaTime, A3DCamera* pA3DCamera)
{
	if (l_iCurTrack < 0)
		return false;

	CAMERATRACK* pTrack = g_aCameraTracks[l_iCurTrack];
	int iKey = pTrack->pPosTrack->GetFloorKeyIndex(l_iTrackTime);
	float fSpeedScale = pTrack->pSpeedTrack->GetKeyValueByIndex(iKey);

	int iAbsTime;
	bool bStop = false;

	iDeltaTime	= (int)(iDeltaTime * fSpeedScale + 0.5f);
	iAbsTime	= l_iTrackTime + iDeltaTime;

	if (iAbsTime > l_iEndTime)
	{
		iAbsTime = l_iEndTime;		//	Keep last moment's state
		bStop	 = true;	
	}

	l_iTrackTime = iAbsTime;

	//	Update position 
	if (pTrack->pPosTrack)
	{
		A3DVECTOR3 vPos = pTrack->pPosTrack->GetKeyValue(iAbsTime);
		pA3DCamera->SetPos(vPos);
	}

	if (pTrack->pRotTrack)
	{
		A3DQUATERNION Quat = pTrack->pRotTrack->GetKeyValue(iAbsTime);
		Quat.Normalize();

		A3DMATRIX4 mat;
		Quat.ConvertToMatrix(mat);

		pA3DCamera->SetDirAndUp(mat.GetRow(2), mat.GetRow(1));
	}

	return bStop;
}

void Camera_BeginActions()
{
	if (!g_aCameraActs.GetSize())
		return;

	l_iCurAction = -1;
	_GoToNextAction();
}

bool Camera_UpdateActions(int iDeltaTime, A3DCamera* pA3DCamera)
{
	if (l_iCurAction < 0)
		return true;

	const CAMERAACTION& Action = g_aCameraActs[l_iCurAction];
	bool bEnd = false;

	if (Action.iAction == CAMERAACT_PLAYTRACK)
	{
		if (Camera_UpdateTrack(iDeltaTime, pA3DCamera))
			bEnd = true;
	}
	else if (Action.iAction == CAMERAACT_WAIT)
	{
		l_iWaitCnt += iDeltaTime;
		if (l_iWaitCnt >= Action.iData)
			bEnd = true;
	}
	else if (Action.iAction == CAMERAACT_PLAYACTION)
	{
		bEnd = true;
	}

	if (bEnd)
		_GoToNextAction();

	return true;
}


