//---------------------------------------------------------------------------
#ifndef ippFilterMedianH
#define ippFilterMedianH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterMedian : public ippFilterBasic<T>
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
  ippFilterMedian(int fType) : ippFilterBasic<T>(fType)
	  {
	  }
  // destructor
  virtual ~ippFilterMedian(void)
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
	  status = ippiFilterMedianBorderGetBufferSize(roiSize, maskSize, ipp8u, channels, &iTmpBufSize);
	  if (status)
		 return status;
	  pBuffer = ippsMalloc_8u(iTmpBufSize);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Median  border
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  // if for now...otherwise specialized template
	  //NOTE: median supports only replicate inmem and const border
	  if (channels == 3)
		 status = ippiFilterMedianBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, ippBorderRepl, borderValue, pBuffer);
	  else
		 status = ippiFilterMedianBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, ippBorderRepl, borderValue[0], pBuffer);
	  return status;
	  }
};
#endif


