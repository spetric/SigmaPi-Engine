//---------------------------------------------------------------------------
#ifndef spPixelMixerH
#define spPixelMixerH
#include "brushBasic.h"
#include "spSurfaceContainer.h"
#include "spCommon.h"
//---------------------------------------------------------------------------
class spPixelMixer
  {
  public:
	typedef spSurfaceContainer Tsc;
	//---------------------------------------------------------------------------
	static void SingleColor(Tsc *sc, bool force_solid);
	static void SingleColorAlpha(Tsc *sc, bool force_solid);
	static void DoubleColor(Tsc *sc, bool force_solid);
	static void DoubleColorAlpha(Tsc *sc, bool force_solid);
	static void SingleColorExternalAlpha(Tsc *sc);
	static void EraseAlpha(Tsc *sc);
    static void Marker(Tsc *sc);
	static ptRGB & GetTexturePixelAligned(Tsc *sc);
	static ptRGB & GetTexturePixelFixed(Tsc *sc);
	static ptRGB & GetCloneImagePixelAligned(Tsc *sc);
	static ptRGB & GetCloneImagePixelFixed(Tsc *sc);
	static ptRGB & GetPolyTexturePixelAligned(Tsc *sc);
	static void BumpCalc(Tsc *sc, int mask_perc);
	static void ShadowColor(Tsc *sc, ptRGB &shadowColor, int &shadowCap, int &si, int &sj);
	static void BlendColor(Tsc *sc);
};
//---------------------------------------------------------------------------
#endif
