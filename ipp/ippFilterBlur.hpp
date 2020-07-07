//---------------------------------------------------------------------------
#ifndef ippFilterBlurH
#define ippFilterBlurH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterBlur : public ippFilterBasic<T>
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
  public:
  // trivial constructor
  ippFilterBlur(int fType) : ippFilterBasic<T>(fType)
	  {
	  }
  // destructor
  virtual ~ippFilterBlur(void)
	  {
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilter(TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  ippFilterBasic<T>::PrepareFilter(srcRect, fParams);
	  ippFilterBasic<T>::addPrepFilterG1(srcRect, fParams);
	  int iTmpBufSize = 0;
	  status = ippiFilterBoxBorderGetBufferSize(roiSize, maskSize, ipp8u, channels, &iTmpBufSize);
	  if (status)
		 return status;
	  pBuffer = ippsMalloc_8u(iTmpBufSize);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Blur (box) border
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterBoxBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterBoxBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue, pBuffer);
	  return status;
	  }
};
#endif

