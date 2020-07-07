//---------------------------------------------------------------------------
#ifndef spCommonH
#define spCommonH
#include <Types.hpp>
//#include "spSurfaceContainer.h"
//#include "brushBasic.h"
#include "spPixelTypes.hpp"
#include "spImage.hpp"
//#include "ippFilters.hpp"
//#include "spPixelArithmetics.hpp"
//---------------------------------------------------------------------------
class spCommon
  {
  public:
	// common helper vars
	static bool ippAlive;
	static TRect bigBoundRect;
    // structures
  struct spsFastBlurGray
	 {
	 int radius;
	 int xs;
	 int ys;
	 int w;
	 int h;
	 int wm;
	 int hm;
	 int wh;
	 int div;
	 int *ch;
	 int *vmin;
	 int *vmax;
	 int *dv;
	 spImage<ptGray> *img;
	 };
  struct spsHistogram
	 {
	 // histogram
	 int value[256];
	 int n;
	 };
  private:
	 static void medDelPixels(spImage<ptGray> *im, int row, int col, int size, spsHistogram *h, TRect *boundRect);
	 static void medAddPixels(spImage<ptGray> *im, int row, int col, int size, spsHistogram *h, TRect *boundRect);
	 static void medInitHistogram(spImage<ptGray> *im, int row, int size, spsHistogram *h, TRect *boundRect);
	 static int qsPartition(double theList[], int assocList[], int start, int end);
  public:
  template <class T>
  static T & GetWrapPixel(spImage<T> *image, int X, int Y, bool wrap = true)
	 {
	if (wrap)
	   {
	   if (X < 0)
		   X = image->Width() - 1 + X;
	   if (Y < 0)
		   Y = image->Height() - 1 + Y;
	   if (X > (image->Width() - 1))
		   X = X - (image->Width() - 1);
	   if (Y > (image->Height() - 1))
		   Y = Y - (image->Height() - 1);
	   }
	else
	   {
	   if (X < 0)
		   X = 0;
	   if (Y < 0)
		   Y = 0;
	   if (X > (image->Width() - 1))
		   X = image->Width() - 1;
	   if (Y > (image->Height() - 1))
		   Y = image->Height() - 1;
	   }
	// proraèun shifta
	return image->Pixel(Y, X);
	}
  //---------------------------------------------------------------------------
  /* moved to pixelTypes ptRGB()
  template <class T>
  static T Color2BGR(unsigned int color)
	{
	T val;
	val.ch[0]  = Byte((color<<8)>>24);
	val.ch[1]  = BYTE((color<<16)>>24);
	val.ch[2]  = BYTE((color<<24)>>24);
	return val;
	}
  */
  //---------------------------------------------------------------------------
  //template <class T>
  static unsigned int ptRGB2Color(ptRGB val)
	{
	unsigned int color = val.ch[2] | ((unsigned int )(val.ch[1])<<8) | ((unsigned int)(val.ch[0])<<16);
	return color;
	}
  //---------------------------------------------------------------------------
  static int CalcVariation(int variation);
  static bool CoinToss(bool coinOn);
  // fast blur grayscale
  static spsFastBlurGray* FBGrayPrepare(spImage<ptGray> *img, TRect *boundRect, int radius);
  static void FBGrayUnPrepare(spsFastBlurGray *fbgData);
  static void FBGrayRun(spsFastBlurGray *fbgData);
  static void FBGrayRun(spImage<ptGray> *img, TRect *boundRect, int radius);
  static void DownsampleGray(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut);
  static void DownsampleRGB(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut);
  static void ResizeBilinearGray(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut);
  static void ResizeBilinearRGB(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut);
  static void AddWeighted(spImage<ptRGB> *imgIn1, float aIn1, spImage<ptRGB> *imgIn2, float aIn2,
			  spImage<ptRGB> *imgOut, TRect *commonRoi = 0);
  static int  BinarySearch(double sortedArray[], int first, int last, double key, bool closest);
  static void Palette3C(ptRGB *palEntry, unsigned int botC, unsigned int midC, unsigned int topC);
  static void PaletteShortPalC(ptRGB *palEntry, unsigned int *shortPal, int shortPalCount);
  static void MedianGray(spImage<ptGray> *im, TRect *boundRect, int size);
  static void BoxBlurGray(spImage<ptGray> *im, int radius);
  static void BoxBlurGray(spImage<ptGray> *im, TRect *boundRect, int radius);
  static void CollectCCD(spImage<ptRGB> *im, TRect *boundRect, double *cumulBGR, double *distanceBGR, spImage<ptGray> *mask);
  static int  QuickSort(double theList[], int assocList[], int start, int end);
  static void CreateLuminanceData(double *palLuminance, int *palIndex,  ptRGB *palEntry, int palCount);
  //
  static void BlendImages(spImage<ptRGB> *imgIn1, spImage<ptRGB> *imgIn2, int nIn2, spImage<ptRGB> *imgOut);
  static void BlendImageMaskColor(spImage<ptRGB> *imgIn, spImage<ptGray> *imgMask, ptRGB color, spImage<ptRGB> *imgOut);
  static void PasteImage(spImage<ptRGB> *imgIn, spImage<ptGray> *alphaIn, spImage<ptRGB> *imgOut, spImage<ptGray> *alphaOut, int nIn);
  static void UpdateMaskHigh(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut, TRect *boundRect);
  static void HardMask(spImage<ptGray> *mask, TRect *boundRect);
  //
  static void ResetBigBound(void);
  static void UpdateBigBound(TRect *boundRect);
  static TRect GetPolyBound(TPoint *polyPts, int polyCount);
  static TRect GetBigBound(int iW, int iH);
  //
  static void RotateBilinearGray(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut, double angle);
  static void ResizeStampMar(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut, int size);
  };
#endif
