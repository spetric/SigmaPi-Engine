//---------------------------------------------------------------------------
#ifndef spProcessingH
#define spProcessingH
#include <Types.hpp>
#include "spGlobals.h"
#include "spCommon.h"
#include "spPixelTypes.hpp"
#include "spImage.hpp"
//---------------------------------------------------------------------------
class spProcess
  {
  private:
  public:
	 //template <class T, class Z>
	 //static void Convert(spImage<T> *imgIn, spImage<Z> *imgOut);
	 static void Convert(spImage<ptRGB> *imgIn, spImage<ptGray> *imgOut, bool lumina = false);
	 static void Convert(spImage<ptGray> *imgIn, spImage<ptRGB> *imgOut);
	 static void Grayscale(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, double &rFact, double &gFact, double &bFact);
	 static void Mirror(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, int axis);
	 static void NearestLuminance(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect,  ptRGB *palEntry, int palCount);
	 static void NearestColor(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, ptRGB *palEntry, int palCount);
	 static void PseudoColor(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, ptRGB *palEntry,int palCount);
	 static void ColorQuantization(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect);
	 static	void ArithBool(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, int opType, int opValue);
  };
#endif
