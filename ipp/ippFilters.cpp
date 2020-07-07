//---------------------------------------------------------------------------
#pragma hdrstop
#include "ippFilters.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
ippFilter::ippFilter(int numBytes)
{
//private
status = ippStsNoErr;
pBuffer = NULL;
roiSize.width = 0;
roiSize.height = 0;
maskSize.width = 0;
maskSize.height = 0;
// public
BorderType = ippBorderRepl;
BorderValue = 0x00;
}
//---------------------------------------------------------------------------
// destructor
ippFilter::~ippFilter(void)
{
//
}
//---------------------------------------------------------------------------
int ippFilter::ippPrepareMedian(TRect *srcRect, int &kernelX, int &kernelY)
{
status = ippStsNoErr;
roiSize.width  = srcRect->Width();
roiSize.height = srcRect->Height();
maskSize.width = kernelX;
maskSize.height = kernelY;
int iTmpBufSize = 0;
status = ippiFilterMedianBorderGetBufferSize(roiSize, maskSize, ipp8u, nChans, &iTmpBufSize);
if (status)
   return status;
status = ippiFilterMedianBorderGetBufferSize(roiSize, maskSize, ipp8u, nChans, &iTmpBufSize);
if (status)
   return status;
pBuffer = ippsMalloc_8u(iTmpBufSize);
return status;
}
//---------------------------------------------------------------------------
int ippFilter::ippMedian(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage)
{
Ipp8u* pSrc  = (unsigned char*)srcImage->Data();
Ipp8u* pDst  = (unsigned char*)dstImage->Data();
int srcStep = srcImage->Stride();
int dstStep = dstImage->Stride();
BorderType = ippBorderConst; 	//ippBorderRepl; // | ippBorderInMemTop | ippBorderInMemRight);
status = ippiFilterMedianBorder_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize, maskSize, BorderType, &BorderValue, pBuffer);
// za test brišemo, inaèe ga ostavimo za drugu turu, a obrišemo ga ako nije iste velièine
ippsFree(pBuffer);
return status;
}

