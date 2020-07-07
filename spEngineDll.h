#ifndef spEngineDllH
#define spEngineDllH
// structures mapping for DLL
//#include "spEngine\spPixelTypes.hpp"
//#include "spEngine\spImage.hpp"
#include "spEngine\spGlobals.h"
#ifdef spHostApp
//---------------------------------------------------------------------------
#define DLLIMPORT extern "C" __declspec( dllimport )
//---------------------------------------------------------------------------
DLLIMPORT bool __stdcall speInit(void);
// surface
DLLIMPORT int __stdcall  speAddSurface(void);
DLLIMPORT bool __stdcall speSetSurfaceId(unsigned long id);
DLLIMPORT int __stdcall  speSelectSurfaceByIndex(int sIndex);
DLLIMPORT int __stdcall  speSelectSurfaceById(unsigned long id);
DLLIMPORT bool __stdcall speDeleteSurfaceByIndex(int sIndex);
DLLIMPORT bool __stdcall speDeleteSurfaceById(int id);
DLLIMPORT bool __stdcall speSetSurface(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool __stdcall speSurfaceBegin(int width, int height);
DLLIMPORT bool __stdcall speSurfaceAddScanline(void *scanOrig, void *scanAlphaOrig);
DLLIMPORT bool __stdcall speSurfaceFinish(void);
DLLIMPORT bool __stdcall speSyncBackSurface(void);
DLLIMPORT bool __stdcall speSyncFrontSurface(void);
DLLIMPORT bool __stdcall speSidekickToBackSurface(void);
DLLIMPORT bool __stdcall speCopyToFrontSurface(void *scanOrig, void *scanAlphaOrig);
// render
DLLIMPORT Tspe_RenderParams* __stdcall speGetRenderParams(void);
DLLIMPORT bool __stdcall speSetAction(TspeActionType actionType);
DLLIMPORT TspeActionType __stdcall speGetAction(void);
// shape
DLLIMPORT TspeShapeType __stdcall speSetShape(TspeShapeType shapeType);
// actions params
DLLIMPORT Tspe_DrawParams*  __stdcall speGetDrawParams(void);
DLLIMPORT Tspe_ColorParams* __stdcall speGetColorParams(void);
DLLIMPORT Tspe_ShiftParams* __stdcall speGetShiftParams(void);
DLLIMPORT Tspe_SpreadParams* __stdcall speGetSpreadParams(void);
DLLIMPORT Tspe_Filter33Params* __stdcall speGetFilter33Params(void);
DLLIMPORT Tspe_FilterIppParams* __stdcall speGetFilterIppParams(void);
DLLIMPORT Tspe_ColorTwistIppParams* __stdcall speGetColorTwistIppParams(void);
DLLIMPORT Tspe_CompoundIppParams* __stdcall speGetCompoundIppParams(void);
DLLIMPORT Tspe_NozzleParams* __stdcall speGetNozzleParams(void);
// shadow
DLLIMPORT Tspe_ShadowParams* __stdcall speGetShadowParams(void);
// brush common
DLLIMPORT bool __stdcall speSetBrush(TspeBrushType brushType);
DLLIMPORT bool __stdcall speSelectBrush(int bIndex, TspeBrushType &bType);
DLLIMPORT Tspe_BrushCommonParams* __stdcall speGetBrushCommon(void);
DLLIMPORT bool __stdcall speUpdateBrush(void);
// brush specific
DLLIMPORT void* __stdcall speGetBrushSpecific(void);
DLLIMPORT bool __stdcall speSetBrushSpecific(void *bsp);
// brush stamp
DLLIMPORT bool __stdcall speSetStamp(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared = false);
DLLIMPORT bool __stdcall speClearStamp(void);
// brush pump
DLLIMPORT bool __stdcall speAddPumpImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared = false);
DLLIMPORT bool __stdcall speSelectPumpImage(int idx);
DLLIMPORT bool __stdcall speDeselectPump(void);
DLLIMPORT bool __stdcall speClearPump(void);
// brush textures
DLLIMPORT bool __stdcall speSetTexture(void *buffer, int width, int height, unsigned int scanlineAlignement);
DLLIMPORT bool __stdcall speDeleteTexture(void);
DLLIMPORT bool __stdcall speSetBackgroundTile(void *buffer, int width, int height, unsigned int scanlineAlignement);
DLLIMPORT bool __stdcall speDeleteBackgroundTile(void);
DLLIMPORT bool __stdcall speSetPolyTexture(void *buffer, int width, int height, unsigned int scanlineAlignement);
DLLIMPORT bool __stdcall speDeletePolyTexture(void);
// brush palette
DLLIMPORT bool __stdcall speSetPalette3C(unsigned int c1, unsigned int c2, unsigned int c3);
DLLIMPORT bool __stdcall speSetPalette255C(unsigned int *pal);
DLLIMPORT bool __stdcall speGetPalette(unsigned int *pal);
// set brush palette from external (short) palette
DLLIMPORT bool __stdcall speSetPaletteFromShortPal(unsigned int *pal, int count);
// global short palette
DLLIMPORT bool __stdcall speSetShortPalette(unsigned int *pal, int count);
DLLIMPORT unsigned int __stdcall speGetShortPaletteColor(int pIndex);
// brush envelope
DLLIMPORT bool __stdcall speSetSizeEnvelope(wchar_t* str);
DLLIMPORT bool __stdcall speSetCapacityEnvelope(wchar_t* str);
// scaling colors
/*
DLLIMPORT bool __stdcall speSetScaleColors(unsigned int botVal, unsigned int topVal);
DLLIMPORT bool __stdcall speResetScaleColors(void);
*/
// draw object
DLLIMPORT bool __stdcall speDrawRectangleRect(TRect &objRect, Tspe_Geometry &geometry, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawEllipseRect(TRect &objRect, Tspe_Geometry &geometry, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawRectangle(int x1, int y1, int x2, int y2, Tspe_Geometry &geometry, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawEllipse(int x1, int y1, int x2, int y2, Tspe_Geometry &geometry, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawPoly(TPoint *pts, int ptsCount, Tspe_Geometry &geometry, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawSpline(TPoint *pts, int ptsCount, Tspe_Geometry &geometry, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawLine(int x1, int y1, int x2, int y2, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawPolySimple(TPoint *pts, int ptsCount, bool usePresetShape = true);
DLLIMPORT bool __stdcall speDrawSplineSimple(TPoint *pts, int ptsCount, bool usePresetShape = true);
// mouse
DLLIMPORT bool __stdcall speGetMouseData(Tspe_MouseData &md);
DLLIMPORT bool __stdcall speHandMouseDown(TShiftState Shift, int X, int Y);
DLLIMPORT bool __stdcall speHandMouseMove(TShiftState Shift, int X, int Y);
DLLIMPORT bool __stdcall speHandMouseUp(TShiftState Shift, int X, int Y);
// obsolete
//DLLIMPORT bool __stdcall speObjectMouseDown(TShiftState Shift, int X, int Y);
//DLLIMPORT bool __stdcall speObjectMouseMove(TShiftState Shift, int X, int Y);
//DLLIMPORT bool __stdcall speObjectMouseUp(TShiftState Shift, int X, int Y);
// clone image
DLLIMPORT bool __stdcall speSetCloneImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
DLLIMPORT bool __stdcall speSetClonePosition(int X, int Y);
DLLIMPORT bool __stdcall speDeleteCloneImage(void);
DLLIMPORT bool __stdcall speCloneSelf(TspeCloneType clType);
// assistant images
// stencil
DLLIMPORT bool __stdcall speSetStencil(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
DLLIMPORT bool __stdcall speSetStencilPositionStatus(int X, int Y, bool alive);
DLLIMPORT bool __stdcall speDeleteStencil(void);
DLLIMPORT bool __stdcall speInvertStencil(void);
// contour
DLLIMPORT bool __stdcall speSetContour(void *buffer, bool shared);
DLLIMPORT bool __stdcall speDeleteContour(void);
DLLIMPORT bool __stdcall speInvertContour(void);
// mask
DLLIMPORT bool __stdcall speSetMask(void *buffer, bool shared);
DLLIMPORT bool __stdcall speDeleteMask(void);
DLLIMPORT bool __stdcall speInvertMask(void);
DLLIMPORT bool __stdcall speInternalToMask(TspeInternalMaskType imt = spe_RightMask);
// sidekick
DLLIMPORT bool __stdcall speSetSidekick(void *buffer, bool shared);
DLLIMPORT bool __stdcall speDeleteSidekick(void);
DLLIMPORT bool __stdcall speInvertSidekick(void);
// nozzle image
DLLIMPORT bool __stdcall speSetNozzle(void *buffer, void *alpha, int width, int height, unsigned int scanlineAlignement, bool shared);
DLLIMPORT bool __stdcall speDeleteNozzle(void);
// selection
DLLIMPORT bool __stdcall speSetSelection(void *buffer, bool shared);
DLLIMPORT bool __stdcall speDeleteSelection(void);
//DLLIMPORT bool __stdcall speSelectionClipping(bool clip);  // obsolete
// marker
DLLIMPORT bool __stdcall speCheckMarker(int width, int height);
DLLIMPORT bool __stdcall speSetMarker(void *buffer, bool shared);
DLLIMPORT bool __stdcall speDeleteMarker(void);
// pixel
DLLIMPORT bool __stdcall speGetPixelRGB(const int &y, const int &x, Byte &r, Byte &g, Byte &b);
DLLIMPORT bool __stdcall speGetPixelColor(const int &y, const int &x, unsigned int &color);
DLLIMPORT bool __stdcall speSetColorCurve(Byte **LUTCurveEntry);
// applay on image(selection), mask, contour
DLLIMPORT bool __stdcall speApplyOnImage(TRect *boundRect, TspeApplyType appType);
// apply using external mask
DLLIMPORT bool __stdcall speApplyAuxMask(TRect *boundRect, void *buffer, int width, int height, unsigned int scanlineAlignment);
// drop shadow
DLLIMPORT bool __stdcall speDropShadow(TRect *boundRect, TspeApplyType appType);
//
DLLIMPORT void __stdcall speGetBoundRect(TRect *boundRect);
DLLIMPORT void __stdcall speResetBigBoundRect(void);
DLLIMPORT void __stdcall speAddBigBoundRect(TRect *boundRect = 0);
DLLIMPORT void __stdcall speGetBigBoundRect(TRect *boundRect);
//
DLLIMPORT bool __stdcall speSetUndoRect(TRect *boundRect, bool createUndoBlock = true);
DLLIMPORT bool __stdcall speCommit(void);
DLLIMPORT bool __stdcall speRollback(void);
DLLIMPORT bool __stdcall speRollbackRect(TRect *boundRect);
//
DLLIMPORT bool __stdcall speSetRoi(TRect &roiRect);
DLLIMPORT bool __stdcall speResetRoi(void);
//
DLLIMPORT bool __stdcall speClearInternalMasks(void);
//
DLLIMPORT Tspe_UndoBlock* __stdcall speGetUndo(void);
DLLIMPORT Tspe_UndoBlock* __stdcall speRestoreUndo(Tspe_UndoBlock* externalBlock);
// color correction
DLLIMPORT bool  __stdcall speCollectCCD(unsigned long surfaceId, TRect *boundRect, bool source, bool selection);
// get internal mask
DLLIMPORT void* __stdcall speGetInternalMask(TspeInternalMaskType imt = spe_RightMask);
// get internal image (copy of original image created by setSurface)
DLLIMPORT void __stdcall speGetInternalImage(void *&image, void *&alpha);
// call back functions
DLLIMPORT void __stdcall speSetCbBrushMask(TCbBrushMask cbBrushMask);
// multistroke
DLLIMPORT void __stdcall speClearMultiStroke(void);
DLLIMPORT int  __stdcall speAddMultiStroke(int brushIndex);
DLLIMPORT bool __stdcall speSetMultiStrokePos(int &msIndex, int &X, int &Y);
DLLIMPORT bool __stdcall speDrawMultiPoly(TPoint **pts, int *ptsCount, Tspe_Geometry &geometry);
DLLIMPORT bool __stdcall speDrawMultiPolyByIndex(int &msIndex, TPoint *pts, int &ptsCount, Tspe_Geometry &geometry);
DLLIMPORT bool __stdcall speDrawMultiPolyFinish(void);
DLLIMPORT bool __stdcall speUpdateMultiBrush(int &msIndex, bool cloneCurrent = false);
DLLIMPORT int __stdcall speGetMultiStrokeSize(void);
DLLIMPORT Tspe_BrushCommonParams* __stdcall speGetMultiBrushCommon(int &msIndex);
// extras
DLLIMPORT bool __stdcall speBlendImageMaskColor(unsigned int color, bool zeroMask = false, TspeInternalMaskType imt = spe_RightMask);
// engine version
DLLIMPORT wchar_t* __stdcall speGetVersion(void);
//===========================================================================
// SPE processing
//===========================================================================
DLLIMPORT bool  __stdcall speProcPalNearestLuminance(TRect *boundRect, int palCount);
DLLIMPORT bool  __stdcall speProcPalNearestColor(TRect *boundRect, int palCount);
DLLIMPORT bool  __stdcall speProcPalPseudoColor(TRect *boundRect, int palCount);
DLLIMPORT bool  __stdcall speProcTest(TRect *boundRect);
DLLIMPORT bool  __stdcall speProcGrayscale(TRect *boundRect, double rFact, double gFact, double bFact);
DLLIMPORT bool  __stdcall speProcMirror(TRect *boundRect, int axis);
DLLIMPORT bool  __stdcall speProcArithBool(TRect *boundRect, TspeArithBool opType, int opValue);
// tessellation - vonronoi, delaunay solid, delaunay with gouraud shading
DLLIMPORT bool  __stdcall speProcTessellation(Tspe_Tessellation &params);
DLLIMPORT bool  __stdcall speProcTessellRedraw(Tspe_Tessellation &params);
DLLIMPORT bool  __stdcall speProcTessellClear(void);
//===========================================================================
// SPE warping
//===========================================================================
DLLIMPORT bool  __stdcall speWarpBegin(Tspe_WarpParams &params);
DLLIMPORT bool  __stdcall speWarpExecute(int &X1, int &Y1, int &X2, int &Y2);
DLLIMPORT bool  __stdcall speWarpEnd(void);
DLLIMPORT bool  __stdcall speApplyWarpMesh(Tspe_WarpParams &params);
//===========================================================================
// SPE utilities (geometry/symmetry/etc..)
//===========================================================================
DLLIMPORT void __stdcall speUtilGetLineData(double &outRadius, double &outAngle, int &X1, int &Y1, int &X2, int &Y2);
DLLIMPORT void __stdcall speUtilGetSimplifiedPoly(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts);
DLLIMPORT void __stdcall speUtilGetSplinePoly(TPoint *polyPts, int &polyCount, bool closed = false);
DLLIMPORT TPoint*  __stdcall speUtilGetEllipsePoints(TRect &objRect, double &RotationAngle, TPoint &RotationOrigin, int &countPts);
DLLIMPORT TPoint* __stdcall speUtilGetLinePoints(int &X1, int &Y1, int &X2, int &Y2, int &countPts);
DLLIMPORT void __stdcall speUtilScalePoly(TPoint *polyPts, int &polyCount, float factorX, float factorY);
// color from temperature
DLLIMPORT unsigned int  __stdcall speUtilColorFromTemperature(double temperature);
//===========================================================================
// ipp calls (surface)
//===========================================================================
DLLIMPORT bool  __stdcall ippIsInstalled(void); 	// is ipp istalled
DLLIMPORT bool  __stdcall ippApplyFilter(TRect *boundRect);
DLLIMPORT bool  __stdcall ippApplyColorTwist(TRect *boundRect);
DLLIMPORT bool  __stdcall ippApplyCompound(TRect *boundRect);
DLLIMPORT bool  __stdcall ippApplyDistanceTransform(TRect *boundRect, bool kernel55, int norm, bool *chanOn);
DLLIMPORT bool  __stdcall ippApplyFastMarchingDistanceTransform(TRect *boundRect, float radius, bool *chanOn);
DLLIMPORT bool  __stdcall ippApplyGradientColor(TRect *boundRect, int norm, bool *chanOn);
DLLIMPORT bool  __stdcall ippApplyReduceBits(TRect *boundRect, int noise, int ditherType, int levels);
DLLIMPORT bool  __stdcall ippApplyMirror(TRect *boundRect, int axis);
DLLIMPORT bool  __stdcall ippApplyQuadTransform(Tspe_QuadIppParams &params);
DLLIMPORT bool  __stdcall ippApplyDeconvolveLR(TRect *boundRect, int kernel, int threshold, int iterations);
DLLIMPORT bool  __stdcall ippComputeHistogram(TRect *boundRect, int nBins, unsigned int** histo);
DLLIMPORT bool  __stdcall ippApplyLUT(TRect *boundRect, int nBins, unsigned int** lut, int interpol);
DLLIMPORT bool  __stdcall ippInitStatistics(void);
DLLIMPORT bool  __stdcall ippInitStatisticsAux(void *bufferColor, void *bufferGray, int width, int height, unsigned int scanlineAlignment);
DLLIMPORT bool  __stdcall ippComputeMoments(TRect *boundRect, bool grayscale = true);
DLLIMPORT double  __stdcall ippGetMoment(int mOrd, int nOrd, int channel, TippMomentType type);
DLLIMPORT double  __stdcall ippComputeNorms(TRect *boundRect, bool grayscale = true);
DLLIMPORT double  __stdcall ippGetNorm(int channel, TippNormType type);
DLLIMPORT bool  __stdcall ippReleaseStatistics(void);
//===========================================================================
// ipp calls (auxilary / external image)
//===========================================================================
// filter: result = buffer2 == 0 ? buffer1 : buffer2
DLLIMPORT bool  __stdcall ippApplyFilterAux(void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment, TspeImageType imgType,
											Tspe_FilterIppParams *filterParams, TRect *boundRect);
// arith bool ops: result =  buffer1
DLLIMPORT bool  __stdcall ippApplyArithBoolAux(TippArithBool aType, void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment,
										   TspeImageType imgType, TRect *boundRect);
// color conversion: result = buffer2 == 0 ? buffer1 : buffer2
DLLIMPORT bool  __stdcall ippApplyCoCoAux(TippCoCoType coType, void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment,
									 TRect *boundRect);
//TODO: list
// parallel with adjustable angle
// particle system
#endif
//---------------------------------------------------------------------------
#endif
