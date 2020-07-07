//---------------------------------------------------------------------------
#ifndef ippFilterGaussianH
#define ippFilterGaussianH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterGaussian : public ippFilterBasic<T>
{
  using ippFilterBasic<T>::status;
  using ippFilterBasic<T>::roiSize;
  using ippFilterBasic<T>::maskSize;
  using ippFilterBasic<T>::channels;
  using ippFilterBasic<T>::pBuffer;
  using ippFilterBasic<T>::pSrc;
  using ippFilterBasic<T>::pDst;
  using ippFilterBasic<T>::srcStep;
  using ippFilterBasic<T>::dstStep;
  using ippFilterBasic<T>::borderType;
  using ippFilterBasic<T>::borderValue;
  using ippFilterBasic<T>::pSpec;

  protected:
      int radius;
	  Ipp32f stDev;

  public:
  // trivial constructor
  ippFilterGaussian(int fType) : ippFilterBasic<T>(fType)
	  {
   	  stDev = 1.35f;
	  }
  // destructor
  virtual ~ippFilterGaussian(void)
	  {
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilter(TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  IppiBorderType bType = borderType;    // stari border tip
	  ippFilterBasic<T>::PrepareFilter(srcRect, fParams);
	  // je li prethodni buffer dobar???
	  if (pBuffer
		 && pSpec
		 && roiSize.width == srcRect->Width()
		 && roiSize.height == srcRect->Height()
		 &&	radius == (fParams->Radius * 2 + 1)
		 && stDev == fParams->StDev
		 && borderType == bType)
			return status;
	  if (pBuffer)
		 ippsFree(pBuffer);
	  if (pSpec)
		 ippsFree(pSpec);
	  pBuffer = NULL;
	  pSpec = NULL;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  int iTmpBufSize = 0, iSpecSize = 0;
	  radius =  fParams->Radius * 2 + 1;
	  stDev = fParams->StDev;
	  if (stDev < (radius/10.0))
		 stDev = radius/10.0;
	  status =  ippiFilterGaussianGetBufferSize(roiSize, radius, ipp8u, channels, &iSpecSize, &iTmpBufSize);
	  if (status)
		 return status;
	  pSpec = ippsMalloc_8u(iSpecSize);
	  pBuffer = ippsMalloc_8u(iTmpBufSize);
	  status = ippiFilterGaussianInit(roiSize, radius, stDev, borderType, ipp8u, channels, (IppFilterGaussianSpec *)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Gaussian
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterGaussianBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderValue, (IppFilterGaussianSpec *)pSpec, pBuffer);
	  else
		 status = ippiFilterGaussianBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderValue[0], (IppFilterGaussianSpec *)pSpec, pBuffer);
	  return status;
	  }
};
#endif


