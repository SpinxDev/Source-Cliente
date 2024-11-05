
#include "APhysXStdAfx.h"


void APhysXForceFieldObject::PutToSleep()
{
	// disable the forcefield effect to rb, cloth, fluid and softbody...
	if(m_pNxForceField)
	{
		m_pAPhysXScene->LockWritingScene();
		
		m_pNxForceField->setRigidBodyType(NX_FF_TYPE_NO_INTERACTION);
		m_pNxForceField->setClothType(NX_FF_TYPE_NO_INTERACTION);
		m_pNxForceField->setFluidType(NX_FF_TYPE_NO_INTERACTION);
		m_pNxForceField->setSoftBodyType(NX_FF_TYPE_NO_INTERACTION);
		
		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXForceFieldObject::WakeUp()
{
	if(m_pNxForceField)
	{
		m_pAPhysXScene->LockWritingScene();
		
		m_pNxForceField->setRigidBodyType(m_ffTypeRB);
		m_pNxForceField->setClothType(m_ffTypeCloth);
		m_pNxForceField->setFluidType(m_ffTypeFluid);
		m_pNxForceField->setSoftBodyType(m_ffTypeSB);

		m_pAPhysXScene->UnlockWritingScene();
	}

}

bool APhysXForceFieldObject::IsSleeping()
{
	if(m_pNxForceField)
	{
		if( m_pNxForceField->getRigidBodyType() == NX_FF_TYPE_NO_INTERACTION	&&
			m_pNxForceField->getClothType() == NX_FF_TYPE_NO_INTERACTION		&&
			m_pNxForceField->getFluidType() == NX_FF_TYPE_NO_INTERACTION		&&
			m_pNxForceField->getSoftBodyType() == NX_FF_TYPE_NO_INTERACTION			)
			
			return true;
		else
			return false;

	}
	else
		return true;

}

// only get the pose relative to the world frame or attached actor frame...
void APhysXForceFieldObject::SetPose(const NxMat34& globalPose)
{
	if(m_pNxForceField)
		m_pNxForceField->setPose(globalPose);
}

NxMat34 APhysXForceFieldObject::GetPose() const
{
	if(m_pNxForceField)
		return m_pNxForceField->getPose();

	NxMat34 mtID;
	return mtID;
}

void APhysXForceFieldObject::SetCollisionChannel(APhysXCollisionChannel apxCollisionChannel, bool bChangeChannelRef)
{
	APhysXObject::SetCollisionChannel(apxCollisionChannel, bChangeChannelRef);

	if(m_pAPhysXScene && m_pNxForceField)
	{
		NxGroupsMask groupMask = m_pAPhysXScene->GetCollisionChannelManager()->GetChannelGroupMask(apxCollisionChannel);

		m_pAPhysXScene->LockWritingScene();
		m_pNxForceField->setGroupsMask(groupMask);
		m_pAPhysXScene->UnlockWritingScene();
	}
}

void APhysXForceFieldObject::SetCollisionGroup(APhysXCollisionGroup apxCollisionGroup)
{
	APhysXObject::SetCollisionGroup(apxCollisionGroup);

	if(m_pAPhysXScene && m_pNxForceField)
	{
		m_pAPhysXScene->LockWritingScene();
		m_pNxForceField->setGroup(apxCollisionGroup);
		m_pAPhysXScene->UnlockWritingScene();
	}

}

void APhysXForceFieldObject::SetForceScale(float fRBForceScale, float fClothForceScale, float fFluidForceScale, float fSBForceScale)
{

	if(!m_ffVariety && fRBForceScale == 1.0f && fClothForceScale == 1.0f && fFluidForceScale == 1.0f && fSBForceScale == 1.0f)
		return;

	m_pAPhysXScene->LockWritingScene();

	if(!m_ffVariety)
	{
		m_ffVariety = m_pAPhysXScene->GetNxScene()->createForceFieldVariety();
		m_pNxForceField->setForceFieldVariety(m_ffVariety);
	}

	m_pAPhysXScene->GetNxScene()->setForceFieldScale(m_ffVariety, m_pAPhysXScene->GetRBForceFieldMaterial(), fRBForceScale);
	m_pAPhysXScene->GetNxScene()->setForceFieldScale(m_ffVariety, m_pAPhysXScene->GetClothForceFieldMaterial(), fClothForceScale);
	m_pAPhysXScene->GetNxScene()->setForceFieldScale(m_ffVariety, m_pAPhysXScene->GetFluidForceFieldMaterial(), fFluidForceScale);
	m_pAPhysXScene->GetNxScene()->setForceFieldScale(m_ffVariety, m_pAPhysXScene->GetSBForceFieldMaterial(), fSBForceScale);

	m_pAPhysXScene->UnlockWritingScene();
}

// use this as the userData member to indicate the kernel is customerized...
static APhysXU32 s_uCustomKernelUserData = 11;

bool APhysXForceFieldObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{

	if(!APhysXObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
		return false;

	APhysXForceFieldObjectDesc& apxForceFieldObjectDesc = *(APhysXForceFieldObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;
	NxForceFieldDesc ffDesc;
	apxForceFieldObjectDesc.GetNxForceFieldDesc(ffDesc);
	ffDesc.pose = apxObjInstanceDesc.mGlobalPose;

	// construct the kernel...
	if(apxForceFieldObjectDesc.mCustomKernel)
	{
		ffDesc.kernel = apxForceFieldObjectDesc.mCustomKernel;
		ffDesc.kernel->userData = (void *)&s_uCustomKernelUserData;
	}
	else
	{
		NxForceFieldLinearKernelDesc fflKernelDesc;
		apxForceFieldObjectDesc.mFFLKernelParameters.GetNxForceFieldLinearKernelDesc(fflKernelDesc);

		m_pAPhysXScene->LockWritingScene();
		NxForceFieldLinearKernel* pNxFFLKernel = m_pAPhysXScene->GetNxScene()->createForceFieldLinearKernel(fflKernelDesc);
		m_pAPhysXScene->UnlockWritingScene();

		if(!pNxFFLKernel)
		{
			gPhysXLog->Log("Warning: Failed on creating NxForceFieldLinearKernel: create a NULL NxForceFieldLinearKernel.");
			return false;
		}
		pNxFFLKernel->userData = NULL;

		ffDesc.kernel = pNxFFLKernel;
	}


	m_pAPhysXScene->LockWritingScene();
	NxForceField* pNxForceField = m_pAPhysXScene->GetNxScene()->createForceField(ffDesc);
	m_pAPhysXScene->UnlockWritingScene();

	// we have to release the created shapes manually
	APhysXShapeDescManager::ReleaseNxForceFieldShapeDescs(ffDesc.includeGroupShapes);

	if(!pNxForceField)
	{
		gPhysXLog->Log("Warning: Failed on creating NxForceField: create a NULL NxForceField.");
		return false;
	}

	m_pNxForceField = pNxForceField;
	m_pNxForceField->userData = this;
	m_pNxForceFieldKernel = ffDesc.kernel;

	m_ffTypeRB = ffDesc.rigidBodyType;
	m_ffTypeCloth = ffDesc.clothType;
	m_ffTypeFluid = ffDesc.fluidType;
	m_ffTypeSB = ffDesc.softBodyType;

	SetForceScale(apxForceFieldObjectDesc.mRBForceScale, apxForceFieldObjectDesc.mClothForceScale, apxForceFieldObjectDesc.mFluidForceScale, apxForceFieldObjectDesc.mSBForceScale);

	return true;
}

void APhysXForceFieldObject::OnReleaseBasicPhysXStuff()
{
	if(m_pAPhysXScene)
	{
		m_pAPhysXScene->LockWritingScene();

		if(m_pNxForceField)
		{
			m_pAPhysXScene->GetNxScene()->releaseForceField(*m_pNxForceField);
			m_pNxForceField = NULL;
		}

		if(m_pNxForceFieldKernel && !m_pNxForceFieldKernel->userData)
		{
			// linear kernel case...
			m_pAPhysXScene->GetNxScene()->releaseForceFieldLinearKernel(*(NxForceFieldLinearKernel *)m_pNxForceFieldKernel);
			m_pNxForceFieldKernel = NULL;
		}

		if(m_ffVariety)
		{
			m_pAPhysXScene->GetNxScene()->releaseForceFieldVariety(m_ffVariety);
		}
		
		m_pAPhysXScene->UnlockWritingScene();

	}

	APhysXObject::OnReleaseBasicPhysXStuff();
}

//---------------------------------------------------------------------------------
#if 1

// try to use our own customized kernel....

#include "NxForceFieldKernelDefs.h"

	// NxForceFieldKernelSimple will be the name of this kernel class
	NX_START_FORCEFIELD(Wind)

	// bindable constants - e.g. the height of your tornado
	NxVConst(Scale);
	NxVConst(Noise);
	NxFConst(LifeCircle_Recip);
	NxFConst(DistCircle_Recip);
	NxFConst(CurLife);
	NxBConst(Rest);

	// all kernel function code goes in here and have to be statements 
	// which were described in the Custom Kernel Statements section of the user guide.
	// input are the vectors Position and Velocity transformed into the chosen coordinate system.
	// output are the vectors force and torque

	NX_START_FUNCTION

		NxFinishIf(Rest)

		NxFloat fCircle = CurLife * APHYSX_PI * LifeCircle_Recip;
		fCircle += (Position.getX() + Position.getZ()) * DistCircle_Recip * APHYSX_PI; 
		NxFloat fScale = NxMath::sin((NxReal &)fCircle);
		fScale = NxSelect(fScale<0, -fScale, fScale);
		
		// currently, we are not considering the Noise term...
		force = Scale * fScale;			

	NX_END_FUNCTION

	
	NX_END_FORCEFIELD(Wind)

#endif



bool APhysXWindForceFieldObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	// create our custom kernel...
	NxForceFieldKernelWind* pKernelWind = new NxForceFieldKernelWind;
	if(!pKernelWind)
	{
		gPhysXLog->Log("Warning: Failed on creating NxForceFieldKernelWind: create a NULL NxForceFieldKernelWind.");
		return false;
	}

	APhysXForceFieldObjectDesc& apxForceFieldObjectDesc = *(APhysXForceFieldObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;
	apxForceFieldObjectDesc.mCustomKernel = pKernelWind;

	if(!APhysXForceFieldObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
	{
		delete pKernelWind;
		return false;
	}

	// init my own members...
	APHYSX_ASSERT(apxObjInstanceDesc.mPhysXObjectDesc->GetObjType() == APX_OBJTYPE_FORCEFIELD_WIND);
	APhysXWindForceFieldObjectDesc& apxWindFFObjectDesc = *(APhysXWindForceFieldObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;
	m_vWindForceConstant = apxWindFFObjectDesc.mFFLKernelParameters.mConstant;
	m_vWindForceNoise = apxWindFFObjectDesc.mFFLKernelParameters.mNoise;

	m_StateTime[APX_FF_WIND_STATE_WIND] = apxWindFFObjectDesc.GetWindFFParameters().mWindTime;
	m_StateTime[APX_FF_WIND_STATE_REST] = apxWindFFObjectDesc.GetWindFFParameters().mRestTime;
	m_fRecipCycleTime = 1.0f / apxWindFFObjectDesc.GetWindFFParameters().mCycleTime;
	m_fRecipCycleDist = 1.0f / apxWindFFObjectDesc.mCycleDistance;

	m_iCurState = 0;
	m_fElapsedTimeInCurState = 0.0f;

	// init Kernel Wind...
	pKernelWind->setScale(m_vWindForceConstant);
	pKernelWind->setNoise(m_vWindForceNoise);
	pKernelWind->setLifeCircle_Recip(m_fRecipCycleTime);
	pKernelWind->setDistCircle_Recip(m_fRecipCycleDist);
	pKernelWind->setCurLife(0.0f);



#if 0

	//----------------------------------------------------------------
	// oops, the following code has been obsolete...

	// make the wind effect position-dependent

	// I find that it is very hard to realize the position-dependent effect for wind using linear kernal

	NxBounds3 aabb;
	if(APhysXUtilLib::GetFFShapeGroupAABB(m_pNxForceField->getIncludeShapeGroup(), aabb))
	{
		NxVec3 vCenter;
		aabb.getCenter(vCenter);

		// set target pos
		APHYSX_ASSERT(m_pNxForceFieldLinearKernel);
		// m_pNxForceFieldLinearKernel->setPositionTarget(apxObjInstanceDesc.mGlobalPose.t);
		m_pNxForceFieldLinearKernel->setPositionTarget(vCenter);

		// compute the position dependence matrix...
		// you should deduce the following matrix computation...
		// Assume W is the Wind direction vector, D is the Position delta vector, we should 
		// create a vector in direction of Wind, we can use Dot function as following:
		//		V = (W dot D) W and in form of matrix we get:
		//		V = W'* D * W
		//		V = W'* W * D
		// So, we can get M = W'* W...

		NxVec3 vWindForceDir(m_vWindForceConstant);
		float fWFMag = vWindForceDir.normalize();
		float fMaxDistMag = aabb.max.distance(vCenter);

		float f_xx = vWindForceDir.x * vWindForceDir.x;
		float f_xy = vWindForceDir.x * vWindForceDir.y;
		float f_xz = vWindForceDir.x * vWindForceDir.z;
		float f_yy = vWindForceDir.y * vWindForceDir.y;
		float f_yz = vWindForceDir.y * vWindForceDir.z;
		float f_zz = vWindForceDir.z * vWindForceDir.z;

		NxMat33 mtPosDep;
		mtPosDep.setRow(0, NxVec3(f_xx, f_xy, f_xz));
		mtPosDep.setRow(1, NxVec3(f_xy, f_yy, f_yz));
		mtPosDep.setRow(2, NxVec3(f_xz, f_yz, f_zz));

		mtPosDep *= fWFMag/fMaxDistMag;
	
		m_pNxForceFieldLinearKernel->setPositionMultiplier(mtPosDep);
	}

#endif

	return true;
}

void APhysXWindForceFieldObject::OnReleaseBasicPhysXStuff()
{
	APhysXForceFieldObject::OnReleaseBasicPhysXStuff();

	// then, try to release the custom kernel...
	if(m_pNxForceFieldKernel && m_pNxForceFieldKernel->userData)
	{
		delete m_pNxForceFieldKernel;
		m_pNxForceFieldKernel = NULL;
	}
}


void APhysXWindForceFieldObject::PreSimulate(float dt)
{
	APhysXForceFieldObject::PreSimulate(dt);

	m_fElapsedTimeInCurState += dt;
	while(m_fElapsedTimeInCurState > m_StateTime[m_iCurState])
	{
		m_fElapsedTimeInCurState -= m_StateTime[m_iCurState];
		if(++m_iCurState == APX_FF_WIND_STATE_NUM)
		{
			m_iCurState = 0;
		}

	}

#if 0

	//----------------------------------------------------------------
	// oops, the following code has been obsolete...

	float fWindScale = 1.0f;

	if(m_iCurState == APX_FF_WIND_STATE_WIND)
	{
		// compute the scale
		fWindScale = NxMath::abs(NxMath::sin(m_fCurLife * APHYSX_PI * m_fRecipCycleTime ));
	}
	else if(m_iCurState == APX_FF_WIND_STATE_REST)
	{
		fWindScale = 0.0f;
	}
	else
	{
		APHYSX_ASSERT(0);
	}



	if(m_pNxForceFieldLinearKernel)
	{
		m_pNxForceFieldLinearKernel->setConstant(fWindScale * m_vWindForceConstant );
		m_pNxForceFieldLinearKernel->setNoise(m_vWindForceNoise * fWindScale);

	}

#endif

	if(m_pNxForceFieldKernel)
	{
		NxForceFieldKernelWind* pKernelWind = (NxForceFieldKernelWind*) m_pNxForceFieldKernel;
		pKernelWind->setCurLife(m_fCurLife);
		pKernelWind->setRest(m_iCurState == APX_FF_WIND_STATE_REST);
	}
}


//---------------------------------------------------------------------------------
NX_START_FORCEFIELD(BossAura)
	NxFConst(K);
	NxFConst(R);
	NxBConst(Rest);
	NxBConst(EnhanceMode);
	NxVConst(LocalXSide);     // input a normalized value
	NxVConst(LocalYUp);       // input a normalized value
	NxVConst(LocalZFaceTo);   // input a normalized value

	NX_START_FUNCTION
		NxFinishIf(Rest)

		NxFloat magPos = Position.magnitude();
		NxFloat RecipMagPos = magPos.recip();
		NxVector posNormalize;
		posNormalize.setX(Position.getX() * RecipMagPos);
		posNormalize.setY(Position.getY() * RecipMagPos);
		posNormalize.setZ(Position.getZ() * RecipMagPos);

		NxFloat RecipR = R;
		RecipR = RecipR.recip();
		NxFloat posRatio = magPos * RecipR;
		NxFinishIf(posRatio > 1.0f);

		NxFloat chance = 0.0f;
		NxFloat costheta = LocalZFaceTo.dot(posNormalize);

		NxBoolean bInPushRange(costheta >= 0.0f);
		if (NxForceFieldInternals::NxSw::getBoolVal(bInPushRange))
		{
			// in the push range
			// compute the chance of disable push
			NxFloat cPushRange = 0.8f;
			NxFloat RecipPRange = cPushRange;
			RecipPRange = RecipPRange.recip();
			
			NxFloat maxRatio = cPushRange;
			NxFloat minRatio = 0.3f;
			NxFloat maxChance = NxSelect(EnhanceMode, 0.3f, 0.7f);  // [last]% (EnhanceMode: [first]%) chance to disable push at maxRatio
			NxFloat minChance = NxSelect(EnhanceMode, 0.1f, 0.45f);  // [last]% (EnhanceMode: [first]%) chance to disable push at minRatio
			NxFloat RecipDRatio = maxRatio - minRatio;
			RecipDRatio = RecipDRatio.recip();
			chance = minChance + (posRatio - minRatio) * RecipDRatio * (maxChance - minChance);
			chance = NxSelect(posRatio > maxRatio, +1.0f, chance);
			chance = NxSelect(posRatio < minRatio, -1.0f, chance);
			NxBoolean bDisablePush(NxMath::rand(0.0f, 1.0f) <= chance);
			NxFinishIf(bDisablePush); 
			
			// compute push dir
			NxVector dirPush;
			dirPush.setX(posNormalize.getX());
			dirPush.setZ(posNormalize.getZ());
			NxFloat ranY = (NxMath::rand(0.0f, 1.0f));
			ranY = NxSelect(ranY > 0.7f * R, ranY * 0.4f, ranY);
			dirPush.setY(ranY);
			
			// normalize the dir
			NxFloat magDP = dirPush.magnitude();
			NxFloat RecipMagDP = magDP.recip();
			dirPush.setX(dirPush.getX() * RecipMagDP);
			dirPush.setY(dirPush.getY() * RecipMagDP);
			dirPush.setZ(dirPush.getZ() * RecipMagDP);
			
			NxFloat degDecay = costheta;
			NxFloat cofPush = degDecay + (1 - posRatio * RecipPRange) * (1 - degDecay);
			force = dirPush * K * cofPush;
			NxFinishIf(bInPushRange);
		}
	
		NxFloat cofPull = costheta * 2 + 1;
		NxBoolean bInPullRange(cofPull >= 0.0f);
		if (NxForceFieldInternals::NxSw::getBoolVal(bInPullRange))
		{
			// in the pull range
			// compute the chance of disable pull
			chance = NxSelect(EnhanceMode, 0.3f, 0.55f);  // [last]% (EnhanceMode: [first]%) chance to disable pull
			NxBoolean bDisablePull(NxMath::rand(0.0f, 1.0f) <= chance);
			NxFinishIf(bDisablePull); 
			
			// compute pull dir and force
			NxVector dirPull = LocalXSide;
			NxFloat theta = dirPull.dot(posNormalize);
			NxFloat signD = NxSelect(theta > 0, -1.0f, 1.0f);
			dirPull.setX(dirPull.getX() * signD);
			dirPull.setY(dirPull.getY() * signD);
			dirPull.setZ(dirPull.getZ() * signD);
			
			cofPull = (1 - cofPull) * 1.3f * posRatio;
			force = dirPull * K * cofPull;
			NxFinishIf(bInPullRange);		
		}
		
		// in the random range
		NxVector dirOpPull = LocalXSide;
		NxFloat theta = dirOpPull.dot(posNormalize);
		if (NxForceFieldInternals::NxSw::getFloatVal(theta) < 0.0f)
		{
			dirOpPull.setX(dirOpPull.getX() * -1);
			dirOpPull.setY(dirOpPull.getY() * -1);
			dirOpPull.setZ(dirOpPull.getZ() * -1);
		}
		
		// compute random dir
		NxBoolean bOppoPullDir(NxMath::rand(0.0f, 1.0f) <= 0.25f); // xx% chance to get the opposite of pull dir
		NxVector randDir;
		randDir.setX(NxSelect(bOppoPullDir, dirOpPull.getX(), NxMath::rand(-1.0f, 1.0f)));
		randDir.setZ(NxSelect(bOppoPullDir, dirOpPull.getZ(), NxMath::rand(-1.0f, 1.0f)));
		randDir.setY(NxMath::rand(-0.5f, 1.5f));
		
		// normalize the dir
		NxFloat magRD = randDir.magnitude();
		NxFloat RecipMagRD = magRD.recip();
		randDir.setX(randDir.getX() * RecipMagRD);
		randDir.setY(randDir.getY() * RecipMagRD);
		randDir.setZ(randDir.getZ() * RecipMagRD);
		
		NxFloat cofRand = NxMath::rand(0.0f, 1.0f);
		cofRand = NxSelect(EnhanceMode, NxMath::rand(0.4f, 1.3f), cofRand);
		cofRand *= NxSelect(bOppoPullDir, 0.5f, 1);
		force = randDir * K * cofRand;

	NX_END_FUNCTION
NX_END_FORCEFIELD(BossAura)

bool APhysXBossForceFieldObject::OnCreateBasicPhysXStuff(APhysXScene* pAPhysXScene, const APhysXObjectInstanceDesc& apxObjInstanceDesc)
{
	// create our custom kernel...
	NxForceFieldKernelBossAura* pKernelBossAura = new NxForceFieldKernelBossAura;
	if(!pKernelBossAura)
	{
		gPhysXLog->Log("Warning: Failed on creating NxForceFieldKernelBossAura: create a NULL NxForceFieldKernelBossAura.");
		return false;
	}

	APhysXForceFieldObjectDesc& apxForceFieldObjectDesc = *(APhysXForceFieldObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;
	apxForceFieldObjectDesc.mCustomKernel = pKernelBossAura;

	if(!APhysXForceFieldObject::OnCreateBasicPhysXStuff(pAPhysXScene, apxObjInstanceDesc))
	{
		delete pKernelBossAura;
		return false;
	}

	// init our custom kernel...
	APHYSX_ASSERT(apxObjInstanceDesc.mPhysXObjectDesc->GetObjType() == APX_OBJTYPE_FORCEFIELD_BOSS);
	APhysXBossForceFieldObjectDesc& apxBossFFObjectDesc = *(APhysXBossForceFieldObjectDesc*)apxObjInstanceDesc.mPhysXObjectDesc;

	const APhysXBossForceFieldObjectDesc::APhysXBossForceFieldParameters& BossFFPara = apxBossFFObjectDesc.GetBossFFParameters();
	pKernelBossAura->setK(BossFFPara.mForceValue);
	pKernelBossAura->setR(BossFFPara.mRadius);
	pKernelBossAura->setRest(BossFFPara.mRest);
	pKernelBossAura->setEnhanceMode(BossFFPara.mEnhanceMode);
	pKernelBossAura->setLocalXSide(BossFFPara.mLocalXSide);
	pKernelBossAura->setLocalYUp(BossFFPara.mLocalYUp);
	pKernelBossAura->setLocalZFaceTo(BossFFPara.mLocalZFaceTo);

	return true;
}
	
void APhysXBossForceFieldObject::OnReleaseBasicPhysXStuff()
{
	APhysXForceFieldObject::OnReleaseBasicPhysXStuff();
	
	// then, try to release the custom kernel...
	if (m_pNxForceFieldKernel && m_pNxForceFieldKernel->userData)
	{
		delete m_pNxForceFieldKernel;
		m_pNxForceFieldKernel = NULL;
	}
}

NxForceFieldKernelBossAura* APhysXBossForceFieldObject::GetOwnKernel() const
{
	if (m_pNxForceFieldKernel && m_pNxForceFieldKernel->userData)
		return static_cast<NxForceFieldKernelBossAura*>(m_pNxForceFieldKernel);
	
	return 0;
}

bool APhysXBossForceFieldObject::GetRestState() const
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		return pKernel->getRest();

	return false;
}

void APhysXBossForceFieldObject::SetRestState(bool rest)
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		pKernel->setRest(rest);
}

bool APhysXBossForceFieldObject::GetEnhanceMode() const
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		return pKernel->getEnhanceMode();
	
	return false;
}

void APhysXBossForceFieldObject::SetEnhanceMode(bool enhance)
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		pKernel->setEnhanceMode(enhance);
}

float APhysXBossForceFieldObject::GetForceValue() const
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		return pKernel->getK();
	
	return 0.0f;
}

void APhysXBossForceFieldObject::SetForceValue(float force)
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		pKernel->setK(force);
}

float APhysXBossForceFieldObject::GetRadius() const
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		return pKernel->getR();
	
	return 0.0f;
}

void APhysXBossForceFieldObject::SetRadius(float r)
{
	NxForceFieldKernelBossAura* pKernel = GetOwnKernel();
	if (pKernel)
		pKernel->setR(r);
}
