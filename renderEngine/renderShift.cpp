//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderShift.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderShift::TRenderShift(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
						:TRenderBasic(surfaceContainer, renderData)

{
ShiftParams = renderData->ShiftParams;
}
//---------------------------------------------------------------------------
// destructor
TRenderShift::~TRenderShift(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderShift::ClassName(void)
{
return "TRenderShift";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void  TRenderShift::Begin(int X, int Y)
{
//int i, k;
prepareColors();
_autoRenderSuggest =  spe_RENDER_FAST;
_colorArithmetic =  &spColorArithmetics::ShiftPixels;
//
_SC->X_shift = ShiftParams->X_shift;
_SC->Y_shift = -ShiftParams->Y_shift;
_SC->Randomizator = ShiftParams->ZeroShiftRandom;
spColorArithmetics::ArithData.topVal = ptRGB(&ShiftParams->ScaleTopRGB[0]);
spColorArithmetics::ArithData.botVal = ptRGB(&ShiftParams->ScaleBottomRGB[0]);
spColorArithmetics::ArithData.invertPixel = ShiftParams->InvertSource;
spColorArithmetics::ArithData.bitShift = ShiftParams->BitShift;
spColorArithmetics::ArithData.bumpScale = ShiftParams->BumpScale;
spColorArithmetics::ArithData.bumpVal = _SC->Brush->ColorParams->PrimaryColor;
switch (ShiftParams->MainMode)
   {
   case spe_ARITH_NORM:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		break;
   case  spe_ARITH_XOR:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_XOR;
		break;
   case spe_ARITH_AND:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_AND;
        break;
   case spe_ARITH_OR:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_OR;
        break;
   case spe_ARITH_ADD:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_ADD;
        break;
   case spe_ARITH_SUB:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_SUB;
        break;
   case spe_ARITH_MULT:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MULT;
        break;
   case spe_ARITH_MULT_nA:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MULT_nA;
		break;
   case spe_ARITH_MULT_nB:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MULT_nB;
		break;
   case spe_ARITH_HIGHER:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Higher;
		break;
   case spe_ARITH_LOWER:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Lower;
		break;
   case spe_ARITH_MINMAX:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MinMax;
		break;
   case spe_ARITH_RANDOM:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Random;
		break;
   case spe_ARITH_EQUALIZE:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Equalize;
		break;
   case spe_ARITH_BURNER:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Burner;
		break;
   case spe_ARITH_INTENSITY_B:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_IntensityB;
		break;
   case spe_ARITH_INTENSITY_A:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_IntensityA;
		break;
   case spe_ARITH_A_MOD_B:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_A_MOD_B;
		break;
   case spe_ARITH_AxB:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_AxB;
		break;
   case spe_ARITH_BURN_LINEAR:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_BurnLinear;
		break;
   case spe_ARITH_SQRT_AB:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_SqrtAB;
		break;
   case spe_ARITH_ABS_SUB:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_AbsSub;
		break;
   case spe_ARITH_EXCLUSION:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Exclusion;
		break;
   case spe_ARITH_BUMP:
		spColorArithmetics::ArithData.topVal = GlobalColorParams->ScaleTop;
		spColorArithmetics::ArithData.botVal = GlobalColorParams->ScaleBottom;
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Bump;
		break;
   case spe_ARITH_BUMP_COLOR:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_BumpColor;
		break;
   }
TRenderBasic::Begin(X, Y);
_SC->skip_empty_alpha = true;
}

