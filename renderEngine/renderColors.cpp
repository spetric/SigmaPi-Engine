#pragma hdrstop
#include "renderColors.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderColors::TRenderColors(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
					   : TRenderBasic(surfaceContainer, renderData)
{
ColorParams  = renderData->ColorParams;
}
//---------------------------------------------------------------------------
// destructor
TRenderColors::~TRenderColors(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderColors::ClassName(void)
{
return "TRenderColors";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderColors::Begin(int X, int Y)
{
int k;
prepareColors();
_autoRenderSuggest =  spe_RENDER_FAST;
_colorArithmetic =  &spColorArithmetics::ColorColor;
//
_SC->CloneImage = 0;
spColorArithmetics::ArithData.topVal = GlobalColorParams->ScaleTop;
spColorArithmetics::ArithData.botVal = GlobalColorParams->ScaleBottom;
//prepareBrushScheme(_SC->Brush->ColorParams->SchemeIndex, X, Y);
//_SC->Brush->ColorParams->WorkPrimary = _SC->Brush->ColorParams->PrimaryColor;
//_SC->Brush->ColorParams->WorkSecondary = _SC->Brush->ColorParams->SecondaryColor;
spColorArithmetics::ColorData.mapRelative = ColorParams->CPercentage;
for (k = 0; k < 3; k++)
	{
	spColorArithmetics::ColorData.mapFactor[k] = (double)(ColorParams->ChanRGB[k] / 100.0) ;
	spColorArithmetics::ColorData.castOn[k]  = ColorParams->CastOn[k];
	spColorArithmetics::ColorData.castVal[k] = ptRGB(ColorParams->CastColor[k]);
	spColorArithmetics::ColorData.scaleRange[k][0] = (float)ColorParams->ScaleBottomRGB[k];
	spColorArithmetics::ColorData.scaleRange[k][1] = (float)ColorParams->ScaleTopRGB[k];
	}
spColorArithmetics::ColorData.castRange[0] = (float)ColorParams->CastRange[0];
spColorArithmetics::ColorData.castRange[1] = (float)ColorParams->CastRange[1];
switch (ColorParams->MainMode)
   {
   case  spe_COLOR_BRIGTHNESS:
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Brightness;
		 break;
   case  spe_COLOR_STRETCH:
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Stretch;
		 break;
   case  spe_COLOR_HSV:
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_HSV;
		 break;
   case  spe_COLOR_LMH:
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_LMH;
		 break;
   case  spe_COLOR_GAMMA:
		 for (k = 0; k < 3; k++)
			 spColorArithmetics::ColorData.mapFactor[k] = (double)ColorParams->ChanGamma[k];
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Gamma;
		 break;
   case  spe_COLOR_EXCHNAGE:
         if (ColorParams->ChanME[0] && ColorParams->ChanME[1] && ColorParams->ChanME[2])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_ExcRGB;
		 else if (ColorParams->ChanME[0] && ColorParams->ChanME[1])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_ExcRG;
         else if (ColorParams->ChanME[1] && ColorParams->ChanME[2])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_ExcGB;
		 else
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_ExcBR;
		 break;
   case  spe_COLOR_MAX:
		 if (ColorParams->ChanME[0] && ColorParams->ChanME[1] && ColorParams->ChanME[2])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MaxRGB;
		 else if (ColorParams->ChanME[0] && ColorParams->ChanME[1])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MaxRG;
		 else if (ColorParams->ChanME[1] && ColorParams->ChanME[2])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MaxGB;
		 else
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MaxBR;
		 break;
   case  spe_COLOR_MIN:
		 if (ColorParams->ChanME[0] && ColorParams->ChanME[1] && ColorParams->ChanME[2])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MinRGB;
		 else if (ColorParams->ChanME[0] && ColorParams->ChanME[1])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MinRG;
		 else if (ColorParams->ChanME[1] && ColorParams->ChanME[2])
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MinGB;
		 else
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_MinBR;
		 break;
   case  spe_COLOR_MIN_MAX:
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_ExcMinMax;
		 break;
   case  spe_COLOR_CAST:
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Cast;
		 break;
   case  spe_COLOR_SCALE:
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Scale;
		 break;
   case  spe_COLOR_TEMPERATURE:
		 spPixelArithmetics::Temperature_RGB(ColorParams->Temperature, _SC->Brush->ColorParams->CustomColorPrim);
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Luminance;
		 break;
   case spe_COLOR_TONING:
		 if (ColorParams->ToneType == 0)
			{
			_SC->Brush->ColorParams->CustomColorPrim =  ptRGB(ColorParams->ToneColor);
			spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Luminance;
			}
		 else
			{
			_colorArithmetic = &spColorArithmetics::NearestLuminance;
			if (ColorParams->ToneType == 1)
			   {
			   _SC->PalEntry = _SC->Brush->ColorParams->PalEntry;
			   _SC->PalCount = 256;
			   }
			else
			   {
			   _SC->PalEntry = &(GlobalColorParams->ShortPalEntry[0]);
			   _SC->PalCount = GlobalColorParams->ShortPalCount;
			   }
			_SC->PalLuminance = palLuminance;
			_SC->PalIndex     = palIndex;
			spCommon::CreateLuminanceData(palLuminance, palIndex, _SC->PalEntry, _SC->PalCount);
			if (ColorParams->TonePreserveLuminance)
			   spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Luminance;
			else
			   spColorArithmetics::ColorFunc =  &spPixelArithmetics::Pixel_Color_NORM;
			}
		 break;
   case spe_COLOR_CURVES:
		 // kao normalno crtanje
		 _SC->LUTCurveEntry =  GlobalColorParams->LUTCurveEntry;
		 _colorArithmetic = &spColorArithmetics::ColorCurve;
		 spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		 break;
   case spe_COLOR_CORRECTION:
		 // NOTE!!!!!!
		 // spColorArithmetics::ColorData.SrcCumul , SrcDistance, DstCumul, DstDistance <- filled in spEngineWrapper
		 //
		 spColorArithmetics::ColorFunc = &spPixelArithmetics::Pixel_Color_Correction;
		 break;
   }
TRenderBasic::Begin(X, Y);
_SC->skip_empty_alpha = true;
}

