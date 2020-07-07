//---------------------------------------------------------------------------
#ifndef spEngineWrapperH
#define spEngineWrapperH
#include "spSurfaceContainer.h"
#include "renderDataKeeper.h"
#include "grapBasic.h"
#include "brushBasic.h"
#include "renderBasic.h"
//#include "ippFilters.hpp"
#include "spWarping.h"
//---------------------------------------------------------------------------
class spEngineWrapper
  {
  //---------------------------------------------------------------------------
  // mouse "events"
  typedef bool (__closure *TWMouseDown)(TShiftState, int, int);
  typedef bool (__closure *TWMouseMove)(TShiftState, int, int);
  typedef bool (__closure *TWMouseUp)(TShiftState, int, int);
  //
  private:
	  //
	  spRenderDataKeeper *FRenderData;
	  spSurfaceContainer *FSurf;
	  TGrapBasic *FShape;
	  TspeShapeType FShapeType;
	  TRenderBasic *FRender;
	  TBrushBasic *FBrush;
	  TBrushBasic *FBucket[9];
	  int FBucketIndex;
	  TList *FSurfList;
	  int FSurfIndex;
	  Tspe_UndoBlock FUndoBlock, FRedoBlock;
	  warpDataStruct FWarpData;
	  spWarp::TWarpExecute FGoWarp;
	  TList *FMultiStroke;
	  TspeActionType FActionType;
      int FMultiStrokeSize(void) {return FMultiStroke->Count;}
	  //ippFilter <ippRGB> *FFilterIpp;
	  // collor correction data
  public:
	  spEngineWrapper(void);
	  virtual ~spEngineWrapper(void);
	  // public vars
	  TWMouseDown WMouseDown;
	  TWMouseMove WMouseMove;
	  TWMouseUp   WMouseUp;
	  // call back functions
	  TCbBrushMask CbBrushMask; // brush mask before clearing (prior finish)
	  //
	  bool Initialize(void);
	  int  AddSurface(void);
	  bool SetSurfaceId(unsigned long id);
	  int SelectSurfaceByIndex(int sIndex);
	  bool DeleteSurfaceByIndex(int sIndex);
	  int SelectSurfaceById(unsigned long id);
	  bool DeleteSurfaceById(unsigned long id);
	  bool SetSurface(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment);
	  bool SetSurface(int width, int height);
	  TspeShapeType SetShape(TspeShapeType shapeType);
	  void* SetBrush(TspeBrushType bsuthType);
	  //bool SetScaleColors(unsigned int botVal, unsigned int topVal);
	  void* SetRenderAction(TspeActionType actionType);
	  void* GetBrushSpecific(void);
	  bool SetBrushSpecific(void *vBsp);
	  bool SetStamp(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
	  bool ClearStamp(void);
	  bool AddPumpImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
	  bool SelectPumpImage(int idx);
	  bool DeselectPump(void);
	  bool ClearPump(void);
	  bool SelectBrush(int bIndex, TspeBrushType &bType);
	  void Finish(TRect *boundRect);
	  void Rollback(TRect *boundRect);
	  bool SetClone(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
	  bool DeleteClone(void);
	  bool SetAssImage(int assType, void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
	  bool DeleteAssImage(int assType);
	  bool InvertAssImage(int assType);
	  bool SetNozzle(void *buffer, void * alpha, int width, int height, unsigned int scanlineAlignement, bool shared);
	  bool DeleteNozzle(void);
	  bool SetSelection(void *buffer, bool shared);
	  bool DeleteSelection(void);
	  bool SetColorCurve(Byte **LUTCurveEntry);
	  bool CreateUndoBlock(TRect *undoRect);
	  bool RestoreUndoBlock(Tspe_UndoBlock* externalBlock);
	  bool CollectCCD(unsigned long surfaceId, TRect *boundRect, bool source, bool selection);
	  // multi brush bucket
	  void ClearMultiStroke(bool recreate);
	  int  AddMultiStroke(int brushIndex);
	  bool SetMultiStrokePos(int &msindex, int &X, int &Y);
	  TStrokeBucket* GetMultiStrokeBucket(int &msIndex);
	  // warping
	  bool WarpBegin(Tspe_WarpParams &params);
	  bool WarpExecute(int &X1, int &Y1, int &X2, int &Y2);
	  bool WarpEnd(TRect *roiRect);
	  bool ApplyWarpMesh(Tspe_WarpParams &params);
	  // utils
	  void GetSimplifiedPoly(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts);
	  void GetSplinePoly(TPoint *polyPts, int &polyCount, bool closed);
	  // ipp-ovci
	  bool AppIppFilter(TRect *roiRect);
	  bool AppIppCompound(TRect *roiRect);
	  bool AppIppColorTwist(TRect *roiRect);
	  bool AppIppQuad(TRect *roiRect, Tspe_QuadIppParams &params);
	  // one for all (for now)
	  // TODO: att one for alpha channel
	  //
	  // mouse "events"
	  // standard hand mouse methods
	  bool StdMouseDown(TShiftState, int, int);
	  bool StdMouseMove(TShiftState, int, int);
	  bool StdMouseUp(TShiftState, int, int);
	  // symmetry hand mouse methods
	  bool MultiMouseDown(TShiftState, int, int);
	  bool MultiMouseMove(TShiftState, int, int);
	  bool MultiMouseUp(TShiftState, int, int);
	  // draw object poly
	  bool DrawObjectPoly(TPoint *pts, int ptsCount, Tspe_Geometry &geometry);
	  bool DrawObjectRect(TRect &objRect, Tspe_Geometry &geometry);
	  bool DrawMultiPoly(TPoint **pts, int *ptsCount, Tspe_Geometry &geometry);
	  bool DrawMultiPolyByIndex(int &msIndex, TPoint *pts, int &ptsCount, Tspe_Geometry &geometry);
	  // undo stuff
	  void DoUndoStuff(TRect &roiRect);
	  // drop shadow
      bool DropShadow(TRect *boundRect, TspeApplyType appType);
	  //
  // R/O properties
  __property spSurfaceContainer* Surf = {read = FSurf};
  __property spRenderDataKeeper* RenderData = {read = FRenderData};
  __property TBrushBasic* Brush = {read = FBrush};
  __property TRenderBasic* Render = {read = FRender};
  __property TGrapBasic *Shape	= {read = FShape};
  __property TspeShapeType ShapeType = {read = FShapeType};
  __property Tspe_UndoBlock UndoBlock = {read = FUndoBlock};
  __property Tspe_UndoBlock RedoBlock = {read = FRedoBlock};
  __property TspeActionType ActionType = {read = FActionType};
  __property int MultiStrokeSize = {read = FMultiStrokeSize};
  };
#endif
