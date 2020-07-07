//---------------------------------------------------------------------------
#pragma hdrstop
#include "ippGeometry.h"
Ipp8u* ippGeometry::pSrc = NULL;
Ipp8u* ippGeometry::pDst = NULL;
int ippGeometry::srcStep = 0;
int ippGeometry::dstStep = 0;
IppiSize ippGeometry::roiSize = {0, 0};
double ippGeometry::Quad[4][2] = {0};
IppiWarpDirection ippGeometry::direction = ippWarpForward ;
IppiBorderType ippGeometry::borderType = ippBorderTransp;
int ippGeometry::specSize = 0;
int ippGeometry::initSize = 0;
int ippGeometry::bufSize = 0;
IppiWarpSpec* ippGeometry::pSpec = NULL;
Ipp8u* ippGeometry::pBuffer = NULL;
IppiPoint ippGeometry::dstOffset = {0, 0};
IppiSize ippGeometry::srcSize = {0, 0};
IppiSize ippGeometry::dstSize = {0, 0};
IppiRect ippGeometry::srcRoi = {0};
IppiRect ippGeometry::dstRoi = {0};
IppStatus ippGeometry::status = ippStsNoErr;
int ippGeometry::nChans = 3;
Ipp8u* ippGeometry::pInitBuf = NULL;
IppiInterpolationType ippGeometry::interpolation = 	ippNearest;
//TODO: clean the house and addd options for grayscale (alpha)
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------
/*
void ippGeometry::prepareData(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
{
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
   pSrc = pSrc + (roiRect->left * 3 + (srcImage->Height() - roiRect->bottom) * srcStep);
   pDst = pDst + (roiRect->left * 3 + (dstImage->Height() - roiRect->bottom) * dstStep);
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
} */
//---------------------------------------------------------------------------
int ippGeometry::perspectiveQ(void)
{
double coeffs[3][3];
Ipp64f valB = 0.0, valC = 0.5;
status = ippiGetPerspectiveTransform(srcRoi, Quad, (double(*)[3])coeffs);
if (status != 0)
   return status;
status = ippiWarpPerspectiveGetSize(srcSize, srcRoi, dstSize, ipp8u, coeffs, interpolation, direction,  borderType, &specSize, &initSize);
if (status != 0)
   return status;
// spec
pSpec   = (IppiWarpSpec*)ippsMalloc_8u(specSize);
if (interpolation == ippNearest)
   status = ippiWarpPerspectiveNearestInit(srcSize, srcRoi, dstSize, ipp8u, coeffs, direction, nChans, borderType, NULL, 0, pSpec);
else if (interpolation == ippLinear)
   status = ippiWarpPerspectiveLinearInit(srcSize, srcRoi, dstSize, ipp8u, coeffs, direction, nChans, borderType, NULL, 0, pSpec);
else
   {
   pInitBuf = ippsMalloc_8u(initSize);
   status = ippiWarpPerspectiveCubicInit(srcSize, srcRoi, dstSize, ipp8u, coeffs, direction, nChans, valB, valC, borderType, NULL, 0, pSpec, pInitBuf);
   }
if (status != 0)
   return status;
status = ippiWarpGetBufferSize(pSpec, dstSize, &bufSize);
if (status != 0)
   return status;
// buffer
pBuffer  = ippsMalloc_8u(bufSize);
// perform transform
if (nChans == 3)
   {
   if (interpolation == ippNearest)
	  status = ippiWarpPerspectiveNearest_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else if (interpolation == ippLinear)
	  status = ippiWarpPerspectiveLinear_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else
	  status = ippiWarpPerspectiveCubic_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   }
else
   {
   if (interpolation == ippNearest)
	  status = ippiWarpPerspectiveNearest_8u_C1R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else if (interpolation == ippLinear)
	  status = ippiWarpPerspectiveLinear_8u_C1R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else
	  status = ippiWarpPerspectiveCubic_8u_C1R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   }
ippsFree(pSpec);
ippsFree(pBuffer);
if (pInitBuf)
   {
   ippsFree(pInitBuf);
   pInitBuf = NULL;
   }
return status;
}
//---------------------------------------------------------------------------
int ippGeometry::bilinearQ(void)
{
double coeffs[2][4];
//
status = ippiGetBilinearTransform(srcRoi, Quad, (double(*)[4])coeffs);
if (status != 0)
   return status;
status = ippiWarpBilinearGetBufferSize(srcSize, srcRoi, dstRoi, direction, coeffs, interpolation /*IPPI_INTER_NN*/, &bufSize);
if (status != 0)
   return status;
// buffer
pBuffer  = ippsMalloc_8u(bufSize);
// perform transform
if (nChans == 3)
   status = ippiWarpBilinear_8u_C3R(pSrc, srcSize, srcStep, srcRoi, pDst, dstStep, dstRoi, coeffs, interpolation, pBuffer);
else
   status = ippiWarpBilinear_8u_C1R(pSrc, srcSize, srcStep, srcRoi, pDst, dstStep, dstRoi, coeffs, interpolation, pBuffer);
ippsFree(pBuffer);
return status;
}
//---------------------------------------------------------------------------
int ippGeometry::affineQ(void)
{
double coeffs[2][3];
status = ippiGetAffineTransform(srcRoi, Quad, (double(*)[3])coeffs);
status = ippiWarpAffineGetSize(srcSize, dstSize, ipp8u, coeffs, interpolation, direction, borderType, &specSize, &initSize);
if (status != 0)
   return status;
// spec
pSpec   = (IppiWarpSpec*)ippsMalloc_8u(specSize);
status = ippiWarpAffineNearestInit(srcSize, dstSize, ipp8u, coeffs, direction, nChans, borderType, NULL, 0, pSpec);
if (status != 0)
   return status;
status = ippiWarpGetBufferSize(pSpec, dstSize, &bufSize);
if (status != 0)
   return status;
// buffer
pBuffer  = ippsMalloc_8u(bufSize);
// perform transform
if (nChans == 3)
   {
   if (interpolation == ippNearest)
	  status = ippiWarpAffineNearest_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else if (interpolation == ippLinear)
	  status = ippiWarpAffineLinear_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else
	  status = ippiWarpAffineNearest_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   }
else
   {
   if (interpolation == ippNearest)
	  status = ippiWarpAffineNearest_8u_C1R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else if (interpolation == ippLinear)
	  status = ippiWarpAffineLinear_8u_C1R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   else
	  status = ippiWarpAffineNearest_8u_C1R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
   }
ippsFree(pSpec);
ippsFree(pBuffer);
return status;
}
//---------------------------------------------------------------------------
/*
int ippGeometry::WarpPerspectiveQ(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
{
prepareData(srcImage, dstImage, roiRect, params);
pDst = (unsigned char*)dstImage->Data();	// vrati ga nazad
double coeffs[3][3];
int nChans = 3;
status = ippiGetPerspectiveTransform(srcRoi, Quad, (double(*)[3])coeffs);
if (status != 0)
   return status;
status = ippiWarpPerspectiveGetSize(srcSize, srcRoi, dstSize, ipp8u, coeffs, ippNearest, direction,  borderType, &specSize, &initSize);
if (status != 0)
   return status;
// spec
pSpec   = (IppiWarpSpec*)ippsMalloc_8u(specSize);
status = ippiWarpPerspectiveNearestInit(srcSize, srcRoi, dstSize, ipp8u, coeffs, direction, nChans, borderType, NULL, 0, pSpec);
if (status != 0)
   return status;
status = ippiWarpGetBufferSize(pSpec, dstSize, &bufSize);
if (status != 0)
   return status;
// buffer
pBuffer  = ippsMalloc_8u(bufSize);
// perform transform
status = ippiWarpPerspectiveNearest_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
ippsFree(pSpec);
ippsFree(pBuffer);
return status;
} */
//---------------------------------------------------------------------------
/*
int ippGeometry::WarpBilinearQ(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
{
prepareData(srcImage, dstImage, roiRect, params);
pDst = (unsigned char*)dstImage->Data();	// vrati ga nazad
double coeffs[2][4];
//
status = ippiGetBilinearTransform(srcRoi, Quad, (double(*)[4])coeffs);
if (status != 0)
   return status;
status = ippiWarpBilinearGetBufferSize(srcSize, srcRoi, dstRoi, direction, coeffs, IPPI_INTER_NN, &bufSize);
if (status != 0)
   return status;
// buffer
pBuffer  = ippsMalloc_8u(bufSize);
// perform transform
status = ippiWarpBilinear_8u_C3R(pSrc, srcSize, srcStep, srcRoi, pDst, dstStep, dstRoi, coeffs, IPPI_INTER_NN, pBuffer);
ippsFree(pBuffer);
return status;
} */
//---------------------------------------------------------------------------
/*
int ippGeometry::WarpAffineQ(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Tspe_QuadIppParams &params)
{
prepareData(srcImage, dstImage, roiRect, params);
pDst = (unsigned char*)dstImage->Data();	// vrati ga nazad
double coeffs[2][3];
status = ippiGetAffineTransform(srcRoi, Quad, (double(*)[3])coeffs);
status = ippiWarpAffineGetSize(srcSize, dstSize, ipp8u, coeffs, interpolation, direction, borderType, &specSize, &initSize);
if (status != 0)
   return status;
// spec
pSpec   = (IppiWarpSpec*)ippsMalloc_8u(specSize);
status = ippiWarpAffineNearestInit(srcSize, dstSize, ipp8u, coeffs, direction, 3, borderType, NULL, 0, pSpec);
if (status != 0)
   return status;
status = ippiWarpGetBufferSize(pSpec, dstSize, &bufSize);
if (status != 0)
   return status;
// buffer
pBuffer  = ippsMalloc_8u(bufSize);
// perform transform
status = ippiWarpAffineNearest_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);
ippsFree(pSpec);
ippsFree(pBuffer);
return status;
} */



