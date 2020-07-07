//---------------------------------------------------------------------------
#ifndef spColorArithmeticsH
#define spColorArithmeticsH
#include "spSurfaceContainer.h"
#include "spPixelArithmetics.hpp"
//---------------------------------------------------------------------------
class spColorArithmetics
   {
   public:
   //typedef spCommon::spsPixel TPixelStruct;
   typedef spSurfaceContainer Tsc;
   typedef spPixelArithmetics::Execute<ptRGB>::ArithFunc TArithFunc;
   typedef spPixelArithmetics::Execute<ptRGB>::ColorFunc TColorFunc;
   typedef spPixelArithmetics::Execute<ptRGB>::FilterFunc TFilterFunc;
   typedef void (*TColorArithmetic)(Tsc*);
   // arith func
   static TArithFunc ArithFunc;
   static TColorFunc ColorFunc;
   static TFilterFunc FilterFunc;
   // arith data
   static spPixelArithmetics::spsArith<ptRGB> ArithData;
   static spPixelArithmetics::spsColor<ptRGB> ColorData;
   static spPixelArithmetics::spsFilter<ptRGB> Filter33Data;
   //
   static void PrimaryColor(Tsc *sc);
   static void SecondaryColor(Tsc *sc);
   static void PolyColor(Tsc *sc);
   static void AlphaMixedColors(Tsc *sc);
   static void SolidMixedColors(Tsc *sc);
   static void AlphaPalette(Tsc *sc);
   static void SolidPalette(Tsc *sc);
   static void PaletteStep(Tsc *sc);
   static void PaletteRandom(Tsc *sc);
   static void AlphaShortPal(Tsc *sc);
   static void SolidShortPal(Tsc *sc);
   static void GradientPrimSec(Tsc *sc);
   static void TextureAligned(Tsc *sc);
   static void TextureFixed(Tsc *sc);
   static void CloneImageAligned(Tsc *sc);
   static void CloneImageFixed(Tsc *sc);
   static void PolyTexture(Tsc *sc);
   static void IppFilter(Tsc *sc);
   static void CustomSingleColor(Tsc *sc);
   static void CustomAlphaMixed(Tsc *sc);
   static void CustomSolidMixed(Tsc *sc);
   static void CustomGradient(Tsc *sc);
   static void ShiftPixels(Tsc *sc);
   static void EraseAlphaChannel(Tsc *sc);
   static void Filter33(Tsc *sc);
   static void SineColor(Tsc *sc);
   static void Marker(Tsc *sc);
   static void SidekickImageAligned(Tsc *sc);
   static void NearestLuminance(Tsc *sc);
   static void ColorColor(Tsc *sc);
   static void ColorCurve(Tsc *sc);
   static void Shuffle(Tsc *sc);
   /*
   static void ColorCorrection(PixelStruct *sc, TPixelArithmetic pixelArithmetic);
   */
   private:
   static void singleOut(Tsc *, bool);
   static void doubleOut(Tsc *, bool);
   };
#endif
