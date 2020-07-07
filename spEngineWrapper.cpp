//---------------------------------------------------------------------------
#pragma hdrstop
#include "spEngineWrapper.h"
#include "grapLine.h"
#include "grapHand.h"
#include "grapEllipse.h"
#include "grapRectangle.h"
#include "grapPoly.h"
#include "grapSplineCR.h"
#include "brushCml.h"
#include "brushHair.h"
#include "brushMesh.h"
#include "brushNeon.h"
#include "brushParallel.h"
#include "brushSolid.h"
#include "brushSpray.h"
#include "brushStamp.h"
#include "brushStar.h"
#include "brushPump.h"
#include "renderDraw.h"
#include "renderColors.h"
#include "renderShift.h"
#include "renderSpread.h"
#include "renderF33.h"
#include "renderIpp.h"
#include "renderTwister.h"
#include "renderCompound.h"
#include "renderNozzle.h"
#include "renderMultiDraw.h"
#include "spGlobals.h"
#include "ippProcessing.h"
#include "spTessellation.h"
#include "spMeshWolberg.h"
#include "ippGeometry.h"
//#include "spCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// trivial constructor
spEngineWrapper::spEngineWrapper(void)
{
FSurf = 0;
FRenderData = 0;
FShape = 0;
FRender = 0;
FUndoBlock.Buffer = 0;
FUndoBlock.Size   = 0;
FUndoBlock.AlphaBuffer = 0;
FUndoBlock.AlphaSize = 0;
FRedoBlock.Buffer = 0;
FRedoBlock.Size   = 0;
FRedoBlock.AlphaBuffer = 0;
FRedoBlock.AlphaSize = 0;
FSurfList = new TList;
for (int k = 0; k < 9; k++)
	FBucket[k] = 0;
FBucketIndex = 0;
FSurfIndex = -1;
// warp data
FWarpData.Prepared = false;
// neon brush by default
SetBrush(spe_brshNeon);
SetShape(spe_shpFreeHand);
//FBrush = FBucket[FBucketIndex];
CbBrushMask = 0;
FMultiStroke = new TList;
FActionType = spe_actDraw;
}
//---------------------------------------------------------------------------
// destructor
spEngineWrapper::~spEngineWrapper(void)
{
if (FSurfList)
   {
   for (int k = 0; k < FSurfList->Count; k++)
	   {
	   FSurf = static_cast <spSurfaceContainer*>(FSurfList->Items[k]);
	   delete FSurf;
	   FSurfList->Items[k] = 0;
	   }
   delete FSurfList;
   }
if (FRenderData)
   {
   if (FRenderData->GlobalColorParams->CloneImage)
	  delete FRenderData->GlobalColorParams->CloneImage;
   delete FRenderData;
   }
if (FShape)
   delete FShape;
for (int k = 0; k < 9; k++)
	{
	if (FBucket[k])
	   delete FBucket[k];
	}
if (FRender)
   delete FRender;
if (FUndoBlock.Buffer)
   FreeMemory(FUndoBlock.Buffer);
if (FUndoBlock.AlphaBuffer)
   FreeMemory(FUndoBlock.AlphaBuffer);
if (FRedoBlock.Buffer)
   FreeMemory(FRedoBlock.Buffer);
if (FRedoBlock.AlphaBuffer)
   FreeMemory(FRedoBlock.AlphaBuffer);
// delete multistroke list
ClearMultiStroke(false);    // clear, do not recreate
// additional cleanup
spTessellation::Clear();
}
//---------------------------------------------------------------------------
// initialize render data
bool spEngineWrapper::Initialize(void)
{
if (FRenderData)
   delete FRenderData;
FRenderData = new spRenderDataKeeper;
return true;
}
//---------------------------------------------------------------------------
// add new empty stave
int spEngineWrapper::AddSurface(void)
{
FSurfList->Add(0);	// empty stave
//FSurfIndex++;
FSurfIndex = FSurfList->Count - 1;
FSurf = 0;
return FSurfIndex;
}
//---------------------------------------------------------------------------
// set surface id
bool spEngineWrapper::SetSurfaceId(unsigned long id)
{
if (!FSurf)
   return false;
FSurf->SurfaceId = id;
return true;
}
//---------------------------------------------------------------------------
// select surface from the list
int spEngineWrapper::SelectSurfaceByIndex(int sIndex)
{
if (sIndex < 0 || sIndex >= FSurfList->Count)
   return -1;
FSurfIndex = sIndex;
FSurf = static_cast <spSurfaceContainer*>(FSurfList->Items[FSurfIndex]);
if (FBrush)
   FBrush->Connect(FSurf->MaskLeft, FSurf->MaskRight, FSurf->Shape);
if (FRender)
   FRender->Connect(FSurf);
return FSurfIndex;
}
//---------------------------------------------------------------------------
// delete surface from the list
bool spEngineWrapper::DeleteSurfaceByIndex(int sIndex)
{
if (sIndex < 0 || sIndex >= FSurfList->Count)
   return false;
FSurfIndex = sIndex;
FSurf = static_cast <spSurfaceContainer*>(FSurfList->Items[FSurfIndex]);
if (!FSurf)
   return false;
delete FSurf;
FSurf = 0;
FSurfList->Delete(FSurfIndex);
return true;
}
//---------------------------------------------------------------------------
// select surface from the list
int spEngineWrapper::SelectSurfaceById(unsigned long sId)
{
for (int i = 0; i < FSurfList->Count; i++)
	{
	 spSurfaceContainer *surf = static_cast <spSurfaceContainer*>(FSurfList->Items[i]);
	if (surf->SurfaceId == sId)
	   {
	   FSurf = surf;
	   FSurfIndex = i;
	   if (FBrush)
		  FBrush->Connect(FSurf->MaskLeft, FSurf->MaskRight, FSurf->Shape);
	   if (FRender)
		  FRender->Connect(FSurf);
	   return FSurfIndex;
	   }
	}
return -1;
}
//---------------------------------------------------------------------------
// delete surface from the list
bool spEngineWrapper::DeleteSurfaceById(unsigned long sId)
{
for (int i = 0; i < FSurfList->Count; i++)
	{
	 spSurfaceContainer *surf = static_cast <spSurfaceContainer*>(FSurfList->Items[i]);
	if (surf->SurfaceId == sId)
	   {
	   FSurf = surf;
	   FSurfIndex = i;
	   delete FSurf;
	   FSurf = 0;
	   FSurfList->Delete(FSurfIndex);
	   FSurfIndex = -1;
	   return true;
	   }
	}
return false;
}
//---------------------------------------------------------------------------
// set surface
bool spEngineWrapper::SetSurface(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment)
{
if (FSurfIndex < 0)
   return false;
if (FSurf)
   delete FSurf;
FSurf = new spSurfaceContainer(scanOrig, scanAlphaOrig, width, height, scanlineAlignment, FBrush, FShape);
FSurfList->Items[FSurfIndex] = static_cast <void*>(FSurf);
FSurf->SurfaceId = (unsigned long)scanOrig;
return (bool)FSurf;
}
//---------------------------------------------------------------------------
// set surface
bool spEngineWrapper::SetSurface(int width, int height)
{
if (FSurfIndex < 0)
   return false;
if (FSurf)
   delete FSurf;
FSurf = new spSurfaceContainer();
if (FSurf)
   {
   FSurf->Open(width, height);
   FSurfList->Items[FSurfIndex] = static_cast <void*>(FSurf);
   }
return (bool)FSurf;
}
//---------------------------------------------------------------------------
// set shape
TspeShapeType spEngineWrapper::SetShape(TspeShapeType shapeType)
{
TspeShapeType prevShape = FShapeType;
if (FShape)
   delete FShape;
if (shapeType == spe_shpFreeHand)
   FShape = new TGrapHand();
else if (shapeType == spe_shpLine)
   FShape = new TGrapLine();
else if (shapeType == spe_shpEllipse)
   FShape = new TGrapEllipse();
else if (shapeType == spe_shpRectangle)
   FShape = new TGrapRectangle();
else if (shapeType == spe_shpPoly)
   FShape = new TGrapPoly();
else if (shapeType == spe_shpSplineCR)
   FShape = new TGrapSplineCR();
else
   FShape = new TGrapHand();
if (FSurf)
   FSurf->SetShape(FShape);
FShapeType = shapeType;
//return static_cast <void*>(FShape);
return prevShape;
}
//---------------------------------------------------------------------------
// set brush
void* spEngineWrapper::SetBrush(TspeBrushType brushType)
{
if (FBrush)
   delete FBrush;
if (brushType == spe_brshNeon)
   FBrush = new TBrushNeon();
else if (brushType == spe_brshSolid)
   FBrush = new TBrushSolid();
else if (brushType == spe_brshSpray)
   FBrush = new TBrushSpray();
else if (brushType == spe_brshStar)
   FBrush = new TBrushStar();
else if (brushType == spe_brshHair)
   FBrush = new TBrushHair();
else if (brushType == spe_brshMesh)
   FBrush = new TBrushMesh();
else if (brushType == spe_brshParallel)
   FBrush = new TBrushParallel();
else if (brushType == spe_brshStamp)
   FBrush = new TBrushStamp();
else if (brushType == spe_brshCml)
   FBrush = new TBrushCml();
else if (brushType == spe_brshPump)
   FBrush = new TBrushPump();
else
   FBrush = new TBrushSolid();
FBucket[FBucketIndex] = FBrush;
if (FSurf)
   FSurf->SetBrush(FBrush);
return static_cast <void*>(FBrush);
}
//---------------------------------------------------------------------------
// get brush specific params
void* spEngineWrapper::GetBrushSpecific(void)
{
if (!FBrush)
   return 0;
if (FBrush->ClassName() == "TBrushNeon")
   {
   TBrushNeon *brsh = dynamic_cast <TBrushNeon*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushSolid")
   {
   TBrushSolid *brsh = dynamic_cast <TBrushSolid*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushSpray")
   {
   TBrushSpray *brsh = dynamic_cast <TBrushSpray*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushStar")
   {
   TBrushStar *brsh = dynamic_cast <TBrushStar*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushHair")
   {
   TBrushHair *brsh = dynamic_cast <TBrushHair*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushMesh")
   {
   TBrushMesh *brsh = dynamic_cast <TBrushMesh*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushParallel")
   {
   TBrushParallel *brsh = dynamic_cast <TBrushParallel*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushStamp")
   {
   TBrushStamp *brsh = dynamic_cast <TBrushStamp*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushCml")
   {
   TBrushCml *brsh = dynamic_cast <TBrushCml*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
else if (FBrush->ClassName() == "TBrushPump")
   {
   TBrushPump *brsh = dynamic_cast <TBrushPump*>(FBrush);
   return static_cast <void*>(brsh->SpecificParams);
   }
return 0;
}
//---------------------------------------------------------------------------
// set brush specific params
bool spEngineWrapper::SetBrushSpecific(void *vBsp)
{
if (!FBrush)
   return false;
if (FBrush->ClassName() == "TBrushNeon")
   {
   TBrushNeon *brsh = dynamic_cast <TBrushNeon*>(FBrush);
   TBrushNeon::structSpecificParams *bsp = static_cast<TBrushNeon::structSpecificParams*>(vBsp);
   brsh->SpecificParams->FalloffIndex = bsp->FalloffIndex;
   brsh->SpecificParams->LinearValue = bsp->LinearValue;
   brsh->SpecificParams->TrigonometricValue = bsp->TrigonometricValue;
   }
else if (FBrush->ClassName() == "TBrushSolid")
   {
   TBrushSolid *brsh = dynamic_cast <TBrushSolid*>(FBrush);
   TBrushSolid::structSpecificParams *bsp = static_cast<TBrushSolid::structSpecificParams*>(vBsp);
   brsh->SpecificParams->Shape = bsp->Shape;
   brsh->SpecificParams->Thickness = bsp->Thickness;
   }
else if (FBrush->ClassName() == "TBrushSpray")
   {
   TBrushSpray *brsh = dynamic_cast <TBrushSpray*>(FBrush);
   TBrushSpray::structSpecificParams *bsp = static_cast<TBrushSpray::structSpecificParams*>(vBsp);
   brsh->SpecificParams->Thickness = bsp->Thickness;
   brsh->SpecificParams->IntVary = bsp->IntVary;
   brsh->SpecificParams->Density = bsp->Density;
   brsh->SpecificParams->Gravity = bsp->Gravity;
   }
else if (FBrush->ClassName() == "TBrushStar")
   {
   TBrushStar *brsh = dynamic_cast <TBrushStar*>(FBrush);
   TBrushStar::structSpecificParams *bsp = static_cast<TBrushStar::structSpecificParams*>(vBsp);
   brsh->SpecificParams->Thickness = bsp->Thickness;
   brsh->SpecificParams->IntVary = bsp->IntVary;
   brsh->SpecificParams->Density = bsp->Density;
   }
else if (FBrush->ClassName() == "TBrushHair")
   {
   TBrushHair *brsh = dynamic_cast <TBrushHair*>(FBrush);
   TBrushHair::structSpecificParams *bsp = static_cast<TBrushHair::structSpecificParams*>(vBsp);
   brsh->SpecificParams->Thickness = bsp->Thickness;
   brsh->SpecificParams->IntVary = bsp->IntVary;
   brsh->SpecificParams->Density = bsp->Density;
   }
else if (FBrush->ClassName() == "TBrushMesh")
   {
   TBrushMesh *brsh = dynamic_cast <TBrushMesh*>(FBrush);
   TBrushMesh::structSpecificParams *bsp = static_cast<TBrushMesh::structSpecificParams*>(vBsp);
   brsh->SpecificParams->Thickness = bsp->Thickness;
   brsh->SpecificParams->IntVary = bsp->IntVary;
   brsh->SpecificParams->Density = bsp->Density;
   }
else if (FBrush->ClassName() == "TBrushParallel")
   {
   TBrushParallel *brsh = dynamic_cast <TBrushParallel*>(FBrush);
   TBrushParallel::structSpecificParams *bsp = static_cast<TBrushParallel::structSpecificParams*>(vBsp);
   brsh->SpecificParams->Thickness = bsp->Thickness;
   brsh->SpecificParams->IntVary = bsp->IntVary;
   brsh->SpecificParams->Spacing = bsp->Spacing;
   brsh->SpecificParams->Angle = bsp->Angle;
   }
else if (FBrush->ClassName() == "TBrushStamp")
   {
   TBrushStamp *brsh = dynamic_cast <TBrushStamp*>(FBrush);
   TBrushStamp::structSpecificParams *bsp = static_cast<TBrushStamp::structSpecificParams*>(vBsp);
   brsh->SpecificParams->AngleMode = bsp->AngleMode;
   brsh->SpecificParams->AngleValue = bsp->AngleValue;
   // stampId can not be changed, so...nada
   }
else if (FBrush->ClassName() == "TBrushCml")
   {
   int k;
   TBrushCml *brsh = dynamic_cast <TBrushCml*>(FBrush);
   TBrushCml::structSpecificParams *bsp = static_cast<TBrushCml::structSpecificParams*>(vBsp);
   brsh->SpecificParams->MinIntensity = bsp->MinIntensity;
   brsh->SpecificParams->MaxIntensity = bsp->MaxIntensity;
   brsh->SpecificParams->SeedIntensity = bsp->SeedIntensity;
   brsh->SpecificParams->LatticeType = bsp->LatticeType;
   for (k = 0; k < 9; k++)
	   {
	   brsh->SpecificParams->CreationOps[k] = bsp->CreationOps[k];
	   brsh->SpecificParams->DestructionOps[k] = bsp->DestructionOps[k];
	   }
   brsh->SpecificParams->EpsilonGrow = bsp->EpsilonGrow;
   brsh->SpecificParams->EpsilonDecay = bsp->EpsilonDecay;
   brsh->SpecificParams->Iterations = bsp->Iterations;
   brsh->SpecificParams->CreateProbability = bsp->CreateProbability;
   brsh->SpecificParams->DecayProbability = bsp->DecayProbability;
   brsh->SpecificParams->GrowProbability = bsp->GrowProbability;
   brsh->SpecificParams->Reborn = bsp->Reborn;
   brsh->SpecificParams->MatrixMes = bsp->MatrixMes;
   brsh->SpecificParams->DelayCreation = bsp->DelayCreation;
   brsh->SpecificParams->RandomSplat = bsp->RandomSplat;
   brsh->SpecificParams->SequentialScan = bsp->SequentialScan;
   brsh->SpecificParams->GrowFunc = bsp->GrowFunc;
   brsh->SpecificParams->DecayFunc = bsp->DecayFunc;
   brsh->SpecificParams->EpsilonWeight = bsp->EpsilonWeight;
   brsh->SpecificParams->CreationMutation = bsp->CreationMutation;
   brsh->SpecificParams->DestructionMutation = bsp->DestructionMutation;
   }
else if (FBrush->ClassName() == "TBrushPump")
   {
   TBrushPump *brsh = dynamic_cast <TBrushPump*>(FBrush);
   TBrushPump::structSpecificParams *bsp = static_cast<TBrushPump::structSpecificParams*>(vBsp);
   brsh->SpecificParams->Mode = bsp->Mode;
   brsh->SpecificParams->Index = bsp->Index;
   brsh->SpecificParams->ResetIndex = bsp->ResetIndex;
   // -- do not set count--> brsh->SpecificParams->Count = bsp->Count;
   }
return true;
}
//---------------------------------------------------------------------------
// set scale colors
/* ne triba, idemo na rukice kroz parametre
bool spEngineWrapper::SetScaleColors(unsigned int botVal, unsigned int topVal)
{
if (!FRenderData)
   return false;
FRenderData->GlobalColorParams->ScaleBottom = ptRGB(botVal);
FRenderData->GlobalColorParams->ScaleTop = ptRGB(topVal);
return true;
}
*/
//---------------------------------------------------------------------------
// set render engine by type
void* spEngineWrapper::SetRenderAction(TspeActionType actionType)
{
if (FRender)
   delete FRender;
// set standard mouse events
WMouseDown = StdMouseDown;
WMouseMove = StdMouseMove;
WMouseUp   = StdMouseUp;
if (actionType == spe_actDraw)
   FRender = new TRenderDraw(FSurf, FRenderData);
else if (actionType == spe_actColor)
   FRender = new TRenderColors(FSurf, FRenderData);
else if (actionType == spe_actShift)
   FRender = new TRenderShift(FSurf, FRenderData);
else if (actionType == spe_actSpread)
   FRender = new TRenderSpread(FSurf, FRenderData);
else if (actionType == spe_actFilter33)
   FRender = new TRenderF33(FSurf, FRenderData);
else if (actionType == spe_actFilterIpp)
   FRender = new TRenderFilterIpp(FSurf, FRenderData);
else if (actionType == spe_actColorTwistIpp)
   FRender = new TRenderTwister(FSurf, FRenderData);
else if (actionType == spe_actCompoundIpp)
   FRender = new TRenderCompound(FSurf, FRenderData);
else if (actionType == spe_actNozzle)
   FRender = new TRenderNozzle(FSurf, FRenderData);
else if (actionType == spe_actMultiDraw)
   {
   FRender = new TRenderMultiDraw(FSurf, FRenderData);
   // set symmetry mouse events
   WMouseDown = MultiMouseDown;
   WMouseMove = MultiMouseMove;
   WMouseUp   = MultiMouseUp;
   }
else
   FRender = new TRenderDraw(FSurf, FRenderData);
FActionType = actionType;
return static_cast <void*>(FRender);
}
//---------------------------------------------------------------------------
// set render engine by class name
/* not for now
void* spEngineWrapper::SetRenderAction(UNicodeString renderName)
{
if (FRender)
   delete FRender;
if (actionType == spe_actDraw)
   FRender = new TRenderDraw(FSurf, FRenderData);
else if (actionType == spe_actColor)
   FRender = new TRenderColors(FSurf, FRenderData);
else if (actionType == spe_actShift)
   FRender = new TRenderShift(FSurf, FRenderData);
else if (actionType == spe_actFilter33)
   FRender = new TRenderF33(FSurf, FRenderData);
else if (actionType == spe_actFilterIpp)
   FRender = new TRenderFilterIpp(FSurf, FRenderData);
else
   FRender = new TRenderDraw(FSurf, FRenderData);
return static_cast <void*>(FRender);
}
*/
//---------------------------------------------------------------------------
// set stamp
bool spEngineWrapper::SetStamp(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (!FBrush)
   return false;
if (FBrush->ClassName() != "TBrushStamp")
   return false;
TBrushStamp *brsh = dynamic_cast <TBrushStamp*>(FBrush);
brsh->SetStampImage(buffer, width, height, scanlineAlignement, shared);
return true;
}
//---------------------------------------------------------------------------
// clear stamp
bool spEngineWrapper::ClearStamp(void)
{
if (!FBrush)
   return false;
if (FBrush->ClassName() != "TBrushStamp")
   return false;
TBrushStamp *brsh = dynamic_cast <TBrushStamp*>(FBrush);
brsh->DeleteStampImage();
return true;
}
//---------------------------------------------------------------------------
// add pump image
bool spEngineWrapper::AddPumpImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (!FBrush)
   return false;
if (FBrush->ClassName() != "TBrushPump")
   return false;
TBrushPump *brsh = dynamic_cast <TBrushPump*>(FBrush);
brsh->AddPumpImage(buffer, width, height, scanlineAlignement, shared);
return true;
}
//---------------------------------------------------------------------------
// selected pump image
bool spEngineWrapper::SelectPumpImage(int idx)
{
if (!FBrush)
   return false;
if (FBrush->ClassName() != "TBrushPump")
   return false;
TBrushPump *brsh = dynamic_cast <TBrushPump*>(FBrush);
brsh->SelectPumpImage(idx);
return true;
}
//---------------------------------------------------------------------------
// deselect pump
bool spEngineWrapper::DeselectPump(void)
{
if (!FBrush)
   return false;
if (FBrush->ClassName() != "TBrushPump")
   return false;
TBrushPump *brsh = dynamic_cast <TBrushPump*>(FBrush);
brsh->DeselectPump();
return true;
}
//---------------------------------------------------------------------------
// clear pump
bool spEngineWrapper::ClearPump(void)
{
if (!FBrush)
   return false;
if (FBrush->ClassName() != "TBrushPump")
   return false;
TBrushPump *brsh = dynamic_cast <TBrushPump*>(FBrush);
brsh->DeletePump();
return true;
}
//---------------------------------------------------------------------------
// set bucket index
bool spEngineWrapper::SelectBrush(int bIndex, TspeBrushType &bType)
{
if (bIndex < 0 || bIndex > 8)
   return false;
FBucketIndex = bIndex;
FBrush = FBucket[FBucketIndex];
if (!FBrush)
   {
   bType = spe_brshUndefined;
   return true;
   }
if (FBrush->ClassName() == "TBrushNeon")
   bType = spe_brshNeon;
else if (FBrush->ClassName() == "TBrushSolid")
   bType = spe_brshSolid;
else if (FBrush->ClassName() == "TBrushSpray")
   bType = spe_brshSpray;
else if (FBrush->ClassName() == "TBrushStar")
   bType = spe_brshStar;
else if (FBrush->ClassName() == "TBrushHair")
   bType = spe_brshHair;
else if (FBrush->ClassName() == "TBrushMesh")
   bType = spe_brshMesh;
else if (FBrush->ClassName() == "TBrushParallel")
   bType = spe_brshParallel;
else if (FBrush->ClassName() == "TBrushStamp")
   bType = spe_brshStamp;
else if (FBrush->ClassName() == "TBrushCml")
   bType = spe_brshCml;
else if (FBrush->ClassName() == "TBrushPump")
   bType = spe_brshPump;
else
   bType = spe_brshSolid;
if (FSurf)
   FSurf->SetBrush(FBrush);
if (FRender)
	FRender->CurrentBrush = FBrush;
return true;
}
//---------------------------------------------------------------------------
// finish drawing
void spEngineWrapper::Finish(TRect *boundRect)
{
Surf->Brush->End(0);
Surf->UpdateImage(imgEdit, boundRect);
// call-back
if (CbBrushMask)
   CbBrushMask(Surf->MaskRight->GetOriginalBuffer(), boundRect);
if (RenderData->RenderParams->ClearImoc)
   Surf->ClearMask(mBoth, boundRect);
Surf->Orig->SetRoi();
Surf->Edit->SetRoi();
Surf->MaskLeft->SetRoi();
Surf->MaskRight->SetRoi();
if (Surf->AlphaOrig)
   {
   Surf->AlphaOrig->SetRoi();
   Surf->AlphaEdit->SetRoi();
   }
}
//---------------------------------------------------------------------------
// rollback drawing
void spEngineWrapper::Rollback(TRect *boundRect)
{
Surf->Brush->End(0);     // ovo æemo ukinit u brush-u
Surf->UpdateImage(imgOrig, boundRect);
Surf->ClearMask(mBoth, boundRect);
Surf->Orig->SetRoi();
Surf->Edit->SetRoi();
Surf->MaskLeft->SetRoi();
Surf->MaskRight->SetRoi();
if (Surf->AlphaOrig)
   {
   Surf->AlphaOrig->SetRoi();
   Surf->AlphaEdit->SetRoi();
   }
}
//---------------------------------------------------------------------------
// set external clone image
bool spEngineWrapper::SetClone(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (!FRenderData)
   return false;
if (FRenderData->GlobalColorParams->CloneImage)
   delete FRenderData->GlobalColorParams->CloneImage;
FRenderData->GlobalColorParams->CloneImage = new spImage<ptRGB>(buffer, width, height, scanlineAlignement, shared, orInvert);
return true;
}
//---------------------------------------------------------------------------
// delete external clone image
bool spEngineWrapper::DeleteClone(void)
{
if (!FRenderData)
   return false;
if (FRenderData->GlobalColorParams->CloneImage)
   {
   delete FRenderData->GlobalColorParams->CloneImage;
   FRenderData->GlobalColorParams->CloneImage = 0;
   }
return true;
}
//---------------------------------------------------------------------------
// set assistant image
bool spEngineWrapper::SetAssImage(int assType, void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (!FSurf)
   return false;
switch (assType)
   {
   case 0:	// stencil
	   if (FSurf->AssStencil)
		  delete FSurf->AssStencil;
	   FSurf->AssStencil = new spImage<ptGray>(buffer, width, height, scanlineAlignement, shared, orInvert);
	   FSurf->Stencil_i = 0;
	   FSurf->Stencil_j = 0;
	   break;
   case 1:	// contour
	   if (FSurf->AssContour)
		  delete FSurf->AssContour;
	   FSurf->AssContour = new spImage<ptGray>(buffer, width, height, scanlineAlignement, shared, orInvert);
	   break;
   case 2:	// mask
	   if (FSurf->AssMask)
		  delete FSurf->AssMask;
	   FSurf->AssMask = new spImage<ptGray>(buffer, width, height, scanlineAlignement, shared, orInvert);
	   break;
   case 3: // marker
	   if (FSurf->AssMarker)
		  delete FSurf->AssMarker;
	   FSurf->AssMarker = new spImage<ptGray>(buffer, width, height, scanlineAlignement, shared, orInvert);
	   //FSurf->AssMarker = new spImage<ptRGB>(buffer, width, height, scanlineAlignement, shared, orInvert);
	   break;
   default:
	   if (FSurf->AssSidekick)
		  delete FSurf->AssSidekick;
	   FSurf->AssSidekick = new spImage<ptRGB>(buffer, width, height, scanlineAlignement, shared, orInvert);
	   break;
   }
return true;
}
//---------------------------------------------------------------------------
// delete assistant image
bool spEngineWrapper::DeleteAssImage(int assType)
{
if (!FSurf)
   return false;
switch (assType)
   {
   case 0:	// stencil
	   if (FSurf->AssStencil)
		  delete FSurf->AssStencil;
	   FSurf->Stencil_i = 0;
	   FSurf->Stencil_j = 0;
	   break;
   case 1:	// contour
	   if (FSurf->AssContour)
		  {
		  delete FSurf->AssContour;
		  FSurf->AssContour = 0;
		  }
	   break;
   case 2:	// mask
	   if (FSurf->AssMask)
		  {
		  delete FSurf->AssMask;
		  FSurf->AssMask = 0;
		  }
	   break;
   case 3: // marker
	   if (FSurf->AssMarker)
		  {
		  delete FSurf->AssMarker;
		  FSurf->AssMarker = 0;
		  }
	   break;
   default:
	   if (FSurf->AssSidekick)
		  {
		  delete FSurf->AssSidekick;
		  FSurf->AssSidekick = 0;
		  }
	   break;
   }
return true;
}
//---------------------------------------------------------------------------
// invert stencil, contour, mask, sidekick
bool spEngineWrapper::InvertAssImage(int assType)
{
if (!FSurf)
   return false;
spImage<ptGray> *assGray = 0;
spImage<ptRGB>  *assRGB  = 0;
switch (assType)
   {
   case 0:	// stencil
	   if (!FSurf->AssStencil)
		  return false;
	   assGray = FSurf->AssStencil;
	   break;
   case 1:	// contour
	   if (!FSurf->AssContour)
		  return false;
	   assGray = FSurf->AssContour;
	   break;
   case 2:	// mask
	   if (!FSurf->AssMask)
		  return false;
	   assGray = FSurf->AssMask;
	   break;
   case 3:	// marker
	   if (!FSurf->AssMask)
		  return false;
	   assGray = FSurf->AssMask;
	   break;
   default:
	   if (!FSurf->AssSidekick)
		  return false;
	   assRGB = FSurf->AssSidekick;
	   break;
   }
if (assGray)
   {
   for (int i = 0; i < assGray->Height(); i++)
		{
		for (int j = 0; j < assGray->Width(); j++)
			assGray->Pixel(i, j).ch[0] = assGray->Pixel(i, j).ch[0] ^ 0xff;
		}
   }
else
   {
   for (int i = 0; i < assRGB->Height(); i++)
		{
		for (int j = 0; j < assRGB->Width(); j++)
			{
			assRGB->Pixel(i, j).ch[0] = assRGB->Pixel(i, j).ch[0] ^ 0xff;
			assRGB->Pixel(i, j).ch[1] = assRGB->Pixel(i, j).ch[1] ^ 0xff;
			assRGB->Pixel(i, j).ch[2] = assRGB->Pixel(i, j).ch[2] ^ 0xff;
			}
		}
   }
return true;
}
//---------------------------------------------------------------------------
// set external nozzle image
bool spEngineWrapper::SetNozzle(void *buffer, void * alpha, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (!FRenderData)
   return false;
if (FRenderData->GlobalColorParams->NozzleImage)
   {
   delete FRenderData->GlobalColorParams->NozzleImage;
   delete FRenderData->GlobalColorParams->NozzleAlpha;
   }
FRenderData->GlobalColorParams->NozzleImage = new spImage<ptRGB>(buffer, width, height, scanlineAlignement, shared, orInvert);
FRenderData->GlobalColorParams->NozzleAlpha = new spImage<ptGray>(alpha, width, height, scanlineAlignement, shared, orInvert);
return true;
}
//---------------------------------------------------------------------------
// delete external clone image
bool spEngineWrapper::DeleteNozzle(void)
{
if (!FRenderData)
   return false;
if (FRenderData->GlobalColorParams->CloneImage)
   {
   delete FRenderData->GlobalColorParams->NozzleImage;
   FRenderData->GlobalColorParams->NozzleImage = 0;
   FRenderData->GlobalColorParams->NozzleAlpha = 0;
   }
return true;
}
//---------------------------------------------------------------------------
// set selection mask
bool spEngineWrapper::SetSelection(void *buffer, bool shared)
{
if (!FSurf)
   return false;
if (FSurf->Selection)
   delete FSurf->Selection;
FSurf->Selection = new spImage<ptGray>(buffer, FSurf->Width, FSurf->Height, FSurf->Orig->Alignment(), shared, orInvert);
return true;
}
//---------------------------------------------------------------------------
// delete selection mask
bool spEngineWrapper::DeleteSelection(void)
{
if (!FSurf)
   return false;
if (FSurf->Selection)
   {
   delete FSurf->Selection;
   FSurf->Selection = 0;
   }
return true;
}
//---------------------------------------------------------------------------
// set color curve LUT
bool spEngineWrapper::SetColorCurve(Byte **LUTCurveEntry)
{
if (!FRenderData)
   return false;
FRenderData->GlobalColorParams->LUTCurveEntry = LUTCurveEntry;
return true;
}
//---------------------------------------------------------------------------
// create undo block
bool spEngineWrapper::CreateUndoBlock(TRect *undoRect)
{
if (FUndoBlock.Buffer)
   {
   FreeMemory(FUndoBlock.Buffer);
   FUndoBlock.Buffer = 0;
   FUndoBlock.Size = 0;
   }
if (!FSurf)
   return false;
if (undoRect->Width() == 0 || undoRect->Height() == 0)
   return false;
FUndoBlock.Rect.left = undoRect->left;
FUndoBlock.Rect.right = undoRect->right;
FUndoBlock.Rect.top = undoRect->top;
FUndoBlock.Rect.bottom = undoRect->bottom;
FUndoBlock.Size   = FSurf->Edit->Depth() * undoRect->Width()* undoRect->Height();
FUndoBlock.Stride = FSurf->Edit->Stride();
FUndoBlock.Buffer = GetMemory(FUndoBlock.Size);
FUndoBlock.CompleteImage = (FSurf->Edit->Width == undoRect->Width() && FSurf->Edit->Height == undoRect->Height());
//TODO: push old roi to stack
FSurf->Edit->SetRoi(undoRect);
// copy roi to external buffer
FSurf->Edit->CopyRoi2External(FUndoBlock.Buffer);
if (FSurf->AlphaEdit)
   {
   FSurf->AlphaEdit->SetRoi(undoRect);
   FUndoBlock.AlphaSize   = FSurf->AlphaEdit->Depth() * undoRect->Width()* undoRect->Height();
   FUndoBlock.AlphaBuffer = GetMemory(FUndoBlock.AlphaSize);
   FSurf->AlphaEdit->CopyRoi2External(FUndoBlock.AlphaBuffer);
   FSurf->AlphaEdit->SetRoi();
   }
//TODO: pop old roi from stack
FSurf->Edit->SetRoi();
return true;
}
//---------------------------------------------------------------------------
// restore undo block
bool spEngineWrapper::RestoreUndoBlock(Tspe_UndoBlock* externalBlock)
{
//TODO: vidi možemo li bez RedoBlock-a
if (FRedoBlock.Buffer)
   {
   FreeMemory(FRedoBlock.Buffer);
   FRedoBlock.Buffer = 0;
   FRedoBlock.Size = 0;
   }
if (!FSurf)
   return false;
FRedoBlock.Rect.left  = externalBlock->Rect.left;
FRedoBlock.Rect.right = externalBlock->Rect.right;
FRedoBlock.Rect.top   = externalBlock->Rect.top;
FRedoBlock.Rect.bottom = externalBlock->Rect.bottom;
FRedoBlock.Size   = externalBlock->Size;
FRedoBlock.Stride = externalBlock->Stride;
FRedoBlock.Buffer = GetMemory(FRedoBlock.Size);
//TODO: push old roi to stack
FSurf->Edit->SetRoi(&(externalBlock->Rect));
FSurf->Orig->SetRoi(&(externalBlock->Rect));
// copy orig roi to redo buffer
FSurf->Orig->CopyRoi2External(FRedoBlock.Buffer);
// copy external buffer to edit roi
FSurf->Edit->CopyExternal2Roi(externalBlock->Buffer);
// copy edit image to orig
FSurf->Orig->CopyRoi(FSurf->Edit);
if (FSurf->AlphaOrig)
   {
   FSurf->AlphaOrig->SetRoi(&(externalBlock->Rect));
   FSurf->AlphaEdit->SetRoi(&(externalBlock->Rect));
   FRedoBlock.AlphaSize   = externalBlock->AlphaSize;
   FRedoBlock.AlphaStride = externalBlock->AlphaStride;
   FRedoBlock.AlphaBuffer = GetMemory(FRedoBlock.AlphaSize);
   // copy alpha orig roi to redo buffer
   FSurf->AlphaOrig->CopyRoi2External(FRedoBlock.AlphaBuffer);
   // copy external buffer to alpha edit roi
   FSurf->AlphaEdit->CopyExternal2Roi(externalBlock->AlphaBuffer);
   // copy alpha edit to alpha orig
   FSurf->AlphaOrig->CopyRoi(FSurf->AlphaEdit);
   FSurf->AlphaOrig->SetRoi();
   FSurf->AlphaEdit->SetRoi();
   }
//TODO: pop old roi from stack
FSurf->Edit->SetRoi();
FSurf->Orig->SetRoi();
return true;
}
//---------------------------------------------------------------------------
// collect color correction data
bool spEngineWrapper::CollectCCD(unsigned long surfaceId, TRect *boundRect, bool source, bool selection)
{
if (selection)
   spCommon::CollectCCD(FSurf->Orig, boundRect, &(FSurf->CumulBGR[0]), &(FSurf->DistanceBGR[0]), FSurf->Selection);
else
   spCommon::CollectCCD(FSurf->Orig, boundRect, &(FSurf->CumulBGR[0]), &(FSurf->DistanceBGR[0]), 0);
int k;
if (source)
   {
   // source data
   for (k = 0; k < 3; k++)
	   {
	   spColorArithmetics::ColorData.SrcCumul[k] = FSurf->CumulBGR[k];
	   spColorArithmetics::ColorData.SrcDistance[k] = FSurf->DistanceBGR[k];
	   }
   }
else
  {
  for (k = 0; k < 3; k++)
	  {
	  spColorArithmetics::ColorData.DstCumul[k] = FSurf->CumulBGR[k];
	  spColorArithmetics::ColorData.DstDistance[k] = FSurf->DistanceBGR[k];
	  }
  }
return true;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::WarpBegin(Tspe_WarpParams &params)
{
if (!FSurf)
   {
   FWarpData.Prepared = false;
   return false;
   }
if (FWarpData.Prepared)
   return false;
FWarpData.srcImage = FSurf->Edit;
FWarpData.dstImage = FSurf->Orig;
FWarpData.srcAlpha = FSurf->AlphaEdit;
FWarpData.dstAlpha = FSurf->AlphaOrig;
FWarpData.Chan[0] = params.Chan[0];
FWarpData.Chan[1] = params.Chan[1];
FWarpData.Chan[2] = params.Chan[2];
FWarpData.bi_linear = !params.SmearMode;
FWarpData.bq_factor = params.BqFactor;
FWarpData.mask_perc = fast_ftol(((float)params.Capacity/ 100.0) * spe_SP_NORM);
FWarpData.mode = params.MainMode;
FWarpData.antialias = params.Antialias;
FWarpData.selImage = FSurf->Selection;
if (FWarpData.selImage)
   FWarpData.SelByte = (Byte)(params.SelectionBound + 1);
else
   FWarpData.SelByte = 0x00;
switch (params.MainMode)
   {
   case spe_WARP_SMEAR:
		FWarpData.Restrict = true;
		FWarpData.watract =  2.0 * (double)params.Strength/100.0;
		FGoWarp =  &spWarp::WarpLinearBox;
		break;
   case spe_WARP_MOVE:
		FWarpData.Restrict = false;
		FWarpData.watract =  1.0;
		FGoWarp =  &spWarp::WarpLinearImage;
		break;
   case spe_WARP_SPLASH:
		FWarpData.Restrict = false;
		FWarpData.watract =  1.0/M_PI * (double)params.Strength/100.0;
		FGoWarp =  &spWarp::WarpSineSplash;
		break;
   case spe_WARP_BLOB:
   case spe_WARP_BLUB:
		FWarpData.Restrict = true;
		FWarpData.watract =  1.0/M_PI * (double)params.Strength/100.0;
		FGoWarp =  &spWarp::WarpSineSplash;
		break;
   case spe_WARP_BALLOON:
   case spe_WARP_DOUBLE_BALLOON:
		FWarpData.Restrict = true;
		FWarpData.watract =  1/M_PI * (double)params.Strength/100.0;
		FGoWarp =  &spWarp::WarpSineRipple;
		break;
   case spe_WARP_RIPPLES:
   case spe_WARP_LONG_RIPPLE:
		FWarpData.Restrict = false;
		FWarpData.watract =  1/M_PI * (double)params.Strength/100.0;
		FGoWarp =  &spWarp::WarpSineRipple;
		break;
   case spe_WARP_INSIDE_TWIRL:
   case spe_WARP_OUTSIDE_TWIRL:
   case spe_WARP_FULL_TWIRL:
		FWarpData.Restrict = false;  // svejedno
		FWarpData.watract =  1.0; //nebitno - ne koristi ga
		FGoWarp =  &spWarp::WarpTwirl;
		break;
   case spe_WARP_DISORDER:
   case spe_WARP_EXPLODE:
   case spe_WARP_IMPLODE:
		FWarpData.Restrict = false;
		FWarpData.watract  = 0.125 * (double)params.Strength/100.0;
		FGoWarp =  &spWarp::WarpDisorder;
		break;
   case spe_WARP_WAVES_HORIZONTAL:
   case spe_WARP_WAVES_VERTICAL:
		FWarpData.Restrict = false;
		FWarpData.watract  = 2 * 0.125;
		FGoWarp =  &spWarp::WarpDisorder;
		break;
   case spe_WARP_MOTION:
		FWarpData.Restrict = false;
		FWarpData.watract  = 0.125 * (double)params.Strength/100.0;
		FGoWarp =  &spWarp::WarpDisorder;
		break;
   default:
		return false;
   }
spWarp::Begin(&FWarpData);
FWarpData.Prepared = true;
return true;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::WarpExecute(int &X1, int &Y1, int &X2, int &Y2)
{
if (!FWarpData.Prepared)
   return false;
FWarpData.X1 = X1;
FWarpData.Y1 = Y1;
FWarpData.X2 = X2;
FWarpData.Y2 = Y2;
FGoWarp(&FWarpData);
return true;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::WarpEnd(TRect *roiRect)
{
if(!FWarpData.Prepared)
  return false;
FWarpData.Prepared = false;
roiRect->left = FWarpData.BoundRect.left;
roiRect->right = FWarpData.BoundRect.right;
roiRect->top = FWarpData.BoundRect.top;
roiRect->bottom = FWarpData.BoundRect.bottom;
return true;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::ApplyWarpMesh(Tspe_WarpParams &params)
{
if (!FSurf)
   return false;
TMeshWolberg *meshWolberg = new TMeshWolberg(FSurf->Edit, FSurf->AlphaEdit, FSurf->Orig, FSurf->AlphaOrig);
meshWolberg->Execute(params);
delete meshWolberg;
return true;
}
//===========================================================================
// UTILS
//===========================================================================
void spEngineWrapper::GetSimplifiedPoly(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts)
{
TGrapPoly *gp = new TGrapPoly();
gp->GetSimplifiedPoints(polyPts, polyCount, dpTolerance, maxPts);
delete gp;
}
//---------------------------------------------------------------------------
void spEngineWrapper::GetSplinePoly(TPoint *polyPts, int &polyCount, bool closed)
{
TGrapSplineCR *gp = new TGrapSplineCR();
gp->GetSplinePoints(polyPts, polyCount, closed);
delete gp;
}
//===========================================================================
// multistroke bucket
//===========================================================================
void spEngineWrapper::ClearMultiStroke(bool recreate)
{
if (FMultiStroke)
   {
   for (int k = 0; k < FMultiStroke->Count; k++)
	   {
	   TStrokeBucket *mStroke = static_cast <TStrokeBucket*>(FMultiStroke->Items[k]);
	   delete mStroke->brush;
	   delete mStroke->shape;
	   delete mStroke;
	   FMultiStroke->Items[k] = 0;
	   }
   delete FMultiStroke;
   }
if (recreate)
   FMultiStroke = new TList;
}
//---------------------------------------------------------------------------
int spEngineWrapper::AddMultiStroke(int brushIndex)
{
if (!FBucket[brushIndex])
   return 0;
/* it should exist
if (!FMultiStroke)
   return 0;
*/
TStrokeBucket *mBucket = new TStrokeBucket();
mBucket->brush = FBucket[brushIndex]->Clone();
mBucket->shape = FShape->Clone();
mBucket->brush->Connect(FSurf->MaskLeft, FSurf->MaskRight, mBucket->shape);
FMultiStroke->Add((void*)mBucket);
return FMultiStroke->Count;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::SetMultiStrokePos(int &msIndex, int &X, int &Y)
{
/* it should exist
if (!FMultiStroke)
   return false;
*/
if (msIndex > FMultiStroke->Count - 1)
   return false;
TStrokeBucket *mStroke = static_cast <TStrokeBucket*>(FMultiStroke->Items[msIndex]);
mStroke->pos.x = X;
mStroke->pos.y = Y;
return true;
}
//---------------------------------------------------------------------------
TStrokeBucket* spEngineWrapper::GetMultiStrokeBucket(int &msIndex)
{
return static_cast <TStrokeBucket*>(FMultiStroke->Items[msIndex]);
}
//===========================================================================
// IPP library calls
//===========================================================================
bool spEngineWrapper::AppIppFilter(TRect *roiRect)
{
bool lRet;
TRenderBasic *saveRender = FRender;	// save current rendering engine
// set render engine to renderIpp
FRender = new TRenderFilterIpp(FSurf, FRenderData);
FRender->Begin(0, 0);
lRet = (FRender->FilterIpp->PrepareFilter(roiRect, FRenderData->FilterIppParams) == 0);
if (lRet)
   lRet =  (FRender->FilterIpp->ApplySingle(FSurf->ippEdit, FSurf->ippOrig) == 0);
delete FRender;
// restore previous rendering engine
FRender = saveRender;
if (FRender)
   FRender->Connect(FSurf);
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::AppIppCompound(TRect *roiRect)
{
bool lRet;
TRenderBasic *saveRender = FRender;	// save current rendering engine
// set render engine to renderIpp
FRender = new TRenderCompound(FSurf, FRenderData);
FRender->Begin(0, 0);
lRet =  (FRender->FilterIpp->PrepareCompound(roiRect, FRenderData->CompoundIppParams) == 0);
if (lRet)
	lRet = (FRender->FilterIpp->ApplyCompound(FSurf->ippEdit, FSurf->ippOrig, roiRect, FRenderData->CompoundIppParams) == 0);
delete FRender;
// restore previous rendering engine
FRender = saveRender;
if (FRender)
   FRender->Connect(FSurf);
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::AppIppColorTwist(TRect *roiRect)
{
Ipp32f twister[3][4];
ippProcess::GetTwister(FRenderData->ColorTwistIppParams->Matrix, FRenderData->ColorTwistIppParams->Scale, twister);
return (ippProcess::ColorTwist(FSurf->ippEdit, FSurf->ippOrig, roiRect, twister, FRenderData->ColorTwistIppParams->Bias) == 0);
}
//---------------------------------------------------------------------------
bool spEngineWrapper::AppIppQuad(TRect *roiRect, Tspe_QuadIppParams &params)
{
switch (params.MainMode)
   {
   case ipp_QT_PERSPECTIVE:
		return (ippGeometry::WarpPerspectiveQ(FSurf->ippEdit, FSurf->ippOrig, roiRect,  params) == 0);
   case ipp_QT_BILINEAR:
		return (ippGeometry::WarpBilinearQ(FSurf->ippEdit, FSurf->ippOrig, roiRect, params) == 0);
   case ipp_QT_AFFINE:
		return (ippGeometry::WarpAffineQ(FSurf->ippEdit, FSurf->ippOrig, roiRect, params) == 0);
   default:
		break;
   }
return false;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::StdMouseDown(TShiftState Shift, int X, int Y)
{
// free hand mouse down
Surf->Shape->Reset();
Surf->Brush->Begin(Surf->Shape);
bool lRet = Shape->MouseDown(Shift, X, Y);
if (lRet)
   {
   Render->Begin(X, Y );
   if (RenderData->RenderParams->DrawTipOnMouseDown)
	  {
	  Shape->MouseMove(Shift, X + 1, Y);
	  Surf->Brush->PreparePoints(true);
	  Render->Execute(Shape->MouseData.X_move_from, Shape->MouseData.Y_move_from,
					   Shape->MouseData.X_move_to, Shape->MouseData.Y_move_to);
	  Render->FinishRender();
	  }
   }
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::StdMouseMove(TShiftState Shift, int X, int Y)
{
// free hand mouse move
bool lRet = Shape->MouseMove(Shift, X, Y);
if (lRet)
   {
   Surf->Brush->PreparePoints(true);
   Render->Execute(Shape->MouseData.X_move_from, Shape->MouseData.Y_move_from,
					Shape->MouseData.X_move_to, Shape->MouseData.Y_move_to);
   Render->FinishRender();
   }
 return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::StdMouseUp(TShiftState Shift, int X, int Y)
{
bool lRet = Shape->MouseUp(Shift, X, Y);
if (lRet)
   {
   TRect boundRect = Render->End(Shape->MouseData.X_min, Shape->MouseData.Y_min,
						   Shape->MouseData.X_max, Shape->MouseData.Y_max);
   DoUndoStuff(boundRect);
   }
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::MultiMouseDown(TShiftState Shift, int X, int Y)
{
// free hand multidraw mouse down
// reset big bound
if (RenderData->RenderParams->MultiDrawResetBoundRect)
   spCommon::ResetBigBound();
TRenderMultiDraw *md = static_cast<TRenderMultiDraw*>(Render);
md->MultiStroke = FMultiStroke;
Render->Begin(X, Y);
//
bool lRet = false;
for (int i = 0; i <  FMultiStroke->Count; i++)
   {
	TStrokeBucket *stroke = static_cast<TStrokeBucket*>(FMultiStroke->Items[i]);
	int xs = stroke->pos.x;
	int ys = stroke->pos.y;
	stroke->brush->Begin(stroke->shape);
	lRet = stroke->shape->MouseDown(TShiftState(), xs, ys);     //TODO: empty shift -> save to protected
	Surf->SetShape(stroke->shape);
	Surf->SetBrush(stroke->brush);
	Render->TRenderBasic::Begin(xs, ys);
   }
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::MultiMouseMove(TShiftState Shift, int X, int Y)
{
bool lRet = false;
for (int i = 0; i <  FMultiStroke->Count; i++)
   {
   TStrokeBucket *stroke = static_cast<TStrokeBucket*>(FMultiStroke->Items[i]);
   int xs = stroke->pos.x;
   int ys = stroke->pos.y;
   Surf->SetShape(stroke->shape);
   Surf->SetBrush(stroke->brush);
   lRet = stroke->shape->MouseMove(Shift, xs, ys);
   if (lRet)
	  {
	  stroke->brush->PreparePoints(true);
	  Render->Execute(stroke->shape->MouseData.X_move_from, stroke->shape->MouseData.Y_move_from,
					  stroke->shape->MouseData.X_move_to, stroke->shape->MouseData.Y_move_to);
	  Render->FinishRender();
	  }
   spCommon::UpdateBigBound(&(Render->BoundRect));
   }
// free hand multidraw mouse move
//Render->Execute(0, 0, X, Y);
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::MultiMouseUp(TShiftState Shift, int X, int Y)
{
bool lRet = false;
TRect bbr;
for (int i = 0; i < FMultiStroke->Count; i++)
   {
   TStrokeBucket *stroke = static_cast<TStrokeBucket*>(FMultiStroke->Items[i]);
   int xs = stroke->pos.x;
   int ys = stroke->pos.y;
   Surf->SetShape(stroke->shape);
   Surf->SetBrush(stroke->brush);
   lRet = Surf->Shape->MouseUp(TShiftState(), xs, ys);
   if (lRet)
	  spCommon::UpdateBigBound(&(Render->BoundRect));
   }
if (lRet)
   {
   bbr = spCommon::GetBigBound(Surf->Width, Surf->Height);
   bbr = Render->End(bbr.left, bbr.top, bbr.right, bbr.bottom);
   DoUndoStuff(bbr);
   }
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::DrawObjectPoly(TPoint *pts, int ptsCount, Tspe_Geometry &geometry)
{
Surf->Shape->Filled   = geometry.Filled;
Surf->Shape->Border   = geometry.Border;
Surf->Shape->Closed   = geometry.PolyClosed;
if (geometry.SimplifyPolyline)
   Surf->Shape->SetSimplifyRule(true, geometry.SimplifyDPTolerance, geometry.SimplifyMaxPts);
TRect objRect;
Surf->Brush->Begin(Surf->Shape);
Surf->Shape->Reset();
Surf->Shape->DrawPoly(&objRect, pts, ptsCount);
Render->Begin(pts[0].x, pts[0].y);
if (geometry.RotationAngle != 0)
   {
   TPoint orig;
   orig.x = geometry.RotationOrigin.x;
   orig.y = geometry.RotationOrigin.y;
   Surf->Shape->Rotate(&objRect, &orig, -geometry.RotationAngle);
   }
//
Surf->Brush->PreparePoints(false);
//gEW.Surf->realStepper = gEW.Brush->State->cpRealStepper;
Surf->Brush->State->wpRealStepper = Surf->Brush->State->cpRealStepper;
if (Surf->Shape->Filled)
   {
   Render->PolyInterior(objRect.left, objRect.top,
						 objRect.right, objRect.bottom);
   }
if (Surf->Shape->Border)
   Render->Execute(objRect.left, objRect.top,
					 objRect.right, objRect.bottom);
Render->FinishRender();
TRect boundRect = Render->End(objRect.left, objRect.top,
							   objRect.right, objRect.bottom);
DoUndoStuff(boundRect);
//Render->CreateUndo = true;
return true;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::DrawObjectRect(TRect &objRect, Tspe_Geometry &geometry)
{
Surf->Shape->Filled   = geometry.Filled;
Surf->Shape->Border   = geometry.Border;
objRect.NormalizeRect();
Surf->Brush->Begin(Surf->Shape);
Shape->Reset();
Surf->Shape->DrawBound(&objRect);
Render->Begin(0, 0);    // test
if (geometry.RotationAngle != 0)
   {
   TPoint orig;
   orig.x = geometry.RotationOrigin.x;
   orig.y = geometry.RotationOrigin.y;
   if (orig.x == -1 && orig.y == -1)
	  {
	  orig.x = (objRect.left + objRect.Width())>>1;
	  orig.y = (objRect.top + objRect.Height())>>1;
	  }
   Surf->Shape->Rotate(&objRect, &orig, -geometry.RotationAngle);
   }
//
Brush->PreparePoints(false);
//gEW.Surf->realStepper = gEW.Brush->State->cpRealStepper;
Brush->State->wpRealStepper = Brush->State->cpRealStepper;
if (Shape->Filled)
   {
   Render->PolyInterior(objRect.left, objRect.top,
						 objRect.right, objRect.bottom);
   }
if (Shape->Border)
	Render->Execute(objRect.left, objRect.top,
						 objRect.right, objRect.bottom);
Render->FinishRender();
TRect boundRect = Render->End(objRect.left, objRect.top,
						 objRect.right, objRect.bottom);
DoUndoStuff(boundRect);
//Render->CreateUndo = true;
return true;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::DrawMultiPoly(TPoint **pts, int *ptsCount, Tspe_Geometry &geometry)
{
// test
//DrawObjectPoly(pts[0], ptsCount, geometry);
//return true;
if (Render->ClassName() != "TRenderMultiDraw")
   return false;
//
bool lRet = true;
Render->Begin(0, 0);   // test
// reset big bound
if (RenderData->RenderParams->MultiDrawResetBoundRect)
   spCommon::ResetBigBound();
TRect bbr;
// find bounding rectangle
/*
for (int i = 0; i < FMultiStroke->Count; i++)
   {
   bbr = spCommon::GetPolyBound(pts[i], ptsCount[i]);
   TStrokeBucket *stroke = static_cast<TStrokeBucket*>(FMultiStroke->Items[i]);
   Surf->SetShape(stroke->shape);
   Surf->SetBrush(stroke->brush);
   Surf->Brush->Begin(Surf->Shape);
   // adjust bounding rectangle (by brush size and drop shadow)
   bbr =  Render->GetBoundRectComplete(bbr.left, bbr.top,
									   bbr.right, bbr.bottom);
   spCommon::UpdateBigBound(&bbr);
   }
bbr = spCommon::GetBigBound(Surf->Width, Surf->Height);
*/
//
for (int i = 0; i < FMultiStroke->Count; i++)
   {
   bbr = spCommon::GetPolyBound(pts[i], ptsCount[i]);
   TStrokeBucket *stroke = static_cast<TStrokeBucket*>(FMultiStroke->Items[i]);
   Surf->SetShape(stroke->shape);
   Surf->SetBrush(stroke->brush);
   //
   Surf->Shape->Filled   = geometry.Filled;
   Surf->Shape->Border   = geometry.Border;
   Surf->Shape->Closed   = geometry.PolyClosed;
   if (geometry.SimplifyPolyline)
	  Surf->Shape->SetSimplifyRule(true, geometry.SimplifyDPTolerance, geometry.SimplifyMaxPts);
   TRect objRect;
   Surf->Brush->Begin(Surf->Shape);
   // adjust bounding rectangle (by brush size and drop shadow)
   bbr =  Render->GetBoundRectComplete(bbr.left, bbr.top,
									   bbr.right, bbr.bottom);
   spCommon::UpdateBigBound(&bbr);
   //bbr = spCommon::GetBigBound(Surf->Width, Surf->Height);
   Surf->Shape->Reset();
   Surf->Shape->DrawPoly(&objRect, pts[i], ptsCount[i]);
   Render->TRenderBasic::Begin(0, 0);
   /* do not rorate polyline...to much work for calculating bounding rectangle
   if (geometry.RotationAngle != 0)
	  {
	  TPoint orig;
	  orig.x = geometry.RotationOrigin.x;
	  orig.y = geometry.RotationOrigin.y;
	  Surf->Shape->Rotate(&objRect, &orig, -geometry.RotationAngle);
	  }
   */
   //
   Surf->Brush->PreparePoints(false);
   Surf->Brush->State->wpRealStepper = Surf->Brush->State->cpRealStepper;
   if (Surf->Shape->Filled)
	  {
	  Render->PolyInterior(objRect.left, objRect.top,
						 objRect.right, objRect.bottom);
	  }
   if (Surf->Shape->Border)
	   Render->Execute(objRect.left, objRect.top,
					 objRect.right, objRect.bottom);
   spCommon::UpdateBigBound(&objRect);
   Render->FinishRender();
   }
if (lRet)
   {
   bbr = Render->End(bbr.left, bbr.top, bbr.right, bbr.bottom);
   //DoUndoStuff(bbr);
   }
//Render->CreateUndo = true;
return lRet;
}
//---------------------------------------------------------------------------
bool spEngineWrapper::DrawMultiPolyByIndex(int &msIndex, TPoint *pts, int &ptsCount, Tspe_Geometry &geometry)
{
if (Render->ClassName() != "TRenderMultiDraw")
   return false;
//
bool lRet = true;
Render->Begin(0, 0);   // test
// reset big bound
if (RenderData->RenderParams->MultiDrawResetBoundRect)
   spCommon::ResetBigBound();
//
TRect bbr = spCommon::GetPolyBound(pts, ptsCount);
TStrokeBucket *stroke = static_cast<TStrokeBucket*>(FMultiStroke->Items[msIndex]);
Surf->SetShape(stroke->shape);
Surf->SetBrush(stroke->brush);
//
Surf->Shape->Filled   = geometry.Filled;
Surf->Shape->Border   = geometry.Border;
Surf->Shape->Closed   = geometry.PolyClosed;
if (geometry.SimplifyPolyline)
   Surf->Shape->SetSimplifyRule(true, geometry.SimplifyDPTolerance, geometry.SimplifyMaxPts);
TRect objRect;
Surf->Brush->Begin(Surf->Shape);
// adjust bounding rectangle (by brush size and drop shadow)
bbr =  Render->GetBoundRectComplete(bbr.left, bbr.top,
								   bbr.right, bbr.bottom);
spCommon::UpdateBigBound(&bbr);
bbr = spCommon::GetBigBound(Surf->Width, Surf->Height);
Surf->Shape->Reset();
Surf->Shape->DrawPoly(&objRect, pts, ptsCount);
Render->TRenderBasic::Begin(0, 0);
//
Surf->Brush->PreparePoints(false);
Surf->Brush->State->wpRealStepper = Surf->Brush->State->cpRealStepper;

if (Surf->Shape->Filled)
   {
   Render->PolyInterior(objRect.left, objRect.top,
						objRect.right, objRect.bottom);
   }
if (Surf->Shape->Border)
   Render->Execute(objRect.left, objRect.top,
					 objRect.right, objRect.bottom);
spCommon::UpdateBigBound(&objRect);
Render->FinishRender();
if (lRet)
   {
   bbr = Render->End(bbr.left, bbr.top, bbr.right, bbr.bottom);
   //spCommon::UpdateBigBound(&objRect);
   //DoUndoStuff(bbr);
   }
//Render->CreateUndo = true;
return lRet;
}
//---------------------------------------------------------------------------
void spEngineWrapper::DoUndoStuff(TRect &roiRect)
{
if (RenderData->RenderParams->CreateUndo && Render->CreateUndo)
   CreateUndoBlock(&roiRect);
if (RenderData->RenderParams->AutoCommit)
   Finish(&roiRect);
}
//---------------------------------------------------------------------------
bool spEngineWrapper::DropShadow(TRect *boundRect, TspeApplyType appType)
{
bool selClip = RenderData->RenderParams->SelectionClipping;
if (appType == spe_appSelection)
   {
   if (!Surf->Selection)
	  return false;
   Surf->MaskLeft->Copy(Surf->Selection);
   Surf->MaskRight->Copy(Surf->Selection);
   }
else if (appType == spe_appMask)
   {
   if (!Surf->AssMask)
	  return false;
   Surf->MaskLeft->Copy(Surf->AssMask);
   }
else if (appType == spe_appContour)
   {
   if (!Surf->AssContour)
	  return false;
   Surf->MaskLeft->Copy(Surf->AssContour);
   }
else 	// boundRect
	{
	//_surfLeft->SetContext(255 , 255, CommonParams->ScratchThicknessValue);
	Surf->MaskLeft->RectangleFilled(*boundRect);
	}
RenderData->RenderParams->SelectionClipping = false;
Render->DropShadow(boundRect);
RenderData->RenderParams->SelectionClipping = selClip;
return true;
}
//---------------------------------------------------------------------------












