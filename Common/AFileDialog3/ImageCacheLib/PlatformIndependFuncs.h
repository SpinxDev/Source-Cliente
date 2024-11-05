#pragma once

class CxImage;
class A3DGFXEx;
class A3DGfxEngine;
class CECModel;
class A3DDevice;
class A3DGFXExMan;

A3DRenderTarget* CreateRenderTarget(A3DDevice* pDev, int iWid, int iHei, bool bTarget, bool bDepth);
bool ApplyRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt);
void RestoreRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt);

bool CreateXImageFromRT(A3DDevice* pDev, A3DRenderTarget* pRt, CxImage* pTarget);
A3DGFXEx* LoadGfx(A3DDevice* pDev, A3DGFXExMan* pGfxExMan, const char* szGfxFile);
CECModel* LoadECModel(A3DDevice* pDev, A3DGfxEngine* pGfxEngine, const char* szECMFile);
