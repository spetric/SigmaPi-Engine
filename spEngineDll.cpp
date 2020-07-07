#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#define DLLEXPORT extern "C" __declspec( dllexport )
#include "spEngineWrapper.h"
#include "grapRectangle.h"
#include "spEngineDll.h"
#include "spCommon.h"
//#include "ippFilters.hpp"
#include "spProcessing.h"
#include "spTessellation.h"
#include "ippProcessing.h"
#include "ippAuxiliary.h"
#include "ippStatistics.h"
//global var wrapper
spEngineWrapper gEW;
// prototyping
//void __stdcall speDoUndoStuff(TRect &roiRect);
//---------------------------------------------------------------------------
void __stdcall speSetProperRectangle(TRect *boundRect, TRect &roiRect)
{
if (boundRect)
   {
   roiRect.left = (boundRect->left < 0) ? 0 : boundRect->left;
   roiRect.top = (boundRect->top < 0) ? 0 : boundRect->top;
   roiRect.right = (boundRect->right > gEW.Surf->Width) ? gEW.Surf->Width : boundRect->right;
   roiRect.bottom = (boundRect->bottom > gEW.Surf->Height) ? gEW.Surf->Height : boundRect->bottom;
   }
else
   {
   roiRect.left = 0;
   roiRect.top  = 0;
   roiRect.right  = gEW.Surf->Width;
   roiRect.bottom = gEW.Surf->Height;
   }
}
//---------------------------------------------------------------------------
/* obsolete
void __stdcall speDoUndoStuff(TRect &roiRect)
{
if (gEW.RenderData->RenderParams->CreateUndo && gEW.Render->CreateUndo)
   gEW.CreateUndoBlock(&roiRect);
if (gEW.RenderData->RenderParams->AutoCommit)
   gEW.Finish(&roiRect);
}
*/
//---------------------------------------------------------------------------
// exported functions
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speInit(void)
{
// ipp dlls exist
if (FileExists(ExtractFilePath(Application->ExeName)+"\\ipp_custom.dll"))
   {
   spCommon::ippAlive = true;
   ippInit();
   }
//
// Engine initialization
//
//int k;
try
  {
  gEW.Initialize();
  return true;
  }
catch (...)
  {
  return false;
  }
}
//---------------------------------------------------------------------------
// surface
//---------------------------------------------------------------------------
DLLEXPORT int __stdcall speAddSurface(void)
{
// add new empty surface
return gEW.AddSurface();
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetSurfaceId(unsigned long id)
{
// set surface id
return gEW.SetSurfaceId(id);
}
//---------------------------------------------------------------------------
DLLEXPORT int __stdcall speSelectSurfaceByIndex(int sIndex)
{
return gEW.SelectSurfaceByIndex(sIndex);
}
//---------------------------------------------------------------------------
DLLEXPORT int __stdcall speSelectSurfaceById(unsigned long id)
{
return gEW.SelectSurfaceById(id);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetSurface(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment)
{
// set surface
return gEW.SetSurface(scanOrig, scanAlphaOrig, width, height,  scanlineAlignment);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSurfaceBegin(int width, int height)
{
// set surface discontiguous
return gEW.SetSurface(width, height);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSurfaceAddScanline(void *scanOrig, void *scanAlphaOrig)
{
// add scanlines
return gEW.Surf->AddScanLine(scanOrig, scanAlphaOrig);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSurfaceFinish(void)
{
// create surface and close
return gEW.Surf->Close();
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteSurfaceByIndex(int sIndex)
{
return gEW.DeleteSurfaceByIndex(sIndex);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteSurfaceById(int id)
{
return gEW.DeleteSurfaceById(id);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSyncBackSurface(void)
{
if (!gEW.Surf)
   return false;
gEW.Surf->UpdateImage(imgEdit, 0);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSyncFrontSurface(void)
{
if (!gEW.Surf)
   return false;
gEW.Surf->UpdateImage(imgOrig, 0);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSidekickToBackSurface(void)
{
if (!gEW.Surf)
   return false;
if (!gEW.Surf->AssSidekick)
   return false;
if (gEW.Surf->AssSidekick->Height() != gEW.Surf->Edit->Height || gEW.Surf->AssSidekick->Width() != gEW.Surf->Edit->Width)
   return false;
gEW.Surf->Edit->Copy(gEW.Surf->AssSidekick);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speCopyToFrontSurface(void *scanOrig, void *scanAlphaOrig)
{
if (!gEW.Surf)
   return false;
if (scanOrig)
	gEW.Surf->Orig->Copy(scanOrig, orInvert);
if (scanAlphaOrig)
	gEW.Surf->AlphaOrig->Copy(scanAlphaOrig, orInvert);
return true;
}
//---------------------------------------------------------------------------
// render
//---------------------------------------------------------------------------
DLLEXPORT Tspe_RenderParams* __stdcall speGetRenderParams(void)
{
return gEW.RenderData->RenderParams;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetAction(TspeActionType actionType)
{
if (!spCommon::ippAlive)
   {
   if (actionType == spe_actFilterIpp || actionType == spe_actColorTwistIpp)
	  return false;
   }
return (bool)(gEW.SetRenderAction(actionType));
}
//---------------------------------------------------------------------------
DLLEXPORT TspeActionType __stdcall speGetAction(void)
{
return gEW.ActionType;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_DrawParams* __stdcall speGetDrawParams(void)
{
return gEW.RenderData->DrawParams;
}
//---------------------------------------------------------------------------
// actions
//---------------------------------------------------------------------------
DLLEXPORT Tspe_ColorParams* __stdcall speGetColorParams(void)
{
//ColorParams->ColorCurve = 0;
//ColorParams->ColorCorrection = 0;
//
return gEW.RenderData->ColorParams;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_ShiftParams* __stdcall speGetShiftParams(void)
{
return gEW.RenderData->ShiftParams;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_SpreadParams* __stdcall speGetSpreadParams(void)
{
return gEW.RenderData->SpreadParams;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_Filter33Params* __stdcall speGetFilter33Params(void)
{
return gEW.RenderData->Filter33Params;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_FilterIppParams* __stdcall speGetFilterIppParams(void)
{
return gEW.RenderData->FilterIppParams;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_ColorTwistIppParams* __stdcall speGetColorTwistIppParams(void)
{
return gEW.RenderData->ColorTwistIppParams;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_CompoundIppParams* __stdcall speGetCompoundIppParams(void)
{
return gEW.RenderData->CompoundIppParams;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_NozzleParams* __stdcall speGetNozzleParams(void)
{
return gEW.RenderData->NozzleParams;
}
//---------------------------------------------------------------------------
// shadow
//---------------------------------------------------------------------------
DLLEXPORT Tspe_ShadowParams* __stdcall speGetShadowParams(void)
{
return gEW.RenderData->ShadowParams;
}
//---------------------------------------------------------------------------
// shape
//---------------------------------------------------------------------------
DLLEXPORT TspeShapeType __stdcall speSetShape(TspeShapeType shapeType)
{
// set shape
return (gEW.SetShape(shapeType));
//void *vShape = gEW.SetShape(className);
//return vShape;
}
//---------------------------------------------------------------------------
// brush common
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speSetBrush(TspeBrushType brushType)
{
// set brush
return (bool)(gEW.SetBrush(brushType));
//
//void *vBrush = gEW.SetBrush(className);
//return vBrush;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSelectBrush(int bIndex, TspeBrushType &bType)
{
return gEW.SelectBrush(bIndex, bType);
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_BrushCommonParams*  __stdcall speGetBrushCommon(void)
{
if (!gEW.Brush)
   return 0;
return gEW.Brush->CommonParams;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speUpdateBrush(void)
{
if (!gEW.Surf)
   return false;
if (!gEW.Brush || !gEW.Surf->Shape)
   return false;
gEW.Surf->Brush->ColorParams->SetPrimaryColor(ptRGB(gEW.Surf->Brush->CommonParams->PrimaryColor));
gEW.Surf->Brush->Begin(gEW.Surf->Shape);
return true;
}
//---------------------------------------------------------------------------
// brush specific
//---------------------------------------------------------------------------
DLLEXPORT void* __stdcall speGetBrushSpecific(void)
{
// get brush specific params
return gEW.GetBrushSpecific();
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetBrushSpecific(void *vBsp)
{
if (!vBsp)
   return false;
// set brush specific params
gEW.SetBrushSpecific(vBsp);
return true;
}
//---------------------------------------------------------------------------
// brush stamp
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetStamp(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
return gEW.SetStamp(buffer, width, height, scanlineAlignement, shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speClearStamp(void)
{
return gEW.ClearStamp();
}
//---------------------------------------------------------------------------
// brush nozzle
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speAddPumpImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
return gEW.AddPumpImage(buffer, width, height, scanlineAlignement, shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSelectPumpImage(int idx)
{
return gEW.SelectPumpImage(idx);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeselectPump(void)
{
return gEW.DeselectPump();
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speClearPump(void)
{
return gEW.ClearPump();
}
//---------------------------------------------------------------------------
// brush textures
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetTexture(void *buffer, int width, int height, unsigned int scanlineAlignement)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->SetTexture(buffer, width, height, scanlineAlignement);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteTexture(void)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->ClearTexture();
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetBackgroundTile(void *buffer, int width, int height, unsigned int scanlineAlignement)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->SetBackgroundTile(buffer, width, height, scanlineAlignement);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteBackgroundTile(void)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->ClearBackgroundTile();
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetPolyTexture(void *buffer, int width, int height, unsigned int scanlineAlignement)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->SetPolyTexture(buffer, width, height, scanlineAlignement);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeletePolyTexture(void)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->ClearPolyTexture();
return true;
}
//---------------------------------------------------------------------------
// brush palette
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetPalette3C(unsigned int c1, unsigned int c2, unsigned int c3)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->SetPalette3C(c1, c2, c3);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetPalette255C(unsigned int *pal)
{
if (!gEW.Brush)
   return false;
for (int k = 0; k < 256; k++)
	gEW.Brush->ColorParams->PalEntry[k] = ptRGB(pal[k]);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetPaletteFromShortPal(unsigned int *pal, int count)
{
if (!gEW.Brush)
   return false;
gEW.Brush->ColorParams->SetPaletteShortPalC(pal, count);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetShortPalette(unsigned int *pal, int count)
{
gEW.RenderData->GlobalColorParams->ShortPalCount = count;
for (int k = 0; k < count; k++)
	gEW.RenderData->GlobalColorParams->ShortPalEntry[k] = ptRGB(pal[k]);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT unsigned int __stdcall speGetShortPaletteColor(int pIndex)
{
if (pIndex >= gEW.RenderData->GlobalColorParams->ShortPalCount)
   pIndex = 0;
return spCommon::ptRGB2Color(gEW.RenderData->GlobalColorParams->ShortPalEntry[pIndex]);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speGetPalette(unsigned int *pal)
{
if (!gEW.Brush)
   return false;
if (!pal)
   return false;
for (int k = 0; k < 256; k++)
	pal[k] = spCommon::ptRGB2Color(gEW.Brush->ColorParams->PalEntry[k]);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawRectangleRect(TRect &objRect, Tspe_Geometry &geometry, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
if (usePresetShape)
   {
   if (gEW.ShapeType == spe_shpRectangle)
	  {
	  return gEW.DrawObjectRect(objRect, geometry);
	  }
   }
TspeShapeType prevShapeType = gEW.ShapeType;
gEW.SetShape(spe_shpRectangle);
gEW.DrawObjectRect(objRect, geometry);
gEW.SetShape(prevShapeType);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawRectangle(int x1, int y1, int x2, int y2, Tspe_Geometry &geometry, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
TRect objRect;
objRect.left = x1;
objRect.top = y1;
objRect.right = x2;
objRect.bottom = y2;
speDrawRectangleRect(objRect, geometry, usePresetShape);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawEllipseRect(TRect &objRect, Tspe_Geometry &geometry, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
if (usePresetShape)
   {
   if (gEW.ShapeType == spe_shpEllipse)
	  {
	  gEW.DrawObjectRect(objRect, geometry);
	  return true;
	  }
   }
TspeShapeType prevShapeType = gEW.ShapeType;
gEW.SetShape(spe_shpEllipse);
if (geometry.SimplifyPolyline)
   gEW.Surf->Shape->SetSimplifyRule(true, geometry.SimplifyDPTolerance, geometry.SimplifyMaxPts);
gEW.DrawObjectRect(objRect, geometry);
gEW.SetShape(prevShapeType);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawEllipse(int x1, int y1, int x2, int y2, Tspe_Geometry &geometry, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
TRect objRect;
objRect.left = x1;
objRect.top = y1;
objRect.right = x2;
objRect.bottom = y2;
speDrawEllipseRect(objRect, geometry, usePresetShape);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawPoly(TPoint *pts, int ptsCount, Tspe_Geometry &geometry, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
if (usePresetShape)
   {
   if (gEW.ShapeType == spe_shpPoly)
	  {
	  return gEW.DrawObjectPoly(pts, ptsCount, geometry);
	  }
   }
TspeShapeType prevShapeType = gEW.ShapeType;
gEW.SetShape(spe_shpPoly);
gEW.DrawObjectPoly(pts, ptsCount, geometry);
gEW.SetShape(prevShapeType);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawPolySimple(TPoint *pts, int ptsCount, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
bool lPreset = false;
if (usePresetShape)
   {
   if (gEW.ShapeType == spe_shpLine)
	  lPreset = true;
   }
TspeShapeType prevShapeType = gEW.ShapeType;
if (!lPreset)
   gEW.SetShape(spe_shpLine);
TRect objRect;
gEW.Surf->Brush->Begin(gEW.Surf->Shape);
gEW.Surf->Shape->Reset();
gEW.Surf->Shape->DrawPoly(&objRect, pts, ptsCount);
gEW.Render->Begin(pts[0].x, pts[0].y);
gEW.Surf->Brush->PreparePoints(false);
gEW.Surf->Brush->State->wpRealStepper = gEW.Surf->Brush->State->cpRealStepper;
gEW.Render->Execute(objRect.left, objRect.top,
					 objRect.right, objRect.bottom);
gEW.Render->FinishRender();
// TODO: bigbound
TRect boundRect = gEW.Render->End(objRect.left, objRect.top,
							   objRect.right, objRect.bottom);
gEW.DoUndoStuff(boundRect);
if (!lPreset)
   gEW.SetShape(prevShapeType);
/*
gEW.Surf->Brush->Begin(gEW.Surf->Shape);
gEW.Brush->State->wpRealStepper = gEW.Brush->State->cpRealStepper;
gEW.Render->Begin(pts[0].x, pts[0].y);
for (int i = 1; i < ptsCount; i++)
	{
	gEW.Shape->Reset();
	gEW.Shape->DrawCoords(pts[i-1].x, pts[i-1].y, pts[i].x, pts[i].y);
	gEW.Brush->PreparePoints(false);
	//gEW.Surf->realStepper = gEW.Brush->State->cpRealStepper;
	gEW.Render->Execute(pts[i-1].x, pts[i-1].y, pts[i].x, pts[i].y);
	gEW.Render->FinishRender();
	}
// TODO: bigbound
TRect boundRect = gEW.Render->End(pts[0].x, pts[0].y, pts[1].x, pts[1].y);
gEW.DoUndoStuff(boundRect);
if (!lPreset)
   gEW.SetShape(prevShapeType);
*/
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawSpline(TPoint *pts, int ptsCount, Tspe_Geometry &geometry, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
if (usePresetShape)
   {
   if (gEW.ShapeType == spe_shpSplineCR)
	  {
	  return gEW.DrawObjectPoly(pts, ptsCount, geometry);
	  }
   }
TspeShapeType prevShapeType = gEW.ShapeType;
gEW.SetShape(spe_shpSplineCR);
gEW.DrawObjectPoly(pts, ptsCount, geometry);
gEW.SetShape(prevShapeType);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawSplineSimple(TPoint *pts, int ptsCount, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
bool lPreset = false;
if (usePresetShape)
   {
   if (gEW.ShapeType == spe_shpSplineCR)
	  lPreset = true;
   }
TspeShapeType prevShapeType = gEW.ShapeType;
if (!lPreset)
   gEW.SetShape(spe_shpSplineCR);
TRect objRect;
gEW.Surf->Brush->Begin(gEW.Surf->Shape);
gEW.Surf->Shape->Reset();
gEW.Surf->Shape->DrawPoly(&objRect, pts, ptsCount);
gEW.Render->Begin(pts[0].x, pts[0].y);
gEW.Surf->Brush->PreparePoints(false);
gEW.Surf->Brush->State->wpRealStepper = gEW.Surf->Brush->State->cpRealStepper;
gEW.Render->Execute(objRect.left, objRect.top,
					 objRect.right, objRect.bottom);
gEW.Render->FinishRender();
// TODO: bigbound
TRect boundRect = gEW.Render->End(objRect.left, objRect.top,
							   objRect.right, objRect.bottom);
gEW.DoUndoStuff(boundRect);
if (!lPreset)
   gEW.SetShape(prevShapeType);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawLine(int x1, int y1, int x2, int y2, bool usePresetShape = true)
{
if (!gEW.Brush)
   return false;
bool lPreset = false;
if (usePresetShape)
   {
   if (gEW.ShapeType == spe_shpLine)
	  lPreset = true;
   }
TspeShapeType prevShapeType = gEW.ShapeType;
if (!lPreset)
   gEW.SetShape(spe_shpLine);
/*
TRect objRect;
objRect.left = x1;
objRect.right = x2;
objRect.top = y1;
objRect.bottom = y2;
objRect.Normalize();
*/
gEW.Surf->Brush->Begin(gEW.Surf->Shape);
gEW.Shape->Reset();
gEW.Shape->DrawCoords(x1, y1, x2, y2);
gEW.Render->Begin(x1, y1);
//
gEW.Brush->PreparePoints(false);
//gEW.Surf->realStepper = gEW.Brush->State->cpRealStepper;
gEW.Brush->State->wpRealStepper = gEW.Brush->State->cpRealStepper;
gEW.Render->Execute(x1, y1, x2, y2);
gEW.Render->FinishRender();
//TODO: dodati EndObject(Trect* )
TRect boundRect = gEW.Render->End(x1, y1, x2, y2);
gEW.DoUndoStuff(boundRect);
if (!lPreset)
   gEW.SetShape(prevShapeType);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speGetMouseData(Tspe_MouseData &md)
{
// save mouse data
if (!gEW.Shape)
   return false;
md.X_down = gEW.Shape->MouseData.X_down;
md.Y_down = gEW.Shape->MouseData.Y_down;
md.X_up   = gEW.Shape->MouseData.X_up;
md.Y_up   = gEW.Shape->MouseData.Y_up;
md.X_move_from = gEW.Shape->MouseData.X_move_from;
md.Y_move_from = gEW.Shape->MouseData.Y_move_from;
md.X_move_to = gEW.Shape->MouseData.X_move_to;
md.Y_move_to = gEW.Shape->MouseData.Y_move_to;
md.X_min  = gEW.Shape->MouseData.X_min;
md.Y_min  = gEW.Shape->MouseData.Y_min;
md.X_max  = gEW.Shape->MouseData.X_max;
md.Y_max  = gEW.Shape->MouseData.Y_max;
md.X_wrap_from = gEW.Shape->MouseData.X_wrap_from;
md.Y_wrap_from = gEW.Shape->MouseData.Y_wrap_from;
md.X_wrap_to = gEW.Shape->MouseData.X_wrap_to;
md.Y_wrap_to = gEW.Shape->MouseData.Y_wrap_to;
md.MouseButtonDown = gEW.Shape->MouseData.MouseButtonDown;
md.Shift  = gEW.Shape->MouseData.Shift;
return true;
}
//---------------------------------------------------------------------------
// NOT exported
/*obsolete
void __stdcall spePolyMouseUp(TShiftState Shift, int X, int Y)
{
TRect boundRect;
gEW.Render->Execute(gEW.Shape->MouseData.X_min, gEW.Shape->MouseData.Y_min,
					 gEW.Shape->MouseData.X_max, gEW.Shape->MouseData.Y_max);
gEW.Render->FinishRender();
boundRect = gEW.Render->End(gEW.Shape->MouseData.X_min, gEW.Shape->MouseData.Y_min,
							 gEW.Shape->MouseData.X_max, gEW.Shape->MouseData.Y_max);
gEW.DoUndoStuff(boundRect);
}
*/
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speHandMouseDown(TShiftState Shift, int X, int Y)
{
return gEW.WMouseDown(Shift, X, Y);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speHandMouseMove(TShiftState Shift, int X, int Y)
{
return gEW.WMouseMove(Shift, X, Y);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speHandMouseUp(TShiftState Shift, int X, int Y)
{
return gEW.WMouseUp(Shift, X, Y);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetSizeEnvelope(wchar_t* str)
{
if (!gEW.Brush)
   return false;
gEW.Brush->SetSizeEnvelope(str);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetCapacityEnvelope(wchar_t* str)
{
if (!gEW.Brush)
   return false;
gEW.Brush->SetCapacityEnvelope(str);
return true;
}
//---------------------------------------------------------------------------
/* obsolete -> set through parameters
DLLEXPORT bool __stdcall speSetScaleColors(unsigned int botVal, unsigned int topVal)
{
return gEW.SetScaleColors(botVal, topVal);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speResetScaleColors(void)
{
return gEW.SetScaleColors(0x00000000, 0x00ffffff);
}
// ovo gore eventualno radi panic momenta
*/
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetCloneImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (!gEW.RenderData)
   return false;
return gEW.SetClone(buffer, width, height, scanlineAlignement, shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetClonePosition(int X, int Y)
{
if (!gEW.RenderData)
   return false;
gEW.RenderData->GlobalColorParams->CloneStartX = X;
gEW.RenderData->GlobalColorParams->CloneStartY = Y;
gEW.RenderData->GlobalColorParams->Clone_XS = X;
gEW.RenderData->GlobalColorParams->Clone_YS = Y;
gEW.RenderData->GlobalColorParams->Clone_X = X;
gEW.RenderData->GlobalColorParams->Clone_Y = Y;
gEW.RenderData->GlobalColorParams->Clone_Started = false;
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteCloneImage(void)
{
if (!gEW.RenderData)
   return false;
return gEW.DeleteClone();
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speCloneSelf(TspeCloneType type)
{
if (!gEW.Surf)
   return false;
if (!gEW.RenderData)
   return false;
if (type == spe_cltDirect)         // clone original
   gEW.Surf->CloneSelf = gEW.Surf->Orig;
else if (type == spe_cltBuffered)  // clone edit
   gEW.Surf->CloneSelf = gEW.Surf->Edit;
else // do not use self cloning - use external image
   gEW.Surf->CloneSelf = 0;
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetStencil(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
return gEW.SetAssImage(0, buffer, width, height, scanlineAlignement, shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetStencilPositionStatus(int X, int Y, bool alive)
{
if (!gEW.Surf)
   return false;
gEW.Surf->Stencil_i = Y;
gEW.Surf->Stencil_j = X;
gEW.Surf->StencilAlive = alive;
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteStencil(void)
{
return gEW.DeleteAssImage(0);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speInvertStencil(void)
{
return gEW.InvertAssImage(0);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetContour(void *buffer, bool shared)
{
if (!gEW.Surf)
   return false;
return gEW.SetAssImage(1, buffer, gEW.Surf->Width, gEW.Surf->Height, gEW.Surf->Orig->Alignment(), shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteContour(void)
{
return gEW.DeleteAssImage(1);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speInvertContour(void)
{
return gEW.InvertAssImage(1);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetMask(void *buffer, bool shared)
{
if (!gEW.Surf)
   return false;
return gEW.SetAssImage(2, buffer, gEW.Surf->Width, gEW.Surf->Height, gEW.Surf->Orig->Alignment(), shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteMask(void)
{
return gEW.DeleteAssImage(2);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speInvertMask(void)
{
return gEW.InvertAssImage(2);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speInternalToMask(TspeInternalMaskType imt)
{
if (!gEW.Surf)
   return false;
if (!gEW.Surf->AssMask)
   return false;
if (imt == spe_RightMask)
   gEW.Surf->AssMask->Copy(gEW.Surf->MaskRight);
else
   gEW.Surf->AssMask->Copy(gEW.Surf->MaskLeft);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetSidekick(void *buffer, bool shared)
{
if (!gEW.Surf)
   return false;
return gEW.SetAssImage(4, buffer, gEW.Surf->Width, gEW.Surf->Height, gEW.Surf->Orig->Alignment(), shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteSidekick(void)
{
return gEW.DeleteAssImage(4);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speInvertSidekick(void)
{
return gEW.InvertAssImage(4);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetNozzle(void *buffer, void *alpha, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (!gEW.RenderData)
   return false;
return gEW.SetNozzle(buffer, alpha, width, height, scanlineAlignement, shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteNozzle(void)
{
if (!gEW.RenderData)
   return false;
return gEW.DeleteNozzle();
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetSelection(void *buffer, bool shared)
{
if (!gEW.Surf)
   return false;
//gEW.Surf->Selection = new spImage<ptGray>(buffer, gEW.Surf->Width, gEW.Surf->Height, gEW.Surf->Orig->Alignment(), shared, orInvert);
return gEW.SetSelection(buffer, shared);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteSelection(void)
{
if (!gEW.Surf)
   return false;
return gEW.DeleteSelection();
}
//---------------------------------------------------------------------------
/* obsolete
DLLEXPORT bool __stdcall speSelectionClipping(bool clip)
{
if (!gEW.Surf)
   return false;
gEW.Surf->SelClipping = clip;
return true;
}
*/
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speCheckMarker(int width, int height)
{
if (!gEW.Surf)
   return false;
if (gEW.Surf->AssMarker)
   return (gEW.Surf->AssMarker->Width() == width && gEW.Surf->AssMarker->Height() == height);
return false;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetMarker(void *buffer, bool shared)
{
if (!gEW.Surf)
   return false;
bool lRet = gEW.SetAssImage(3, buffer, gEW.Surf->Width, gEW.Surf->Height, gEW.Surf->Orig->Alignment(), shared);
if (lRet)
   {
   gEW.Surf->MaskLeft->Copy(gEW.Surf->AssMarker);
   gEW.Surf->MaskRight->Copy(gEW.Surf->AssMarker);
   gEW.RenderData->RenderParams->CreateUndo = false;
   gEW.RenderData->RenderParams->AutoCommit = false;
   gEW.RenderData->RenderParams->ClearImoc  = false;
   }
return lRet;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDeleteMarker(void)
{
if (!gEW.Surf)
   return false;
gEW.Surf->MaskLeft->Zero();
gEW.Surf->MaskRight->Zero();
gEW.RenderData->RenderParams->CreateUndo = true;
gEW.RenderData->RenderParams->AutoCommit = true;
gEW.RenderData->RenderParams->ClearImoc =  true;
return gEW.DeleteAssImage(3);
}
//---------------------------------------------------------------------------
// get image pixel from surface
DLLEXPORT bool __stdcall speGetPixelRGB(const int &y, const int &x, Byte &r, Byte &g, Byte &b)
{
if (!gEW.Surf)
   return false;
ptRGB *pix = &(gEW.Surf->Edit->Pix[y][x]);
b = pix->ch[0];
g = pix->ch[1];
r = pix->ch[2];
return true;
}
//---------------------------------------------------------------------------
// get image pixel from surface
DLLEXPORT bool __stdcall speGetPixelColor(const int &y, const int &x, unsigned int &color)
{
if (!gEW.Surf)
   return false;
ptRGB *pix = &(gEW.Surf->Edit->Pix[y][x]);
color = (unsigned int)RGB(pix->ch[2], pix->ch[1], pix->ch[0]);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speSetColorCurve(Byte **LUTCurveEntry)
{
return gEW.SetColorCurve(LUTCurveEntry);
}
//---------------------------------------------------------------------------
// apply action on complete image  or roi  (or selection if any)
DLLEXPORT bool __stdcall speApplyOnImage(TRect *boundRect, TspeApplyType appType)
{
if (!gEW.Surf)
   return false;
bool selClip = gEW.RenderData->RenderParams->SelectionClipping;
if (appType == spe_appSelection)
   {
   if (!gEW.Surf->Selection)
	  return false;
   gEW.RenderData->RenderParams->SelectionClipping = true;
   gEW.Surf->MaskRight->Copy(gEW.Surf->Selection);
   }
else if (appType == spe_appMask)
   {
   if (!gEW.Surf->AssMask)
	  return false;
   gEW.RenderData->RenderParams->SelectionClipping = false;
   gEW.Surf->MaskRight->Copy(gEW.Surf->AssMask);
   }
else if (appType == spe_appContour)
   {
   if (!gEW.Surf->AssContour)
	  return false;
   gEW.RenderData->RenderParams->SelectionClipping = false;
   gEW.Surf->MaskRight->Copy(gEW.Surf->AssContour);
   }
else 	// complete image
	{
	gEW.RenderData->RenderParams->SelectionClipping = false;
	gEW.Surf->MaskRight->Fill(255);
	}
gEW.Render->Begin(0, 0);
gEW.Render->DrawImage(boundRect);
gEW.Render->FinishRender();
gEW.RenderData->RenderParams->SelectionClipping = selClip;
gEW.DoUndoStuff((TRect &)(gEW.Render->BoundRect));
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speApplyAuxMask(TRect *boundRect, void *buffer, int width, int height, unsigned int scanlineAlignment)
{
if (!gEW.Surf)
   return false;
bool selClip = gEW.RenderData->RenderParams->SelectionClipping;
gEW.RenderData->RenderParams->SelectionClipping = false;
spImage<ptGray> *maskAux = new spImage<ptGray>(buffer, width, height, scanlineAlignment, true, orInvert);
gEW.Surf->MaskRight->Fill(0);
gEW.Surf->MaskRight->SetRoi(boundRect);
gEW.Surf->MaskRight->CopyRoi(maskAux);
gEW.Render->Begin(0, 0);
gEW.Render->DrawImage(boundRect);
gEW.Render->FinishRender();
gEW.Render->End(boundRect->left, boundRect->top,
							   boundRect->right, boundRect->bottom);
gEW.RenderData->RenderParams->SelectionClipping = selClip;
gEW.Surf->MaskRight->SetRoi();
delete maskAux;
gEW.DoUndoStuff((TRect &)(gEW.Render->BoundRect));
return true;
}
//---------------------------------------------------------------------------
// drop shadow inside bound rectangle
DLLEXPORT bool __stdcall speDropShadow(TRect *boundRect, TspeApplyType appType)
{
if (!gEW.Surf)
   return false;
gEW.DropShadow(boundRect, appType);
gEW.DoUndoStuff(*boundRect);
return true;
}
//---------------------------------------------------------------------------
// get last bound rect
DLLEXPORT void __stdcall speGetBoundRect(TRect *boundRect)
{
boundRect->left   = gEW.Render->BoundRect.left;
boundRect->right  = gEW.Render->BoundRect.right;
boundRect->top    = gEW.Render->BoundRect.top;
boundRect->bottom = gEW.Render->BoundRect.bottom;
}
//---------------------------------------------------------------------------
// reset cumulative bound rect
DLLEXPORT void __stdcall speResetBigBoundRect(void)
{
if (!gEW.Surf)
   return;
gEW.Surf->BigBoundRect.left  = 999999;
gEW.Surf->BigBoundRect.right = 0;
gEW.Surf->BigBoundRect.top   = 999999;
gEW.Surf->BigBoundRect.bottom = 0;
}
//---------------------------------------------------------------------------
DLLEXPORT void __stdcall speAddBigBoundRect(TRect *boundRect)
{
if (!gEW.Surf)
   return;
TRect *bbr = &(gEW.Surf->BigBoundRect);
TRect *addRect;
if (boundRect)
	addRect = boundRect;
else
    addRect = &(gEW.Render->BoundRect);
if (addRect->left < bbr->left)
	bbr->left = addRect->left;
if (addRect->top < bbr->top)
	bbr->top = addRect->top;
if (addRect->right > bbr->right)
	bbr->right = addRect->right;
if (addRect->bottom > bbr->bottom)
   bbr->bottom = addRect->bottom;
}
//---------------------------------------------------------------------------
DLLEXPORT void __stdcall speGetBigBoundRect(TRect *boundRect)
{
if (!gEW.Surf)
   return;
TRect *bbr = &(gEW.Surf->BigBoundRect);
if (bbr->left < 0)
   bbr->left = 0;
if (bbr->top < 0)
   bbr->top = 0;
if (bbr->right > gEW.Surf->Width)
   bbr->right = gEW.Surf->Width;
if (bbr->bottom > gEW.Surf->Height)
   bbr->bottom = gEW.Surf->Height;
boundRect->left   = bbr->left;
boundRect->right  = bbr->right;
boundRect->top    = bbr->top;
boundRect->bottom = bbr->bottom;
}
//---------------------------------------------------------------------------
// set bound rectangle
DLLEXPORT bool __stdcall speSetUndoRect(TRect *boundRect, bool createUndoBlock = true)
{
if (!gEW.Surf)
   return false;
if (!gEW.Render)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
gEW.Render->SetBoundRect(roiRect);
if (createUndoBlock)
   gEW.CreateUndoBlock(&roiRect);
return true;
}
//---------------------------------------------------------------------------
// commit action
DLLEXPORT bool __stdcall speCommit()
{
if (!gEW.Surf)
   return false;
gEW.Finish(&(gEW.Render->BoundRect));
return true;
}
//---------------------------------------------------------------------------
// roll back last rendering
DLLEXPORT bool __stdcall speRollback(void)
{
if (!gEW.Surf)
   return false;
gEW.Rollback(&(gEW.Render->BoundRect));
return true;
}
//---------------------------------------------------------------------------
// roll back last rendering
DLLEXPORT bool __stdcall speRollbackRect(TRect *boundRect)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
gEW.Rollback(&roiRect);
return true;
}
//---------------------------------------------------------------------------
// set surface roi
DLLEXPORT bool __stdcall speSetRoi(TRect &roiRect)
{
if (!gEW.Surf)
   return false;
gEW.Surf->Orig->SetRoi(&roiRect);
gEW.Surf->Edit->SetRoi(&roiRect);
return true;
}
//---------------------------------------------------------------------------
// reset surface roi
DLLEXPORT bool __stdcall speResetRoi(void)
{
if (!gEW.Surf)
   return false;
gEW.Surf->Orig->SetRoi();
gEW.Surf->Edit->SetRoi();
return true;
}
//---------------------------------------------------------------------------
// clear internal masks
DLLEXPORT bool __stdcall speClearInternalMasks(void)
{
if (!gEW.Surf)
   return false;
gEW.Surf->MaskLeft->Zero();
gEW.Surf->MaskRight->Zero();
return true;
}
//---------------------------------------------------------------------------
// set surface roi
DLLEXPORT bool __stdcall speUpdateImmediate(bool updateMode)
{
if (!gEW.Surf)
   return false;
gEW.Surf->Orig->SetRoi();
gEW.Surf->Edit->SetRoi();
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_UndoBlock* __stdcall speGetUndo(void)
// get undo block
{
if (!gEW.Surf)
   return 0;
return &(gEW.UndoBlock);
}
//---------------------------------------------------------------------------
// restore undo block - redo
DLLEXPORT Tspe_UndoBlock*  __stdcall speRestoreUndo(Tspe_UndoBlock* externalBlock)
{
if (!gEW.Surf)
   return 0;
//TODO: eventually add surface ID into undo structure and return false if not the same
if (!gEW.RestoreUndoBlock(externalBlock))
   return 0;
return &(gEW.RedoBlock);
}
//---------------------------------------------------------------------------
// collect color correction data
DLLEXPORT bool  __stdcall speCollectCCD(unsigned long surfaceId, TRect *boundRect, bool source, bool selection)
{
if (!gEW.Surf)
   return false;
bool lRet;
int saveId = gEW.Surf->SurfaceId;
if (surfaceId != gEW.Surf->SurfaceId)
   {
   if (gEW.SelectSurfaceById(surfaceId) == -1) // ups!
	  {
	  gEW.SelectSurfaceById(saveId); //restore original surface
	  return false;
	  }
   }
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
lRet = gEW.CollectCCD(surfaceId, &roiRect, source, selection);
gEW.SelectSurfaceById(saveId); //restore original surface
return lRet;
}
//---------------------------------------------------------------------------
// get internal mask
DLLEXPORT void* __stdcall speGetInternalMask(TspeInternalMaskType imt)
{
if (!gEW.Surf)
   return 0;
if (imt == spe_RightMask)
	return gEW.Surf->MaskRight->GetOriginalBuffer();
return gEW.Surf->MaskLeft->GetOriginalBuffer();
}
//---------------------------------------------------------------------------
// get internal image (imageEdit)
DLLEXPORT void __stdcall speGetInternalImage(void *&image, void *&alpha)
{
if (!gEW.Surf)
   {
   image = 0;
   alpha = 0;
   return;
   }
image = gEW.Surf->Edit->GetOriginalBuffer();
if (gEW.Surf->AlphaEdit)
   alpha = gEW.Surf->AlphaEdit->GetOriginalBuffer();
else
   alpha = 0;
}
//---------------------------------------------------------------------------
// set brush mask call back function
DLLEXPORT void __stdcall speSetCbBrushMask(TCbBrushMask cbBrushMask)
{
gEW.CbBrushMask = cbBrushMask;
}
//---------------------------------------------------------------------------
// clear multistroke bucket
DLLEXPORT void __stdcall speClearMultiStroke(void)
{
gEW.ClearMultiStroke(true);
}
//---------------------------------------------------------------------------
// add brush to multistroke bucket
DLLEXPORT int  __stdcall speAddMultiStroke(int brushIndex)
{
return gEW.AddMultiStroke(brushIndex);
}
//---------------------------------------------------------------------------
// set multistroke position
DLLEXPORT bool  __stdcall speSetMultiStrokePos(int &msIndex, int &X, int &Y)
{
return gEW.SetMultiStrokePos(msIndex, X, Y);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawMultiPoly(TPoint **pts, int *ptsCount, Tspe_Geometry &geometry)
{
if (!gEW.Surf)
   return false;
return gEW.DrawMultiPoly(pts, ptsCount, geometry);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawMultiPolyByIndex(int &msIndex, TPoint *pts, int &ptsCount, Tspe_Geometry &geometry)
{
if (!gEW.Surf)
   return false;
return gEW.DrawMultiPolyByIndex(msIndex, pts, ptsCount, geometry);
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speDrawMultiPolyFinish(void)
{
if (!gEW.Surf)
   return false;
TRect bbr = spCommon::GetBigBound(gEW.Surf->Width, gEW.Surf->Height);
gEW.DoUndoStuff(bbr);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall speUpdateMultiBrush(int &msIndex, bool cloneCurrent)
{
if (!gEW.Surf)
   return false;
TStrokeBucket *mBucket =  gEW.GetMultiStrokeBucket(msIndex);
if (cloneCurrent)
   {
   delete mBucket->brush;
   mBucket->brush = gEW.Brush->Clone();
   mBucket->brush->Connect(gEW.Surf->MaskLeft, gEW.Surf->MaskRight, mBucket->shape);
   }
else
   mBucket->brush->ColorParams->SetPrimaryColor(ptRGB(mBucket->brush->CommonParams->PrimaryColor));
mBucket->brush->Begin(mBucket->shape);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT int __stdcall speGetMultiStrokeSize(void)
{
return gEW.MultiStrokeSize;
}
//---------------------------------------------------------------------------
DLLEXPORT Tspe_BrushCommonParams*  __stdcall speGetMultiBrushCommon(int &msIndex)
{
if (!gEW.Brush)
   return 0;
TStrokeBucket *stroke =  gEW.GetMultiStrokeBucket(msIndex);
return stroke->brush->CommonParams;
}
//---------------------------------------------------------------------------
// some extras
DLLEXPORT bool __stdcall speBlendImageMaskColor(unsigned int color, bool zeroMask, TspeInternalMaskType imt)
{
if (!gEW.Surf)
   return false;
spImage<ptGray> *iMask;
spImage<ptRGB>  *iRGB  = gEW.Surf->Orig;
if (imt == spe_RightMask)
	iMask = gEW.Surf->MaskRight;
else
	iMask = gEW.Surf->MaskLeft;
if (zeroMask)
   {
   for (int i = 0; i < iRGB->Height(); i++)
	   {
	   for (int j = 0; j < iRGB->Width(); j++)
		   {
		   if (iMask->Pixel(i, j).ch[0] == 0x00)
			   iRGB->Pixel(i, j) = ptRGB(color);
		   }
	   }
	}
else
   spCommon::BlendImageMaskColor(iRGB, iMask, ptRGB(color), iRGB);
return true;
}
//---------------------------------------------------------------------------
// get library version
DLLEXPORT wchar_t* __stdcall speGetVersion(void)
{
UnicodeString ver = "2.0";
return ver.c_str();
}
//===========================================================================
// SPE processing  - direct
//===========================================================================
DLLEXPORT bool  __stdcall speProcPalNearestLuminance(TRect *boundRect, int palCount)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
spProcess::NearestLuminance(gEW.Surf->Edit, gEW.Surf->Orig, &roiRect, gEW.Brush->ColorParams->PalEntry, palCount);
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcPalNearestColor(TRect *boundRect, int palCount)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
spProcess::NearestColor(gEW.Surf->Edit, gEW.Surf->Orig, &roiRect, gEW.Brush->ColorParams->PalEntry, palCount);
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcPalPseudoColor(TRect *boundRect, int palCount)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
spProcess::PseudoColor(gEW.Surf->Edit, gEW.Surf->Orig, &roiRect, gEW.Brush->ColorParams->PalEntry, palCount);
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcTest(TRect *boundRect)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
spProcess::ColorQuantization(gEW.Surf->Edit, gEW.Surf->Orig, &roiRect);
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcGrayscale(TRect *boundRect, double rFact, double gFact, double bFact)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
spProcess::Grayscale(gEW.Surf->Edit, gEW.Surf->Orig, &roiRect, rFact, gFact, bFact);
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcMirror(TRect *boundRect, int axis)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
spProcess::Mirror(gEW.Surf->Edit, gEW.Surf->Orig, &roiRect, axis);
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcArithBool(TRect *boundRect, TspeArithBool opType, int opValue)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
spProcess::ArithBool(gEW.Surf->Edit, gEW.Surf->Orig, &roiRect, opType, opValue);
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcTessellRedraw(Tspe_Tessellation &params)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(0, roiRect);
spTessellation::Redraw(gEW.Surf->Edit, gEW.Surf->Orig, gEW.Surf->MaskLeft, params);
if (params.EdgeType > 0)
   {
   if (params.EdgeType < 3)
	  spTessellation::DrawSolidEdges(gEW.Surf->Edit, gEW.Surf->Orig, params);
   else
	  {
	  spImage<ptRGB> *saveEdit = new spImage<ptRGB>(gEW.Surf->Edit);
	  gEW.Surf->Edit->Copy(gEW.Surf->Orig);
	  gEW.Surf->MaskLeft->Zero();
	  gEW.Surf->MaskRight->Zero();
	  spTessellation::DrawMaskedEdges(gEW.Surf->MaskRight, params);
	  if (params.EdgeBlurRadius > 0)
		 spCommon::FBGrayRun(gEW.Surf->MaskRight, &roiRect, params.EdgeBlurRadius);
	  // draw mask with current brush
	  gEW.Render->Begin(0, 0);
	  gEW.Render->DrawImage(&roiRect);
	  gEW.Render->FinishRender();
	  gEW.Surf->Edit->Copy(saveEdit);
	  delete saveEdit;
	  //
	  }
   }
gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcTessellation(Tspe_Tessellation &params)
{
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(0, roiRect);
spTessellation::Tesselate(gEW.Surf->Edit, gEW.Surf->Orig, gEW.Surf->MaskLeft, params);
return speProcTessellRedraw(params);
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speProcTessellClear(void)
{
spTessellation::Clear();
return true;
}
//===========================================================================
// SPE warping
//===========================================================================
DLLEXPORT bool  __stdcall speWarpBegin(Tspe_WarpParams &params)
{
return gEW.WarpBegin(params);
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speWarpExecute(int &X1, int &Y1, int &X2, int &Y2)
{
if (!gEW.Surf)
   return false;
return gEW.WarpExecute(X1, Y1, X2, Y2);
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speWarpEnd(void)
{
TRect roiRect;
if (gEW.WarpEnd(&roiRect))
   gEW.DoUndoStuff(roiRect);
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall speApplyWarpMesh(Tspe_WarpParams &params)
{
TRect roiRect;
speSetProperRectangle(0, roiRect);
gEW.ApplyWarpMesh(params);
gEW.DoUndoStuff(roiRect);
return true;
}
//===========================================================================
// SPE utilities (geometry/symmetry/etc..)
//===========================================================================
DLLEXPORT void __stdcall speUtilGetLineData(double &outRadius, double &outAngle, int &X1, int &Y1, int &X2, int &Y2)
{
spGeometry::GetLineData(&outRadius, &outAngle, X1, Y1, X2, Y2);
}
//---------------------------------------------------------------------------
DLLEXPORT void __stdcall speUtilGetSimplifiedPoly(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts)
{
gEW.GetSimplifiedPoly(polyPts, polyCount, dpTolerance, maxPts);
}
//---------------------------------------------------------------------------
DLLEXPORT void __stdcall speUtilGetSplinePoly(TPoint *polyPts, int &polyCount, bool closed)
{
gEW.GetSplinePoly(polyPts, polyCount, closed);
}
//---------------------------------------------------------------------------
DLLEXPORT TPoint* __stdcall speUtilGetEllipsePoints(TRect &objRect, double &RotationAngle, TPoint &RotationOrigin, int &countPts)
{
//if (!gEW.Surf)
//   return 0;
if (gEW.ShapeType != spe_shpEllipse)
   gEW.SetShape(spe_shpEllipse);
gEW.Shape->Reset();
gEW.Surf->Shape->DrawBound(&objRect);
if (RotationAngle != 0)
   gEW.Surf->Shape->Rotate(&objRect, &RotationOrigin, -RotationAngle);
gEW.Surf->Shape->GetRawPoints();
countPts = gEW.Surf->Shape->CountRaw;
return gEW.Surf->Shape->PointRaw;
}
//---------------------------------------------------------------------------
DLLEXPORT TPoint* __stdcall speUtilGetLinePoints(int &X1, int &Y1, int &X2, int &Y2, int &countPts)
{
//if (!gEW.Surf)
//   return 0;
if (gEW.ShapeType != spe_shpLine)
   gEW.SetShape(spe_shpLine);
gEW.Shape->Reset();
gEW.Shape->DrawCoords(X1, Y1, X2, Y2);
gEW.Surf->Shape->GetRawPoints();
countPts = gEW.Surf->Shape->CountRaw;
return gEW.Surf->Shape->PointRaw;
}
//---------------------------------------------------------------------------
DLLEXPORT void __stdcall speUtilScalePoly(TPoint *polyPts, int &polyCount, float factorX, float factorY)
{
int index = 1;
float vect_x, vect_y;
TPoint pt;
for (int i = 1; i < polyCount; i++)
   {
   pt = polyPts[i];
   // very primitive - more precise (use trigonometry)
   vect_x = pt.x - polyPts[i - 1].x;
   vect_y = pt.y - polyPts[i - 1].y;
   vect_x *= factorX;
   vect_y *= factorY;
   pt.x = fast_ftol(polyPts[index-1].x + vect_x);
   pt.y = fast_ftol(polyPts[index-1].y + vect_y);
   if (pt.x == polyPts[index-1].x && pt.y == polyPts[index-1].y) // overlap points
	  continue;
   polyPts[index] = pt;
   index++;
   }
polyCount = index;
}
//---------------------------------------------------------------------------
// get color from temperature
DLLEXPORT unsigned int  __stdcall speUtilColorFromTemperature(double temperature)
{
ptRGB color;
spPixelArithmetics::Temperature_RGB(temperature, color);
return spCommon::ptRGB2Color(color);
}
//===========================================================================
// IPP library calls
//===========================================================================
// is ipp library installed
DLLEXPORT bool  __stdcall ippIsInstalled(void)
{
return spCommon::ippAlive;
}
//---------------------------------------------------------------------------
// apply direct filter edit->orig
DLLEXPORT bool  __stdcall ippApplyFilter(TRect *boundRect)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (gEW.AppIppFilter(&roiRect))
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// apply direct color twisting edit->orig
DLLEXPORT bool  __stdcall ippApplyColorTwist(TRect *boundRect)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (gEW.AppIppColorTwist(&roiRect))
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// apply direct compound filter edit->orig
DLLEXPORT bool  __stdcall ippApplyCompound(TRect *boundRect)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (gEW.AppIppCompound(&roiRect))
   {
   gEW.DoUndoStuff(roiRect);
   return false;
   }
return true;
}
//---------------------------------------------------------------------------
// apply direct distance transform edit->orig
DLLEXPORT bool  __stdcall ippApplyDistanceTransform(TRect *boundRect, bool kernel55, int norm, bool *chanOn)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (ippProcess::DistanceTransform(gEW.Surf->ippEdit, gEW.Surf->ippOrig, &roiRect, kernel55 ? 5 : 3, norm, chanOn) == 0)
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// apply direct distance fast marching edit->orig
DLLEXPORT bool  __stdcall ippApplyFastMarchingDistanceTransform(TRect *boundRect, float radius, bool *chanOn)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (ippProcess::FastMarching(gEW.Surf->ippEdit, gEW.Surf->ippOrig, &roiRect, radius, chanOn) == 0)
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// apply direct gradient color to gray
DLLEXPORT bool  __stdcall ippApplyGradientColor(TRect *boundRect, int norm, bool *chanOn)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (ippProcess::GradientColor2Gray(gEW.Surf->ippEdit, gEW.Surf->ippOrig, &roiRect, norm, chanOn) == 0)
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// apply direct reduce bits
DLLEXPORT bool  __stdcall ippApplyReduceBits(TRect *boundRect, int noise, int ditherType, int levels)
{
// noise [0..100], ditherType (IPP manual), levels [2..(1<<8)] => [2..256]
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (ippProcess::ReduceBits(gEW.Surf->ippEdit, gEW.Surf->ippOrig, &roiRect, noise, ditherType, levels) == 0)
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// apply direct mirror
DLLEXPORT bool  __stdcall ippApplyMirror(TRect *boundRect, int axis)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (ippProcess::Mirror(gEW.Surf->ippEdit, gEW.Surf->ippOrig, &roiRect, axis) == 0)
   {
   if (gEW.Surf->AlphaEdit)
   	  ippProcess::Mirror(gEW.Surf->ippAlphaEdit, gEW.Surf->ippAlphaOrig, &roiRect, axis);
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// apply ipp warping
DLLEXPORT bool  __stdcall ippApplyQuadTransform(Tspe_QuadIppParams &params)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(&(params.RoiRect), roiRect);
if (gEW.AppIppQuad(&roiRect, params))
   {
   TRect roiRect;
   speSetProperRectangle(0, roiRect);
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
DLLEXPORT bool __stdcall ippApplyDeconvolveLR(TRect *boundRect, int kernel, int threshold, int iterations)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (ippProcess::DeconvolveLR(gEW.Surf->ippEdit, gEW.Surf->ippOrig, &roiRect, kernel, (float) threshold, iterations) == 0)
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// compute histogram
DLLEXPORT bool  __stdcall ippComputeHistogram(TRect *boundRect, int nBins, unsigned int** histo)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
bool lRet;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
return ippStatistic::Histogram(gEW.Surf->ippOrig, &roiRect, nBins, histo);
}
//---------------------------------------------------------------------------
// apply lut
DLLEXPORT bool  __stdcall ippApplyLUT(TRect *boundRect, int nBins, unsigned int** lut, int interpol)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
bool lRet;
TRect roiRect;
speSetProperRectangle(boundRect, roiRect);
if (ippStatistic::ApplyLUT(gEW.Surf->ippOrig, &roiRect, nBins, lut, interpol) == 0)
   {
   gEW.DoUndoStuff(roiRect);
   return true;
   }
return false;
}
//---------------------------------------------------------------------------
// init image statistics (moments)
DLLEXPORT bool  __stdcall ippInitStatistics(void)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
// use left mask as grayscaled - luminance
spProcess::Convert(gEW.Surf->Orig, gEW.Surf->MaskLeft, true);
ippStatistic::SetStatImages(gEW.Surf->ippOrig, gEW.Surf->ippMaskLeft);
ippStatistic::InitMoments();
return true;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall ippInitStatisticsAux(void *bufferColor, void *bufferGray, int width, int height, unsigned int scanlineAlignment)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
// set aux statistic images
ippStatistic::SetStatImages(bufferColor, bufferGray, width, height, scanlineAlignment);
ippStatistic::InitMoments();
return true;
}
//---------------------------------------------------------------------------
// compute image moments
DLLEXPORT bool  __stdcall ippComputeMoments(TRect *boundRect, bool grayscale)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
ippStatistic::SetStatRectangle(boundRect, roiRect);
int nRet;
if (grayscale)
   nRet = ippStatistic::ComputeMoments(ippStatistic::ImageGray, &roiRect);
else
   nRet = ippStatistic::ComputeMoments(ippStatistic::ImageColor, &roiRect);
return (nRet == 0);
}
//---------------------------------------------------------------------------
// get image moment
DLLEXPORT double  __stdcall ippGetMoment(int mOrd, int nOrd, int channel, TippMomentType type)
{
if (!spCommon::ippAlive)
   return 0;
if (!gEW.Surf)
   return 0;
return ippStatistic::GetMoment(mOrd, nOrd, channel, type);
}
//---------------------------------------------------------------------------
// compute image norms
DLLEXPORT bool  __stdcall ippComputeNorms(TRect *boundRect, bool grayscale)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
TRect roiRect;
ippStatistic::SetStatRectangle(boundRect, roiRect);
int nRet;
if (grayscale)
   nRet = ippStatistic::ComputeNorms(ippStatistic::ImageGray, &roiRect);
else
   nRet = ippStatistic::ComputeNorms(ippStatistic::ImageColor, &roiRect);
return (nRet == 0);
}
//---------------------------------------------------------------------------
// get image norm
DLLEXPORT double  __stdcall ippGetNorm(int channel, TippNormType type)
{
if (!spCommon::ippAlive)
   return 0;
if (!gEW.Surf)
   return 0;
return ippStatistic::GetNorm(channel, type);
}
//---------------------------------------------------------------------------
// release image statistics (moments)
DLLEXPORT bool  __stdcall ippReleaseStatistics(void)
{
if (!spCommon::ippAlive)
   return false;
if (!gEW.Surf)
   return false;
// clear left mask
gEW.Surf->MaskLeft->Zero();
ippStatistic::ReleaseMoments();
return true;
}
//===========================================================================
DLLEXPORT bool  __stdcall ippApplyFilterAux(void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment, TspeImageType imgType,
										   Tspe_FilterIppParams *filterParams, TRect *boundRect)
{
if (!spCommon::ippAlive)
   return false;
bool lRet;
TRect roiRect;
if (boundRect)
   roiRect = *boundRect;
else
   {
   roiRect.left = 0;
   roiRect.right = width;
   roiRect.top = 0;
   roiRect.bottom = height;
   }
if (imgType == spe_it_24RGB)
   lRet = ippAux::IppFilter<ippRGB>(buffer1, buffer2, width, height, scanlineAlignment, filterParams, &roiRect);
else if (imgType == spe_it_8Gray)
   lRet = ippAux::IppFilter<ippGray>(buffer1, buffer2, width, height, scanlineAlignment, filterParams, &roiRect);
else
   return false; 	// currently - float not supported
return lRet;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall ippApplyArithBoolAux(TippArithBool aType, void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment,
										   TspeImageType imgType, TRect *boundRect)
{
if (!spCommon::ippAlive)
   return false;
bool lRet;
TRect roiRect;
if (boundRect)
   roiRect = *boundRect;
else
   {
   roiRect.left = 0;
   roiRect.right = width;
   roiRect.top = 0;
   roiRect.bottom = height;
   }
if (imgType == spe_it_24RGB)
   lRet = ippAux::IppArith<ippRGB>(aType, buffer1, buffer2, width, height, scanlineAlignment, &roiRect);
else if (imgType == spe_it_8Gray)
   lRet = ippAux::IppArith<ippGray>(aType, buffer1, buffer2, width, height, scanlineAlignment, &roiRect);
else
   return false; 	// currently - float not supported
return lRet;
}
//---------------------------------------------------------------------------
DLLEXPORT bool  __stdcall ippApplyCoCoAux(TippCoCoType coType, void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment,
									 TRect *boundRect)
{
if (!spCommon::ippAlive)
   return false;
bool lRet;
TRect roiRect;
if (boundRect)
   roiRect = *boundRect;
else
   {
   roiRect.left = 0;
   roiRect.right = width;
   roiRect.top = 0;
   roiRect.bottom = height;
   }
lRet = ippAux::IppColorConversion<ippRGB>(coType, buffer1, buffer2, width, height, scanlineAlignment, &roiRect);
return lRet;
}
//===========================================================================
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//===========================================================================

