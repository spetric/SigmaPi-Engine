//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderMultiDraw.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderMultiDraw::TRenderMultiDraw(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
					   : TRenderDraw(surfaceContainer, renderData)
{
_saveBrush = 0;
}
//---------------------------------------------------------------------------
// destructor
TRenderMultiDraw::~TRenderMultiDraw(void)
{
//delete _container;
//deleteOutPoly();
}
//---------------------------------------------------------------------------
UnicodeString TRenderMultiDraw::ClassName(void)
{
return "TRenderMultiDraw";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderMultiDraw::Begin(int X, int Y)
{
prepareColors();
_autoRenderSuggest = spe_RENDER_FAST;
RenderParams->DrawPolyInterior = (DrawParams->MainMode !=  spe_ARITH_ERASE);
//
_SC->PalEntry = _SC->Brush->ColorParams->PalEntry;
_SC->PalCount = 256;
_SC->CloneImage = 0;
spColorArithmetics::ArithData.topVal = GlobalColorParams->ScaleTop;
spColorArithmetics::ArithData.botVal = GlobalColorParams->ScaleBottom;
spColorArithmetics::ArithData.invertPixel = false;
spColorArithmetics::ArithData.bitShift = 0;
prepareBrushScheme(_SC->Brush->ColorParams->SchemeIndex, X, Y);
_SC->Brush->ColorParams->WorkPrimary = _SC->Brush->ColorParams->PrimaryColor;
_SC->Brush->ColorParams->WorkSecondary = _SC->Brush->ColorParams->SecondaryColor;
spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
// save current shape and brush
_saveShape = _SC->Shape;
_saveBrush = _SC->Brush;
//
/* moved to wrapper
for (int i = 0; i <  MultiStroke->Count; i++)
   {
	TStrokeBucket *stroke = static_cast<TStrokeBucket*>(MultiStroke->Items[i]);
	int xs = stroke->pos.x;
	int ys = stroke->pos.y;
	stroke->brush->Begin(stroke->shape);
	StatusOK = stroke->shape->MouseDown(TShiftState(), xs, ys);     //TODO: empty shift -> save to protected
	_SC->SetShape(stroke->shape);
	_SC->SetBrush(stroke->brush);
	TRenderBasic::Begin(xs, ys);
   }
*/
_SC->skip_empty_alpha = true;
}
//---------------------------------------------------------------------------
TRect TRenderMultiDraw::End(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
TRect bbr = TRenderBasic::End(X1, Y1, X2, Y2);
// restore shape and brush
_SC->SetShape(_saveShape);
_SC->SetBrush(CurrentBrush);
// return big bound rectangle
return bbr;
}




