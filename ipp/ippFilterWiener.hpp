//---------------------------------------------------------------------------
#ifndef ippFilterWienerH
#define ippFilterWienerH
#include <Types.hpp>
#include "ippFilterBasic.hpp"
//---------------------------------------------------------------------------
template <class T> class ippFilterWiener : public ippFilterBasic<T>
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
  using ippFilterBasic<T>::udsBadDstRoi;

  protected:
	  IppiPoint anchor;
	  Ipp32f noise;
  public:
  // trivial constructor
  ippFilterWiener(int fType) : ippFilterBasic<T>(fType)
	  {
	  anchor.x = 1;
	  anchor.y = 1;
      noise = 0.0f;
	  }
  // destructor
  virtual ~ippFilterWiener(void)
	  {
	  }
  //---------------------------------------------------------------------------
  // filter preparation (common)
  //---------------------------------------------------------------------------
  int PrepareFilter(TRect *srcRect, Tspe_FilterIppParams *fParams)
	  {
	  ippFilterBasic<T>::PrepareFilter(srcRect, fParams);
	  // is previous buffer good enough???
	  if (pBuffer
		 && roiSize.width == srcRect->Width()
		 && roiSize.height == srcRect->Height()
		 && maskSize.height == fParams->KernelY
		 && anchor.x ==  fParams->AnchorX
		 && anchor.y ==  fParams->AnchorY
		 && noise == fParams->NoiseLevel)
			return status;
	  if (pBuffer)
		 ippsFree(pBuffer);
	  pBuffer = NULL;
	  roiSize.width  = srcRect->Width();
	  roiSize.height = srcRect->Height();
	  int iTmpBufSize = 0, iSpecSize = 0;
	  noise = fParams->NoiseLevel;
	  if (noise == 1.0f)
	     noise -= 0.01f;
	  maskSize.width  = fParams->KernelX;
	  maskSize.height = fParams->KernelY;
	  anchor.x = fParams->AnchorX;
	  anchor.y = fParams->AnchorY;
	  status = ippiFilterWienerGetBufferSize(roiSize, maskSize, channels, &iTmpBufSize);
	  if (status)
		 return status;
	  if (status)
		 return status;
	  pBuffer = ippsMalloc_8u(iTmpBufSize);
	  return status;
	  }
  //---------------------------------------------------------------------------
  // Wiener
  //---------------------------------------------------------------------------
  int ApplySingle(ippImage<T> *srcImage, ippImage<T> *dstImage)
	  {
	  this->prepareFilterData(srcImage, dstImage);
	  Ipp32f tnoise[1] = { 0.0 }; // inaèe puca...vrlo opskurno
	  tnoise[0] = noise;
	  IppiSize dstRoiSize;
	  if (!this->adjustNonBorder(dstRoiSize, anchor.x, anchor.y))
		 return udsBadDstRoi;
	  // nahhh...
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
};
#endif


