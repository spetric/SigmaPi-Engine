//---------------------------------------------------------------------------
#pragma hdrstop
#include "ippAuxiliary.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
Ipp8u* ippAux::pSrc = NULL;
Ipp8u* ippAux::pDst = NULL;
int ippAux::srcStep = 0;
int ippAux::dstStep = 0;
IppiSize ippAux::roiSize = {0, 0};
int ippAux::channels = 0;
//---------------------------------------------------------------------------
bool ippAux::IppAdd(void)
  {
   // I1 + I2 -> I1
   int status;
   if (channels == 3)
	  status = ippiAdd_8u_C3IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
   else
	  status = ippiAdd_8u_C1IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
   return (status == 0);
   }
//---------------------------------------------------------------------------
bool ippAux::IppSub(void)
   {
   // I1 - I2 -> I1
   int status;
   if (channels == 3)
	  status = ippiSub_8u_C3IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
   else
	  status = ippiSub_8u_C1IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
   return (status == 0);
   }
//---------------------------------------------------------------------------
/*
bool ippAux::IppAdd16S(void)
  {
  int stride1, stride2, strideRes;
  Ipp16s *ippBuffer1;
  Ipp16s *ippBuffer2;
  // I1 + I2 -> I1
  int status;
  if (channels == 3)
	  {
	  ippBuffer1 = ippiMalloc_16s_C3(roiSize.width, roiSize.height, &stride1);
	  ippBuffer2 = ippiMalloc_16s_C3(roiSize.width, roiSize.height, &stride2);
	  ippiScale_8u16s_C3R(pSrc, srcStep, ippBuffer1, stride1, roiSize);
	  ippiScale_8u16s_C3R(pDst, dstStep, ippBuffer2, stride2, roiSize);
	  ippiAdd_16s_C3IRSfs(ippBuffer1, stride1, ippBuffer2, stride2, roiSize, 1);
	  ippiScale_16s8u_C3R(ippBuffer1, stride1, pSrc, srcStep, roiSize, ippAlgHintNone);
	  }
   else
	  {
	  //TODO: --- isto i za jednokanalnu sliku
	  status = ippiAdd_8u_C1IRSfs(pSrc, srcStep, pDst, dstStep, roiSize, 1);
	  }
   return (status == 0);
   }
*/



