//---------------------------------------------------------------------------
#ifndef ippFilterCompoundH
#define ippFilterCompoundH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
#include "ippFilterMax.hpp"
#include "ippFilterMin.hpp"
#include "ippFilterMedian.hpp"
#include "ippFilterBlur.hpp"
#include "ippFilterMorph.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterCompound : public ippFilterBasic<T>
{
  using ippFilterBasic<T>::status;
  using ippFilterBasic<T>::roiSize;
  using ippFilterBasic<T>::pSrc;
  using ippFilterBasic<T>::pDst;
  using ippFilterBasic<T>::srcStep;
  using ippFilterBasic<T>::dstStep;

  private:
	  ippFilterMax<ippRGB> *fMax;
	  ippFilterMin<ippRGB> *fMin;
	  ippFilterMedian<ippRGB> *fMedian;
	  ippFilterBlur<ippRGB> *fBlur;
	  ippFilterMorph<ippRGB> *fMorph;
	  typedef ippFilterBasic<ippRGB>::Execute::CompoundFunc TCompounder;
	  TCompounder _compounder;
  // compounders
  int minsubmax(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  fMin->PrepareFilter(srcRect, &fParams);
	  nRet = fMin->ApplySingle(srcImage, work1);
	  if (!nRet)
		 {
		 fMax->PrepareFilter(srcRect, &fParams);
		 nRet = fMax->ApplySingle(srcImage, dstImage);
		 if (!nRet)
			{
			this->prepareFilterData(work1, dstImage);
			status = ippiSub_8u_C3IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
			}
		 }
	  delete work1;
	  return nRet;
	  }
  int minmaxcf(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  // copy source rect to destination
	  this->prepareFilterData(srcImage, dstImage);
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  //
	  fMin->PrepareFilter(srcRect, &fParams);
	  nRet = fMin->ApplySingle(srcImage, work1);
	  if (!nRet)
		 {
		 fMax->PrepareFilter(srcRect, &fParams);
		 nRet = fMax->ApplySingle(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  int medianminmaxcf(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  fMedian->PrepareFilter(srcRect, &fParams);
	  nRet = fMedian->ApplySingle(srcImage, dstImage);
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  TRect workRect;
	  workRect.init(0, 0, dstImage->Width(), dstImage->Height());
	  fMax->PrepareFilter(&workRect, &fParams);
	  nRet = fMax->ApplySingle(dstImage, work1);
	  if (!nRet)
		 {
		 fMin->PrepareFilter(&workRect, &fParams);
		 nRet = fMin->ApplySingle(dstImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  int blurminmaxcf(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  fBlur->PrepareFilter(srcRect, &fParams);
	  nRet = fBlur->ApplySingle(srcImage, dstImage);
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  TRect workRect;
	  workRect.init(0, 0, dstImage->Width(), dstImage->Height());
	  fMax->PrepareFilter(&workRect, &fParams);
	  nRet = fMax->ApplySingle(dstImage, work1);
	  if (!nRet)
		 {
		 fMin->PrepareFilter(&workRect, &fParams);
		 nRet = fMin->ApplySingle(dstImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  int blurmediancf(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  this->prepareFilterData(srcImage, dstImage);
	  // copy source rect to destination
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  fBlur->PrepareFilter(srcRect, &fParams);
	  nRet = fBlur->ApplySingle(srcImage, work1);
	  if (!nRet)
		 {
		 fMedian->PrepareFilter(srcRect, &fParams);
		 nRet = fMedian->ApplySingle(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  //---------------------------------------------------------------------------
  int erodedilatecf(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.KernelX = compParams->KernelX;
	  fParams.KernelY = compParams->KernelY;
	  fParams.MorphMask = 0;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  this->prepareFilterData(srcImage, dstImage);
	  // copy source rect to destination
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  fParams.MainMode = ipp_ERODE;
	  fMorph->PrepareFilter(srcRect, &fParams);
	  nRet = fMorph->ApplySingle(srcImage, work1);
	  if (!nRet)
		 {
		 fParams.MainMode = ipp_DILATE;
		 fMorph->PrepareFilter(srcRect, &fParams);
		 nRet = fMorph->ApplySingle(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  int medianerodedilatecf(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.KernelX = compParams->KernelX;
	  fParams.KernelY = compParams->KernelY;
	  fParams.RadiusX = fParams.KernelX;
	  fParams.RadiusY = fParams.KernelY;
	  fParams.MorphMask = 0;
	  fMedian->PrepareFilter(srcRect, &fParams);
	  nRet = fMedian->ApplySingle(srcImage, dstImage);
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  ippImage<ippRGB> *work2 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  fParams.MainMode = ipp_ERODE;
	  fMorph->PrepareFilter(srcRect, &fParams);
	  nRet = fMorph->ApplySingle(srcImage, work1);
	  if (!nRet)
		 {
		 fParams.MainMode = ipp_DILATE;
		 fMorph->PrepareFilter(srcRect, &fParams);
		 nRet = fMorph->ApplySingle(srcImage, work2);
		 if (!nRet)
			ippCommon::CompCloseFar(work1, work2, dstImage, compParams);
		 }
	  delete work1;
	  delete work2;
	  return nRet;
	  }
  int unsharpmask(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  int nRet;
	  fParams.RadiusX = compParams->RadiusX;
	  fParams.RadiusY = compParams->RadiusY;
	  ippImage<ippRGB> *work1 = new ippImage<ippRGB>(dstImage->Width(), dstImage->Height());
	  this->prepareFilterData(srcImage, dstImage);
	  // copy source rect to destination
	  nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
	  fBlur->PrepareFilter(srcRect, &fParams);
	  // blur source rect to work map
	  nRet = fBlur->ApplySingle(srcImage, work1);
	  if (!nRet)
		 {
		 this->prepareFilterData(work1, dstImage);
		 roiSize.width  = work1->Width();
		 roiSize.height = work1->Height();
		 // subtract work1 (blured image) from destination (copy of source rect)
		 ippiSub_8u_C3IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
		 roiSize.width  = srcRect->Width();
		 roiSize.height = srcRect->Height();
		 this->prepareFilterData(srcImage, work1);
		 // copy source rect to work map
		 nRet = ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
		 ippCommon::FactorAddition(work1, dstImage, dstImage, 1.0f, compParams->KFactor);
		 }
	  delete work1;
	  return nRet;
	  }
  public:
  // trivial constructor
  ippFilterCompound(int fType) : ippFilterBasic<T>(fType)
	  {
	  fMax = 0;
	  fMin = 0;
	  fMedian = 0;
	  fBlur = 0;
	  fMorph = 0;
	  }
  // destructor
  virtual ~ippFilterCompound(void)
	  {
	  if (fMax)
		 delete fMax;
	  if (fMin)
		 delete fMin;
	  if (fMedian)
		 delete fMedian;
	  if (fBlur)
		 delete fBlur;
	  if (fMorph)
		 delete fMorph;
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareCompound(TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  Tspe_FilterIppParams fParams;
	  fParams.BorderType = compParams->BorderType;
	  ippFilterBasic<ippRGB>::PrepareFilter(srcRect, &fParams);
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  switch (compParams->MainMode)
		 {
		 case ipp_CMP_MAX_SUB_MIN:
			  _compounder = &minsubmax;
		 case ipp_CMP_MINMAX_CF:
			  if (!fMax)
				 fMax = new ippFilterMax<ippRGB>(ipp_MAX);
			  if (!fMin)
				 fMin = new ippFilterMin<ippRGB>(ipp_MIN);
			  if (compParams->MainMode == ipp_CMP_MINMAX_CF)
				 _compounder = &minmaxcf;
			  break;
		 case ipp_CMP_MEDIAN_MINMAX_CF:
			  if (!fMedian)
				 fMedian = new ippFilterMedian<ippRGB>(ipp_MEDIAN);
			  if (!fMax)
				 fMax = new ippFilterMax<ippRGB>(ipp_MAX);
			  if (!fMin)
				 fMin = new ippFilterMin<ippRGB>(ipp_MIN);
			  _compounder = &medianminmaxcf;
			  break;
		 case ipp_CMP_BLUR_MINMAX_CF:
			  if (!fBlur)
				 fBlur = new ippFilterBlur<ippRGB>(ipp_BLUR);
			  if (!fMax)
				 fMax = new ippFilterMax<ippRGB>(ipp_MAX);
			  if (!fMin)
				 fMin = new ippFilterMin<ippRGB>(ipp_MIN);
			  _compounder = &blurminmaxcf;
			  break;
		 case ipp_CMP_BLUR_MEDIAN_CF:
			  if (!fBlur)
				 fBlur = new ippFilterBlur<ippRGB>(ipp_BLUR);
			  if (!fMedian)
				 fMedian = new ippFilterMedian<ippRGB>(ipp_MEDIAN);
			  _compounder = &blurmediancf;
			  break;
		 case ipp_CMP_ERODE_DILATE_CF:
			  if (!fMorph)
				 fMorph = new ippFilterMorph<ippRGB>(ipp_ERODE);
			  _compounder = &erodedilatecf;
			  break;
		 case ipp_CMP_MEDIAN_ERODE_DILATE_CF:
			  if (!fMorph)
				 fMorph = new ippFilterMorph<ippRGB>(ipp_ERODE);
			  if (!fMedian)
				 fMedian = new ippFilterMedian<ippRGB>(ipp_MEDIAN);
			  _compounder = &medianerodedilatecf;
			  break;
		 case ipp_CMP_UNSHARP_MASK:
			  if (!fBlur)
				 fBlur = new ippFilterBlur<ippRGB>(ipp_BLUR);
			  _compounder = &unsharpmask;
		 }
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Max - Min    sub(Min, Max)
  //---------------------------------------------------------------------------
  int ApplyCompound(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  return _compounder(srcImage, dstImage, srcRect, compParams);
	  }
};
#endif

