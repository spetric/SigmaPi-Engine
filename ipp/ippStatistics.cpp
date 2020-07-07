//---------------------------------------------------------------------------
#pragma hdrstop
#include "ippStatistics.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
IppiMomentState_64f* ippStatistic::pMomentState = NULL;
Ipp8u* ippStatistic::pSrc = NULL;
Ipp8u* ippStatistic::pDst = NULL;
int ippStatistic::srcStep = 0;
int ippStatistic::dstStep = 0;
int ippStatistic::nChans = 3;
IppiSize ippStatistic::roiSize = {0, 0};
Ipp64f ippStatistic::normInf[4] = {0, 0, 0, 0};
Ipp64f ippStatistic::normL1[4] = {0, 0, 0, 0};
Ipp64f ippStatistic::normL2[4] = {0, 0, 0, 0};
ippImage<ippRGB>* ippStatistic::imgColor = 0;
ippImage<ippGray>* ippStatistic::imgGray = 0;
bool ippStatistic::statImagesOwned = false;
//---------------------------------------------------------------------------
// rgb only
//---------------------------------------------------------------------------
int ippStatistic::Histogram(ippImage<ippRGB> *img, TRect *roi, int nBins, unsigned int** histo)
{
prepareData(img, img, roi);
IppStatus sts;
int nLevels[] = { nBins, nBins, nBins};
Ipp32f lowerLevel[] = {0, 0, 0};
Ipp32f upperLevel[] = {255, 255, 255};
Ipp32f **pLevels = new Ipp32f*[3];
pLevels[0] = new Ipp32f[nBins];
pLevels[1] = new Ipp32f[nBins];
pLevels[2] = new Ipp32f[nBins];
int sizeHistObj, sizeBuffer;
IppiHistogramSpec* pHistObj;
Ipp8u* pBuffer;
Ipp32u **pHistVec = new Ipp32u*[3];
pHistVec[0] = new Ipp32u[nBins];
pHistVec[1] = new Ipp32u[nBins];
pHistVec[2] = new Ipp32u[nBins];
IppiSize roiSize = {roi->Width(), roi->Height()};
// get sizes for spec and buffer
ippiHistogramGetBufferSize(ipp8u, roiSize, nLevels, 3/*nChan*/, 1/*uniform*/, &sizeHistObj, &sizeBuffer);
pHistObj = (IppiHistogramSpec*)ippsMalloc_8u( sizeHistObj );
pBuffer = (Ipp8u*)ippsMalloc_8u( sizeBuffer );
// initialize spec
ippiHistogramUniformInit( ipp8u, lowerLevel, upperLevel, nLevels, 3, pHistObj );
// check levels of bins
//ppLevels[0] = pLevels;
sts = ippiHistogramGetLevels( pHistObj, pLevels );
// calculate histogram
Ipp8u *pImg = (Ipp8u*)(img->Data());
sts = ippiHistogram_8u_C3R(pImg, img->Stride(), roiSize, pHistVec, pHistObj, pBuffer );
if (sts == 0)
   {
   for (int i = 0; i < nBins; i++)
	   {
	   histo[0][i] = pHistVec[0][i];
	   histo[1][i] = pHistVec[1][i];
	   histo[2][i] = pHistVec[2][i];
	   }
   }
//
ippsFree( pHistObj );
ippsFree( pBuffer );
delete pLevels[0];
delete pLevels[1];
delete pLevels[2];
delete []pLevels;
delete pHistVec[0];
delete pHistVec[1];
delete pHistVec[2];
delete []pHistVec;
return sts;
}
//---------------------------------------------------------------------------
// rgb only
//---------------------------------------------------------------------------
int ippStatistic::ApplyLUT(ippImage<ippRGB> *img, TRect *roi, int nBins, unsigned int** lut, int interpol)
{
prepareData(img, img, roi);
IppStatus sts;
int specSize;
int  nLevels[3] = {nBins, nBins, nBins};
float range = 255.0/(nBins - 1), c_level = 0.0;
Ipp32s **pLevels = new Ipp32s*[3];
pLevels[0] = new Ipp32s[nBins];
pLevels[1] = new Ipp32s[nBins];
pLevels[2] = new Ipp32s[nBins];
Ipp32s **pValues = new Ipp32s*[3];
pValues[0] = new Ipp32s[nBins];
pValues[1] = new Ipp32s[nBins];
pValues[2] = new Ipp32s[nBins];
IppiLUT_Spec* pSpec;
for (int i = 0; i < nBins; i++)
   {
   pLevels[0][i] = (int)c_level;
   pLevels[1][i] = (int)c_level;
   pLevels[2][i] = (int)c_level;
   //unsigned int test = lut[0][i];
   pValues[0][i] = lut[0][i];
   pValues[1][i] = lut[1][i];
   pValues[2][i] = lut[2][i];
   c_level += range;
   }
sts = ippiLUT_GetSize( ippLinear, ipp8u, ippC3, roiSize, nLevels, &specSize );
pSpec = (IppiLUT_Spec*)ippMalloc( specSize );
IppiInterpolationType interpolation;
if (interpol == 0)
   interpolation = ippNearest;
else if (interpol == 1)
   interpolation = ippLinear;
else
   interpolation = ippCubic;
sts = ippiLUT_Init_8u( interpolation, ippC3, roiSize, (const int **) pValues, (const int **)pLevels, nLevels,  pSpec );
sts = ippiLUT_8u_C3IR( pSrc, srcStep, roiSize, pSpec );
delete pLevels[0];
delete pLevels[1];
delete pLevels[2];
delete []pLevels;
delete pValues[0];
delete pValues[1];
delete pValues[2];
delete []pValues;
return sts;
}
/* equlize code
#define GrayLevels 256

Ipp32s histo[GrayLevels];
Ipp32f s[GrayLevels];
Ipp32s levels[GrayLevels+1], values[GrayLevels+1];

//calculate histogram
ippiHistogramEven_8u_C1R(pImage, imgStep, imgSize, histo, levels, GrayLevels+1, 0, GrayLevels);

for (int i = 0; i < GrayLevels; i ++)
{
s[i] = float(histo[i]) / imgSize.width / imgSize.height;
if (i > 0)
s[i] = s[i] + s[i-1];
values[i] = Ipp32s(s[i] * (GrayLevels-1));
}
values[GrayLevels] = GrayLevels;

//LUT
ippiLUT_8u_C1IR(pImage, imgStep, imgSize, values, levels, GrayLevels+1);
*/
//---------------------------------------------------------------------------
void ippStatistic::SetStatRectangle(TRect *boundRect, TRect &roiRect)
{
int w, h;
if (imgColor)
   {
   w = imgColor->Width();
   h = imgColor->Height();
   }
else
   {
   w = imgGray->Width();
   h = imgGray->Height();
   }
if (boundRect)
   {
   roiRect.left = (boundRect->left < 0) ? 0 : boundRect->left;
   roiRect.top = (boundRect->top < 0) ? 0 : boundRect->top;
   roiRect.right = (boundRect->right > w) ? w : boundRect->right;
   roiRect.bottom = (boundRect->bottom > h) ? h : boundRect->bottom;
   }
else
   {
   roiRect.left = 0;
   roiRect.top  = 0;
   roiRect.right  = w;
   roiRect.bottom = h;
   }
}
//---------------------------------------------------------------------------
void ippStatistic::SetStatImages(void *bufferColor, void *bufferGray, int width, int height, unsigned int scanlineAlignment)
{
if (bufferColor)
   imgColor  = new ippImage<ippRGB>(bufferColor, width, height, scanlineAlignment);
if (bufferGray)
   imgGray  =  new ippImage<ippGray>(bufferGray, width, height, scanlineAlignment);
statImagesOwned = true;
}
//---------------------------------------------------------------------------
void ippStatistic::SetStatImages(ippImage<ippRGB> *iColor, ippImage<ippGray> *iGray)
{
imgColor  = iColor;
imgGray  =  iGray;
statImagesOwned = false;
}
//---------------------------------------------------------------------------
int ippStatistic::InitMoments(void)
{
IppStatus sts;
int sizeState;
IppHintAlgorithm hint = ippAlgHintNone;
sts = ippiMomentGetStateSize_64f(hint, &sizeState);
if (sts != 0)
   return sts;
pMomentState  = (IppiMomentState_64f*) ippMalloc(sizeState);
sts = ippiMomentInit_64f(pMomentState, hint);
if (sts != 0)
   {
   ippFree(pMomentState);
   pMomentState = NULL;
   return sts;
   }
return 0;
}
//---------------------------------------------------------------------------
int ippStatistic::ComputeMoments(ippImage<ippRGB> *img, TRect *roi)
{
if (!pMomentState)
   return -1;
prepareData(img, img, roi);
IppStatus sts;
sts = ippiMoments64f_8u_C3R( pSrc, srcStep, roiSize, pMomentState);
return sts;
}
//---------------------------------------------------------------------------
int ippStatistic::ComputeMoments(ippImage<ippGray> *img, TRect *roi)
{
if (!pMomentState)
   return -1;
prepareData(img, img, roi);
IppStatus sts;
sts = ippiMoments64f_8u_C1R( pSrc, srcStep, roiSize, pMomentState);
return sts;
}
//---------------------------------------------------------------------------
double ippStatistic::GetMoment(int mOrd, int nOrd, int channel, TippMomentType type)
{
if (!pMomentState)
   return -9999;
IppStatus sts;
Ipp64f pValue;
IppiPoint roiOffset = {0, 0};
switch (type)
   {
   case ipp_MOMENT_SPATIAL:
		sts = ippiGetSpatialMoment_64f(pMomentState, mOrd, nOrd, channel, roiOffset, &pValue);
		break;
   case ipp_MOMENT_CENTRAL:
		sts = ippiGetCentralMoment_64f(pMomentState, mOrd, nOrd, channel, &pValue);
		break;
   case ipp_MOMENT_SPATIAL_NORM:
		sts = ippiGetNormalizedSpatialMoment_64f(pMomentState, mOrd, nOrd, channel, roiOffset, &pValue);
		break;
   case ipp_MOMENT_CENTRAL_NORM:
		sts = ippiGetNormalizedCentralMoment_64f(pMomentState, mOrd, nOrd, channel, &pValue);
		break;
   }
return (double)(pValue);
}
//---------------------------------------------------------------------------
void ippStatistic::ReleaseStatImages(void)
{
if (statImagesOwned)
   {
   if (imgColor)
	   delete imgColor;
   if (imgGray)
	   delete imgGray;
   }
}
//---------------------------------------------------------------------------
void ippStatistic::ReleaseMoments(void)
{
if (pMomentState)
   {
   ippFree(pMomentState);
   pMomentState = NULL;
   }
}
//---------------------------------------------------------------------------
int ippStatistic::ComputeNorms(ippImage<ippRGB> *img, TRect *roi)
{
prepareData(img, img, roi);
IppStatus sts;
sts = ippiNorm_Inf_8u_C3R( pSrc, srcStep, roiSize, normInf);
if (sts != 0)
   return sts;
sts = ippiNorm_L1_8u_C3R( pSrc, srcStep, roiSize, normL1);
if (sts != 0)
   return sts;
sts = ippiNorm_L2_8u_C3R( pSrc, srcStep, roiSize, normL2);
return sts;
}
//---------------------------------------------------------------------------
int ippStatistic::ComputeNorms(ippImage<ippGray> *img, TRect *roi)
{
prepareData(img, img, roi);
IppStatus sts;
sts = ippiNorm_Inf_8u_C1R( pSrc, srcStep, roiSize, &normInf[0]);
if (sts != 0)
   return sts;
sts = ippiNorm_L1_8u_C1R( pSrc, srcStep, roiSize, &normL1[0]);
if (sts != 0)
   return sts;
sts = ippiNorm_L2_8u_C1R( pSrc, srcStep, roiSize, &normL2[0]);
return sts;
}
//---------------------------------------------------------------------------
double ippStatistic::GetNorm(int channel, TippNormType type)
{
Ipp64f pValue;
switch (type)
   {
   case ipp_NORM_INF:
		pValue = normInf[channel];
		break;
   case ipp_NORM_L1:
		pValue = normL1[channel];
		break;
   case ipp_NORM_L2:
		pValue = normL2[channel];
		break;
   }
return (double)pValue;
}



