//---------------------------------------------------------------------------
#ifndef ippStatisticsH
#define ippStatisticsH
#include "ippImage.hpp"
#include "spGlobals.h"
//---------------------------------------------------------------------------
class ippStatistic
  {
  private:
	static IppiMomentState_64f *pMomentState;
	static Ipp8u* pSrc;
	static Ipp8u* pDst;
	static int srcStep;
	static int dstStep;
	static IppiSize roiSize;
	static int nChans;
	static Ipp64f normInf[4];
	static Ipp64f normL1[4];
	static Ipp64f normL2[4];
	static ippImage<ippRGB> *imgColor;
	static ippImage<ippGray> *imgGray;
	static bool statImagesOwned;
	//
	template <class T>
	static void prepareData(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect)
		{
		nChans = sizeof(T);
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
	//
	static int Histogram(ippImage<ippRGB> *img, TRect *roi, int nBins, unsigned int** histo);
	static int ApplyLUT(ippImage<ippRGB> *img, TRect *roi, int nBins, unsigned int** lut, int interpol);
	static void SetStatRectangle(TRect *boundRect, TRect &roiRect);
	static void SetStatImages(void *bufferColor, void *bufferGray, int width, int height, unsigned int scanlineAlignment);
	static void SetStatImages(ippImage<ippRGB> *iColor, ippImage<ippGray> *iGray);
	static int InitMoments(void);
	static int ComputeMoments(ippImage<ippRGB> *img, TRect *roi);
	static int ComputeMoments(ippImage<ippGray> *img, TRect *roi);
	static double GetMoment(int mOrd, int nOrd, int channel, TippMomentType type);
	static void ReleaseStatImages(void);
	static void ReleaseMoments(void);
	static int ComputeNorms(ippImage<ippRGB> *img, TRect *roi);
	static int ComputeNorms(ippImage<ippGray> *img, TRect *roi);
	static double GetNorm(int channel, TippNormType type);
	__property ippImage<ippRGB>* ImageColor = {read = imgColor};
    __property ippImage<ippGray>* ImageGray = {read = imgGray};
  };
#endif
