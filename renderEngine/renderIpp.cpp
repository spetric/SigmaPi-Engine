//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderIpp.h"
#include "ippFilterBlur.hpp"
#include "ippFilterMedian.hpp"
#include "ippFilterBilateral.hpp"
#include "ippFilterGaussian.hpp"
#include "ippFilterMax.hpp"
#include "ippFilterMin.hpp"
#include "ippFilterFixed.hpp"
#include "ippFilterGeneral.hpp"
#include "ippFilterWiener.hpp"
#include "ippFilterMorph.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderFilterIpp::TRenderFilterIpp(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
						:TRenderBasic(surfaceContainer, renderData)

{
FilterIppParams  = renderData->FilterIppParams;
//_filterIpp = new ippFilter<ippRGB>();
_filterIpp = 0;
}
//---------------------------------------------------------------------------
// destructor
 TRenderFilterIpp::~TRenderFilterIpp(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderFilterIpp::ClassName(void)
{
return "TRenderFilterIpp";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderFilterIpp::Begin(int X, int Y)
{
int roiTolAdd = 0;
prepareColors();
_autoRenderSuggest =  spe_RENDER_FAST;
//
GoFilterIppRenderCoords = &renderIppFilterCoords;
GoFilterIppRenderRect   = &renderIppFilterRect;
_colorArithmetic = &spColorArithmetics::IppFilter;
if (FilterIppParams->Chan[0] && FilterIppParams->Chan[1] && FilterIppParams->Chan[2])
   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
else
   {
   spColorArithmetics::ArithData.chanOn[0] =  FilterIppParams->Chan[0];
   spColorArithmetics::ArithData.chanOn[1] =  FilterIppParams->Chan[1];
   spColorArithmetics::ArithData.chanOn[2] =  FilterIppParams->Chan[2];
   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM_CHAN;
   }
if (_filterIpp)
   {
   if (_filterIpp->FilterType != FilterIppParams->MainMode)
	  {
	  delete _filterIpp;
	  _filterIpp = 0;
	  }
   }
switch (FilterIppParams->MainMode)
   {
   case ipp_BLUR:
		if (!_filterIpp)
			_filterIpp = new ippFilterBlur<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = max(FilterIppParams->RadiusX,  FilterIppParams->RadiusY) * 2 + 1;
		break;
   case ipp_MEDIAN:
		if (!_filterIpp)
			_filterIpp = new ippFilterMedian<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = max(FilterIppParams->RadiusX,  FilterIppParams->RadiusY) * 2 + 1;
		break;
   case ipp_BILATERAL:
		if (!_filterIpp)
			_filterIpp = new ippFilterBilateral<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = FilterIppParams->Radius * 2;
		break;
   case ipp_GAUSSIAN:
		if (!_filterIpp)
			_filterIpp = new ippFilterGaussian<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = FilterIppParams->Radius * 2;
		break;
   case ipp_MAX:
		if (!_filterIpp)
			_filterIpp = new ippFilterMax<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = max(FilterIppParams->RadiusX,  FilterIppParams->RadiusY) * 2 + 1;
		break;
   case ipp_MIN:
		if (!_filterIpp)
			_filterIpp = new ippFilterMin<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = max(FilterIppParams->RadiusX,  FilterIppParams->RadiusY) * 2 + 1;
		break;
   case ipp_MEDIAN_CROSS_XX:
   case ipp_MEDIAN_COLOR_XX:
   case ipp_HIPASS_XX:
   case ipp_LAPLACE_XX:
		if (!_filterIpp)
			_filterIpp = new ippFilterFixed<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = 5;
		break;
   case ipp_SHARPEN_XX:    // special case...33->fixed, 55->general predefined
		if (!_filterIpp)
		   delete _filterIpp;
		if (FilterIppParams->Fixed33)
		   _filterIpp = new ippFilterFixed<ippRGB>(FilterIppParams->MainMode);
		else
		   _filterIpp = new ippFilterGeneral<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = 5;
		break;
   case ipp_WIENER:
		if (!_filterIpp)
			_filterIpp = new ippFilterWiener<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = max(FilterIppParams->KernelX,  FilterIppParams->KernelY);
		break;
   case ipp_DILATE:
   case ipp_ERODE:
   case ipp_CLOSE:
   case ipp_OPEN:
   case ipp_MORPH_TOPHAT:
   case ipp_MORPH_GRADIENT:
 		if (!_filterIpp)
			_filterIpp = new ippFilterMorph<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = max(FilterIppParams->KernelX,  FilterIppParams->KernelY);
		break;
   case ipp_GENERAL_FILTER:
		if (!_filterIpp)
		  _filterIpp = new ippFilterGeneral<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = max(FilterIppParams->GeneralRows,  FilterIppParams->GeneralColumns);
		break;
   default:
		if (!_filterIpp)
		  _filterIpp = new ippFilterGeneral<ippRGB>(FilterIppParams->MainMode);
		roiTolAdd = 5;
		break;
   }
//TODO: we don't need this anymore, we have inherited class
//_ippFilterPrepare = &(_filterIpp->PrepareFilter);
//_ippFilterApply = &(_filterIpp->ApplySingle);
//TODO: ovo dole ne ovako, nego pozvati lijepu metodu
if (_SC->Brush->State->cpROItol < roiTolAdd)
	_SC->Brush->State->cpROItol += roiTolAdd;
TRenderBasic::Begin(X, Y);
_SC->skip_empty_alpha = true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
bool TRenderFilterIpp::renderIppFilterCoords(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
if (!prepareSpareImageCoords(X1, Y1, X2, Y2))
   return false;
_filterIpp->PrepareFilter(&_boundRect, FilterIppParams);
_filterIpp->ApplySingle(_SC->ippEdit, _SC->FilterImage->ImageIpp);
return true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
bool TRenderFilterIpp::renderIppFilterRect(const TRect &roiRect)
{
if (!prepareSpareImageRect(roiRect))
   return false;
_filterIpp->PrepareFilter(&_boundRect, FilterIppParams);
_filterIpp->ApplySingle(_SC->ippEdit, _SC->FilterImage->ImageIpp);
//_filterIpp->PrepareFilter((TRect *)&roiRect, FilterIppParams);
//_filterIpp->ApplySingle(_SC->ippEdit, _SC->FilterImage->ImageIpp);
return true;
}







