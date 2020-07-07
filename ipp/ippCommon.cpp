//---------------------------------------------------------------------------
#pragma hdrstop
#include "ippCommon.h"
#include "AsmVars.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// compare inner
//---------------------------------------------------------------------------
bool ippCommon::compInner(int intensity, int lo, int hi)
{ return (intensity >= lo && intensity <= hi); }
//---------------------------------------------------------------------------
// compare outer
//---------------------------------------------------------------------------
bool ippCommon::compOuter(int intensity, int lo, int hi)
{ return (intensity <= lo || intensity >= hi); }
//---------------------------------------------------------------------------
// compare out hi
//---------------------------------------------------------------------------
bool ippCommon::compOutHi(int intensity, int lo, int hi)
{ return (intensity >= hi); }
//---------------------------------------------------------------------------
// compare out lo
//---------------------------------------------------------------------------
bool ippCommon::compOutLo(int intensity, int lo, int hi)
{ return (intensity <= lo);}
//---------------------------------------------------------------------------
// compare pixels work1 vs work2 -> dstImage
//---------------------------------------------------------------------------
 void ippCommon::CompCloseFar(ippImage<ippRGB> *work1, ippImage<ippRGB> *work2, ippImage<ippRGB> *dstImage, Tspe_CompoundIppParams *ipf)
{
int w = work1->Width();
int h = work1->Height();
int left_val, right_val; //, left_intensity, right_intensity;
ippRGB left_pix, right_pix, refer_pix, put_pix;
int put_intensity, lo_intensity = ipf->LoVal * 3 , hi_intensity = ipf->HiVal *3;
TIppCompoType compTyper;
switch (ipf->RangeType)
	{
	case 1:		// outer
		 compTyper = compOuter;
		 break;
	case 2:		// out high
		 compTyper = compOutHi;
		 break;
	case 3:		// out low
		 compTyper = compOutLo;
		 break;
	default:		// inner
		 compTyper = compInner;
		 break;
	}
for (int i = 0; i < h; i++)
	{
	for (int j = 0; j < w; j++)
		{
		left_pix  = work1->Pixel(i, j);
		right_pix = work2->Pixel(i, j);
		refer_pix = dstImage->Pixel(i, j);
        //TODO: refer_pix = patch average MxM
		put_intensity = MinDistL1(refer_pix, left_pix, right_pix, left_val, right_val);
		if (right_val < left_val)
		   {
		   if (ipf->CDMode == 0)
			   put_pix = right_pix;
		   else
			   put_pix = left_pix;
		   }
		else
		   {
		   if (ipf->CDMode == 0)
			   put_pix = left_pix;
		   else
			   put_pix = right_pix;
		   }
		if (compTyper(put_intensity, lo_intensity, hi_intensity))
		   dstImage->Pixel(i, j) = put_pix;
		// test
		//dstImage->Pixel(i, j) = put_pix;
		}
	}
}
//---------------------------------------------------------------------------
// factor addition - unsharp mask
//---------------------------------------------------------------------------
void ippCommon::FactorAddition(ippImage<ippRGB> *work1, ippImage<ippRGB> *work2, ippImage<ippRGB> *dstImage, float left_factor, float right_factor)
{
ippRGB left_pix, right_pix;
float result_chan;
int h = work1->Height();
int w = work1->Width();
for (int i = 0; i < h; i++)
	{
	for (int j = 0; j < w; j++)
		{
		left_pix  = work1->Pixel(i, j);
		right_pix = work2->Pixel(i, j);
		for (int k = 0; k < 3; k++)
			{
			result_chan = left_factor*(float)left_pix.ch[k] +  right_factor*(float)right_pix.ch[k];
			if (result_chan > 255.0)
				result_chan = 255;
			dstImage->Pixel(i, j).ch[k] = (Byte)fast_ftol(result_chan);
			}
		}
	}
}
//---------------------------------------------------------------------------
// blend two images - const alpha
//---------------------------------------------------------------------------
void ippCommon::Blend(ippImage<ippRGB> *srcImage, TRect *srcRect, ippImage<ippRGB> *dstImage, TRect *dstRect, Ipp8u srcAlpha, IppiAlphaType alphaType, Byte parivrta)
{
Ipp8u *pSrc = (unsigned char*)srcImage->Data();
Ipp8u* pDst = (unsigned char*)dstImage->Data();
int srcStep = srcImage->Stride();
int dstStep = dstImage->Stride();
IppiSize roiSize;
roiSize.width  = srcRect->Width();
roiSize.height = srcRect->Height();
// parivrta
if (parivrta == 0x00)
   {
   pSrc = pSrc + srcRect->left * 3 + srcRect->top * srcStep;
   pDst = pDst + dstRect->left * 3 + dstRect->top * dstStep;
   }
else
   {
   if (parivrta & 0x01)
	  pSrc = pSrc + srcRect->left * 3 + (srcImage->Height() - srcRect->bottom) * srcStep;
   if (parivrta & 0x02)
	  pDst = pDst + dstRect->left * 3 + (dstImage->Height() - dstRect->bottom) * dstStep;
   }
Ipp8u dstAlpha = 0xff - srcAlpha;
ippiAlphaCompC_8u_C3IR(pSrc, srcStep, srcAlpha, pDst, dstStep, dstAlpha, roiSize, alphaType);
}
//---------------------------------------------------------------------------
// blend two images and output to third - const alpha
//---------------------------------------------------------------------------
void ippCommon::Blend(ippImage<ippRGB> *srcImage1, ippImage<ippRGB> *srcImage2, TRect *srcRect, ippImage<ippRGB> *dstImage, TRect *dstRect, Ipp8u srcAlpha, IppiAlphaType alphaType, Byte parivrta)
{
Ipp8u *pSrc1 = (unsigned char*)srcImage1->Data();
Ipp8u *pSrc2 = (unsigned char*)srcImage2->Data();
Ipp8u* pDst = (unsigned char*)dstImage->Data();
int src1Step = srcImage1->Stride();
int src2Step = srcImage2->Stride();
int dstStep = dstImage->Stride();
IppiSize roiSize;
roiSize.width  = srcRect->Width();
roiSize.height = srcRect->Height();
// parivrta
if (parivrta == 0x00)
   {
   pSrc1 = pSrc1 + srcRect->left * 3 + srcRect->top * src1Step;
   pSrc2 = pSrc2 + srcRect->left * 3 + srcRect->top * src2Step;
   pDst = pDst + dstRect->left * 3 + dstRect->top * dstStep;
   }
else
   {
   if (parivrta & 0x01)
	  {
	  pSrc1 = pSrc1 + srcRect->left * 3 + (srcImage1->Height() - srcRect->bottom) * src1Step;
	  pSrc2 = pSrc2 + srcRect->left * 3 + (srcImage2->Height() - srcRect->bottom) * src2Step;
	  }
   if (parivrta & 0x02)
	  pDst = pDst + dstRect->left * 3 + (dstImage->Height() - dstRect->bottom) * dstStep;
   }
Ipp8u dstAlpha = 0xff - srcAlpha;
ippiAlphaCompC_8u_C3R(pSrc1, src1Step, srcAlpha, pSrc2, src2Step, dstAlpha, pDst, dstStep, roiSize, alphaType);
}
//---------------------------------------------------------------------------
// copy data from one channel image to three channel image
//---------------------------------------------------------------------------
void ippCommon::CopyOne2Three(ippImage<ippGray> *srcImage, ippImage<ippRGB> *dstImage, TRect *srcRect, TRect *dstRect, Byte parivrta)
{
Ipp8u *pSrc = (unsigned char*)srcImage->Data();
Ipp8u* pDst = (unsigned char*)dstImage->Data();
int srcStep = srcImage->Stride();
int dstStep = dstImage->Stride();
IppiSize roiSize;
roiSize.width  = srcRect->Width();
roiSize.height = srcRect->Height();
// parivrta
if (parivrta == 0x00)
   {
   pSrc = pSrc + srcRect->left + srcRect->top * srcStep;
   pDst = pDst + dstRect->left * 3 + dstRect->top * dstStep;
   }
else
   {
   if (parivrta & 0x01)
	  pSrc = pSrc + srcRect->left + (srcImage->Height() - srcRect->bottom) * srcStep;
   if (parivrta & 0x02)
	  pDst = pDst + dstRect->left * 3 + (dstImage->Height() - dstRect->bottom) * dstStep;
   }
ippiCopy_8u_C1C3R(pSrc, srcStep, pDst, dstStep, roiSize);
}
//---------------------------------------------------------------------------
// copy data RGB -  a mogil smo i s templateom
//---------------------------------------------------------------------------
void ippCommon::Copy(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *srcRect, TRect *dstRect, Byte parivrta)
{
Ipp8u *pSrc = (unsigned char*)srcImage->Data();
Ipp8u* pDst = (unsigned char*)dstImage->Data();
int srcStep = srcImage->Stride();
int dstStep = dstImage->Stride();
IppiSize roiSize;
roiSize.width  = srcRect->Width();
roiSize.height = srcRect->Height();
// parivrta
if (parivrta == 0x00)
   {
   pSrc = pSrc + srcRect->left + srcRect->top * srcStep;
   pDst = pDst + dstRect->left * 3 + dstRect->top * dstStep;
   }
else
   {
   if (parivrta & 0x01)
	  pSrc = pSrc + srcRect->left + (srcImage->Height() - srcRect->bottom) * srcStep;
   if (parivrta & 0x02)
	  pDst = pDst + dstRect->left * 3 + (dstImage->Height() - dstRect->bottom) * dstStep;
   }
ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
}
//---------------------------------------------------------------------------
// copy data RGB -  a mogil smo i s templateom
//---------------------------------------------------------------------------
void ippCommon::Copy(ippImage<ippGray> *srcImage, ippImage<ippGray> *dstImage, TRect *srcRect, TRect *dstRect, Byte parivrta)
{
Ipp8u *pSrc = (unsigned char*)srcImage->Data();
Ipp8u* pDst = (unsigned char*)dstImage->Data();
int srcStep = srcImage->Stride();
int dstStep = dstImage->Stride();
IppiSize roiSize;
roiSize.width  = srcRect->Width();
roiSize.height = srcRect->Height();
// parivrta
if (parivrta == 0x00)
   {
   pSrc = pSrc + srcRect->left + srcRect->top * srcStep;
   pDst = pDst + dstRect->left + dstRect->top * dstStep;
   }
else
   {
   if (parivrta & 0x01)
	  pSrc = pSrc + srcRect->left + (srcImage->Height() - srcRect->bottom) * srcStep;
   if (parivrta & 0x02)
	  pDst = pDst + dstRect->left + (dstImage->Height() - dstRect->bottom) * dstStep;
   }
ippiCopy_8u_C1R(pSrc, srcStep, pDst, dstStep, roiSize);
}
//---------------------------------------------------------------------------
// calculate L1 distance and return lowest intensity
//---------------------------------------------------------------------------
int ippCommon::MinDistL1(ippRGB &refer_pix, ippRGB &left_pix, ippRGB &right_pix, int &left_val, int &right_val)
{
left_val = 0;
right_val = 0;
int left_intensity = 0, right_intensity = 0;
for (int k = 0; k < 3; k++)
	{
	// by intensity
	left_val  += abs((int)left_pix.ch[k] - (int)refer_pix.ch[k]);
	right_val += abs((int)refer_pix.ch[k] - (int)right_pix.ch[k]);
	left_intensity += (int)left_pix.ch[k];
	right_intensity += (int)right_pix.ch[k];
	}
return (right_val < left_val) ? right_intensity : left_intensity;
}
//---------------------------------------------------------------------------
// calculate euclidean distance and return lowest intensity
//---------------------------------------------------------------------------
int ippCommon::MinDistEuclid(ippRGB &refer_pix, ippRGB &left_pix, ippRGB &right_pix, int &left_val, int &right_val)
{
left_val = 0;
right_val = 0;
int left_intensity = 0, right_intensity = 0;
int quadrance;
for (int k = 0; k < 3; k++)
	{
	quadrance = (int)left_pix.ch[k] - (int)refer_pix.ch[k];
	quadrance *= quadrance;
	left_val  += quadrance;
	quadrance = (int)refer_pix.ch[k] - (int)right_pix.ch[k];
	quadrance *= quadrance;
	right_val += quadrance;
	left_intensity += (int)left_pix.ch[k];
	right_intensity += (int)right_pix.ch[k];
	}
return (right_val < left_val) ? right_intensity : left_intensity;
}





