//---------------------------------------------------------------------------
#pragma hdrstop
#include "ippRemap.h"
#include "spGeometry.hpp"
#include "ippCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
// constructor
ippRemap::ippRemap(int radius)
{
//
_radius = radius;
int dia = radius * 2;
ippIN = new ippImage<ippRGB>(dia, dia);
ippOUT = new ippImage<ippRGB>(dia, dia);
//ippMASK = new ippImage<ippRGB>(dia, dia);
ippFX = new ippImage<ippFloat>(dia, dia);
ippFY = new ippImage<ippFloat>(dia, dia);
patchRect.init(0, 0, dia, dia);
}
// destructor
ippRemap::~ippRemap(void)
{
delete ippIN;
delete ippOUT;
//delete ippMASK;
delete ippFX;
delete ippFY;
}
//---------------------------------------------------------------------------
void ippRemap::setSrcRect(ippImage<ippRGB> *srcImage, const int &X, const int &Y)
{
srcRect.init(X - _radius, Y - _radius, X + _radius, Y + _radius);
if (srcRect.left < 0)
   srcRect.left = 0;
if (srcRect.top < 0)
   srcRect.top = 0;
if (srcRect.right >= srcImage->Width())
   srcRect.right = srcImage->Width()-1;
if (srcRect.bottom >= srcImage->Height())
   srcRect.bottom = srcImage->Height()-1;
}
//---------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------
void ippRemap::preparePatch(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect)
{
pSrc = (unsigned char*)srcImage->Data();
pDst = (unsigned char*)dstImage->Data();
srcStep = srcImage->Stride();
dstStep = dstImage->Stride();
roiSize.width  = roiRect->Width();
roiSize.height = roiRect->Height();
}
//---------------------------------------------------------------------------
void ippRemap::prepareIn(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect)
{
pSrc = (unsigned char*)srcImage->Data();
pDst = (unsigned char*)dstImage->Data();
srcStep = srcImage->Stride();
dstStep = dstImage->Stride();
roiSize.width  = roiRect->Width();
roiSize.height = roiRect->Height();
// parivrta
if (srcImage->Width() != roiRect->Width() || srcImage->Height() != roiRect->Height())
   pSrc = pSrc + (roiRect->left * 3 + (srcImage->Height() - roiRect->bottom) * srcStep);
}
//---------------------------------------------------------------------------
void ippRemap::prepareOut(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect)
{
pSrc = (unsigned char*)srcImage->Data();
pDst = (unsigned char*)dstImage->Data();
srcStep = srcImage->Stride();
dstStep = dstImage->Stride();
roiSize.width  = roiRect->Width();
roiSize.height = roiRect->Height();
// parivrta
if (dstImage->Width() != roiRect->Width() || dstImage->Height() != roiRect->Height())
   pDst = pDst + (roiRect->left * 3 + (dstImage->Height() - roiRect->bottom) * dstStep);
}
//---------------------------------------------------------------------------
void  ippRemap::remapBlob(int wRange, int wStrength, int bSign)
{
double r_factor, X_norig, Y_norig, NEW_diag, ROI_diag = ((double)(wRange>>1));
int i, j, shiftX, shiftY;
float fx, fy, fxx, fyy;
double blob_attr = (1/M_PI)* (double)wStrength/100.0;
double blob_factor = ((double)(wRange>>1)) / M_PI_2;
shiftX = ((ippFX->Width())>>1) - (wRange>>1) + ROI_diag;
shiftY = shiftX;
for (i = 0; i < ippFX->Height(); i++)
	{
	for (j = 0; j < ippFX->Width(); j++)
		{
		fx = ippFX->Pixel(i, j).ch[0];
		fy = ippFY->Pixel(i, j).ch[0];
		X_norig	 =  (double)(fx - shiftX); // (ROI_diag + shiftX));
		Y_norig	 =  (double)(fy - shiftY); // (ROI_diag + shiftY));
		NEW_diag = sqrt(X_norig*X_norig + Y_norig*Y_norig);
		if (NEW_diag <= ROI_diag)
		   {
		   r_factor = bSign * cos(NEW_diag / blob_factor);
		   fxx = fx - (X_norig *  blob_attr) * r_factor;
		   fyy = fy - (Y_norig *  blob_attr) * r_factor;
		   if ((fxx > 0) && (fyy > 0) && (fxx < float(ippFX->Width() - 1)) && (fyy < float(ippFY->Height() - 1)))
			  {
			  fx = fxx;
			  fy = fyy;
			  }
		   }
		ippFX->Pixel(i, j).ch[0] = fx;
		ippFY->Pixel(i, j).ch[0] = fy;
		}
	}
}
//---------------------------------------------------------------------------
void  ippRemap::remapTwirl(int wRange, double wAngle)
{
double angle;
double radius, NEW_diag, ROI_diag = ((double)(wRange >> 1));
double step_angle = wAngle/ROI_diag;
double rotoX, rotoY;
int i, j, X1, Y1;
float fx, fy;
TPoint pStart, pEnd;
X1 = ((ippFX->Width())>>1);
Y1 = X1;
pStart.x = X1;
pStart.y = Y1;
for (i = 0; i < ippFX->Height(); i++)
	{
	for (j = 0; j < ippFX->Width(); j++)
		{
		fx = ippFX->Pixel(i, j).ch[0];
		fy = ippFY->Pixel(i, j).ch[0];
		spGeometry::GetLineData(&radius, &angle, X1, Y1, fast_ftol(fx), fast_ftol(fy));
		NEW_diag = ROI_diag - radius;
		if (NEW_diag > 0)
		   {
		   pEnd.x = fast_ftol(fx);
		   pEnd.y = fast_ftol(fy);
		   spGeometry::GetRotateData(&pEnd, &pStart, -step_angle, &rotoX, &rotoY, false); //relative or absolute ???
		   fx = (float)rotoX;
		   fy = (float)rotoY;
		   if (fx > (ippFX->Width() - 1))
			   fx = ippFX->Width() - 1.0;
		   if (fy > (ippFX->Height() - 1))
			   fx = ippFX->Height() - 1.0;
		   }
		ippFX->Pixel(i, j).ch[0] = fx;
		ippFY->Pixel(i, j).ch[0] = fy;
		}
	}
}
//---------------------------------------------------------------------------
void  ippRemap::remapExplode(int wRange, int wStrength, int bSign)
{
double r_factor, X_norig, Y_norig, NEW_diag, ROI_diag = ((double)(wRange>>1));
int i, j, shiftX, shiftY;
float fx, fy, fr, fxx, fyy;
shiftX = ((ippFX->Width())>>1) - (wRange>>1) + ROI_diag;
shiftY = shiftX;
r_factor = (double)wStrength / 100.0;
for (i = 0; i < ippFX->Height(); i++)
	{
	for (j = 0; j < ippFX->Width(); j++)
		{
		fx = ippFX->Pixel(i, j).ch[0];
		fy = ippFY->Pixel(i, j).ch[0];
		X_norig	 =  (double)(fx - shiftX);
		Y_norig	 =  (double)(fy - shiftY);
		NEW_diag = sqrt(X_norig*X_norig + Y_norig*Y_norig);
		if ((NEW_diag <= ROI_diag) && (NEW_diag > 0))
		   {
		   fr = (float)random(ROI_diag);
           fr *= r_factor;
		   X_norig =  fr*X_norig / ROI_diag;
           Y_norig =  fr*Y_norig / ROI_diag;
           NEW_diag = sqrt(X_norig*X_norig + Y_norig*Y_norig);
           if (NEW_diag <= ROI_diag)
              {
			  fxx = fx - (float)(bSign * X_norig);
              fyy = fy - (float)(bSign * Y_norig);
              if ((fxx > 0) && (fyy > 0) && (fxx < float(ippFX->Width() - 1)) && (fyy < float(ippFX->Height() - 1 )))
                 {
				 fx = fxx;
                 fy = fyy;
                 }
              }
		   }
		ippFX->Pixel(i, j).ch[0] = fx;
		ippFY->Pixel(i, j).ch[0] = fy;
		}
	}
}
//---------------------------------------------------------------------------
// public
//---------------------------------------------------------------------------
void ippRemap::Prepare(Tspe_SpreadParams *spreadParams)
{
//
// initial fill
int i, j;
float fx, fy;
for (i = 0; i < ippFX->Height(); i++)
   {
   fy = (float)i;
   for (j = 0; j < ippFX->Width(); j++)
	   {
	   fx = (float)j;
	   ippFX->Pixel(i, j).ch[0] = fx;
	   ippFY->Pixel(i, j).ch[0] = fy;
	   }
   }
// let's try twirl - works ok
int index;
for (i = 0; i < spreadParams->WarpCount; i++)
	{
	for (j = 0; j < spreadParams->WarpCount; j++)
		{
		if (spreadParams->WarpOrder[j] == i && spreadParams->WarpOn[j])
           {
		   switch (j)
			   {
			   case 0:  // blob
				   remapBlob(spreadParams->WarpParam1[j], spreadParams->WarpParam2[j], +1 );
				   break;
			   case 1:  // blub
				   remapBlob(spreadParams->WarpParam1[j], spreadParams->WarpParam2[j], -1 );
				   break;
			   case 2: // twirl
				   remapTwirl(spreadParams->WarpParam1[j], spreadParams->WarpParam2[j] *  M_PI/180.0);
				   break;
			   case 3: // explode
				   remapExplode(spreadParams->WarpParam1[j], spreadParams->WarpParam2[j], +1);
				   break;
			   case 4: // impolde
				   remapExplode(spreadParams->WarpParam1[j], spreadParams->WarpParam2[j], -1);
				   break;

			   }
		   }
		}
	}
}
//---------------------------------------------------------------------------
void ippRemap::BlendSrcDst(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, const int &X, const int &Y, Ipp8u blendIn)
{
if (blendIn > 0x00)
   {
   setSrcRect(srcImage, X, Y);
   ippCommon::Blend(srcImage, &srcRect, dstImage, &srcRect, blendIn, ippAlphaPlus, 0x03);
   }
}
//---------------------------------------------------------------------------
void ippRemap::PatchIn(ippImage<ippRGB> *srcImage1, ippImage<ippRGB> *srcImage2, const int &X, const int &Y, Ipp8u blendIn)
{
setSrcRect(srcImage1, X, Y);
if (blendIn < 0xff)
   ippCommon::Blend(srcImage1, srcImage2, &srcRect, ippIN, &srcRect, blendIn, ippAlphaPlus, 0x01);
else
   {
   prepareIn(srcImage1, ippIN, &srcRect);
   ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
   }
}
//---------------------------------------------------------------------------
void ippRemap::PatchOut(ippImage<ippRGB> *dstImage, const int &X, const int &Y, Ipp8u blendOut)
{
setSrcRect(dstImage, X, Y);
prepareOut(ippOUT, dstImage, &srcRect);
if (blendOut < 0xff)
   {
   TRect tpRect;
   tpRect.init(0, 0, srcRect.Width(), srcRect.Height());
   //ippCommon::Blend(ippOUT, &patchRect, dstImage, &srcRect, blendOut, ippAlphaPlus, 0x02);
   ippCommon::Blend(ippOUT, &tpRect, dstImage, &srcRect, blendOut, ippAlphaPlus, 0x02);
   }
else
   ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
}
//---------------------------------------------------------------------------
void ippRemap::PatchOutHard(ippImage<ippRGB> *dstImage, const int &X, const int &Y,  ippImage<ippGray> *maskImage)
{
// this is confusing: - scr(inv) -> patch(as is) + mask(as is) -> dst(inv)
setSrcRect(dstImage, X, Y);
int ii, jj, oo;
for (int i = patchRect.bottom - 1; i >= patchRect.top; i--)
	{
	ii = srcRect.bottom - 1 - i;
	for (int j = patchRect.left; j < patchRect.right; j++)
		{
		jj = srcRect.left + j;
		if (maskImage->Pixel(ii, jj).ch[0] > 0x00)
		   {
		   oo = dstImage->Height() - 1 - (srcRect.bottom - 1 - i);
		   dstImage->Pixel(oo, jj) = ippOUT->Pixel(i, j);
		   }
		}
	}
}
//---------------------------------------------------------------------------
void ippRemap::PatchOutSoft(ippImage<ippRGB> *dstImage, const int &X, const int &Y, ippImage<ippGray> *maskImage)
{
//this is confusing: - scr(inv) -> patch(as is) + mask (as is) -> dst(inv)
ippRGB srcPix, dstPix;
Byte maskByte;
setSrcRect(dstImage, X, Y);
int ii, jj, oo;
int nIn1, nIn2;
for (int i = patchRect.bottom - 1; i >= patchRect.top; i--)
	{
	ii = srcRect.bottom - 1 - i;
	for (int j = patchRect.left; j < patchRect.right; j++)
		{
		jj = srcRect.left + j;
		oo = dstImage->Height() - 1 - (srcRect.bottom - 1 - i);
		maskByte = maskImage->Pixel(ii, jj).ch[0];
		if (maskByte > 0x00)
		   {
		   srcPix = ippOUT->Pixel(i, j);
		   dstPix = dstImage->Pixel(oo, jj);
		   nIn1 = (spe_SP_NORM * (int)maskByte)>>8;
		   nIn2 = spe_SP_NORM - nIn1;
		   dstPix.ch[0] = (srcPix.ch[0] * nIn1 + dstPix.ch[0] * nIn2)>>spe_SP_SHIFT;
		   dstPix.ch[1] = (srcPix.ch[1] * nIn1 + dstPix.ch[1] * nIn2)>>spe_SP_SHIFT;
		   dstPix.ch[2] = (srcPix.ch[2] * nIn1 + dstPix.ch[2] * nIn2)>>spe_SP_SHIFT;
		   dstImage->Pixel(oo, jj) = dstPix;
		   }
		}
	}
}
//---------------------------------------------------------------------------
int ippRemap::Apply(void)
{
//eberything ok...but...
//TODO: check roiRect boundaries!!!!!!!
preparePatch(ippIN, ippOUT, &patchRect);
//ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
//return 0;
int xMapStep = ippFX->Stride()*4;
int yMapStep = ippFY->Stride()*4;
Ipp32f *pxMap  = (Ipp32f*)ippFX->Data();
Ipp32f *pyMap  = (Ipp32f*)ippFY->Data();
IppiSize srcSize;
srcSize.width  = ippIN->Width();
srcSize.height = ippOUT->Height();
IppiRect srcRoi;
srcRoi.x = patchRect.left;
srcRoi.y = patchRect.top;
srcRoi.width = patchRect.Width();
srcRoi.height = patchRect.Height();
int interpolation =  IPPI_INTER_LINEAR;
IppStatus status = ippiRemap_8u_C3R(pSrc, srcSize, srcStep, srcRoi, pxMap, xMapStep, pyMap, yMapStep, pDst, dstStep, roiSize, interpolation);
return status;
}
//---------------------------------------------------------------------------

