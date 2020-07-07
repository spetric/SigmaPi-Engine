//---------------------------------------------------------------------------
#ifndef renderBasicH
#define renderBasicH
#include "grapBasic.h"
#include "renderDataKeeper.h"
//#include "spCommon.h"
#include "spSurfaceContainer.h"
#include "spColorArithmetics.h"
#include "ippFilterBasic.hpp"
//#include "spPixelArithmetics.hpp"
//#include <Classes.hpp>
//---------------------------------------------------------------------------
typedef void (__closure *TRenderer)(const int &, const int &, const int &, const int &);
typedef bool (__closure *TFilterIppRenderCoords)(const int &, const int &, const int &, const int &);
typedef bool (__closure *TFilterIppRenderRect)(const TRect &);
typedef void (__closure *TOnStepEvent)(const int &, const int &);
typedef void (__closure *TStrokeStepper)(void);
struct TStrokeBucket
   {
   TBrushBasic *brush;
   TGrapBasic *shape;
   TPoint pos;
   };
//---------------------------------------------------------------------------
class TRenderBasic
{
  //TODO: check and set some methods as static
  private:
	//void __fastcall setStepper(void);
    bool contourEncountered(const int &X, const int &Y);
  protected:
	TStrokeStepper strokeStepper;
	void doStencil(void);
	//
	typedef spColorArithmetics::TColorArithmetic TColorArithmetic;
	TColorArithmetic _colorArithmetic;
	//TODO: ovo ide vani i ide s private metodama unutar pojedine vrste filtera
	//typedef ippFilterBasic<ippRGB>::Execute::FilterFunc TIppFilterApply;
	//typedef ippFilterBasic<ippRGB>::Execute::CompoundFunc TIppCompoundApply;
	//TIppFilterPrepare _ippFilterPrepare;
	//TIppFilterApply _ippFilterApply;
	//TIppCompoundApply _ippCompoundApply;
	virtual void prepareColors(void);
	virtual bool prepareSpareImageCoords(const int &X1, const int &Y1, const int &X2, const int &Y2);
	virtual bool prepareSpareImageRect(const TRect &roiRect);
	virtual void renderBrushTip(const int &X1, const int &Y1, const int &X2, const int &Y2);
	virtual void renderShape(const int &X1, const int &Y1, const int &X2, const int &Y2);
	virtual void renderShapeFast(const int &X1, const int &Y1, const int &X2, const int &Y2);
	virtual void renderDummy(const int &X1, const int &Y1, const int &X2, const int &Y2);
	virtual void setDropShadowRect(TRect *boundRect);
	virtual void stepperDummy(void);
	virtual void stepperCircular(void);
	virtual void stepperFB(void);
	virtual void stepperImage(void);
	virtual void stepperClone(void);
	virtual void stepperStop(void);

	TRect _boundRect, _polyBoundRect;
	ippFilterBasic<ippRGB> *_filterIpp;
	spFatImage<ptRGB,ippRGB> *_spareImage;  // fat image
    spImage<ptRGB> *_polyInterImage;
	int _autoRenderSuggest;
	bool _contourClipping;
  public:
	//spCommon::spsPixel *PixData;
	bool CreateUndo;
	bool StatusOK; // for sym mouse lRet (or something else)
	Tspe_RenderParams *RenderParams;
	//spRenderDataKeeper::spsSelectionParams *SelectionParams;
	Tspe_ShadowParams *ShadowParams;
	spRenderDataKeeper::spsGlobalColorParams *GlobalColorParams;
	spSurfaceContainer *_SC;
	TBrushBasic *CurrentBrush;      // current brush
	//
	TRenderBasic(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderBasic(void);
	virtual UnicodeString ClassName(void);
	//
	//TRenderer     GoRender;
	TRenderer    GoRenderStroke;
	TFilterIppRenderCoords GoFilterIppRenderCoords;
	TFilterIppRenderRect   GoFilterIppRenderRect;
	TOnStepEvent  OnShapeStep;
	//
	virtual void Begin(int X, int Y);
	virtual void Execute(const int &X1, const int &Y1, const int &X2, const int &Y2);
	virtual TRect End(const int &X1, const int &Y1, const int &X2, const int &Y2);
	//virtual TRect EndObject(const int &X1, const int &Y1, const int &X2, const int &Y2); removed
	virtual void FinishRender(void);
	virtual void PolyInterior(const int &X1, const int &Y1, const int &X2, const int &Y2);
	TRect GetBoundRect(const int &X1, const int &Y1, const int &X2, const int &Y2);
	TRect GetBoundRectClear(const int &X1, const int &Y1, const int &X2, const int &Y2);
	TRect GetBoundRectComplete(const int &X1, const int &Y1, const int &X2, const int &Y2);
	TRect GetTipBoundRect(const int &X, const int &Y);
	TRect GetTipBoundRectClear(const int &X, const int &Y, const int &half_size);
	void DrawImage(TRect *boundRect);
	void DrawBoundRect(TRect *boundRect);
	void DropShadow(TRect *boundRect);
	void Connect(spSurfaceContainer *surfaceContainer);
	void SetBoundRect(TRect boundRect);
	//
	__property TColorArithmetic ColorArithmetic = {read = _colorArithmetic};
	//TODO: __property TIplFilterArithmetic IplFilterArithmetic = {read = _iplFilterArithmetic};
	//__property TSpareImage *SpareImage = {read = _spareImage};
	__property spFatImage<ptRGB, ippRGB> *SpareImage = {read = _spareImage};
	__property TRect BoundRect = {read = _boundRect};
	__property ippFilterBasic<ippRGB> *FilterIpp = {read = _filterIpp};
	//__property TIppFilterPrepare PrepareIppFilter = {read = _ippFilterPrepare};
	//__property TIppFilterApply  ApplyIppFilter = {read = _ippFilterApply};
	//__property TIppCompoundApply ApplyIppCompound = {read = _ippCompoundApply};
};
//---------------------------------------------------------------------------
#endif
