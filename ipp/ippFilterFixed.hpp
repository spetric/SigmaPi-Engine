//---------------------------------------------------------------------------
#ifndef ippFilterFixedH
#define ippFilterFixedH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterFixed : public ippFilterBasic<T>
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
  using ippFilterBasic<T>::filterType;
  using ippFilterBasic<T>::udsBadDstRoi;
  using ippFilterBasic<T>::udsNotSupported;
  using ippFilterBasic<T>::udsBadFilterType;

  protected:
	  IppiMaskSize fixedMask;
  private:
	  //typedef ippFilterBasic<ippRGB>::Execute::FilterFunc TFilterer;
	  typedef typename ippFilterBasic<T>::Execute::FilterFunc TFilterer;
	  TFilterer _filterer;
  int mediancrossxx(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  IppiSize dstRoiSize;
	  int anchor = (fixedMask == ippMskSize3x3) ? 1 : 2;
	  if (!this->adjustNonBorder(dstRoiSize, anchor, anchor))
		 return udsBadDstRoi;
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiFilterMedianCross_8u_C3R(pSrc, srcStep, pDst, dstStep, dstRoiSize, fixedMask);
	  else
		 status = ippiFilterMedianCross_8u_C1R(pSrc, srcStep, pDst, dstStep, dstRoiSize, fixedMask);
	  return status;
	  }
  int mediancolorxx(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  IppiSize dstRoiSize;
	  int anchor = (fixedMask == ippMskSize3x3) ? 1 : 2;
	  if (!this->adjustNonBorder(dstRoiSize, anchor, anchor))
		 return udsBadDstRoi;
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiFilterMedianColor_8u_C3R(pSrc, srcStep, pDst, dstStep, dstRoiSize, fixedMask);
	  else
		 status = udsNotSupported;
	  return status;
	  }
  int hipassxx(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiFilterHipassBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterHipassBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  int laplacexx(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiFilterLaplaceBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterLaplaceBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  int sharpenxx(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiFilterSharpenBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterSharpenBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  public:
  // trivial constructor
  ippFilterFixed(int fType) : ippFilterBasic<T>(fType)
	  {
      fixedMask = ippMskSize3x3;
	  }
  // destructor
  virtual ~ippFilterFixed(void)
	  {
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilter(TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  IppiMaskSize oldMask = fixedMask;
	  ippFilterBasic<T>::PrepareFilter(srcRect, fParams);
	  fixedMask = fParams->Fixed33 ? ippMskSize3x3 : ippMskSize5x5;
	  // je li prethodni buffer dobar???
	  if (filterType != fParams->MainMode
		  && pBuffer
		  && roiSize.width == srcRect->Width()
		  && roiSize.height == srcRect->Height()
		  && fixedMask == oldMask)
			 return status;
	  if (pBuffer)
		 ippsFree(pBuffer);
	  pBuffer = NULL;
	  filterType = fParams->MainMode;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  if (fixedMask == ippMskSize3x3)
		  {
		  maskSize.width  = 3;
		  maskSize.height = 3;
		  }
	  else
		  {
		  maskSize.width  = 5;
		  maskSize.height = 5;
		  }
	  int iTmpBufSize = 0;
	  switch (filterType)
		 {
		 case ipp_MEDIAN_CROSS_XX:
			  _filterer = &mediancrossxx;
			  return status;
		 case ipp_MEDIAN_COLOR_XX:
			  _filterer = &mediancolorxx;
			  return status;
		 case ipp_HIPASS_XX:
			  status = ippiFilterHipassBorderGetBufferSize(roiSize, fixedMask, ipp8u, ipp8u, channels, &iTmpBufSize);
			  _filterer = &hipassxx;
			  break;
		 case ipp_LAPLACE_XX:
			  status = ippiFilterLaplaceBorderGetBufferSize(roiSize, fixedMask, ipp8u, ipp8u, channels, &iTmpBufSize);
			  _filterer = &laplacexx;
			  break;
		 case ipp_SHARPEN_XX:
			  status = ippiFilterSharpenBorderGetBufferSize(roiSize, fixedMask, ipp8u, ipp8u, channels, &iTmpBufSize);
			  _filterer = &sharpenxx;
			  break;
		 default:
			  status = udsBadFilterType;
			  break;
		 }
	  if (status)
		 return status;
	  pBuffer = ippsMalloc_8u(iTmpBufSize);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Fixed (box) border
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  return _filterer(srcImage, dstImage);
	  }
};
#endif

