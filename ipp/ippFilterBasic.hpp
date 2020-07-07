//---------------------------------------------------------------------------
#ifndef ippFilterBasicH
#define ippFilterBasicH
#include <Types.hpp>
#include "ippCommon.h"
//typedef int (__closure *TIppFilterPrepare)(TRect *srcRect, Tspe_FilterIppParams *fParams);
//typedef int (__closure *TIppCompundPrepare)(TRect *srcRect, Tspe_CompoundIppParams *compParams);
//---------------------------------------------------------------------------
template <class T> class ippFilterBasic
{
  protected:
  	  // protected variables (for all filters)
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
	  //IppiPoint anchor;
	  //Ipp32f noise;
	  //Ipp32f sigmaColor;
	  //Ipp32f sigmaSpace;
	  //Ipp32f stDev;
	  //int radius;
	  //IppiMaskSize fixedMask;
	  Ipp8u *pMask;
	  //
	  //int div;
	  //Ipp8u bias[3];
	  IppDataType dataType, kernelType;
	  // user def status
	  IppStatus udsBadFilterType, udsBadDstRoi, udsNotSupported;
  //---------------------------------------------------------------------------
  // prepare image data (pointer + step)
  //---------------------------------------------------------------------------
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
  //---------------------------------------------------------------------------
  // get border type
  //---------------------------------------------------------------------------
  IppiBorderType getBorderType(Tspe_FilterIppParams *fParams)
	  {
	  IppiBorderType bt;
	  if (fParams->BorderType == 0)
		  bt = ippBorderRepl;
	  else if (fParams->BorderType == 1)
		  bt = ippBorderMirror;
	  else
		  bt = (IppiBorderType)(ippBorderRepl | ippBorderMirror);
	  return bt;
	  }
  //---------------------------------------------------------------------------
  // adjust non-border filter
  //---------------------------------------------------------------------------
  bool adjustNonBorder(IppiSize &rs, int &anchorX, int &anchorY)
	 {
	 rs.width  = roiSize.width  -  maskSize.width;
	 rs.height = roiSize.height -  maskSize.height;
	 pSrc = (pSrc + anchorX * channels) + anchorY * srcStep;
	 pDst = (pDst + anchorX * channels) + anchorY * dstStep;
	 return (bool)(rs.width * rs.height);
	 }
  //---------------------------------------------------------------------------
  // additional preparation for filters group 1
  //---------------------------------------------------------------------------
  int addPrepFilterG1(TRect *srcRect, Tspe_FilterIppParams *fParams)
	 {
	  status = ippStsNoErr;
	  // is previous buffer good enough???
	  if (pBuffer
		  && roiSize.width == srcRect->Width()
		  && roiSize.height == srcRect->Height()
		  && maskSize.width  ==  (fParams->RadiusX*2 + 1)
		  && maskSize.height == (fParams->RadiusY*2 + 1))
			 return status;
	  if (pBuffer)
		 ippsFree(pBuffer);
	  pBuffer = NULL;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  maskSize.width  = fParams->RadiusX*2 + 1;
	  maskSize.height = fParams->RadiusY*2 + 1;
	  return status;
	 }
  //---------------------------------------------------------------------------
  // public section
  //---------------------------------------------------------------------------
  public:
	struct Execute
		{
		//typedef int (*TIppFilterPrepare)(TRect *srcRect, int &kernelX, int &kernelY);
		typedef int (__closure *FilterFunc)(ippImage<T>*, ippImage<T> *);
		typedef int (__closure *CompoundFunc)(ippImage<T>*, ippImage<T>*, TRect *srcRect, Tspe_CompoundIppParams *compParams);
		};
  // trivial constructor
  ippFilterBasic(int fType)
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
	  pSpec = NULL;
	  //fixedMask = ippMskSize3x3;
	  pMask = NULL;
	  //stDev = 1.35f;
	  //noise = 0.0f;
	  //sigmaColor = 1400.0f;
	  //sigmaSpace = 40.0f;
	  //anchor.x = 1;
	  //anchor.y = 1;
	  // this should be constants...but who cares
	  udsBadFilterType = 997;
	  udsBadDstRoi     = 998;
	  udsNotSupported  = 999;
	  filterType = fType;
	  }
  // destructor
  virtual ~ippFilterBasic(void)
	  {
	  if (pBuffer)
		 ippsFree(pBuffer);
	  if (pSpec)
		 ippsFree(pSpec);
	  if (pMask)
		 delete pMask;
	  }
  //---------------------------------------------------------------------------
  // filter preparation
  //---------------------------------------------------------------------------
  virtual int PrepareFilter(TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  borderType = getBorderType(fParams);
	  roiRect.left = srcRect->left;
	  roiRect.right = srcRect->right;
	  roiRect.top	= srcRect->top;
	  roiRect.bottom = srcRect->bottom;
	  status = ippStsNoErr;
	  return status;
	  }
  //---------------------------------------------------------------------------
  // apply filter single
  //---------------------------------------------------------------------------
  virtual int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  return ippStsNoErr;
	  }
  //---------------------------------------------------------------------------
  // compound preparation
  //---------------------------------------------------------------------------
  virtual int PrepareCompound(TRect *srcRect,  Tspe_CompoundIppParams *compParams)
	  {
	  status = ippStsNoErr;
	  return status;
	  }
  //---------------------------------------------------------------------------
  // apply filter compound
  //---------------------------------------------------------------------------
  virtual int ApplyCompound(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *srcRect, Tspe_CompoundIppParams *compParams)
	  {
	  return ippStsNoErr;
	  }
  __property int FilterType = {read = filterType};
  };
#endif
