//---------------------------------------------------------------------------
#ifndef ippFilterBilateralH
#define ippFilterBilateralH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterBilateral : public ippFilterBasic<T>
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
	  Ipp32f sigmaColor;
	  Ipp32f sigmaSpace;
	  int radius;

  public:
  // trivial constructor
  ippFilterBilateral(int fType) : ippFilterBasic<T>(fType)
	  {
	  sigmaColor = 1400.0f;
	  sigmaSpace = 40.0f;
	  }
  // destructor
  virtual ~ippFilterBilateral(void)
	  {
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilter(TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  ippFilterBasic<T>::PrepareFilter(srcRect, fParams);
	  // je li prethodni buffer dobar???
	  if (pBuffer
		  && pSpec
		  && roiSize.width == srcRect->Width()
		  && roiSize.height == srcRect->Height()
		  && radius == fParams->Radius
		  && sigmaColor == fParams->SigmaColor
		  && sigmaSpace == fParams->SigmaSpace)
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
	  //bias[0] = bias[1] = bias[2] =  0;
	  radius =  fParams->Radius;
	  sigmaColor = fParams->SigmaColor;
	  sigmaSpace = fParams->SigmaSpace;
	  //TODO: sredi  distnorm (ippDistNormL1) kao parametar
	  status = ippiFilterBilateralBorderGetBufferSize(ippiFilterBilateralGauss, roiSize, radius, ipp8u, channels, ippDistNormL1, &iSpecSize, &iTmpBufSize);
	  if (status)
		 return status;
	  pSpec = ippsMalloc_8u(iSpecSize);
	  if (status)
		 return status;
	  status = ippiFilterBilateralBorderInit(ippiFilterBilateralGauss, roiSize, radius, ipp8u, channels, ippDistNormL1, sigmaColor, sigmaSpace, (IppiFilterBilateralSpec *)pSpec);
	  if (status)
		 return status;
	  pBuffer = ippsMalloc_8u(iTmpBufSize);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Bilateral
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterBilateralBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiFilterBilateralSpec *)pSpec, pBuffer);
	  else
		 status = ippiFilterBilateralBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiFilterBilateralSpec *)pSpec, pBuffer);
	  return status;
	  }
};
#endif


