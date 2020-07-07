//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderTwister.h"
#include "ippProcessing.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
// constructor
//---------------------------------------------------------------------------
TRenderTwister::TRenderTwister(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
						:TRenderBasic(surfaceContainer, renderData)

{
ColorTwistIppParams  = renderData->ColorTwistIppParams;
}
//---------------------------------------------------------------------------
// destructor
 TRenderTwister::~TRenderTwister(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderTwister::ClassName(void)
{
return "TRenderTwister";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderTwister::Begin(int X, int Y)
{
//int roiTolAdd = 0;
_autoRenderSuggest =  spe_RENDER_FAST;
//
if (!ColorTwistIppParams->SyntaxOk)
   {
   _colorArithmetic = 0;
   spColorArithmetics::ArithFunc = 0;
   }
else
   {
   GoFilterIppRenderCoords = &renderColorTwistCoords;
   GoFilterIppRenderRect   = &renderColorTwistRect;
   _colorArithmetic = &spColorArithmetics::IppFilter;
   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
   _SC->skip_empty_alpha = false;
   /*
   int i, k, m;
   m = k = -1;
   for (i = 0; i < 12; i++)
	   {
	   if (i % 4 == 0)
		  {
		  m++;
		  k = 0;
		  }
	   _Twister[m][k++] = ColorTwistIppParams->Matrix[i] / (float)ColorTwistIppParams->Scale;
	   }
   */
   ippProcess::GetTwister(ColorTwistIppParams->Matrix, ColorTwistIppParams->Scale, _Twister);
   }
TRenderBasic::Begin(X, Y);
_SC->skip_empty_alpha = true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
bool TRenderTwister::renderColorTwistCoords(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
if (!prepareSpareImageCoords(X1, Y1, X2, Y2))
   return false;
return ippProcess::ColorTwist(_SC->ippEdit, _SC->FilterImage->ImageIpp, &_boundRect, _Twister, ColorTwistIppParams->Bias);
}
//---------------------------------------------------------------------------
bool TRenderTwister::renderColorTwistRect(const TRect &roiRect)
{
if (!prepareSpareImageRect(roiRect))
   return false;
return ippProcess::ColorTwist(_SC->ippEdit, _SC->FilterImage->ImageIpp, (TRect *)&roiRect, _Twister, ColorTwistIppParams->Bias);
}


