//---------------------------------------------------------------------------
#ifndef ippProcessingH
#define ippProcessingH
#include <Types.hpp>
#include "ippImage.hpp"
//---------------------------------------------------------------------------
class ippProcess
  {
  private:
	//static void prepareData(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect);
	template <class T>
	static void prepareData(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect)
		{
        int nChans = sizeof(T);
		pSrc = (unsigned char*)srcImage->Data();
		pDst = (unsigned char*)dstImage->Data();
		srcStep = srcImage->Stride();
		dstStep = dstImage->Stride();
		roiSize.width  = roiRect->Width();
		roiSize.height = roiRect->Height();
		// parivrta
		if (srcImage->Width() != roiRect->Width() || srcImage->Height() != roiRect->Height())
		   {
		   pSrc = pSrc + (roiRect->left * nChans + (srcImage->Height() - roiRect->bottom) * srcStep);
		   pDst = pDst + (roiRect->left * nChans + (dstImage->Height() - roiRect->bottom) * dstStep);
		   }
		}
  public:
	static Ipp8u* pSrc;
	static Ipp8u* pDst;
	static int srcStep;
	static int dstStep;
	static IppiSize roiSize;
	//
	static void GetTwister(float *matrix, int scale, Ipp32f twister[][4]);
	static int ColorTwist(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Ipp32f twister[][4], int bias);
	static int DistanceTransform(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int kernelSize, int norm, bool *chanOn);
	static int FastMarching(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, float radius, bool *chanOn);
	static int GradientColor2Gray(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int norm, bool *chanOn);
	static int ReduceBits(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int noise, int ditherType, int levels);
	template <class T>
	static int Mirror(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect, int axis);
	//static int Mirror(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int axis);
	static int Remap(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, ippImage<ippFloat> *xMap, ippImage<ippFloat> *yMap, TRect *roiRect);
	static int DeconvolveLR(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int kernelSize, float threshold, int numIter);
  };
#endif
