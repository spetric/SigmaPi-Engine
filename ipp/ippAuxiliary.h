//---------------------------------------------------------------------------
#ifndef ippAuxiliaryH
#define ippAuxiliaryH
#include <Types.hpp>
#include "ippImage.hpp"
#include "spGlobals.h"
#include "ippFilterBlur.hpp"
#include "ippFilterMedian.hpp"
#include "ippFilterBilateral.hpp"
#include "ippFilterGaussian.hpp"
#include "ippFilterMax.hpp"
#include "ippFilterMin.hpp"
#include "ippFilterFixed.hpp"
#include "ippFilterGeneral.hpp"
#include "ippFilterWiener.hpp"
#include "ippFilterMorph.hpp"
//---------------------------------------------------------------------------
class ippAux
  {
  private:
	  static Ipp8u* pSrc;
	  static Ipp8u* pDst;
	  static int srcStep;
	  static int dstStep;
	  static IppiSize roiSize;
	  static int channels;
	  template <class T>
	  static void prepareImageData(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect)
		  {
		  channels = sizeof(T);
		  roiSize.width = roiRect->Width();
		  roiSize.height = roiRect->Height();
		  pSrc  = (unsigned char*)srcImage->Data();
		  pDst  = (unsigned char*)dstImage->Data();
		  srcStep = srcImage->Stride();
		  dstStep = dstImage->Stride();
		  // parivrta
		  if (srcImage->Width() != roiRect->Width() || srcImage->Height() != roiRect->Height())
			 pSrc = pSrc + (roiRect->left * channels + (srcImage->Height() - roiRect->bottom) * srcStep);
		  }
  public:
	  static bool IppAdd(void);
	  static bool IppSub(void);
	  static bool IppAdd16S(void);
	  template <class T>
	  static bool IppArith(TippArithBool aType, void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment,
						   TRect *roiRect)
			{
			ippImage<T> *ippI1, *ippI2;
			ippI1  = new ippImage<T>(buffer1, width, height, scanlineAlignment);
			ippI2 =  new ippImage<T>(buffer2, width, height, scanlineAlignment);
			prepareImageData(ippI1, ippI2, roiRect);
			bool lRet;
			switch (aType)
				{
				case ipp_ADD:
					 lRet = IppAdd();
					 break;
				case ipp_SUB:
					 lRet = IppSub();
					 break;
				case ipp_MULT:
					 lRet = false;
					 break;
				case ipp_DIV:
					 lRet = false;
					 break;
				default:
					 lRet = false;
					 break;
				}
			delete ippI1;
			delete ippI2;
			return lRet;
			}
	  template <class T>
	  static bool IppColorConversion(TippCoCoType coType, void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment,
									 TRect *roiRect, bool swapChannels = true)
		   {
			if (sizeof(T) != 3)
				return false;
			int status;
			bool swap_back = false;
			ippImage<T> *ippI1, *ippI2;
			ippI1  = new ippImage<T>(buffer1, width, height, scanlineAlignment);
			if (buffer2)
			   ippI2 =  new ippImage<T>(buffer2, width, height, scanlineAlignment);
			else
			   ippI2 = new ippImage<T>(width, height); // nova noventa slika
			prepareImageData(ippI1, ippI2, roiRect);
			// TODO: swap channels where necessary
			int order[3] = { 2, 1, 0 };
			switch (coType)
				{
				case ipp_RGB2HSV:
					 if (swapChannels)
						 {
						 ippiSwapChannels_8u_C3IR(pSrc, srcStep, roiSize, &order[0]);
						 swap_back = true;
                         }
					 status =  ippiRGBToHSV_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 break;
				case ipp_HSV2RGB:
					 status =  ippiHSVToRGB_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 if (swapChannels)
						ippiSwapChannels_8u_C3IR(pDst, dstStep, roiSize, &order[0]);
					 break;
				case ipp_RGB2XYZ:
					 if (swapChannels)
						 {
						 ippiSwapChannels_8u_C3IR(pSrc, srcStep, roiSize, &order[0]);
						 swap_back = true;
						 }
					 status =  ippiRGBToXYZ_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 break;
				case ipp_XYZ2RGB:
					 status =  ippiXYZToRGB_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 if (swapChannels)
						ippiSwapChannels_8u_C3IR(pDst, dstStep, roiSize, &order[0]);
					 break;
				case ipp_RGB2YUV:
					 if (swapChannels)
						 {
						 ippiSwapChannels_8u_C3IR(pSrc, srcStep, roiSize, &order[0]);
						 swap_back = true;
						 }
					 status =  ippiRGBToYUV_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 break;
				case ipp_YUV2RGB:
					 status =  ippiYUVToRGB_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 if (swapChannels)
						ippiSwapChannels_8u_C3IR(pDst, dstStep, roiSize, &order[0]);
					 break;
				case ipp_RGB2YCbCr:
					 if (swapChannels)
						 {
						 ippiSwapChannels_8u_C3IR(pSrc, srcStep, roiSize, &order[0]);
						 swap_back = true;
						 }
					 status =  ippiRGBToYCbCr_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 break;
				case ipp_YCbCr2RGB:
					 status =  ippiYCbCrToRGB_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 if (swapChannels)
						ippiSwapChannels_8u_C3IR(pDst, dstStep, roiSize, &order[0]);
					 break;
				case ipp_RGB2Lab:
					 if (swapChannels)
						{
						ippiSwapChannels_8u_C3IR(pSrc, srcStep, roiSize, &order[0]);
						swap_back = true;
						}
					 status =  ippiRGBToLab_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 break;
				case ipp_Lab2RGB:
					 status =  ippiLabToRGB_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 if (swapChannels)
						ippiSwapChannels_8u_C3IR(pDst, dstStep, roiSize, &order[0]);
					 break;
				case ipp_RGB2HLS:
					 if (swapChannels)
						 {
						 ippiSwapChannels_8u_C3IR(pSrc, srcStep, roiSize, &order[0]);
						 swap_back = true;
						 }
					 status =  ippiRGBToHLS_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 break;
				case ipp_HLS2RGB:
					 status =  ippiHLSToRGB_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
					 if (swapChannels)
						ippiSwapChannels_8u_C3IR(pDst, dstStep, roiSize, &order[0]);
					 break;
				default:
					 status = 999;
					 break;
				}
			if (status == 0)
			   {
			   if (buffer2)
				  {
				  if (swap_back)
					 ippiSwapChannels_8u_C3IR(pSrc, srcStep, roiSize, &order[0]);
				  }
			   else
				   ippCommon::Copy(ippI2, ippI1, roiRect, roiRect, 0x00); // naštimaj parivrtu
			   }
			delete ippI1;
			delete ippI2;
			return (status == 0);
		   }
	  template <class T>
	  static bool IppFilter(void *buffer1, void *buffer2, int width, int height, unsigned int scanlineAlignment,
								Tspe_FilterIppParams *filterParams, TRect *roiRect)
		   {
		   bool lRet;
		   // lokalne slike
		   ippImage<T> *ippI1, *ippI2;
		   ippI1  = new ippImage<T>(buffer1, width, height, scanlineAlignment);
		   if (buffer2)
			   ippI2 =  new ippImage<T>(buffer2, width, height, scanlineAlignment);
		   else
			   ippI2 = new ippImage<T>(width, height); // nova noventa slika
		   ippFilterBasic<T> *filterIpp;
		   switch (filterParams->MainMode)
			   {
			   case ipp_BLUR:
					filterIpp = new ippFilterBlur<T>(filterParams->MainMode);
					break;
			   case ipp_MEDIAN:
					filterIpp = new ippFilterMedian<T>(filterParams->MainMode);
					break;
			   case ipp_BILATERAL:
					filterIpp = new ippFilterBilateral<T>(filterParams->MainMode);
					break;
			   case ipp_GAUSSIAN:
					filterIpp = new ippFilterGaussian<T>(filterParams->MainMode);
					break;
			   case ipp_MAX:
					filterIpp = new ippFilterMax<T>(filterParams->MainMode);
					break;
			   case ipp_MIN:
					filterIpp = new ippFilterMin<T>(filterParams->MainMode);
					break;
			   case ipp_MEDIAN_CROSS_XX:
			   case ipp_MEDIAN_COLOR_XX:
			   case ipp_HIPASS_XX:
			   case ipp_LAPLACE_XX:
					filterIpp = new ippFilterFixed<T>(filterParams->MainMode);
					break;
			   case ipp_SHARPEN_XX:    // poseban sluèaj...33->fixed, 55->general predefined
					if (filterParams->Fixed33)
					   filterIpp = new ippFilterFixed<T>(filterParams->MainMode);
					else
					   filterIpp = new ippFilterGeneral<T>(filterParams->MainMode);
					break;
			   case ipp_WIENER:
					filterIpp = new ippFilterWiener<T>(filterParams->MainMode);
					break;
			   case ipp_DILATE:
			   case ipp_ERODE:
			   case ipp_CLOSE:
			   case ipp_OPEN:
			   case ipp_MORPH_TOPHAT:
			   case ipp_MORPH_GRADIENT:
					filterIpp = new ippFilterMorph<T>(filterParams->MainMode);
					break;
			   case ipp_GENERAL_FILTER:
					filterIpp = new ippFilterGeneral<T>(filterParams->MainMode);
					break;
			   default:
					filterIpp = new ippFilterGeneral<T>(filterParams->MainMode);
					break;
			   }
		   lRet = (filterIpp->PrepareFilter(roiRect, filterParams) == 0);
		   if (lRet)
			  {
			  lRet =  (filterIpp->ApplySingle(ippI1, ippI2) == 0);
			  if (lRet)
				 {
				 if (!buffer2)
					ippCommon::Copy(ippI2, ippI1, roiRect, roiRect, 0x00); // naštimaj parivrtu
				 }
			  }
		   delete ippI1;
		   delete ippI2;
		   return lRet;
		   }
  };
#endif
