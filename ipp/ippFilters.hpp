//---------------------------------------------------------------------------
#ifndef ippFiltersH
#define ippFiltersH
#include <Types.hpp>
//#include "spPixelTypes.hpp"
//#include "ippImage.hpp"
#include "ippCommon.h"
//TODO: kreiraj baznu klasu i onda naslijedi za pojedine filtere!!!!
//typedef int (__closure *TIppFilterPrepare)(int type, TRect *srcRect, int &kernelX, int &kernelY);
//typedef int (__closure *TIppFilterPrepare)(int type, TRect *srcRect, Tspe_FilterIppParams *fParams);
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
const Ipp16s sharpen_33[3*3] = {-1, -1, -1
								-1, 16, -1,
								-1, -1, -1};	// div = 8
const Ipp16s sharpen_55[5*5] = {-1, -3, -4, -3, -1,
								-3, 0, 6, 0, -3,
								-4, 6, 40, 6, -4,
								-3, 0, 6, 0, -3,
								-1, -3, -4, -3, -1 };   // div = 20;
//---------------------------------------------------------------------------
template <class T> class ippFilter
{
  private:
	  IppStatus status;
	  IppiSize roiSize;
	  IppiSize maskSize;
	  Ipp8u *pBuffer;
	  IppiBorderType borderType;
	  Ipp8u borderValue[3];
	  int channels;
	  int channelsSize;
	  int filterType;
	  Ipp8u* pSrc;
	  Ipp8u* pDst;
	  int srcStep;
	  int dstStep;
	  Ipp8u *pSpec;
	  TRect roiRect;
	  IppiPoint anchor;
	  Ipp32f noise;
	  Ipp32f sigmaColor;
	  Ipp32f sigmaSpace;
	  Ipp32f stDev;
	  int radius;
	  IppiMaskSize fixedMask;
	  Ipp8u *pMask;
	  //
	  int div;
	  Ipp8u bias[3];
	  IppDataType dataType, kernelType;
	  // user def status
	  IppStatus udsBadFilterType, udsBadDstRoi, udsNotSupported;
  // prepare image data (pointer + step)
  void prepareFilterData(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  pSrc  = (unsigned char*)srcImage->Data();
	  pDst  = (unsigned char*)dstImage->Data();
	  srcStep = srcImage->Stride();
	  dstStep = dstImage->Stride();
	  // parivrta
	  if (srcImage->Width() != roiRect.Width() || srcImage->Height() != roiRect.Height())
		 //pSrc = pSrc + (roiRect.left * channels + (srcImage->Height() - 1 - roiRect.bottom) * srcStep);
		 pSrc = pSrc + (roiRect.left * channels + (srcImage->Height() - roiRect.bottom) * srcStep);
	  }
  // get border type
  IppiBorderType getBorderType(Tspe_FilterIppParams *fParams)
	  {
	  IppiBorderType bt;
	  if (fParams->BorderType == 0)
		  bt = ippBorderRepl;
	  else if (fParams->BorderType == 1)
		  bt = ippBorderMirror;
	  else
		  bt = ippBorderRepl | ippBorderMirror;
	  return bt;
	  }
  // adjust non-border filter
  bool adjustNonBorder(IppiSize &rs, int &anchorX, int &anchorY)
	 {
	 rs.width  = roiSize.width  -  maskSize.width;
	 rs.height = roiSize.height -  maskSize.height;
	 pSrc = (pSrc + anchorX * channels) + anchorY * srcStep;
	 pDst = (pDst + anchorX * channels) + anchorY * dstStep;
	 return (bool)(rs.width * rs.height);
	 }
  //
  public:
	struct Execute
		{
		//typedef int (*TIppFilterPrepare)(TRect *srcRect, int &kernelX, int &kernelY);
		typedef int (__closure *FilterFunc)(ippImage<T>*, ippImage<T> *);
		typedef int (__closure *CompoundFunc)(ippImage<T>*, ippImage<T>*, TRect *srcRect, Tspe_CompoundIppParams *compParams);
		};
  // trivial constructor
  ippFilter(void)
	  {
	  status = ippStsNoErr;
	  pBuffer = NULL;
	  roiSize.width = 0;
	  roiSize.height = 0;
	  maskSize.width = 0;
	  maskSize.height = 0;
	  borderType = ippBorderRepl;
	  borderValue[0] = 0xff;
	  borderValue[1] = 0xff;
	  borderValue[2] = 0xff;
	  // todo
	  channels = sizeof(T);
	  channelsSize = 1;
	  filterType = -1;
	  pSpec = NULL;
	  fixedMask = ippMskSize3x3;
	  pMask = NULL;
	  stDev = 1.35f;
	  noise = 0.0f;
	  sigmaColor = 1400.0f;
	  sigmaSpace = 40.0f;
	  anchor.x = 1;
	  anchor.y = 1;
	  // trabale bi biti konstante, ali ko æe sad to
	  udsBadFilterType = 997;
	  udsBadDstRoi     = 998;
	  udsNotSupported  = 999;
	  }
  // destructor
  virtual ~ippFilter(void)
	  {
	  if (pBuffer)
		 ippsFree(pBuffer);
	  if (pSpec)
		 ippsFree(pSpec);
	  if (pMask)
		 delete pMask;
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilterCommon(int type, TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  borderType = getBorderType(fParams);
	  roiRect.left = srcRect->left;
	  roiRect.right = srcRect->right;
	  roiRect.top	= srcRect->top;
	  roiRect.bottom = srcRect->bottom;
	  status = ippStsNoErr;
	  // je li prethodni buffer dobar???
	  if (filterType == type
		  && pBuffer
		  && roiSize.width == srcRect->Width()
		  && roiSize.height == srcRect->Height()
		  && maskSize.width  ==  (fParams->RadiusX*2 + 1)
		  && maskSize.height == (fParams->RadiusY*2 + 1))
			 return status;
	  if (pBuffer)
		 ippsFree(pBuffer);
	  pBuffer = NULL;
	  filterType = type;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  maskSize.width  = fParams->RadiusX*2 + 1;
	  maskSize.height = fParams->RadiusY*2 + 1;
	  // test - parni ne rade za median
	  /*
	  maskSize.width  = 4;
	  maskSize.height = 4;
	  */
	  int iTmpBufSize = 0;
	  switch (type)
		 {
		 case ipp_MEDIAN:
			  status = ippiFilterMedianBorderGetBufferSize(roiSize, maskSize, ipp8u, channels, &iTmpBufSize);
			  break;
		 case ipp_BLUR:
			  status = ippiFilterBoxBorderGetBufferSize(roiSize, maskSize, ipp8u, channels, &iTmpBufSize);
			  break;
		 case ipp_MAX:
			  status = ippiFilterMaxBorderGetBufferSize(roiSize, maskSize, ipp8u, channels, &iTmpBufSize);
			  break;
		 case ipp_MIN:
			  status = ippiFilterMinBorderGetBufferSize(roiSize, maskSize, ipp8u, channels, &iTmpBufSize);
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
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilterFixed(int type, TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  roiRect.left = srcRect->left;
	  roiRect.right = srcRect->right;
	  roiRect.top	= srcRect->top;
	  roiRect.bottom = srcRect->bottom;
	  status = ippStsNoErr;
	  IppiMaskSize oldMask = fixedMask;
	  fixedMask = fParams->Fixed33 ? ippMskSize3x3 : ippMskSize5x5;
	  // je li prethodni buffer dobar???
	  if (filterType == type
		  && pBuffer
		  && roiSize.width == srcRect->Width()
		  && roiSize.height == srcRect->Height()
		  && fixedMask == oldMask)
			 return status;
	  if (pBuffer)
		 ippsFree(pBuffer);
	  pBuffer = NULL;
	  filterType = type;
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
	  switch (type)
		 {
		 case ipp_MEDIAN_CROSS_XX:
		 case ipp_MEDIAN_COLOR_XX:
			  return status;
		 case ipp_HIPASS_XX:
			  status = ippiFilterHipassBorderGetBufferSize(roiSize, fixedMask, ipp8u, ipp8u, channels, &iTmpBufSize);
			  break;
		 case ipp_LAPLACE_XX:
			  status = ippiFilterLaplaceBorderGetBufferSize(roiSize, fixedMask, ipp8u, ipp8u, channels, &iTmpBufSize);
			  break;
		 case ipp_SHARPEN_XX:
			  status = ippiFilterSharpenBorderGetBufferSize(roiSize, fixedMask, ipp8u, ipp8u, channels, &iTmpBufSize);
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
  // filter preparation (specifc)
  //---------------------------------------------------------------------------
  int PrepareFilterSpec(int type, TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  // ove dvije linije su radi gaussiana, jer zahtijeva bordertype
	  IppiBorderType bType = borderType;    // stari border tip
	  borderType = getBorderType(fParams);  // novi border
	  roiRect.left = srcRect->left;
	  roiRect.right = srcRect->right;
	  roiRect.top	= srcRect->top;
	  roiRect.bottom = srcRect->bottom;
	  status = ippStsNoErr;
	  // je li prethodni buffer dobar???
	  // nije, fali mu za ostale
	  if (filterType == type && pBuffer && pSpec && roiSize.width == srcRect->Width() && roiSize.height == srcRect->Height())
		 {
		 if (type == ipp_BILATERAL)
			{
			if (sigmaColor == fParams->SigmaColor && sigmaSpace == fParams->SigmaSpace)
				return status;
			}
		 else if (type == ipp_GAUSSIAN)
			{
			if (radius == (fParams->Radius * 2 + 1) && stDev == fParams->StDev && borderType == bType)
			   return status;
			}
		 else if (type ==ipp_WIENER)
			{
			if (maskSize.width == fParams->KernelX
				&& maskSize.height == fParams->KernelY
				&& anchor.x ==  fParams->AnchorX
				&& anchor.y ==  fParams->AnchorY
				&& noise == fParams->NoiseLevel)
				  return status;
			}
		 else
			{
			//TODO: general predefined filters
			}
		 }
	  if (pBuffer)
		 ippsFree(pBuffer);
	  if (pSpec)
		 ippsFree(pSpec);
	  pBuffer = NULL;
	  pSpec = NULL;
	  filterType = type;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  int iTmpBufSize = 0, iSpecSize = 0;
	  bias[0] = bias[1] = bias[2] =  0;
	  switch (type)
		 {
		 case ipp_BILATERAL:
			  radius =  fParams->Radius;
			  sigmaColor = fParams->SigmaColor;
			  sigmaSpace = fParams->SigmaSpace;
			  status = ippiFilterBilateralBorderGetBufferSize(ippiFilterBilateralGauss, roiSize, radius, ipp8u, channels, ippDistNormL1, &iSpecSize, &iTmpBufSize);
			  if (status)
				 break;
			  pSpec = ippsMalloc_8u(iSpecSize);
			  if (status)
				 break;
			  status = ippiFilterBilateralBorderInit(ippiFilterBilateralGauss, roiSize, radius, ipp8u, channels, ippDistNormL1, sigmaColor, sigmaSpace, (IppiFilterBilateralSpec *)pSpec);
			  if (status)
				 break;
			  break;
		 case ipp_GAUSSIAN:
			  radius =  fParams->Radius * 2 + 1;
			  stDev = fParams->StDev;
			  if (stDev < (radius/10.0))
			     stDev = radius/10.0;
			  status =  ippiFilterGaussianGetBufferSize(roiSize, radius, ipp8u, channels, &iSpecSize, &iTmpBufSize);
			  if (status)
				 break;
			  pSpec = ippsMalloc_8u(iSpecSize);
			  pBuffer = ippsMalloc_8u(iTmpBufSize);
			  status = ippiFilterGaussianInit(roiSize, radius, stDev, borderType, ipp8u, channels, (IppFilterGaussianSpec *)pSpec, pBuffer);
			  return status;
		 case ipp_WIENER:
			  noise = fParams->NoiseLevel;
			  maskSize.width  = fParams->KernelX;
			  maskSize.height = fParams->KernelY;
			  anchor.x = fParams->AnchorX;
			  anchor.y = fParams->AnchorY;
			  status = ippiFilterWienerGetBufferSize(roiSize, maskSize, channels, &iTmpBufSize);
			  break;
		 case ipp_PREWITT_VERT:
		 case ipp_PREWITT_HOR:
		 case ipp_SOBEL_VERT:
		 case ipp_SOBEL_HOR:
		 case ipp_SHARPEN_XX:	// 5x5 only, 3x3 -> fixed
			  {
			  div = 1;
			  maskSize.width  = fParams->Fixed33 ? 3 : 5;
			  maskSize.height = maskSize.width;
			  status = ippiFilterBorderGetSize (maskSize, roiSize, ipp8u, ipp16s, channels, &iSpecSize, &iTmpBufSize);
			  if (status)
				 break;
			  pSpec = ippsMalloc_8u(iSpecSize);
			  pBuffer = ippsMalloc_8u(iTmpBufSize);
			  Ipp16s *kernel;
			  if (type == ipp_PREWITT_VERT)
				 kernel = fParams->Fixed33 ? (Ipp16s*)prewitt_33V : (Ipp16s*)prewitt_55V;
			  else if (type == ipp_PREWITT_HOR)
				 kernel = fParams->Fixed33 ? (Ipp16s*)prewitt_33H : (Ipp16s*)prewitt_55H;
			  else if (type == ipp_SOBEL_VERT)
				 kernel = fParams->Fixed33 ? (Ipp16s*)sobel_33V : (Ipp16s*)sobel_55V;
			  else if (type == ipp_SOBEL_HOR)
				 kernel = fParams->Fixed33 ? (Ipp16s*)sobel_33H : (Ipp16s*)sobel_55H;
			  else if (type == ipp_SHARPEN_XX)
				 {
				 maskSize.width  =  5;
				 maskSize.height = maskSize.width;
				 div = 20;
				 kernel = (Ipp16s*)sharpen_55;
				 }
			  status = ippiFilterBorderInit_16s(kernel, maskSize, div, ipp8u, channels, ippRndNear, (FilterBorderSpec*)pSpec);
			  }
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
  // filter preparation (morph)
  //---------------------------------------------------------------------------
  int PrepareFilterMorph(int type, TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  borderType = getBorderType(fParams);
	  roiRect.left = srcRect->left;
	  roiRect.right = srcRect->right;
	  roiRect.top	= srcRect->top;
	  roiRect.bottom = srcRect->bottom;
	  status = ippStsNoErr;
	  // je li prethodni buffer dobar???
	  if (filterType == type
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
	  filterType = type;
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
	  switch (type)
		 {
		 case ipp_DILATE:
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
				 status =  ippiMorphologyBorderGetSize_8u_C1R(roiSize, maskSize, &iSpecSize, &iTmpBufSize);
				 }
			  return status;
			  break;
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
			  return status;
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
  // filter preparation (specifc)
  //---------------------------------------------------------------------------
  int PrepareFilterGeneral(int type, TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  roiRect.left = srcRect->left;
	  roiRect.right = srcRect->right;
	  roiRect.top	= srcRect->top;
	  roiRect.bottom = srcRect->bottom;
	  status = ippStsNoErr;
	  // je li prethodni buffer dobar???
	  /* ko æe to sad ispitivat (ako je kernel razlièit)
	  if (pBuffer && pSpec && roiSize.width == srcRect->Width() && roiSize.height == srcRect->Height())
		 {
		 return status;
		 }
	  */
	  if (pBuffer)
		 ippsFree(pBuffer);
	  if (pSpec)
		 ippsFree(pSpec);
	  pBuffer = NULL;
	  pSpec = NULL;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  int iTmpBufSize = 0, iSpecSize = 0;
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
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Median border
  //---------------------------------------------------------------------------
  int MedianBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  //NOTE: median podržava samo replicate inmem i const border
	  if (channels == 3)
		 status = ippiFilterMedianBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, ippBorderRepl, borderValue, pBuffer);
	  else
		 status = ippiFilterMedianBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, ippBorderRepl, borderValue[0], pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Blur (box) border
  //---------------------------------------------------------------------------
  int BlurBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterBoxBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterBoxBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Bilateral border
  //---------------------------------------------------------------------------
  int BilateralBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterBilateralBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiFilterBilateralSpec *)pSpec, pBuffer);
	  else
		 status = ippiFilterBilateralBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiFilterBilateralSpec *)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Gaussian border
  //---------------------------------------------------------------------------
  int GaussianBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterGaussianBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderValue, (IppFilterGaussianSpec *)pSpec, pBuffer);
	  else
		 status = ippiFilterGaussianBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderValue[0], (IppFilterGaussianSpec *)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Max border
  //---------------------------------------------------------------------------
  int MaxBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterMaxBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterMaxBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Min border
  //---------------------------------------------------------------------------
  int MinBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterMinBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterMinBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Median cross 3x3 | 5x5
  //---------------------------------------------------------------------------
  int MedianCrossXX(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  IppiSize dstRoiSize;
	  int anchor = (fixedMask == ippMskSize3x3) ? 1 : 2;
	  if (!adjustNonBorder(dstRoiSize, anchor, anchor))
		 return udsBadDstRoi;
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterMedianCross_8u_C3R(pSrc, srcStep, pDst, dstStep, dstRoiSize, fixedMask);
	  else
		 status = ippiFilterMedianCross_8u_C1R(pSrc, srcStep, pDst, dstStep, dstRoiSize, fixedMask);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Median color 3x3 | 5x5
  //---------------------------------------------------------------------------
  int MedianColorXX(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  IppiSize dstRoiSize;
	  int anchor = (fixedMask == ippMskSize3x3) ? 1 : 2;
	  if (!adjustNonBorder(dstRoiSize, anchor, anchor))
		 return udsBadDstRoi;
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterMedianColor_8u_C3R(pSrc, srcStep, pDst, dstStep, dstRoiSize, fixedMask);
	  else
		 status = udsNotSupported;
	  return status;
	  }
  //---------------------------------------------------------------------------
  // High-pass 3x3 | 5x5
  //---------------------------------------------------------------------------
  int HiPassBorderXX(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterHipassBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterHipassBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Laplacian 3x3 | 5x5
  //---------------------------------------------------------------------------
  int LaplaceBorderXX(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterLaplaceBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterLaplaceBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Laplacian 3x3 | 5x5 <- njega nema, pa možemo na general filter varijantu
  //---------------------------------------------------------------------------
  int SharpenBorder33(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiFilterSharpenBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue, pBuffer);
	  else
		 status = ippiFilterSharpenBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, fixedMask, borderType, borderValue[0], pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Wiener filter -malo je u kurcu
  //---------------------------------------------------------------------------
  int Wiener(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  Ipp32f tnoise[1] = { 0.0 }; // inaèe puca...vrlo opskurno
	  tnoise[0] = noise;
	  IppiSize dstRoiSize;
	  if (!adjustNonBorder(dstRoiSize, anchor.x, anchor.y))
		 return udsBadDstRoi;
	  // zajebano do jaja...treba ga maknuti
	  /*
	  dstRoiSize.width  = roiSize.width  -  maskSize.width;
	  dstRoiSize.height = roiSize.height -  maskSize.height;
	  pSrc = (pSrc + anchor.x * channels) + anchor.y * srcStep;
	  pDst = (pDst + anchor.x * channels) + anchor.y * dstStep;
	  */
	  if (channels == 3)
		 status = ippiFilterWiener_8u_C3R(pSrc, srcStep, pDst, dstStep, dstRoiSize, maskSize, anchor, tnoise, pBuffer);
	  else
		 status = ippiFilterWiener_8u_C1R(pSrc, srcStep, pDst, dstStep, dstRoiSize, maskSize, anchor, tnoise, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Dilate
  //---------------------------------------------------------------------------
  int DilateBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiDilateBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphState*)pSpec, pBuffer);
	  else
		 status = ippiDilateBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphState*)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Erode
  //---------------------------------------------------------------------------
  int ErodeBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiErodeBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphState*)pSpec, pBuffer);
	  else
		 status = ippiErodeBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphState*)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Close
  //---------------------------------------------------------------------------
  int CloseBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiMorphCloseBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphCloseBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Open
  //---------------------------------------------------------------------------
  int OpenBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiMorphOpenBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphOpenBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Morphological top-hat
  //---------------------------------------------------------------------------
  int MorphTopHatBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiMorphTophatBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphTophatBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Morphological top-hat
  //---------------------------------------------------------------------------
  int MorphGradientBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
	  if (channels == 3)
		 status = ippiMorphGradientBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue, (IppiMorphAdvState*)pSpec, pBuffer);
	  else
		 status = ippiMorphGradientBorder_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, borderType, borderValue[0], (IppiMorphAdvState*)pSpec, pBuffer);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // General linear filter
  //---------------------------------------------------------------------------
  int GeneralBorder(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  prepareFilterData(srcImage, dstImage);
	  // za sada na if, a inaèe specijaliziraj template
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
  //TODO: canny edge, etc...
  //---------------------------------------------------------------------------
  //===========================================================================
  // Compound filters block
  //===========================================================================
  //---------------------------------------------------------------------------
  // Max - Min    sub(Min, Max)
  //---------------------------------------------------------------------------
  int CompMaxSubMin(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  int nRet;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  PrepareFilterCommon(ipp_MIN, srcRect, &fParams);
	  nRet = MinBorder(srcImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterCommon(ipp_MAX, srcRect, &fParams);
		 nRet = MaxBorder(srcImage, dstImage);
		 if (!nRet)
			{
			prepareFilterData(work1, dstImage);
			ippiSub_8u_C3IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
			}
		 }
	  delete work1;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  // src -> $1 Max -> $2 Min -> $3  cf($1, $2, $3) -> $1
  //---------------------------------------------------------------------------
  int CompMinMaxCF(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  int nRet;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  PrepareFilterCommon(ipp_MIN, srcRect, &fParams);
	  prepareFilterData(srcImage, dstImage);
	  // copy source rect to destination
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  //
	  nRet = MinBorder(srcImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterCommon(ipp_MAX, srcRect, &fParams);
		 nRet = MaxBorder(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  // median -> $1 Max -> $2 Min -> $3  cf($1, $2, $3) -> $1
  //---------------------------------------------------------------------------
  int CompMedianMinMaxCF(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  int nRet;
	  PrepareFilterCommon(ipp_MEDIAN, srcRect, &fParams);
	  nRet = MedianBorder(srcImage, dstImage);
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  TRect workRect;
	  workRect.init(0, 0, dstImage->Width(), dstImage->Height());
	  PrepareFilterCommon(ipp_MAX, &workRect, &fParams);
	  nRet = MaxBorder(dstImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterCommon(ipp_MIN, &workRect, &fParams);
		 nRet = MinBorder(dstImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  // blur -> $1 Max -> $2 Min -> $3  cf($1, $2, $3) -> $1
  //---------------------------------------------------------------------------
  int CompBlurMinMaxCF(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  int nRet;
	  PrepareFilterCommon(ipp_BLUR, srcRect, &fParams);
	  nRet = BluerBorder(srcImage, dstImage);
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  TRect workRect;
	  workRect.init(0, 0, dstImage->Width(), dstImage->Height());
	  PrepareFilterCommon(ipp_MAX, &workRect, &fParams);
	  nRet = MaxBorder(dstImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterCommon(ipp_MIN, &workRect, &fParams);
		 nRet = MinBorder(dstImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, dstImage, compParams, true);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  // blur -> $1 Max -> $2 Min -> $3  cf($1, $2, $3) -> $1
  //---------------------------------------------------------------------------
  int CompBlurMedianCF(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  int nRet;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  PrepareFilterCommon(ipp_BLUR, srcRect, &fParams);
	  prepareFilterData(srcImage, dstImage);
	  // copy source rect to destination
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  nRet = BlurBorder(srcImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterCommon(ipp_MEDIAN, srcRect, &fParams);
		 nRet = MedianBorder(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  // erode -> $1 dilate -> $2 cf($1, $2, $3) -> $1
  //---------------------------------------------------------------------------
  int CompErodeDilateCF(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.KernelX = compParams->KernelX;
	  fParams.KernelY = compParams->KernelY;
	  fParams.MorphMask = 0;
	  int nRet;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  PrepareFilterMorph(ipp_ERODE, srcRect, &fParams);
	  prepareFilterData(srcImage, dstImage);
	  // copy source rect to destination
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  nRet = ErodeBorder(srcImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterMorph(ipp_DILATE, srcRect, &fParams);
		 nRet = DilateBorder(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  // erode -> $1 dilate -> $2 cf($1, $2, $3) -> $1
  //---------------------------------------------------------------------------
  int CompMedianErodeDilateCF(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.KernelX = compParams->KernelX;
	  fParams.KernelY = compParams->KernelY;
	  fParams.RadiusX = fParams.KernelX;
	  fParams.RadiusY = fParams.KernelY;
	  fParams.MorphMask = 0;
	  int nRet;
	  PrepareFilterCommon(ipp_MEDIAN, srcRect, &fParams);
	  nRet = MedianBorder(srcImage, dstImage);
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  PrepareFilterMorph(ipp_ERODE, srcRect, &fParams);
	  nRet = ErodeBorder(srcImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterMorph(ipp_DILATE, srcRect, &fParams);
		 nRet = DilateBorder(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  // erode -> $1 dilate -> $2 cf($1, $2, $3) -> $1
  //---------------------------------------------------------------------------
  int CompBlurErodeDilateCF(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.KernelX = compParams->KernelX;
	  fParams.KernelY = compParams->KernelY;
	  fParams.RadiusX = fParams.KernelX;
	  fParams.RadiusY = fParams.KernelY;
	  fParams.MorphMask = 0;
	  int nRet;
	  PrepareFilterCommon(ipp_BLUR, srcRect, &fParams);
	  nRet = BlurBorder(srcImage, dstImage);
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  PrepareFilterMorph(ipp_ERODE, srcRect, &fParams);
	  nRet = ErodeBorder(srcImage, work1);
	  if (!nRet)
		 {
		 PrepareFilterMorph(ipp_DILATE, srcRect, &fParams);
		 nRet = DilateBorder(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
   //---------------------------------------------------------------------------
  //
  //---------------------------------------------------------------------------
  int CompUnsharpMask(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  int nRet;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  PrepareFilterCommon(ipp_BLUR, srcRect, &fParams);
	  prepareFilterData(srcImage, dstImage);
	  // copy source rect to destination
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  // blur source rect to work map
	  nRet = BlurBorder(srcImage, work1);
	  if (!nRet)
		 {
		 prepareFilterData(work1, dstImage);
		 roiSize.width  = work1->Width();
		 roiSize.height = work1->Height();
		 // subtract work1 (blured image) from destination (copy of source rect)
		 ippiSub_8u_C3IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
		 roiSize.width  = srcRect->Width();
		 roiSize.height = srcRect->Height();
		 prepareFilterData(srcImage, work1);
		 // copy source rect to work map
		 nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
		 ippCommon::FactorAddition(work1, dstImage, dstImage, 1.0f, compParams->KFactor);
		 }
	  delete work1;
	  return nRet;
	  }
  };
#endif
