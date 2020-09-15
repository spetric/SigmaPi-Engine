//---------------------------------------------------------------------------
#pragma hdrstop
#include "spColorArithmetics.h"
#include "spPixelMixer.h"
#include "spShuffler.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// static vars
//---------------------------------------------------------------------------
spColorArithmetics::TArithFunc spColorArithmetics::ArithFunc = 0;
spColorArithmetics::TColorFunc spColorArithmetics::ColorFunc = 0;
spColorArithmetics::TFilterFunc spColorArithmetics::FilterFunc = 0;
spPixelArithmetics::spsArith<ptRGB> spColorArithmetics::ArithData;
spPixelArithmetics::spsColor<ptRGB> spColorArithmetics::ColorData;
spPixelArithmetics::spsFilter<ptRGB> spColorArithmetics::Filter33Data;
//---------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------
void spColorArithmetics::singleOut(Tsc *sc, bool force_solid)
{
if (sc->AlphaOrig)
   spPixelMixer::SingleColorAlpha(sc, force_solid);
else
   spPixelMixer::SingleColor(sc, force_solid);
}
//---------------------------------------------------------------------------
void spColorArithmetics::doubleOut(Tsc *sc, bool force_solid)
{
if (sc->AlphaOrig)
   spPixelMixer::DoubleColorAlpha(sc, force_solid);
else
   spPixelMixer::DoubleColor(sc, force_solid);
}
//---------------------------------------------------------------------------
void spColorArithmetics::PrimaryColor(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->PrimaryColor,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::SecondaryColor(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->SecondaryColor,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::PolyColor(Tsc *sc)
{
//sc->Brush->ColorParams->WorkPrimary = sc->Brush->ColorParams->PolyColor;
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->PolyColor,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::AlphaMixedColors(Tsc *sc)
{
//sc->Brush->ColorParams->WorkPrimary   = sc->Brush->ColorParams->PrimaryColor;
//sc->Brush->ColorParams->WorkSecondary = sc->Brush->ColorParams->SecondaryColor;
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->PrimaryColor,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->SecondaryColor,
				sc->Brush->ColorParams->WorkSecondary, &ArithData);
doubleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::SolidMixedColors(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->PrimaryColor,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->SecondaryColor,
				sc->Brush->ColorParams->WorkSecondary, &ArithData);
doubleOut(sc, true);
}
//---------------------------------------------------------------------------
void spColorArithmetics::AlphaPalette(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->PalEntry[(int)sc->MaskRight->Byte0[sc->i][sc->j]],
				sc->Brush->ColorParams->WorkPrimary, &(ArithData));
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::SolidPalette(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->PalEntry[(int)sc->MaskRight->Byte0[sc->i][sc->j]],
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, true);
}
//---------------------------------------------------------------------------
void spColorArithmetics::PaletteStep(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				//sc->PalEntry[sc->stepCount],
				sc->PalEntry[sc->Brush->State->wpStepCount],
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::PaletteRandom(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				//sc->PalEntry[sc->stepCount],
				sc->PalEntry[random(sc->PalCount)],
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::AlphaShortPal(Tsc *sc)
{
int index = fast_ftol((float)(sc->MaskRight->Byte0[sc->i][sc->j]) * sc->PalCount / 255.0);
if (index > (sc->PalCount - 1))
   index = sc->PalCount - 1;
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->PalEntry[index],
				sc->Brush->ColorParams->WorkPrimary, &(ArithData));
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::SolidShortPal(Tsc *sc)
{
int index = fast_ftol((float)(sc->MaskRight->Byte0[sc->i][sc->j]) * sc->PalCount / 255.0);
if (index > (sc->PalCount - 1))
   index = sc->PalCount - 1;
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->PalEntry[index],
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, true);
}
//---------------------------------------------------------------------------
void spColorArithmetics::GradientPrimSec(Tsc *sc)
{
// more li malo bolje
ptRGB val;
for (int k = 0; k < 3; k++)
	//val.ch[k] = (BYTE)fast_ftol(((255.0 - sc->realStepCount) * sc->Brush->ColorParams->PrimaryColor.ch[k]
	//														 + sc->realStepCount * 1.0 * sc->Brush->ColorParams->SecondaryColor.ch[k])/ 255.0);
	val.ch[k] = (BYTE)fast_ftol(((255.0 - sc->Brush->State->wpRealStepCount) * sc->Brush->ColorParams->PrimaryColor.ch[k]
															 + sc->Brush->State->wpRealStepCount * 1.0 * sc->Brush->ColorParams->SecondaryColor.ch[k])/ 255.0);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::TextureAligned(Tsc *sc)
{
ptRGB val = spPixelMixer::GetTexturePixelAligned(sc);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::TextureFixed(Tsc *sc)
{
ptRGB val = spPixelMixer::GetTexturePixelFixed(sc);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::CloneImageAligned(Tsc *sc)
{
ptRGB val = spPixelMixer::GetCloneImagePixelAligned(sc);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::CloneImageFixed(Tsc *sc)
{
ptRGB val = spPixelMixer::GetCloneImagePixelFixed(sc);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::PolyTexture(Tsc *sc)
{
// always aligned
ptRGB val = spPixelMixer::GetPolyTexturePixelAligned(sc);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::IppFilter(Tsc *sc)
{
// ipp filtered image
//int test_i = sc->i_start;
//int test_j = sc->j_start;
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->FilterImage->ImageSp->Pixel(sc->i - sc->i_start, sc->j - sc->j_start),
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::CustomSingleColor(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->CustomColorPrim,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::CustomAlphaMixed(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->CustomColorPrim,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->CustomColorSec,
				sc->Brush->ColorParams->WorkSecondary, &ArithData);
doubleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::CustomSolidMixed(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->CustomColorPrim,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->CustomColorSec,
				sc->Brush->ColorParams->WorkSecondary, &ArithData);
doubleOut(sc, true);
}
//---------------------------------------------------------------------------
void spColorArithmetics::CustomGradient(Tsc *sc)
{
ptRGB val;
for (int k = 0; k < 3; k++)
	//val.ch[k] = (BYTE)fast_ftol(((255.0 - sc->realStepCount) * sc->Brush->ColorParams->CustomColorPrim.ch[k]
	//							 + sc->realStepCount * 1.0 * sc->Brush->ColorParams->CustomColorSec.ch[k])/ 255.0);
	val.ch[k] = (BYTE)fast_ftol(((255.0 - sc->Brush->State->wpRealStepCount) * sc->Brush->ColorParams->CustomColorPrim.ch[k]
								 + sc->Brush->State->wpRealStepCount * 1.0 * sc->Brush->ColorParams->CustomColorSec.ch[k])/ 255.0);
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::ShiftPixels(Tsc *sc)
{
ptRGB val;
if (sc->X_shift == 0 && sc->Y_shift == 0 && sc->Randomizator > 0)
   {
   int rx = sc->Randomizator + 1;
   int ry = sc->Randomizator + 1;
   rx = (rx>>1) - random(rx);
   ry = (ry>>1) - random(ry);
   val = spCommon::GetWrapPixel(sc->Edit,
								  sc->j - rx,
								  sc->i - ry); // wrap
   }
else
   val = spCommon::GetWrapPixel(sc->Edit,
								  sc->j - sc->X_shift,
								  sc->i - sc->Y_shift); // wrap
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				val,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::EraseAlphaChannel(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->PrimaryColor,
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
if (ArithData.goOn)
   spPixelMixer::EraseAlpha(sc);
}
//---------------------------------------------------------------------------
void spColorArithmetics::Filter33(Tsc *sc)
{
int m, si, sj;
// napuni pixele susidstva
sc->Edit->PixelOverFlow = Filter33Data.wrap ? eopWrap : eopBorder;
for (m = 0; m < 9; m++)
	{
	si = sc->i + spe_K33I[m];
	sj = sc->j + spe_K33J[m];
	if (!sc->Edit->SetSafeIndex(&si, &sj))
	   continue;
	Filter33Data.nbh[m] = sc->Edit->Pix[si][sj];
	}
FilterFunc(sc->Edit->Pix[sc->i][sc->j],
			 sc->Brush->ColorParams->PrimaryColor,
			 sc->Brush->ColorParams->WorkPrimary, &Filter33Data);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::SineColor(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->ArithSurf->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::Marker(Tsc *sc)
{
spPixelMixer::Marker(sc);
}
//---------------------------------------------------------------------------
void spColorArithmetics::SidekickImageAligned(Tsc *sc)
{
ArithFunc(sc->ArithSurf->Pix[sc->i][sc->j],
				sc->AssSidekick->Pixel(sc->i, sc->j),
				sc->Brush->ColorParams->WorkPrimary, &ArithData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::NearestLuminance(Tsc *sc)
{
// specijalac do boli
int k, m, pal_index;
double pix_lumina = 0;
for (k = 0; k < 3; k++)
	pix_lumina += sc->Edit->Pix[sc->i][sc->j].ch[k] * spe_D_LUMINANCE[k];
	m = spCommon::BinarySearch(sc->PalLuminance, 0, sc->PalCount - 1, pix_lumina, true);
if (m < 0)
	m = 0;
if (m > (sc->PalCount - 1))
	m = sc->PalCount - 1;
pal_index = sc->PalIndex[m];
ColorFunc(sc->Edit->Pix[sc->i][sc->j],
				sc->PalEntry[pal_index],
				sc->Brush->ColorParams->WorkPrimary, &ColorData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::ColorColor(Tsc *sc)
{
ColorFunc(sc->Edit->Pix[sc->i][sc->j],
				sc->Brush->ColorParams->CustomColorPrim,
				sc->Brush->ColorParams->WorkPrimary, &ColorData);
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::ColorCurve(Tsc *sc)
{
// LUT curve
Byte **LUTEntry = sc->LUTCurveEntry;
int k, eidx;
for (k = 0; k < 3; k++)
	{
	eidx = sc->Edit->Pix[sc->i][sc->j].ch[k];
	sc->Brush->ColorParams->WorkPrimary.ch[k] = LUTEntry[0][LUTEntry[3 - k][eidx]];
	}
// no arithmetics
singleOut(sc, false);
}
//---------------------------------------------------------------------------
void spColorArithmetics::Shuffle(Tsc *sc)
{
TShuffler *shuffler = reinterpret_cast <TShuffler*>(sc->userData);
shuffler->Execute(sc->Edit, sc->i_pick, sc->j_pick, sc->i, sc->j);
sc->Brush->ColorParams->WorkPrimary =  shuffler->Pixel;
// no arithmetics
singleOut(sc, false);
}



