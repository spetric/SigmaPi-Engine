//---------------------------------------------------------------------------
#ifndef ippFilterMorphH
#define ippFilterMorphH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterMorph : public ippFilterBasic<T>
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
  using ippFilterBasic<T>::filterType;
  using ippFilterBasic<T>::borderType;
  using ippFilterBasic<T>::borderValue;
  using ippFilterBasic<T>::pSpec;
  using ippFilterBasic<T>::pMask;
  using ippFilterBasic<T>::udsBadFilterType;


  private:
	  //typedef ippFilterBasic<ippRGB>::Execute::FilterFunc TFilterer;
	  typedef typename ippFilterBasic<T>::Execute::FilterFunc TFilterer;
	  TFilterer _filterer;
  int dilate(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiDilateBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphState*)pSpec, pBuffer);
	  else
		 status = ippiDilateBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphState*)pSpec, pBuffer);
	  return status;
	  }
  int erode(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiErodeBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphState*)pSpec, pBuffer);
	  else
		 status = ippiErodeBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphState*)pSpec, pBuffer);
	  return status;
	  }
  int close(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiMorphCloseBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphCloseBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  int open(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiMorphOpenBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphOpenBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  int tophat(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiMorphTophatBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphTophatBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  int gradient(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiMorphGradientBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphGradientBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  public:
  // trivial constructor
  ippFilterMorph(int fType) : ippFilterBasic<T>(fType)
	  {
	  }
  // destructor
  virtual ~ippFilterMorph(void)
	  {
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilter(TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  ippFilterBasic<T>::PrepareFilter(srcRect, fParams);
	  // is previous buffer good enough???
	  if (filterType == fParams->MainMode
		  && pBuffer
		  && pSpec
		  && pMask
		  && maskSize.width == fParams->KernelX && maskSize.height == fParams->KernelY
		  && roiSize.width == srcRect->Width() && roiSize.height == srcRect->Height())
		 {
		 return status;
		 }
	  if (pBuffer)
		 ippsFree(pBuffer);
	  if (pSpec)
		 ippsFree(pSpec);
	  if (pMask)
		 delete pMask;
	  pBuffer = NULL;
	  filterType = fParams->MainMode;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  maskSize.width  = fParams->KernelX;
	  maskSize.height = fParams->KernelY;
	  // test <- rade i parni i neparni
	  /*
	  maskSize.width  = 2;
	  maskSize.height = 2;
	  */
	  //
	  int mSize = maskSize.width * maskSize.height;
	  pMask = new Ipp8u[mSize];
	  if (fParams->MorphMask)
		 {
		 //TODO: get external mask value
		 }
	  else
		 {
		 // default mask: 0 in the middle
		 for (int i = 0; i < mSize; i++)
		 	 pMask[i] = 1;
		 pMask[mSize>>1] = 0;
		 }
	  int iTmpBufSize = 0, iSpecSize = 0;
	  switch (filterType)
		 {
		 case ipp_DILATE:
			  _filterer = &dilate;
		 case ipp_ERODE:
			  if (channels == 3)
				 {
				 status =  ippiMorphologyBorderGetSize_8u_C3R(roiSize, maskSize, &iSpecSize, &iTmpBufSize);
				 if (status)
					break;
				 pSpec = ippsMalloc_8u(iSpecSize);
				 pBuffer = ippsMalloc_8u(iTmpBufSize);
				 status = ippiMorphologyBorderInit_8u_C3R(roiSize, pMask, maskSize, (IppiMorphState*)pSpec, pBuffer);
				 }
			  else
				 {
				 status =  ippiMorphologyBorderGetSize_8u_C1R(roiSize, maskSize, &iSpecSize, &iTmpBufSize);
				 if (status)
					break;
				 pSpec = ippsMalloc_8u(iSpecSize);
				 pBuffer = ippsMalloc_8u(iTmpBufSize);
				 status = ippiMorphologyBorderInit_8u_C1R(roiSize, pMask, maskSize, (IppiMorphState*)pSpec, pBuffer );
				 }
			  if (filterType == ipp_ERODE)
				 _filterer = &erode;
			  return status;
		 case ipp_CLOSE:
		 case ipp_OPEN:
		 case ipp_MORPH_TOPHAT:
		 case ipp_MORPH_GRADIENT:
			  if (channels == 3)
				 {
				 status =  ippiMorphAdvGetSize_8u_C3R(roiSize, maskSize, &iSpecSize, &iTmpBufSize);
				 if (status)
					break;
				 pSpec = ippsMalloc_8u(iSpecSize);
				 pBuffer = ippsMalloc_8u(iTmpBufSize);
				 status = ippiMorphAdvInit_8u_C3R(roiSize, pMask, maskSize, (IppiMorphAdvState*)pSpec, pBuffer);
				 }
			  else
				 {
				 status =  ippiMorphAdvGetSize_8u_C1R(roiSize, maskSize, &iSpecSize, &iTmpBufSize);
				 if (status)
					break;
				 pSpec = ippsMalloc_8u(iSpecSize);
				 pBuffer = ippsMalloc_8u(iTmpBufSize);
				 status = ippiMorphAdvInit_8u_C1R(roiSize, pMask, maskSize, (IppiMorphAdvState*)pSpec, pBuffer);
				 }
			  if (filterType == ipp_CLOSE)
				 _filterer = &close;
			  else if (filterType == ipp_OPEN)
				 _filterer = &open;
			  else if (filterType == ipp_MORPH_TOPHAT)
				 _filterer = &tophat;
			  else
				 _filterer = &gradient;
			  return status;
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
  // Morph
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  return _filterer(srcImage, dstImage);
	  }
};
#endif

