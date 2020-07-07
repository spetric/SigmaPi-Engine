//---------------------------------------------------------------------------
#pragma hdrstop
#include "spProcessing.h"
#include "AsmVars.h"
#include "spByteLogic.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// conversion  rgb -> gray (average)
void spProcess::Convert(spImage<ptRGB> *imgIn, spImage<ptGray> *imgOut, bool lumina)
{
ptRGB pix;
if (lumina)
   {
   for (int i = 0; i < imgIn->Height(); i++)
	   {
	   for (int j = 0; j < imgIn->Height(); j++)
		   {
		   pix = imgIn->Pixel(i, j);
		   imgOut->Pixel(i, j).ch[0] = (Byte)fast_ftol((double)(pix.ch[0]* spe_D_LUMINANCE[0]
															  + pix.ch[1]* spe_D_LUMINANCE[1]
															  + pix.ch[2]* spe_D_LUMINANCE[2]));
			}

	   }
   }
else
   {
   for (int i = 0; i < imgIn->Height(); i++)
	   {
	   for (int j = 0; j < imgIn->Height(); j++)
		   {
		   pix = imgIn->Pixel(i, j);
		   imgOut->Pixel(i, j).ch[0] = (Byte)fast_ftol((double)(pix.ch[0] + pix.ch[1] + pix.ch[2]) / 3.0);
		   }
	   }
   }
}
//---------------------------------------------------------------------------
// conversion  gray -> rgb
void spProcess::Convert(spImage<ptGray> *imgIn, spImage<ptRGB> *imgOut)
{
ptGray pix;
for (int i = 0; i < imgIn->Height(); i++)
	{
	for (int j = 0; j < imgIn->Height(); j++)
		{
		pix = imgIn->Pixel(i, j);
		imgOut->Pixel(i, j).ch[0] = pix.ch[0];
		imgOut->Pixel(i, j).ch[1] = pix.ch[0];
		imgOut->Pixel(i, j).ch[2] = pix.ch[0];
		}
	}
}
//---------------------------------------------------------------------------
// conversion  rgb -> gray (custom) both images RGB
void spProcess::Grayscale(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, double &rFact, double &gFact, double &bFact)
{
ptRGB pix;
int outVal;
for (int i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (int j = boundRect->left; j < boundRect->right; j++)
		{
		pix = imgIn->Pixel(i, j);
		outVal = fast_ftol(pix.ch[0]*bFact + pix.ch[1]*gFact + pix.ch[2]*rFact);
		if (outVal > 255)
			outVal = 255;
		imgOut->Pixel(i, j).ch[0] = (Byte)outVal;
		imgOut->Pixel(i, j).ch[1] = (Byte)outVal;
		imgOut->Pixel(i, j).ch[2] = (Byte)outVal;
		}
	}
}
//---------------------------------------------------------------------------
// mirror or flip image or both
void spProcess::Mirror(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, int axis)
{
int i, j, iOut, jOut;
switch (axis)
	{
	case 0:	 // flip
		 for (i = boundRect->top, iOut = boundRect->bottom-1; i < boundRect->bottom; i++, iOut--)
			 {
			 for (j = boundRect->left;  j < boundRect->right; j++)
				 imgOut->Pixel(iOut, j) = imgIn->Pixel(i, j);
			 }
		 break;
	case 1: // mirror
		 for (i = boundRect->top; i < boundRect->bottom; i++)
			 {
			 for (j = boundRect->left, jOut = boundRect->right-1; j < boundRect->right; j++, jOut--)
				 imgOut->Pixel(i, jOut) = imgIn->Pixel(i, j);
			 }
		 break;
	default:  // flip + mirror
		 for (i = boundRect->top, iOut = boundRect->bottom-1; i < boundRect->bottom; i++, iOut--)
			 {
			 for (j = boundRect->left, jOut = boundRect->right-1; j < boundRect->right; j++, jOut--)
				 imgOut->Pixel(iOut, jOut) = imgIn->Pixel(i, j);
			 }
		 break;
	}
}
//---------------------------------------------------------------------------
void spProcess::NearestLuminance(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, ptRGB *palEntry, int palCount)
{
int i, j, k, m, pal_index;
double pix_lumina;
// prepare pallete for sorting
double pallumina[256];
int index[256];
ptRGB pix;
for (i = 0; i < palCount; i++)
	{
	pallumina[i] = 0;
	for (k = 0; k < 3; k++)
		pallumina[i] += palEntry[i].ch[k] * spe_D_LUMINANCE[k];
	index[i] = i;
	}
spCommon::QuickSort(pallumina, index, 0, palCount-1);
for (i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (j = boundRect->left; j < boundRect->right; j++)
		{
		pix_lumina = 0;
		pix = imgIn->Pixel(i, j);
		for (k = 0; k < 3; k++)
			pix_lumina += pix.ch[k] * spe_D_LUMINANCE[k];
		m = spCommon::BinarySearch(pallumina, 0, palCount- 1, pix_lumina, true);
		if (m < 0)
			m = 0;
		if (m > (palCount- 1))
			m = palCount- 1;
		pal_index = index[m];
		imgOut->Pixel(i, j) = palEntry[pal_index];
		}
	}
}
//---------------------------------------------------------------------------
void spProcess::NearestColor(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, ptRGB *palEntry, int palCount)
{
int i, j, k, m, pal_index;
double pix_color;
// prepare for sort
double pallColors[256];
int index[256];
ptRGB pix;
for (i = 0; i < palCount; i++)
	{
	pallColors[i] = (double)palEntry[i];
	index[i] = i;
	}
spCommon::QuickSort(pallColors, index, 0, palCount-1);
for (i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (j = boundRect->left; j < boundRect->right; j++)
		{
		pix_color = (double)imgIn->Pixel(i, j);
		m = spCommon::BinarySearch(pallColors, 0, palCount-1, pix_color, true);
		if (m < 0)
			m = 0;
		if (m > (palCount- 1))
			m = palCount- 1;
		pal_index = index[m];
		imgOut->Pixel(i, j) = palEntry[pal_index];
		}
	}
}
//---------------------------------------------------------------------------
void spProcess::PseudoColor(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, ptRGB *palEntry, int palCount)
{
int i, j, k, pal_index;
double pal_dbl;
int pixel_val[3];
ptRGB pix;
for (i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (j = boundRect->left; j < boundRect->right; j++)
		{
		pal_dbl = 0;
		pix = imgIn->Pixel(i, j);
		for (k = 0; k < 3; k++)
			pal_dbl += pix.ch[k]*spe_D_LUMINANCE[k];
		pal_index = fast_ftol(pal_dbl);
		if (pal_index > palCount)
			pal_index = palCount-1;
		imgOut->Pixel(i, j) = palEntry[pal_index];
		}
	}
}
//---------------------------------------------------------------------------
void spProcess::ColorQuantization(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect)
{
//TODO: color quantization...or something
}
//---------------------------------------------------------------------------
void spProcess::ArithBool(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut, TRect *boundRect, int opType, int opValue)
{
// jednostavne operacije
int i, j, k;
double opFact, opAdd;
bool opArith = true;
TByteLogic bLogic;
switch (opType)
	{
	case spe_AB_Add:
		 opFact = 1.0;
		 opAdd  = opValue;
		 break;
	case spe_AB_Sub:
		 opFact = 1.0;
		 opAdd  = -opValue;
		 break;
	case spe_AB_Mult:
		 opFact = opValue;
		 opAdd  = 0.0;
		 break;
	case spe_AB_Div:
		 opFact = 1.0 / opValue;
		 opAdd  = 0.0;
		 break;
	case spe_AB_And:
		 bLogic =  Byte_AND;
		 opArith = false;
		 break;
	case spe_AB_Or:
		 bLogic =  Byte_OR;
		 opArith = false;
		 break;
	case spe_AB_Xor:
		 bLogic =  Byte_XOR;
		 opArith = false;
		 break;
	default:
		 break;
	}
if (opArith)
   {
	int outVal;
	for (i = boundRect->top; i < boundRect->bottom; i++)
		{
		for (j = boundRect->left; j < boundRect->right; j++)
			{
			for (k = 0; k < 3; k++)
				{
				outVal = fast_ftol((double)imgIn->Pixel(i, j).ch[k] * opFact + opAdd);
				if (outVal > 255)
					outVal = 255;
				if (outVal < 0)
					outVal = 0;
				imgOut->Pixel(i, j).ch[k] = (Byte)outVal;
				}
			}
		}
   }
else
   {
	for (i = boundRect->top; i < boundRect->bottom; i++)
		{
		for (j = boundRect->left; j < boundRect->right; j++)
			{
			for (k = 0; k < 3; k++)
				imgOut->Pixel(i, j).ch[k] = bLogic(imgIn->Pixel(i, j).ch[k], (Byte)opValue);
			}
		}
   }
}









