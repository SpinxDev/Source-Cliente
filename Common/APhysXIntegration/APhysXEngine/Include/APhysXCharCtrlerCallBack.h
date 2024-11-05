/*
* FILE: APhysXCharCtrlerCallBack.h
*
* DESCRIPTION: 
*	This class is used for the customized callback of character controller.
*
*
* CREATED BY: Yang Liu, 2009/09/02
*
* HISTORY: 
*
* Copyright (c) 2009 Perfect World, All Rights Reserved.
*/

#pragma once

#ifndef _APHYSX_CHARACTER_CONTROLLER_CALLBACK_H_
#define _APHYSX_CHARACTER_CONTROLLER_CALLBACK_H_

/*
User callback class for character controller events. For more details, see PhysX API class NxUserControllerHitReport.
User can set different callback routine for each CC if it is necessary. 
Callback routine can be changed in the runtime.
Note: only support by APhysXCharacterController
*/

/*
Describes a controller shape hit. Passed to onShapeHit()
*/
struct APhysXCCShapeHit
{
	APhysXCharacterController* 	controller;		// Current controller
	APhysXObject*	object;			// Touched object
	NxVec3			worldPos;		// Contact position in world space
	NxVec3			worldNormal;	// Contact normal in world space
	APhysXU32		id;				// Reserved for future use
	NxVec3			dir;			// Motion direction
	float			length;			// Motion length
	bool			pushing;		// Indicate controller will add push force to the touched object
};

/*
Describe a controller hit. Passed to onControllerHit().
*/
struct APhysXCCsHit
{
	APhysXCharacterController*	controller;		// Current controller
	APhysXCharacterController*	other;			// Touched controller
};

/*
Describe a controller overlap. Passed to onOverlapBegin() and onOverlapContinue().
*/
struct APhysXCCOverlap
{
	APhysXCharacterController*	controller;		// Current controller
	APhysXU32		nbObjects;		// Number of overlapping objects 
	APhysXObject**	objects;		// Buffer to store overlapping objects
	APhysXU32		nbNxActors;		// Number of overlapping NxActors 
	NxActor**		nxactors;		// Buffer to store overlapping nxactors
};

class APHYSX_DLL_EXPORT APhysXCCHitReport : public APhysXSimplePtrManager<APhysXCCHitReport>
{
//----------------------------------------------------------
// singleton object
	APHYSX_DECLARE_SINGLETON(APhysXCCHitReport)
//----------------------------------------------------------

public:
	virtual ~APhysXCCHitReport() {};
	int GetRegisteredCount() const;

	/*
	Called when current controller hits a shape. For more information, see PhysX API function NxUserControllerHitReport::onShapeHit().
	*/
	virtual void onShapeHit(const APhysXCCShapeHit& hit);

	/*
	Called when current controller hits another controller. For more information, see PhysX API function NxUserControllerHitReport::onControllerHit().
	*/
	virtual void onControllerHit(const APhysXCCsHit& hit);

	/*
	Called when current controller just begin overlap with any actor.
	*/
	virtual void onOverlapBegin(const APhysXCCOverlap& ol);

	/*
	Called when current controller keep overlapping with any actor.
	*/
	virtual void onOverlapContinue(const APhysXCCOverlap& ol);

	/*
	Called when current controller end overlap with any actor.
	*/
	virtual void onOverlapEnd(APhysXCharacterController& controller);
};

#define gPhysXCCHitReport (APhysXCCHitReport::GetPtrInstance())


#endif