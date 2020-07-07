//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderConvolve.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderConvolve::TRenderConvolve(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
						:TRenderBasic(surfaceContainer, renderData)

{
ConvolveParams  = renderData->ConvolutionIppParams;
_filterIpp = new ippFilter<ippRGB>();
}
//---------------------------------------------------------------------------
// destructor
 TRenderConvolve::~TRenderConvolve(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderConvolve::ClassName(void)
{
return "TRenderConvolve";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderConvolve::Begin(int X, int Y)
{
int roiTolAdd = 0;;
prepareColors();
_autoRenderSuggest =  spe_RENDER_FAST;
//
GoFilterIppRenderCoords = &renderIppFilterCoords;
GoFilterIppRenderRect   = &renderIppFilterRect;
_colorArithmetic = &spColorArithmetics::IppFilter;
if (ConvolveParams->Chan[0] && ConvolveParams->Chan[1] && ConvolveParams->Chan[2])
   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
else
   {
   spColorArithmetics::ArithData.chanOn[0] =  ConvolveParams->Chan[0];
   spColorArithmetics::ArithData.chanOn[1] =  ConvolveParams->Chan[1];
   spColorArithmetics::ArithData.chanOn[2] =  ConvolveParams->Chan[2];
   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM_CHAN;
   }
roiTolAdd = 5;
_ippFilterPrepare = &(FilterIpp->PrepareFilterGeneral);
_ippFilterApply = &(FilterIpp->GeneralBorder);
//TODO: ovo dole ne ovako, nego pozvati lijepu metodu
if (_SC->Brush->CalculatedParams->cpROItol < roiTolAdd)
	_SC->Brush->CalculatedParams->cpROItol += roiTolAdd;
TRenderBasic::Begin(X, Y);
_SC->skip_empty_alpha = true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
bool TRenderConvolve::renderIppFilterCoords(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
if (!prepareSpareImageCoords(X1, Y1, X2, Y2))
   return false;
if (_ippFilterPrepare)
	_ippFilterPrepare(&_boundRect, ConvolveParams);
_ippFilterApply(_SC->ippEdit, _SC->FilterImage->ImageIpp);
return true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
bool TRenderConvolve::renderIppFilterRect(const TRect &roiRect)
{
if (!prepareSpareImageRect(roiRect))
   return false;
if (_ippFilterPrepare)
	_ippFilterPrepare(ConvolveParams->MainMode, (TRect *)&roiRect, ConvolveParams);
_ippFilterApply(_SC->ippEdit, _SC->FilterImage->ImageIpp);
return true;
}

