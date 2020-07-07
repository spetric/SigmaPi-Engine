//---------------------------------------------------------------------------
#pragma hdrstop
#include "ippProcessing.h"
Ipp8u* ippProcess::pSrc = NULL;
Ipp8u* ippProcess::pDst = NULL;
int ippProcess::srcStep = 0;
int ippProcess::dstStep = 0;
IppiSize ippProcess::roiSize = {0, 0};
//
#pragma package(smart_init)
//---------------------------------------------------------------------------
// privatluk
/*
void ippProcess::prepareData(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect)
{
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
}
*/
//---------------------------------------------------------------------------
void ippProcess::GetTwister(float *matrix, int scale, Ipp32f twister[][4])
{
int i, k, m;
m = k = -1;
for (i = 0; i < 12; i++)
   {
   if (i % 4 == 0)
	  {
	  m++;
	  k = 0;
	  }
   twister[m][k++] = matrix[i] / (float)scale;
   }
}
//---------------------------------------------------------------------------
int ippProcess::ColorTwist(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, Ipp32f twister[][4], int bias)
{
prepareData(srcImage, dstImage, roiRect);
IppStatus status = ippiColorTwist32f_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, twister);
if (status)
   return status;
if (bias > 0)
   {
   Ipp8u value[3];
   value[0] = value[1] = value[2] = (Ipp8u)bias;
   status = ippiAddC_8u_C3IRSfs(value, pDst, dstStep, roiSize, 1);
   }
return status;
}
//---------------------------------------------------------------------------
int ippProcess::DistanceTransform(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int kernelSize, int norm, bool *chanOn)
{
prepareData(srcImage, dstImage, roiRect);
IppStatus status;
// we need 1 channel 8u temporaty image
int interStep;
Ipp8u *interImage = ippiMalloc_8u_C1(roiRect->Width(), roiRect->Height(), &interStep);
//Ipp32s pMetrics[2] = { 2, 2 };
Ipp32s pMetrics[3];
ippiGetDistanceTransformMask_32s(kernelSize, (IppiNorm)norm, pMetrics);
for (int i = 0; i < 3; i++)
	{
	if (!chanOn[i])
	   continue;
	ippiCopy_8u_C3C1R((Ipp8u*)(pSrc + i), srcStep, interImage, interStep, roiSize);	// copy forward BGR -> single channel
	status = ippiDistanceTransform_3x3_8u_C1IR(interImage, interStep, roiSize, pMetrics);
	if (status)
	   break;
	ippiCopy_8u_C1C3R(interImage, interStep, (Ipp8u*)(pDst + i), dstStep, roiSize);	// copy backward BGR -> single channel
	}
ippiFree(interImage);
return status;
}
//---------------------------------------------------------------------------
int ippProcess::FastMarching(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, float radius, bool *chanOn)
{
prepareData(srcImage, dstImage, roiRect);
IppStatus status;
// we need 1 channel 8u temporaty image
int interStepIn;
Ipp8u *interImageIn = ippiMalloc_8u_C1(roiRect->Width(), roiRect->Height(), &interStepIn);
// + 1 channel 32f temporaty image
int interStepOut;
Ipp32f *interImageOut = ippiMalloc_32f_C1(roiRect->Width(), roiRect->Height(), &interStepOut);
// buffer
int bufferSize;
status = ippiFastMarchingGetBufferSize_8u32f_C1R(roiSize, &bufferSize);
Ipp8u* pBuffer = ippsMalloc_8u(bufferSize);
for (int i = 0; i < 3; i++)
	{
	if (!chanOn[i])
	   continue;
	ippiCopy_8u_C3C1R((Ipp8u*)(pSrc + i), srcStep, interImageIn, interStepIn, roiSize);	// copy forward BGR -> single channel
	status = ippiFastMarching_8u32f_C1R(interImageIn, interStepIn, interImageOut, interStepOut,  roiSize, (Ipp32f) radius, pBuffer);
	if (status)
	   break;
	ippiConvert_32f8u_C1R(interImageOut, interStepOut, interImageIn, interStepIn, roiSize, ippRndNear);  // convert backward
	ippiCopy_8u_C1C3R(interImageIn, interStepIn, (Ipp8u*)(pDst + i), dstStep, roiSize);	// copy backward BGR -> single channel
	}
ippiFree(interImageIn);
ippiFree(interImageOut);
ippsFree(pBuffer);
return status;
}
//---------------------------------------------------------------------------
int ippProcess::GradientColor2Gray(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int norm, bool *chanOn)
{
prepareData(srcImage, dstImage, roiRect);
IppStatus status;
// we need 1 channel 8u temporaty image
int interStep;
Ipp8u *interImage = ippiMalloc_8u_C1(roiRect->Width(), roiRect->Height(), &interStep);
status = ippiGradientColorToGray_8u_C3C1R(pSrc, srcStep, interImage, interStep, roiSize, (IppiNorm) norm);
if (!status)
   {
	for (int i = 0; i < 3; i++)
		{
		if (!chanOn[i])
		   continue;
		ippiCopy_8u_C1C3R(interImage, interStep, (Ipp8u*)(pDst + i), dstStep, roiSize);	// copy backward BGR -> single channel
		}
   }
ippiFree(interImage);
return status;
}
//---------------------------------------------------------------------------
int ippProcess::ReduceBits(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int noise, int ditherType, int levels)
{
prepareData(srcImage, dstImage, roiRect);
IppStatus status;
int bufferSize;
status = ippiReduceBitsGetBufferSize(ippC3, roiSize, noise, (IppiDitherType)ditherType, &bufferSize);
Ipp8u* pBuffer = ippsMalloc_8u(bufferSize);
status = ippiReduceBits_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, noise, (IppiDitherType) ditherType, levels, pBuffer);
ippsFree(pBuffer);
return status;
}
//---------------------------------------------------------------------------
// geometric transforms
//---------------------------------------------------------------------------
template <>
int ippProcess::Mirror(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int axis)
{
prepareData(srcImage, dstImage, roiRect);
IppStatus status = ippiMirror_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, (IppiAxis) axis);
return status;
}
//---------------------------------------------------------------------------
template <>
int ippProcess::Mirror(ippImage<ippGray> *srcImage, ippImage<ippGray> *dstImage, TRect *roiRect, int axis)
{
prepareData(srcImage, dstImage, roiRect);
IppStatus status = ippiMirror_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize, (IppiAxis) axis);
return status;
}
//---------------------------------------------------------------------------
int ippProcess::Remap(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, ippImage<ippFloat> *xMap, ippImage<ippFloat> *yMap, TRect *roiRect)
{
prepareData(srcImage, dstImage, roiRect);
int xMapStep = xMap->Stride();
int yMapStep = yMap->Stride();
Ipp32f *pxMap  = (Ipp32f*)xMap->Data();
Ipp32f *pyMap  = (Ipp32f*)yMap->Data();
// parivrta
if (xMap->Width() != roiRect->Width() || xMap->Height() != roiRect->Height())
   {
   pxMap = pxMap + (roiRect->left  + (xMap->Height() - roiRect->bottom) * xMapStep);
   pyMap = pxMap + (roiRect->left  + (yMap->Height() - roiRect->bottom) * yMapStep);
   }
IppiSize srcSize;
srcSize.width  = srcImage->Width();
srcSize.height = srcImage->Height();
IppiRect srcRoi;
srcRoi.x = roiRect->left;
srcRoi.y = srcImage->Height() - roiRect->top;
srcRoi.width = roiRect->Width();
srcRoi.height = roiRect->Height();
int interpolation = 0;  // test
IppStatus status = ippiRemap_8u_C3R(pSrc, srcSize, srcStep, srcRoi, pxMap, xMapStep, pyMap, yMapStep, pDst, dstStep, roiSize, interpolation);
return status;
}
//---------------------------------------------------------------------------
int ippProcess::DeconvolveLR(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect, int kernelSize, float threshold, int numIter)
{
// calculate srcStep/dstStep/roiSize with respect to roiRect
prepareData(srcImage, dstImage, roiRect);
IppStatus status;
Ipp32f thr = threshold;
IppiSize maxRoi;
kernelSize = 3;
maxRoi.width  = roiSize.width  + kernelSize;
maxRoi.height = roiSize.height + kernelSize;
IppiDeconvLR_32f_C3R *pDeconvLR = 0;
int pSize;
status = ippiDeconvLRGetSize_32f(3, kernelSize, maxRoi, &pSize);
if (status)
   return status;
pDeconvLR = (IppiDeconvLR_32f_C3R *)ippsMalloc_32f(pSize);
Ipp32f *kernel = new Ipp32f[kernelSize*kernelSize * 3];
for (int i = 0; i < kernelSize*kernelSize*3; i++)
	kernel[i] = 1;
// test
/*
if (kernelSize == 3)
   {
   //{0,40,0,0,40,0,0,40,0};
   kernel[0] = 0;
   kernel[1] = 0;
   kernel[2] = 0;
   kernel[3] = 40;
   kernel[4] = 40;
   kernel[5] = 40;
   kernel[6] = 0;
   kernel[7] = 0;
   kernel[8] = 0;
   kernel[9] = 0;
   kernel[10] = 0;
   kernel[11] = 0;
   }
*/
status = ippiDeconvLRInit_32f_C3R(pDeconvLR, kernel, kernelSize, maxRoi, thr);
if (status)
   {
   delete []kernel;
   ippsFree(pDeconvLR);
   return status;
   }
int interStepIn, interStepOut;
Ipp32f *interImageIn  = ippiMalloc_32f_C3(maxRoi.width, maxRoi.height, &interStepIn);
Ipp32f *interImageOut = ippiMalloc_32f_C3(roiSize.width, roiSize.height, &interStepOut);
//
status = ippiConvert_8u32f_C3R(pSrc, srcStep, interImageIn, interStepIn, roiSize);          // convert forward
//status = ippiScale_8u32f_C3R(pSrc, srcStep, interImageIn, interStepIn, roiSize, 0.0, 1.0);    // scale forward
status = ippiDeconvLR_32f_C3R(interImageIn, interStepIn, interImageOut, interStepOut, roiSize, numIter, pDeconvLR);
if (!status)
   ippiConvert_32f8u_C3R(interImageOut, interStepOut, pDst, dstStep, roiSize, ippRndNear);  // convert backward
   //ippiScale_32f8u_C3R(interImageOut, interStepOut, pDst, dstStep, roiSize, 0.0, 1.0);    // scale backward
ippsFree(pDeconvLR);
ippiFree(interImageIn);
ippiFree(interImageOut);
delete []kernel;
return status;
}
//---------------------------------------------------------------------------
/* to statistics
int ippProcess::histogramInit(TRect *roiRect, int nBins)
{
// privatluk
HistogramRelease();	// release previous buffers
IppStatus status;
int nLevels[] = {nBins + 1, nBins + 1, nBins + 1};
Ipp32f lowerLevel[] = {0, 0, 0};
Ipp32f upperLevel[] = {255, 255, 255};
int histObjSize, bufferSize;
// set public vars
histRoiSize.width  = roiRect->Width();
histRoiSize.height = roiRect->Height();
// set local vars
pHistVec = new Ipp32u*[3];
pHistVec[0] = new Ipp32u[nBins];
pHistVec[1] = new Ipp32u[nBins];
pHistVec[2] = new Ipp32u[nBins];
// get sizes for spec and buffer
status = ippiHistogramGetBufferSize(ipp8u, histRoiSize, nLevels, 3, 1, &histObjSize, &bufferSize);
pHistObj = (IppiHistogramSpec*)ippsMalloc_8u( histObjSize);
pHistBuffer = (Ipp8u*)ippsMalloc_8u(bufferSize);
// initialize spec
ippiHistogramUniformInit(ipp8u, lowerLevel, upperLevel, nLevels, 3, pHistObj );
return status;
}
//---------------------------------------------------------------------------
void ippProcess::HistogramRelease(void)
{
if (pHistVec)
   {
   delete []pHistVec[0];
   delete []pHistVec[1];
   delete []pHistVec[2];
   delete []pHistVec;
   }
if (pHistObj)
	ippsFree(pHistObj);
if (pHistBuffer)
   ippsFree(pHistBuffer);
pHistVec = NULL;
pHistObj = NULL;
pHistBuffer = NULL;
}
//---------------------------------------------------------------------------
unsigned int** ippProcess::Histogram(ippImage<ippRGB> *srcImage, TRect *roiRect, int nBins)
{
histogramInit(roiRect, nBins);  // inicijaliziraj ga
IppStatus status;
// calculate histogram
Ipp8u *pSrc = (Ipp8u*)(srcImage->Data());
// parivrta
if (srcImage->Width() != roiRect->Width() || srcImage->Height() != roiRect->Height())
   pSrc = pSrc + (roiRect->left * 3 + (srcImage->Height() - roiRect->bottom) * srcStep);
int srcStep = srcImage->Stride();
status = ippiHistogram_8u_C3R(pSrc, srcStep, histRoiSize, pHistVec, pHistObj, pHistBuffer);
if (status)
   return 0;
return pHistVec;
}
*/


