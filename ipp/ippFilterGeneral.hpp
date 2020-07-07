//---------------------------------------------------------------------------
#ifndef ippFilterGeneralH
#define ippFilterGeneralH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
const Ipp16s prewitt_33V[3*3] = { -1, 0, 1,
								  -1, 0, 1,
								  -1, 0, 1 };
const Ipp16s prewitt_33H[3*3] = { 1, 1, 1,
								  0, 0, 0,
								 -1, -1, -1 };
const Ipp16s prewitt_55V[5*5] = {-2, -1, 0, 1, 2,
								  -2, -1, 0, 1, 2,
								  -2, -1, 0, 1, 2,
								  -2, -1, 0, 1, 2,
								  -2, -1, 0, 1, 2 };
const Ipp16s prewitt_55H[5*5] = {2, 2, 2, 2, 2,
								  1, 1, 1, 1, 1,
								  0, 0, 0, 0, 0,
								  -1, -1, -1, -1, -1,
								  -2, -2, -2, -2, -2 };
const Ipp16s sobel_33V[3*3]  = { 1, 0, -1,
								 2, 0, -2,
								 1, 0, -1 };
const Ipp16s sobel_33H[3*3]  = { 1, 2, 1,
								 0, 0, 0,
								-1, -2, -1 };
const Ipp16s sobel_55V[5*5] = {1, 2,  0, -2, -1,
								4, 8,  0, -8, -4,
								6, 12, 0, -12, -6,
								4, 8,  0, -8, -4,
								1, 2,  0, -2, -1 };

const Ipp16s sobel_55H[5*5] = {1, 4, 6, 4, 1,
								2, 8, 12, 8, 1,
								0, 0, 0, 0, 0,
							   -2, -8, -12, -8, -1,
							   -1, -4, -6, -6, -1 };
const Ipp16s sharpen_33[3*3] = {-1, -1, -1,
								-1, 16, -1,
								-1, -1, -1};	// div = 8
const Ipp16s sharpen_55[5*5] = {-1, -3, -4, -3, -1,
								-3, 0, 6, 0, -3,
								-4, 6, 40, 6, -4,
								-3, 0, 6, 0, -3,
								-1, -3, -4, -3, -1 };   // div = 20;
const Ipp16s gauss_33[3*3]   = {1, 2, 1,
								2, 4, 2,
								1, 2, 1,}; 		//div = 16
const Ipp16s gauss_55[5*5]   = {2, 7, 12, 7, 2,
								7, 31, 52, 31, 7,
								12, 52, 127, 52, 12,
								7, 31, 52, 31, 7,
								2, 7, 12, 7, 2};   // div = 571
//---------------------------------------------------------------------------
template <class T> class ippFilterGeneral : public ippFilterBasic<T>
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
  using ippFilterBasic<T>::pSpec;

  protected:
	 IppiMaskSize fixedMask;
	 int div;
	 Ipp8u bias[3];
  public:
  // trivial constructor
  ippFilterGeneral(int fType) : ippFilterBasic<T>(fType)
	  {
      fixedMask = ippMskSize3x3;
	  }
  // destructor
  virtual ~ippFilterGeneral(void)
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
	  // je li prethodni buffer dobar??? Samo za predefinirane
	  if (filterType != fParams->MainMode
		  && filterType != ipp_GENERAL_FILTER
		  && pBuffer
		  && roiSize.width == srcRect->Width()
		  && roiSize.height == srcRect->Height()
		  && fixedMask == oldMask)
			 return status;
	  if (pBuffer)
		 ippsFree(pBuffer);
	  if (pSpec)
		 ippsFree(pSpec);
	  pBuffer = NULL;
	  pSpec = NULL;
	  filterType = fParams->MainMode;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  int iTmpBufSize = 0, iSpecSize = 0;
	  switch (filterType)
		 {
		 case ipp_PREWITT_VERT:
		 case ipp_PREWITT_HOR:
		 case ipp_SOBEL_VERT:
		 case ipp_SOBEL_HOR:
		 case ipp_SHARPEN_XX:	// 5x5 only, 3x3 -> fixed
		 case ipp_GAUSS_XX:
			  {
			  bias[0] = bias[1] = bias[2] =  0;
			  div = 1;
			  maskSize.width  = fParams->Fixed33 ? 3 : 5;
			  maskSize.height = maskSize.width;
			  status = ippiFilterBorderGetSize (maskSize, roiSize, ipp8u, ipp16s, channels, &iSpecSize, &iTmpBufSize);
			  if (status)
				 break;
			  pSpec = ippsMalloc_8u(iSpecSize);
			  pBuffer = ippsMalloc_8u(iTmpBufSize);
			  Ipp16s *kernel;
			  if (filterType == ipp_PREWITT_VERT)
				 kernel = fParams->Fixed33 ? (Ipp16s*)prewitt_33V : (Ipp16s*)prewitt_55V;
			  else if (filterType == ipp_PREWITT_HOR)
				 kernel = fParams->Fixed33 ? (Ipp16s*)prewitt_33H : (Ipp16s*)prewitt_55H;
			  else if (filterType == ipp_SOBEL_VERT)
				 kernel = fParams->Fixed33 ? (Ipp16s*)sobel_33V : (Ipp16s*)sobel_55V;
			  else if (filterType == ipp_SOBEL_HOR)
				 kernel = fParams->Fixed33 ? (Ipp16s*)sobel_33H : (Ipp16s*)sobel_55H;
			  else if (filterType == ipp_SHARPEN_XX)
				 {
				 maskSize.width  =  5;
				 maskSize.height = maskSize.width;
				 div = 20;
				 kernel = (Ipp16s*)sharpen_55;
				 }
			  else if (filterType == ipp_GAUSS_XX)
				 {
				 kernel = fParams->Fixed33 ? (Ipp16s*)gauss_33 : (Ipp16s*)gauss_55;
				 div    = fParams->Fixed33 ? 16 : 527;
				 }
			  status = ippiFilterBorderInit_16s(kernel, maskSize, div, ipp8u, channels, ippRndNear, (FilterBorderSpec*)pSpec);
			  }
			  break;
		 case ipp_GENERAL_FILTER:
			  {
			  div = fParams->GeneralDiv;
			  bias[0] = bias[1] = bias[2] =  fParams->GeneralBias;
			  maskSize.width  = fParams->GeneralRows;
			  maskSize.height = fParams->GeneralColumns;
			  status = ippiFilterBorderGetSize (maskSize, roiSize, ipp8u, ipp16s, channels, &iSpecSize, &iTmpBufSize);
			  if (status)
				 return status;
			  pSpec = ippsMalloc_8u(iSpecSize);
			  pBuffer = ippsMalloc_8u(iTmpBufSize);
			  Ipp16s *kernel = fParams->GeneralKernel;
			  status = ippiFilterBorderInit_16s(kernel, maskSize, div, ipp8u, channels, ippRndNear, (FilterBorderSpec*)pSpec);
              }
			  break;
		 default:
			  status = udsBadFilterType;
			  break;
		 }
	  return status;
	  }
  //---------------------------------------------------------------------------
  // General (box) border filter
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  // if for now...otherwise specialized template
	  if (channels == 3)
		 status = ippiFilterBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (FilterBorderSpec*)pSpec, pBuffer);
	  else
		 status = ippiFilterBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (FilterBorderSpec*)pSpec, pBuffer);
	  if (!status && bias[0] > 0)
		 {
		 status = ippiAddC_8u_C3IRSfs(bias, pDst, dstStep, roiSize, 1);
		 }
	  return status;
	  }
  //---------------------------------------------------------------------------
  // TODO: convolution
  //---------------------------------------------------------------------------
  //
};
#endif

