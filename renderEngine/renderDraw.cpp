//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderDraw.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderDraw::TRenderDraw(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
					   : TRenderBasic(surfaceContainer, renderData)
{
DrawParams  = renderData->DrawParams;
}
//---------------------------------------------------------------------------
// destructor
TRenderDraw::~TRenderDraw(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderDraw::ClassName(void)
{
return "TRenderDraw";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderDraw::Begin(int X, int Y)
{
//int i, k;
prepareColors();
_autoRenderSuggest = spe_RENDER_FAST;
RenderParams->DrawPolyInterior = (DrawParams->MainMode !=  spe_ARITH_ERASE);
//
//_SC->realStepCount = 0.0;
//_SC->stepCount = 0;
//_SC->realStepper = _SC->Brush->CommonParams->PalGradStepper;
_SC->CloneImage = 0;
spColorArithmetics::ArithData.topVal = ptRGB(&DrawParams->ScaleTopRGB[0]);
spColorArithmetics::ArithData.botVal = ptRGB(&DrawParams->ScaleBottomRGB[0]);
//spColorArithmetics::ArithData.topVal = ptRGB(DrawParams->ScaleTopRGB[0],DrawParams->ScaleTopRGB[1],DrawParams->ScaleTopRGB[2] );
//spColorArithmetics::ArithData.botVal = ptRGB(DrawParams->ScaleBottomRGB[0], DrawParams->ScaleBottomRGB[1], DrawParams->ScaleBottomRGB[2]);
spColorArithmetics::ArithData.invertPixel = DrawParams->InvertSource;
spColorArithmetics::ArithData.bitShift = DrawParams->BitShift;
prepareBrushScheme(_SC->Brush->ColorParams->SchemeIndex, X, Y);
_SC->Brush->ColorParams->WorkPrimary = _SC->Brush->ColorParams->PrimaryColor;
_SC->Brush->ColorParams->WorkSecondary = _SC->Brush->ColorParams->SecondaryColor;
switch (DrawParams->MainMode)
   {
   case spe_ARITH_NORM:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		break;
   case spe_ARITH_SCALED:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_SCALED;
		break;
   case spe_ARITH_ERASE:
		spColorArithmetics::ArithData.testVal = ptRGB(DrawParams->EraseMatchColor);
		spColorArithmetics::ArithData.repVal  = ptRGB(DrawParams->EraseReplaceColor);
		spColorArithmetics::ArithData.mode   = DrawParams->EraseMatchMode;
		spColorArithmetics::ArithData.tolerance  = DrawParams->EraseMatchColorTolerance;
		if (_SC->AlphaOrig)
		   _colorArithmetic = &spColorArithmetics::EraseAlphaChannel;
		else
		   _colorArithmetic = &spColorArithmetics::PrimaryColor;
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Erase_MatchColor;
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
   case spe_ARITH_MEAN:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Mean;
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
   default:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		break;
   }
TRenderBasic::Begin(X, Y);
/* unnecessarly
if (Brush->State->cpROItol < roiTolAdd)
    Brush->State->cpROItol += roiTolAdd; */
//_SC->skip_empty_alpha = false;
_SC->skip_empty_alpha = DrawParams->DrawOnAlpha;
}
//---------------------------------------------------------------------------
// protected
//---------------------------------------------------------------------------
void TRenderDraw::prepareBrushScheme(int schemeIndex, int X, int Y)
{
_SC->Brush->State->cpStepperMax = 255.0;
_SC->PalEntry = _SC->Brush->ColorParams->PalEntry;
_SC->PalCount = 256;
switch (_SC->Brush->ColorParams->SchemeIndex)
   {
   case spe_csPrimary:
		_colorArithmetic = &spColorArithmetics::PrimaryColor;
		break;
   case spe_csSecondary:
		_colorArithmetic = &spColorArithmetics::SecondaryColor;
		break;
   case spe_csMixed:
		_colorArithmetic = &spColorArithmetics::AlphaMixedColors;
		break;
   case spe_csSolidMixed:
		_colorArithmetic = &spColorArithmetics::SolidMixedColors;
		break;
   case spe_csTextureAligned:
		if (_SC->Brush->ColorParams->Texture)
		   _colorArithmetic = &spColorArithmetics::TextureAligned;
		else
		   _colorArithmetic = &spColorArithmetics::PrimaryColor;
		break;
   case spe_csTextureFixed:
		if (_SC->Brush->ColorParams->Texture)
		   _colorArithmetic = &spColorArithmetics::TextureFixed;
		else
		   _colorArithmetic = &spColorArithmetics::PrimaryColor;
		_autoRenderSuggest = spe_RENDER_SLOW;
		break;
   case spe_csCloneFixed:
		if (_SC->CloneSelf)
		   {
			_SC->CloneImage = _SC->CloneSelf;
		   _colorArithmetic = &spColorArithmetics::CloneImageFixed;
		   }
		else if (GlobalColorParams->CloneImage)
		   {
			_SC->CloneImage = GlobalColorParams->CloneImage;
		   _colorArithmetic = &spColorArithmetics::CloneImageFixed;
		   }
		else
		   {
		   _colorArithmetic = &spColorArithmetics::PrimaryColor;
		   //TODO: return some error...
		   break;
		   }
		_SC->Clone_j = GlobalColorParams->CloneStartX;
		_SC->Clone_i = GlobalColorParams->CloneStartY;
		_autoRenderSuggest =  spe_RENDER_SLOW;
		break;
   case spe_csCloneRepos:
		GlobalColorParams->Clone_XS = GlobalColorParams->CloneStartX;
		GlobalColorParams->Clone_YS = GlobalColorParams->CloneStartY;
		//_autoRenderSuggest =  spe_RENDER_SLOW;
		GlobalColorParams->Clone_Started =  true;
   case spe_csCloneAligned:
		if (_SC->CloneSelf)
		   {
			_SC->CloneImage = _SC->CloneSelf;
		   _colorArithmetic = &spColorArithmetics::CloneImageAligned;
		   }
		else if (GlobalColorParams->CloneImage)
		   {
		   _SC->CloneImage = GlobalColorParams->CloneImage;
		   _colorArithmetic = &spColorArithmetics::CloneImageAligned;
		   }
		else
		   {
		   _colorArithmetic = &spColorArithmetics::PrimaryColor;
		   //TODO: return some error...
		   break;
		   }
		if (GlobalColorParams->Clone_Started && _SC->Brush->ColorParams->SchemeIndex == spe_csCloneAligned)
		   {
		   GlobalColorParams->Clone_XS = X - GlobalColorParams->Clone_X + GlobalColorParams->Clone_XS;
		   GlobalColorParams->Clone_YS = Y - GlobalColorParams->Clone_Y + GlobalColorParams->Clone_YS;
		   }
		else
		   GlobalColorParams->Clone_Started = true;
		GlobalColorParams->Clone_X = X;
		GlobalColorParams->Clone_Y = Y;
		_SC->Clone_j = GlobalColorParams->Clone_X - GlobalColorParams->Clone_XS;
		_SC->Clone_i = GlobalColorParams->Clone_Y - GlobalColorParams->Clone_YS;
		break;
   case spe_csPaletteAlpha:
		_colorArithmetic = &spColorArithmetics::AlphaPalette;
		break;
   case spe_csPaletteSolid:
		_colorArithmetic = &spColorArithmetics::SolidPalette;
		break;
   case spe_csPaletteStepC:
   case spe_csPaletteStepFB:
   case spe_csPaletteStepStop:
		_colorArithmetic = &spColorArithmetics::PaletteStep;
		_autoRenderSuggest =  spe_RENDER_SLOW;
		break;
   case spe_csPaletteRandom:
		_colorArithmetic  = &spColorArithmetics::PaletteRandom;
		_autoRenderSuggest =  spe_RENDER_SLOW;
		break;
   case spe_csImageStep:
		_autoRenderSuggest =  spe_RENDER_SLOW;
   case spe_csMouseDown:
		{
		/*
		TRGB mouseColorRGB = ImageContainer->ImageEdit->SpImage->ieBitmap->Pixels[X][Y];
		pxcf_Color2BGR(TRGB2TColor(mouseColorRGB), Brush->ColorParams->WorkSecondaryBGR);
		*/
		_SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[Y][X];
		_colorArithmetic = &spColorArithmetics::CustomSingleColor;
		}
		break;
   case spe_csAbsque:
		_colorArithmetic = &spColorArithmetics::SineColor;
		break;
   case spe_csSidekick:
		if (_SC->AssSidekick)
		   {
		   _autoRenderSuggest =  spe_RENDER_SLOW;
		   _SC->Brush->ColorParams->CustomColorPrim = _SC->AssSidekick->Pixel(Y, Y);
		   }
		break;
   case spe_csGradienStepC:
   case spe_csGradientStepFB:
   case spe_csGradientStepStop:
		_colorArithmetic = &spColorArithmetics::GradientPrimSec;
		_autoRenderSuggest =  spe_RENDER_SLOW;
		break;
   case spe_csShortPalAlpha:
		_SC->PalEntry = &(GlobalColorParams->ShortPalEntry[0]);
		_SC->PalCount = GlobalColorParams->ShortPalCount;
		_SC->Brush->State->cpStepperMax = (float)(_SC->PalCount - 1);
		_colorArithmetic = &spColorArithmetics::AlphaShortPal;
		break;
   case spe_csShortPalSolid:
		_SC->PalEntry = &(GlobalColorParams->ShortPalEntry[0]);
		_SC->PalCount = GlobalColorParams->ShortPalCount;
		_SC->Brush->State->cpStepperMax = (float)(_SC->PalCount - 1);
		_colorArithmetic = &spColorArithmetics::SolidShortPal;
		break;
   case spe_csShortPalStepC:
   case spe_csShortPalStepFB:
   case spe_csShortPalStepStop:
		_SC->PalEntry = &(GlobalColorParams->ShortPalEntry[0]);
		_SC->PalCount = GlobalColorParams->ShortPalCount;
		_SC->Brush->State->cpStepperMax = (float)(_SC->PalCount - 1);
		_colorArithmetic = &spColorArithmetics::PaletteStep;
		_autoRenderSuggest =  spe_RENDER_SLOW;
		break;
   case spe_csShortPalRandom:
		_SC->PalEntry = &(GlobalColorParams->ShortPalEntry[0]);
		_SC->PalCount = GlobalColorParams->ShortPalCount;
		_SC->Brush->State->cpStepperMax = (float)(_SC->PalCount - 1);
		_colorArithmetic  = &spColorArithmetics::PaletteRandom;
		_autoRenderSuggest =  spe_RENDER_SLOW;
		break;
   case spe_csMarker:
		_colorArithmetic = &spColorArithmetics::Marker;
		break;
   default:
		_colorArithmetic = &spColorArithmetics::PrimaryColor;
		break;
   }
// set steppers
switch (_SC->Brush->ColorParams->SchemeIndex)
   {
   case spe_csPaletteStepC:
   case spe_csGradienStepC:
   case spe_csShortPalStepC:
		strokeStepper = stepperCircular;
		break;
   case spe_csPaletteStepFB:
   case spe_csGradientStepFB:
   case spe_csShortPalStepFB:
		strokeStepper = stepperFB;
		break;
   case spe_csImageStep:
		strokeStepper = stepperImage;
		break;
   case spe_csPaletteStepStop:
   case spe_csGradientStepStop:
   case spe_csShortPalStepStop:
		strokeStepper = stepperStop;
		break;
   default:
		strokeStepper = stepperDummy;
		break;
   }
_SC->Brush->State->wpRealStepper = _SC->Brush->State->cpRealStepper * _SC->Brush->State->cpStepperMax / 255.0;
}
//---------------------------------------------------------------------------





