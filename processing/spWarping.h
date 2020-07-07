//---------------------------------------------------------------------------
#ifndef spWarpingH
#define spWarpingH
//---------------------------------------------------------------------------
#include <Types.hpp>
#include "spGlobals.h"
#include "spCommon.h"
#include "spPixelTypes.hpp"
#include "spImage.hpp"
struct warpDataStruct
  {
  spImage<ptRGB> *srcImage;
  spImage<ptRGB> *dstImage;
  spImage<ptGray> *srcAlpha;
  spImage<ptGray> *dstAlpha;
  spImage<ptGray> *selImage;
  bool Prepared;
  int X1;
  int Y1;
  int X2;
  int Y2;
  bool Restrict;
  double watract;
  double bq_factor;
  bool bi_linear;
  bool antialias;
  bool Chan[3];
  int capacity;
  int mask_perc;
  int mode;
  TRect BoundRect;
  //int SelFeather;
  Byte SelByte;
  //bool RestoreSelection;
  };
//---------------------------------------------------------------------------
class spWarp
  {
  public:
	 typedef void (*TWarpExecute)(warpDataStruct *);
	 typedef void (*TWarpCalcDispl)(double &xd, double &yd, double &xm, double &ym, double &xf, double &yf,  int &i, int &j, double &bf);
  private:
	 static void pixInterpol(warpDataStruct *warpData, int &i, int &j, double &X_displ, double &Y_displ);
     static void restorePixel(warpDataStruct *warpData, int &i, int &j);
	 static void displBiLinear(double &xd, double &yd, double &xm, double &ym, double &xf, double &yf,  int &i, int &j, double &bf);
	 static void displBiQuadratic(double &xd, double &yd, double &xm, double &ym, double &xf, double &yf,  int &i, int &j, double &bf);
     static void updateBB(warpDataStruct *WarpData, TRect *boundRect, int &iW, int &iH);
  public:
	 //template <class T, class Z>
	 //static void Convert(spImage<T> *imgIn, spImage<Z> *imgOut);
	 static void Begin(warpDataStruct *WarpData);
	 static void End(warpDataStruct *WarpData);
	 static void WarpLinearBox(warpDataStruct *WarpData);
	 static void WarpLinearImage(warpDataStruct *WarpData);
	 static void WarpSineSplash(warpDataStruct *WarpData);
	 static void WarpSineRipple(warpDataStruct *WarpData);
	 static void WarpTwirl(warpDataStruct *WarpData);
	 static void WarpDisorder(warpDataStruct *WarpData);
  };
#endif
