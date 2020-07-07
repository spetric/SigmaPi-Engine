//---------------------------------------------------------------------------
#ifndef ippGeometryH
#define ippGeometryH
#include <Types.hpp>
#include "ippImage.hpp"
#include "spGlobals.h"
//---------------------------------------------------------------------------
class ippGeometry
  {
  private:
	static Ipp8u* pSrc;
	static Ipp8u* pDst;
	static int srcStep;
	static int dstStep;
	static IppiSize roiSize;
	static double Quad[4][2];
	static IppiWarpDirection direction;
	static IppiBorderType borderType;
	static int specSize, initSize, bufSize;
	static IppiWarpSpec* pSpec;
	static Ipp8u* pBuffer;
	static IppiPoint dstOffset;
	static IppiSize srcSize, dstSize;
	static IppiRect srcRoi, dstRoi;
	static IppStatus status;
	static int nChans;
    static Ipp8u* pInitBuf;
	static IppiInterpolationType interpolation;
	template <class T>
	static void prepareData(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
		{
		nChans = sizeof(T);
		Quad[0][0] = (double)params.Quad[2].x;
		Quad[0][1] = (double)(srcImage->Height() - 1 - params.Quad[2].y);
		Quad[1][0] = (double)params.Quad[3].x;
		Quad[1][1] = (double)(srcImage->Height() - 1 - params.Quad[3].y);
		Quad[2][0] = (double)params.Quad[1].x;
		Quad[2][1] = (double)(srcImage->Height() - 1 - params.Quad[1].y);
		Quad[3][0] = (double)params.Quad[0].x;
		Quad[3][1] = (double)(srcImage->Height() - 1 - params.Quad[0].y);
		//
		pSrc = (unsigned char*)srcImage->Data();
		pDst = (unsigned char*)dstImage->Data();
		srcStep = srcImage->Stride();
		dstStep = dstImage->Stride();
		roiSize.width  = roiRect->Width();
		roiSize.height = roiRect->Height();
		// parivrta
		if (srcImage->Width() != roiRect->Width() || srcImage->Height() != roiRect->Height())
		   {
		   pSrc = pSrc + (roiRect->left * nChans + (srcImage->Height() - roiRect->bottom) * srcStep);
		   pDst = pDst + (roiRect->left * nChans + (dstImage->Height() - roiRect->bottom) * dstStep);
		   }
		direction  = params.Forward ? ippWarpForward : ippWarpBackward;
		//borderType  = ippBorderTransp;
		srcSize.width  = srcImage->Width();
		srcSize.height = srcImage->Height();
		dstSize.width  = dstImage->Width();
		dstSize.height = dstImage->Height();
		srcRoi.x = 0;
		srcRoi.y = 0;
		srcRoi.width  = roiRect->Width();
		srcRoi.height = roiRect->Height();
		dstRoi.x = 0;
		dstRoi.y = 0;
		dstRoi.width  = dstImage->Width();
		dstRoi.height = dstImage->Height();
		specSize = initSize = bufSize = 0;
		pSpec  = NULL;
		pBuffer = NULL;
		dstOffset.x  = 0;
		dstOffset.y  = 0;
		if (params.Interpolation == 0)
		   interpolation = ippNearest;
		else if (params.Interpolation == 1)
		   interpolation = ippLinear;
		else
		   interpolation = ippCubic;
		}
	static int perspectiveQ(void);
	static int bilinearQ(void);
	static int affineQ(void);
  public:
	//static int WarpPerspectiveQ(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params);
	template <class T>
	static int WarpPerspectiveQ(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
		{
		prepareData(srcImage, dstImage, roiRect, params);
		pDst = (unsigned char*)dstImage->Data();	// vrati ga nazad
		return perspectiveQ();
		}
	//static int WarpBilinearQ(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params);
	template <class T>
	static int WarpBilinearQ(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
		{
		prepareData(srcImage, dstImage, roiRect, params);
		pDst = (unsigned char*)dstImage->Data();	// vrati ga nazad
		return bilinearQ();
		}
	//static int WarpAffineQ(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params);
	template <class T>
	static int WarpAffineQ(ippImage<T> *srcImage, ippImage<T> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
		{
		prepareData(srcImage, dstImage, roiRect, params);
		pDst = (unsigned char*)dstImage->Data();	// vrati ga nazad
		return affineQ();
		}
  };
#endif
