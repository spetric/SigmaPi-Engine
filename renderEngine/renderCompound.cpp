//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderCompound.h"
#include "ippFilterCompund.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderCompound::TRenderCompound(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
						:TRenderBasic(surfaceContainer, renderData)

{
CompoundIppParams  = renderData->CompoundIppParams;
//_filterIpp = new ippFilter<ippRGB>();
_filterIpp = 0;
}
//---------------------------------------------------------------------------
// destructor
 TRenderCompound::~TRenderCompound(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderCompound::ClassName(void)
{
return "TRenderCompound";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderCompound::Begin(int X, int Y)
{
int roiTolAdd = 0;;
prepareColors();
_autoRenderSuggest =  spe_RENDER_FAST;
//
GoFilterIppRenderCoords = &renderIppFilterCoords;
GoFilterIppRenderRect   = &renderIppFilterRect;
_colorArithmetic = &spColorArithmetics::IppFilter;
if (CompoundIppParams->Chan[0] && CompoundIppParams->Chan[1] && CompoundIppParams->Chan[2])
   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
else
   {
   spColorArithmetics::ArithData.chanOn[0] =  CompoundIppParams->Chan[0];
   spColorArithmetics::ArithData.chanOn[1] =  CompoundIppParams->Chan[1];
   spColorArithmetics::ArithData.chanOn[2] =  CompoundIppParams->Chan[2];
   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM_CHAN;
   }
if (!_filterIpp)
	_filterIpp = new ippFilterCompound<ippRGB>(CompoundIppParams->MainMode);
/*
if (_filterIpp)
   {
   if (_filterIpp->FilterType != CompoundIppParams->MainMode)
	  {
	  delete _filterIpp;
	  _filterIpp = 0;
	  }
   }
*/
switch (CompoundIppParams->MainMode)
   {
   case ipp_CMP_MAX_SUB_MIN:
		roiTolAdd = max(CompoundIppParams->RadiusX,  CompoundIppParams->RadiusY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompMaxSubMin);
		break;
   case ipp_CMP_MINMAX_CF:
		roiTolAdd = max(CompoundIppParams->RadiusX,  CompoundIppParams->RadiusY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompMinMaxCF);
		break;
   case	ipp_CMP_MEDIAN_MINMAX_CF:
		roiTolAdd = max(CompoundIppParams->RadiusX,  CompoundIppParams->RadiusY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompMedianMinMaxCF);
		break;
   case	ipp_CMP_BLUR_MINMAX_CF:
		roiTolAdd = max(CompoundIppParams->RadiusX,  CompoundIppParams->RadiusY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompMedianMinMaxCF);
		break;
   case	ipp_CMP_BLUR_MEDIAN_CF:
		roiTolAdd = max(CompoundIppParams->RadiusX,  CompoundIppParams->RadiusY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompBlurMedianCF);
		break;
   case	ipp_CMP_ERODE_DILATE_CF:
		roiTolAdd = max(CompoundIppParams->KernelX,  CompoundIppParams->KernelY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompErodeDilateCF);
		break;
   case	ipp_CMP_MEDIAN_ERODE_DILATE_CF:
		roiTolAdd = max(CompoundIppParams->KernelX,  CompoundIppParams->KernelY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompMedianErodeDilateCF);
		break;
   case	ipp_CMP_UNSHARP_MASK:
		roiTolAdd = max(CompoundIppParams->RadiusX,  CompoundIppParams->RadiusY) * 2 + 1;
		//_ippCompoundApply = &(FilterIpp->CompUnsharpMask);
		break;
   }
//_ippCompoundApply = &(_filterIpp->ApplyCompound);
//TODO: not in such brute way...call the method
if (_SC->Brush->State->cpROItol < roiTolAdd)
	_SC->Brush->State->cpROItol += roiTolAdd;
TRenderBasic::Begin(X, Y);
_SC->skip_empty_alpha = true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
bool TRenderCompound::renderIppFilterCoords(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
if (!prepareSpareImageCoords(X1, Y1, X2, Y2))
   return false;
_filterIpp->PrepareCompound(&_boundRect, CompoundIppParams);
_filterIpp->ApplyCompound(_SC->ippEdit, _SC->FilterImage->ImageIpp, &_boundRect, CompoundIppParams);
return true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
bool TRenderCompound::renderIppFilterRect(const TRect &roiRect)
{
if (!prepareSpareImageRect(roiRect))
   return false;
_filterIpp->PrepareCompound(&_boundRect, CompoundIppParams);
_filterIpp->ApplyCompound(_SC->ippEdit, _SC->FilterImage->ImageIpp, &_boundRect, CompoundIppParams);
return true;
}

