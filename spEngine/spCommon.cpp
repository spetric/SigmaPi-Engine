//---------------------------------------------------------------------------
#pragma hdrstop
#include "spCommon.h"
#include "spGlobals.h"
#include "ASMVars.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
bool spCommon::ippAlive = false;
TRect spCommon::bigBoundRect = TRect();
//---------------------------------------------------------------------------
int spCommon::CalcVariation(int variation)
{
if (variation == 0)
	return 0;
return (variation - random(variation * 2));
}
//---------------------------------------------------------------------------
bool spCommon::CoinToss(bool coinOn)
{
if (!coinOn)
   return true; // no coin - return true
// toss coin
return (random(2) > 0);
}
//---------------------------------------------------------------------------
spCommon::spsFastBlurGray* spCommon::FBGrayPrepare(spImage<ptGray> *img, TRect *boundRect, int radius)
{
if (radius < 1)
	 return 0;
spsFastBlurGray *fbgData = new spsFastBlurGray;
fbgData->radius = radius;
fbgData->xs = boundRect ? boundRect->left : 0;
fbgData->ys = boundRect ? boundRect->top : 0;
fbgData->w  = boundRect ? boundRect->Width() : img->Width();
fbgData->h  = boundRect ? boundRect->Height() : img->Height();
fbgData->wm = fbgData->w - 1;
fbgData->hm = fbgData->h - 1;
fbgData->wh = fbgData->w * fbgData->h;
fbgData->div = radius + radius + 1;
fbgData->ch = new int[fbgData->wh];
fbgData->vmin = new int[max(fbgData->w, fbgData->h)];
fbgData->vmax = new int[max(fbgData->w, fbgData->h)];
fbgData->dv = new int[256 * fbgData->div];
for (int i = 0; i < 256 * fbgData->div; i++)
	 fbgData->dv[i]=(i/ fbgData->div);
fbgData->img = img;
return fbgData;
}
//---------------------------------------------------------------------------
void spCommon::FBGrayUnPrepare(spsFastBlurGray *fbgData)
{
if (!fbgData)
   return;
delete [](fbgData->ch);
delete [](fbgData->dv);
delete [](fbgData->vmin);
delete [](fbgData->vmax);
delete fbgData;
}
//---------------------------------------------------------------------------
void spCommon::FBGrayRun(spsFastBlurGray *fbgData)
{
if (!fbgData)
   return;
int chSum, x, y, i, yp, yi;
int p1, p2;
ptGray **pix = fbgData->img->Data();
yi = 0;

for (y = 0; y < fbgData->h; y++)
	{
	chSum = 0;
	for (i = -fbgData->radius; i <= fbgData->radius; i++)
		{
		chSum += (int)(pix[y + fbgData->ys][min(fbgData->wm, max(i, 0)) + fbgData->xs].ch[0]);
		}
	for (x = 0; x < fbgData->w; x++)
	  {
	  fbgData->ch[yi] = fbgData->dv[chSum];
	  if (y == 0)
		  {
		  fbgData->vmin[x] = min(x + fbgData->radius + 1, fbgData->wm);
		  fbgData->vmax[x] = max(x - fbgData->radius, 0);
		  }
	  p1 = (int)pix[y + fbgData->ys][fbgData->vmin[x] + fbgData->xs].ch[0];
	  p2 = (int)pix[y + fbgData->ys][fbgData->vmax[x] + fbgData->xs].ch[0];
	  chSum += p1 - p2;
	  yi++;
	  }
	}
for (x = 0; x < fbgData->w; x++)
	{
	chSum = 0;
	yp = -fbgData->radius * fbgData->w;
	for (i = -fbgData->radius; i <= fbgData->radius; i++)
	   {
	   yi = max(0, yp) + x;
	   if (yi >= fbgData->wh)
		  {
          //int mate = 23;
		  break;
		  }
	   chSum += fbgData->ch[yi];
	   yp += fbgData->w;
	   }
	yi = x;
	for (y = 0; y < fbgData->h ; y++)
	   {
	   pix[y + fbgData->ys][x + fbgData->xs] = ptGray(fbgData->dv[chSum]);
	   if (x == 0)
		  {
		  fbgData->vmin[y] = min(y + fbgData->radius+1, fbgData->hm) * fbgData->w;
		  fbgData->vmax[y] = max(y - fbgData->radius, 0) * fbgData->w;
		  }
	  p1 = x + fbgData->vmin[y];   //translate ??
	  p2 = x + fbgData->vmax[y];   //translate ??
	  chSum += fbgData->ch[p1] - fbgData->ch[p2];
	  yi += fbgData->w;
	  }
	}
}
//---------------------------------------------------------------------------
void spCommon::FBGrayRun(spImage<ptGray> *img, TRect *boundRect, int radius)
{
if (boundRect->Width() < 2 || boundRect->Height() < 2)
   return;
spsFastBlurGray *fbgData = FBGrayPrepare(img, boundRect, radius);
FBGrayRun(fbgData);
FBGrayUnPrepare(fbgData);
}
//---------------------------------------------------------------------------
void spCommon::DownsampleGray(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut)
{
// Algorithm: https://sigmapi-design.com/media/fast_box_filter.pdf
imgOut->Zero();
unsigned long norm = spe_SP_NORM;
unsigned long shift = spe_SP_SHIFT;
unsigned int x, y, yi, xi, xoc, yi_start, xi_start, yi_end, xi_end, xo_start, xo_end, yo_start, yo_end;
TRect iBound = imgIn->GetRoi();
TRect oBound = imgOut->GetRoi();
yi_start = iBound.top;
xi_start = iBound.left;
yi_end   = iBound.bottom;
xi_end   = iBound.right;
yo_start  = oBound.top;
xo_start = oBound.left;
yo_end   = oBound.bottom;
xo_end   = oBound.Right;
float fX = (float)iBound.Width() / (float)oBound.Width();
float fY = (float)iBound.Height() / (float)oBound.Height();
unsigned long pixContribX = (unsigned long)((float)norm / fX);
unsigned long pixContribY = (unsigned long)((float)norm / fY);
unsigned long sumContribX, sumContribY, restContribY, yContrib;
unsigned long outVal;
unsigned long *cumulY = new unsigned long [oBound.Width()];
bool outX, outY, outLastRow;
memset(cumulY, 0, oBound.Width()*sizeof(unsigned long));
//for (int i = 0; i < oBound.Width(); i++)
//    cumulY[i] = 0;
//
y = yo_start;
sumContribY = norm - pixContribY;
restContribY = 0;
yContrib = pixContribY;
outY = false;
outLastRow = false;
for (yi = yi_start; yi < yi_end; yi++)
	{
	x = xo_start;
	xoc = 0;
	outVal = 0;
	sumContribX = norm;
	for (xi = xi_start; xi < xi_end; xi++)
		{
		// first pass -> X
		outX = false;
		if (sumContribX > pixContribX)
		   {
		   outVal += (unsigned long)imgIn->Pixel(yi, xi).ch[0] * pixContribX;
		   sumContribX -= pixContribX;
		   continue;
		   }
		// rest of contribution factor (sumContrib < pixContrib)
		outVal += (unsigned long)imgIn->Pixel(yi, xi).ch[0] * sumContribX;
		// done - output pixel to y culmulative
		cumulY[xoc] += (outVal>>shift) * yContrib;
		if (outY)
		   {
		   // done - output pixel
		   imgOut->Pixel(y, x).ch[0] = (Byte)(cumulY[xoc] >>shift);
		   // contribution factor for next pixels block (pixContribY - sumContribY)
		   cumulY[xoc] = (outVal>>shift) * restContribY;
		   }
		// contribution factor for next pixels block (pixContribX - sumContribX)
		sumContribX = pixContribX - sumContribX;
		outVal = (unsigned long)imgIn->Pixel(yi, xi).ch[0] * sumContribX;
		sumContribX = norm - sumContribX;
		//
		outX = true;
		x++;
		xoc = x - xo_start;
		if (x >= xo_end)
		   break;
		}
	// output last pixel
	if (!outX)
	   {
	   cumulY[xoc] += (outVal>>shift)*yContrib;
	   if (outY)
		  {
		  // done - output pixel
		  imgOut->Pixel(y, x).ch[0] = (Byte)(cumulY[xoc] >>shift);
		  // contribution factor for next pixels block (pixContribY - sumContribY)
		  cumulY[xoc] = (outVal>>shift) * restContribY;
		  }
	   }
	if (outY)
	   {
	   if (y == yo_end-1)
		  {
		  outLastRow = true;
		  break;
		  }
	   y++;
	   }
	if (y >= yo_end)
	   break;
	outY = false;
	if (sumContribY > pixContribY)
	   {
	   yContrib = pixContribY;
	   sumContribY -= pixContribY;
	   continue;
	   }
	yContrib = sumContribY;
	restContribY = pixContribY - sumContribY;
	sumContribY = norm - restContribY;
	outY = true;
	}
// output last row
if (!outLastRow)
   {
   for (unsigned int xo = xo_start; xo < xo_end; xo++)
	   imgOut->Pixel(y, xo).ch[0] = (Byte)(cumulY[xo - xo_start] >>shift);
   }
delete []cumulY;
}
//---------------------------------------------------------------------------
void spCommon::DownsampleRGB(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut)
{
// Algorithm: https://sigmapi-design.com/media/fast_box_filter.pdf
imgOut->Zero();
unsigned long norm = spe_SP_NORM;
unsigned long shift = spe_SP_SHIFT;
unsigned int x, y, yi, xi, xoc, yi_start, xi_start, yi_end, xi_end, xo_start, xo_end, yo_start, yo_end;
TRect iBound = imgIn->GetRoi();
TRect oBound = imgOut->GetRoi();
yi_start = iBound.top;
xi_start = iBound.left;
yi_end   = iBound.bottom;
xi_end   = iBound.right;
yo_start  = oBound.top;
xo_start = oBound.left;
yo_end   = oBound.bottom;
xo_end   = oBound.Right;
float fX = (float)iBound.Width() / (float)oBound.Width();
float fY = (float)iBound.Height() / (float)oBound.Height();
unsigned long pixContribX = (unsigned long)((float)norm / fX);
unsigned long pixContribY = (unsigned long)((float)norm / fY);
unsigned long sumContribX, sumContribY, restContribY, yContrib;
unsigned long outVal[3];
unsigned long *cumulY[3];
for (int i = 0; i < 3; i++)
	{
	cumulY[i] = new unsigned long [oBound.Width()];
	memset(cumulY[i], 0, oBound.Width() * sizeof(unsigned long));
	}
bool outX, outY, outLastRow;
y = yo_start;
sumContribY = norm - pixContribY;
restContribY = 0;
yContrib = pixContribY;
outY = false;
outLastRow = false;
for (yi = yi_start; yi < yi_end; yi++)
	{
	x = xo_start;
	xoc = 0;
	outVal[0] = outVal[1] = outVal[2] = 0;
	sumContribX = norm;
	for (xi = xi_start; xi < xi_end; xi++)
		{
		// first pass -> X
		outX = false;
		if (sumContribX > pixContribX)
		   {
		   outVal[0] += (unsigned long)imgIn->Pixel(yi, xi).ch[0] * pixContribX;
		   outVal[1] += (unsigned long)imgIn->Pixel(yi, xi).ch[1] * pixContribX;
		   outVal[2] += (unsigned long)imgIn->Pixel(yi, xi).ch[2] * pixContribX;
		   sumContribX -= pixContribX;
		   continue;
		   }
		// rest of contribution factor (sumContrib < pixContrib)
		outVal[0] += (unsigned long)imgIn->Pixel(yi, xi).ch[0] * sumContribX;
		outVal[1] += (unsigned long)imgIn->Pixel(yi, xi).ch[1] * sumContribX;
		outVal[2] += (unsigned long)imgIn->Pixel(yi, xi).ch[2] * sumContribX;
		// done - output pixel to y culmulative
		cumulY[0][xoc] += (outVal[0]>>shift) * yContrib;
		cumulY[1][xoc] += (outVal[1]>>shift) * yContrib;
		cumulY[2][xoc] += (outVal[2]>>shift) * yContrib;
		if (outY)
		   {
		   // done - output pixel
		   imgOut->Pixel(y, x).ch[0] = (Byte)(cumulY[0][xoc] >>shift);
		   imgOut->Pixel(y, x).ch[1] = (Byte)(cumulY[1][xoc] >>shift);
		   imgOut->Pixel(y, x).ch[2] = (Byte)(cumulY[2][xoc] >>shift);
		   // contribution factor for next pixels block (pixContribY - sumContribY)
		   cumulY[0][xoc] = (outVal[0]>>shift) * restContribY;
		   cumulY[1][xoc] = (outVal[1]>>shift) * restContribY;
		   cumulY[2][xoc] = (outVal[2]>>shift) * restContribY;
		   }
		// contribution factor for next pixels block (pixContribX - sumContribX)
		sumContribX = pixContribX - sumContribX;
		outVal[0] = (unsigned long)imgIn->Pixel(yi, xi).ch[0] * sumContribX;
		outVal[1] = (unsigned long)imgIn->Pixel(yi, xi).ch[1] * sumContribX;
		outVal[2] = (unsigned long)imgIn->Pixel(yi, xi).ch[2] * sumContribX;
		sumContribX = norm - sumContribX;
		//
		outX = true;
		x++;
		xoc = x - xo_start;
		if (x >= xo_end)
		   break;
		}
	// output last pixel
	if (!outX)
	   {
	   cumulY[0][xoc] += (outVal[0]>>shift)*yContrib;
	   cumulY[1][xoc] += (outVal[1]>>shift)*yContrib;
	   cumulY[2][xoc] += (outVal[2]>>shift)*yContrib;
	   if (outY)
		  {
		  // done - output pixel
		  imgOut->Pixel(y, x).ch[0] = (Byte)(cumulY[0][xoc] >>shift);
		  imgOut->Pixel(y, x).ch[1] = (Byte)(cumulY[1][xoc] >>shift);
		  imgOut->Pixel(y, x).ch[2] = (Byte)(cumulY[2][xoc] >>shift);
		  // contribution factor for next pixels block (pixContribY - sumContribY)
		  cumulY[0][xoc] = (outVal[0]>>shift) * restContribY;
		  cumulY[1][xoc] = (outVal[1]>>shift) * restContribY;
		  cumulY[2][xoc] = (outVal[2]>>shift) * restContribY;
		  }
	   }
	if (outY)
	   {
	   if (y == yo_end-1)
		  {
		  outLastRow = true;
		  break;
		  }
	   y++;
	   }
	if (y >= yo_end)
	   break;
	outY = false;
	if (sumContribY > pixContribY)
	   {
	   yContrib = pixContribY;
	   sumContribY -= pixContribY;
	   continue;
	   }
	yContrib = sumContribY;
	restContribY = pixContribY - sumContribY;
	sumContribY = norm - restContribY;
	outY = true;
	}
// output last row
if (!outLastRow)
   {
   for (unsigned int xo = xo_start; xo < xo_end; xo++)
	   {
	   imgOut->Pixel(y, xo).ch[0] = (Byte)(cumulY[0][xo - xo_start] >>shift);
	   imgOut->Pixel(y, xo).ch[1] = (Byte)(cumulY[1][xo - xo_start] >>shift);
	   imgOut->Pixel(y, xo).ch[2] = (Byte)(cumulY[2][xo - xo_start] >>shift);
	   }
   }
for (int i = 0; i < 3; i++)
	delete []cumulY[i];
}
//---------------------------------------------------------------------------
void spCommon::ResizeBilinearGray(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut)
{
int x, y, gray ;
int h = imgIn->Height();
int w = imgIn->Width();
int h2 = imgOut->Height();
int w2 = imgOut->Width();
//
float A, B, C, D;
float x_ratio = ((float)(w-1))/w2 ;
float y_ratio = ((float)(h-1))/h2 ;
float x_diff, y_diff, ya, yb ;
//
for (int i = 0; i < h2 ; i++)
	{
	for (int j = 0; j < w2; j++)
		{
		x = (int)(x_ratio * j);
		y = (int)(y_ratio * i);
		//x = fast_ftol(x_ratio * j);
		//y = fast_ftol(y_ratio * i);
		x_diff = (x_ratio * j) - x ;
		y_diff = (y_ratio * i) - y ;
		// range is 0 to 255 thus bitwise AND with 0xff
		A = imgIn->Pixel(y, x).ch[0];
		B = imgIn->Pixel(y, x + 1).ch[0];
		C = imgIn->Pixel(y + 1, x).ch[0];
		D = imgIn->Pixel(y + 1, x + 1).ch[0];

		// Y = A(1-w)(1-h) + B(w)(1-h) + C(h)(1-w) + Dwh
		//gray = (int)(A*(1-x_diff)*(1-y_diff) +  B*(x_diff)*(1-y_diff) +
		//			C*(y_diff)*(1-x_diff)   +  D*(x_diff*y_diff)) ;
		gray = fast_ftol(A*(1-x_diff)*(1-y_diff) +  B*(x_diff)*(1-y_diff) +
					C*(y_diff)*(1-x_diff)   +  D*(x_diff*y_diff));
		imgOut->Pixel(i, j).ch[0] = (Byte)gray;
		}
	}
}
//---------------------------------------------------------------------------
void spCommon::ResizeBilinearRGB(spImage<ptRGB> *imgIn, spImage<ptRGB> *imgOut)
{
int x, y, rgb[3];
int h = imgIn->Height();
int w = imgIn->Width();
int h2 = imgOut->Height();
int w2 = imgOut->Width();
//
float A[3], B[3], C[3], D[3];
float x_ratio = ((float)(w-1))/w2 ;
float y_ratio = ((float)(h-1))/h2 ;
float x_diff, y_diff, ya, yb ;
//
for (int i = 0; i < h2 ; i++)
	{
	for (int j = 0; j < w2; j++)
		{
		x = (int)(x_ratio * j);
		y = (int)(y_ratio * i);
		x_diff = (x_ratio * j) - x ;
		y_diff = (y_ratio * i) - y ;
		// range is 0 to 255 thus bitwise AND with 0xff
		for (int k  = 0; k < 3; k++)
			{
			A[k] = imgIn->Pixel(y, x).ch[k];
			B[k] = imgIn->Pixel(y, x + 1).ch[k];
			C[k] = imgIn->Pixel(y + 1, x).ch[k];
			D[k] = imgIn->Pixel(y + 1, x + 1).ch[k];

			rgb[k] = fast_ftol(A[k]*(1-x_diff)*(1-y_diff) +  B[k]*(x_diff)*(1-y_diff) +
						C[k]*(y_diff)*(1-x_diff)   +  D[k]*(x_diff*y_diff));
			imgOut->Pixel(i, j).ch[k] = (Byte)rgb[k];
			}
		}
	}
}
//---------------------------------------------------------------------------
void spCommon::AddWeighted(spImage<ptRGB> *imgIn1, float aIn1, spImage<ptRGB> *imgIn2, float aIn2, spImage<ptRGB> *imgOut, TRect *commonRoi)
{
//NOTE: aIn1 + aIn2 <= 255.0
if ((aIn1 + aIn2) > 255.0)
   return;
imgOut->Zero();
unsigned long norm = spe_SP_NORM;
unsigned long shift = spe_SP_SHIFT;
unsigned int yi1, xi1, yi1_start, xi1_start, yi1_end, xi1_end;
unsigned int yi2, xi2, yi2_start, xi2_start, yi2_end, xi2_end;
unsigned int yo, xo, xo_start, xo_end, yo_start, yo_end;
unsigned long alpha1 = fast_ftol(aIn1 * norm/255.0);
unsigned long alpha2 = fast_ftol(aIn2 * norm/255.0);
unsigned long outVal[3];
if (commonRoi)
   {
   yi1_start = commonRoi->top;
   xi1_start = commonRoi->left;
   yi1_end   = commonRoi->bottom;
   xi1_end   = commonRoi->right;
   for (yi1 = yi1_start; yi1 < yi1_end; yi1++)
	   {
	   outVal[0] = outVal[1] = outVal[2] = 0;
	   for (xi1 = xi1_start; xi1 < xi1_end; xi1++)
		   {
		   outVal[0] += (unsigned long)imgIn1->Pixel(yi1, xi1).ch[0] * alpha1 + (unsigned long)imgIn2->Pixel(yi1, xi1).ch[0] * alpha2;
		   outVal[1] += (unsigned long)imgIn1->Pixel(yi1, xi1).ch[1] * alpha1 + (unsigned long)imgIn2->Pixel(yi1, xi1).ch[1] * alpha2;
		   outVal[2] += (unsigned long)imgIn1->Pixel(yi1, xi1).ch[2] * alpha1 + (unsigned long)imgIn2->Pixel(yi1, xi1).ch[2] * alpha2;
		   imgOut->Pixel(yi1, xi1).ch[0] = (Byte)(outVal[0]>>shift);
		   imgOut->Pixel(yi1, xi1).ch[1] = (Byte)(outVal[1]>>shift);
		   imgOut->Pixel(yi1, xi1).ch[2] = (Byte)(outVal[2]>>shift);
		   }
	   }
   }
else
   {
   TRect iB1, iB2, oB;
   iB1 = imgIn1->GetRoi();
   iB2 = imgIn2->GetRoi();
   oB  = imgOut->GetRoi();

   yi1_start = iB1.top;
   xi1_start = iB1.left;
   yi1_end   = iB1.bottom;
   xi1_end   = iB1.right;
   yi2_start = iB2.top;
   xi2_start = iB2.left;
   yi2_end   = iB2.bottom;
   xi2_end   = iB2.right;
   yo_start  = oB.top;
   xo_start  = oB.left;
   yo_end    = oB.bottom;
   xo_end    = oB.right;
   for (yi1 = yi1_start, yi2 = yi2_start, yo = yo_start; yi1 < yi1_end || yi2 < yi2_end || yo < yo_end ; yi1++, yi2++, yo++)
	   {
	   outVal[0] = outVal[1] = outVal[2] = 0;
	   for (xi1 = xi1_start, xi2 = xi2_start, xo = xo_start; xi1 < xi1_end || xi2 < xi2_end || xo < xo_end; xi1++, xi2++, xo++)
		   {
		   outVal[0] += (unsigned long)imgIn1->Pixel(yi1, xi1).ch[0] * alpha1 + (unsigned long)imgIn2->Pixel(yi2, xi2).ch[0] * alpha2;
		   outVal[1] += (unsigned long)imgIn1->Pixel(yi1, xi1).ch[1] * alpha1 + (unsigned long)imgIn2->Pixel(yi2, xi2).ch[1] * alpha2;
		   outVal[2] += (unsigned long)imgIn1->Pixel(yi1, xi1).ch[2] * alpha1 + (unsigned long)imgIn2->Pixel(yi2, xi2).ch[2] * alpha2;
		   imgOut->Pixel(yo, xo).ch[0] = (Byte)(outVal[0]>>shift);
		   imgOut->Pixel(yo, xo).ch[1] = (Byte)(outVal[1]>>shift);
		   imgOut->Pixel(yo, xo).ch[2] = (Byte)(outVal[2]>>shift);
		   }
	   }
   }
}
//---------------------------------------------------------------------------
int spCommon::BinarySearch(double sortedArray[], int first, int last, double key, bool closest)
{
int mid;
while (first <= last)
   {
   mid = (first + last) / 2;  // calculate mid point
   if (key > sortedArray[mid])
	   first = mid + 1;       // upper part
   else if (key < sortedArray[mid])
	   last = mid - 1;        // lower part
   else
	   return mid;            // bool's eye, get out!!!
   }
// Nothing found. But, if option is to find closest, go and get 'im!
if (closest)
   {
   if (fabs(sortedArray[mid] - sortedArray[first]) < fabs(sortedArray[last] - sortedArray[mid]))
      return first;
   else
      return last;
   }
return -(first + 1);    // nope...I can't find 'im.
}
//---------------------------------------------------------------------------
void spCommon::Palette3C(ptRGB *palEntry, unsigned int botC, unsigned int midC, unsigned int topC)
{
float bgrs_fact[3], float_bgrs;
// default gradient (some rainbow colors)
int i, k;
palEntry[0]   = ptRGB(botC);
palEntry[127] = ptRGB(midC);
palEntry[255] = ptRGB(topC);
for (k = 0; k < 3; k++)
	{
	bgrs_fact[k] = (float)palEntry[127].ch[k] -
				   (float)palEntry[0].ch[k];
	bgrs_fact[k] /= 128;
	}
for (i = 0; i < 128; i++)
	{
	for (k = 0; k < 3; k++)
		{
		palEntry[i].ch[k] = palEntry[0].ch[k];
		float_bgrs = (float)(palEntry[i].ch[k]) + bgrs_fact[k] * i;
		if (float_bgrs < 0)
			float_bgrs = 0;
		palEntry[i].ch[k] = (BYTE)(fast_ftol(float_bgrs));
		}
	}
for (k = 0; k < 3; k++)
    {
	bgrs_fact[k] = (float)palEntry[255].ch[k] -
                   (float)palEntry[127].ch[k];
	bgrs_fact[k] /= 128;
	}
for (i = 127; i < 256; i++)
    {
    for (k = 0; k < 3; k++)
        {
		palEntry[i].ch[k] = palEntry[127].ch[k];
		float_bgrs = (float)(palEntry[i].ch[k]) + bgrs_fact[k] * (i - 127) ;
		if (float_bgrs < 0)
			float_bgrs = 0;
		palEntry[i].ch[k] = (BYTE)(fast_ftol(float_bgrs));
        }
    }
}
//---------------------------------------------------------------------------
void spCommon::PaletteShortPalC(ptRGB *palEntry, unsigned int *shortPal, int shortPalCount)
{
float bgrs_fact[3], float_bgrs, range;
range = 256.0/(float)(shortPalCount-1);
// create gradient between color
int jj, i, imul, k, pindex[16];
pindex[0] = 0;
palEntry[0]   = ptRGB(shortPal[0]);
for (jj = 1; jj < shortPalCount; jj++)
	{
	if (jj == (shortPalCount - 1))
	   pindex[jj] = 255;
	else
	   pindex[jj] = fast_ftol(jj*range) - 1;
	palEntry[pindex[jj]]   = ptRGB(shortPal[jj]);
	for (k = 0; k < 3; k++)
		{
		bgrs_fact[k] = (float)palEntry[pindex[jj]].ch[k] -
					   (float)palEntry[pindex[jj-1]].ch[k];
		bgrs_fact[k] /= range;
		}
	for (i = pindex[jj-1] + 1, imul = 1; i < pindex[jj]; i++, imul++)
		{
		for (k = 0; k < 3; k++)
			{
			palEntry[i].ch[k] = palEntry[pindex[jj-1]].ch[k];
			float_bgrs = (float)(palEntry[i].ch[k]) + bgrs_fact[k] * imul;
			if (float_bgrs < 0.0)
				float_bgrs = 0.0;
			else if (float_bgrs > 255.0)
    			float_bgrs = 255.0;
			palEntry[i].ch[k] = (BYTE)(fast_ftol(float_bgrs));
			}
		}
	}
}
//---------------------------------------------------------------------------
// median filter grayscale
//---------------------------------------------------------------------------
void spCommon::medDelPixels(spImage<ptGray> *im, int row, int col, int size, spsHistogram *h, TRect *boundRect)
{
int i;
/*
if (col < 0 || col >= im->Width())
   return;
for (i = row - size; i <= row + size && i < im->Height(); i++)
	{
	if (i < 0)
	   continue;
	h->value[im->Pixel(i, col).ch[0]]--;
	h->n--;
	}
*/
if (col < boundRect->left || col >= boundRect->right)
   return;
for (i = row - size; i <= row + size && i < boundRect->bottom; i++)
	{
	if (i < boundRect->top)
	   continue;
	h->value[im->Pixel(i, col).ch[0]]--;
	h->n--;
	}
}
//---------------------------------------------------------------------------
void spCommon::medAddPixels(spImage<ptGray> *im, int row, int col, int size, spsHistogram *h, TRect *boundRect)
{
int i;
/*
if (col < 0 || col >= im->Width())
   return;
for (i = row - size; i <= row + size && i < im->Height(); i++)
	{
	if (i < 0)
	   continue;
	h->value[im->Pixel(i, col).ch[0]]++;
	h->n++;
	}
*/
if (col < boundRect->left || col >= boundRect->right)
   return;
for (i = row - size; i <= row + size && i < boundRect->bottom; i++)
	{
	if (i < boundRect->top)
	   continue;
	h->value[im->Pixel(i, col).ch[0]]++;
	h->n++;
	}
}
//---------------------------------------------------------------------------
void spCommon::medInitHistogram(spImage<ptGray> *im, int row, int size, spsHistogram *h,  TRect *boundRect)
{
int j;
memset(h, 0, sizeof(spsHistogram));
//for (j = 0; j < size && j < im->Width(); j++)
for (j = boundRect->left; j <= (boundRect->left + size) && j < boundRect->right; j++)
	medAddPixels(im, row, j, size, h, boundRect);
}
//---------------------------------------------------------------------------
void spCommon::MedianGray(spImage<ptGray> *im, TRect * boundRect, int size)
{
if (boundRect->Width() == 0 || boundRect->Height() == 0)
   return;
spImage<ptGray> *out = new spImage<ptGray>(boundRect->Width(), boundRect->Height());
int row, col, i, n;
spsHistogram h;
TRect saveRoi = im->GetRoi();
im->SetRoi(boundRect);
out->Zero();
for (row = boundRect->top; row < boundRect->bottom; row ++)
	{
	for (col = boundRect->left; col < boundRect->right; col++)
		{
		if (col == boundRect->left)
		   medInitHistogram(im, row, size, &h, boundRect);
		else
		   {
		   medDelPixels(im, row, col - size - 1, size, &h, boundRect);
		   medAddPixels(im, row, col + size, size, &h, boundRect);
		   }
		n = h.n >> 1;
		for (i = 0; i < 256 && (n -= h.value[i]) > 0; i++);
		out->Pixel(row - boundRect->top, col - boundRect->left).ch[0] = (Byte)i;
		}
	}
im->CopyRoi(out);
im->SetRoi(&saveRoi);
delete out;
}
//---------------------------------------------------------------------------
void spCommon::BoxBlurGray(spImage<ptGray> *img, int radius)
{
TRect boundRect(0, 0, img->Width(), img->Height());
BoxBlurGray(img, &boundRect, radius);
}
//---------------------------------------------------------------------------
void spCommon::BoxBlurGray(spImage<ptGray> *img, TRect *boundRect, int radius)
{
if (boundRect->Width() < 1 || boundRect->Height() < 1)
   return;
// hor
int* newPixVal;
int y, x, oldPixel, newPixel;
int w = img->Width();
int h = img->Height();
int hits, cumVal, index;
//
ptGray **pix = img->Data();
newPixVal = new int[boundRect->Width()];
memset(newPixVal, 0, boundRect->Width()*sizeof(int));
// step 1  - hor
for (y = boundRect->top; y < boundRect->bottom; y++)
	{
	hits = 0;
	cumVal = 0;
	index = 0;
	for (x = boundRect->left - radius; x < boundRect->right; x++)
		{
		oldPixel = x - radius - 1;
		if (oldPixel >= boundRect->left)
			{
			cumVal -= pix[y][oldPixel].ch[0];
			hits--;
			}
		newPixel = x + radius;
		if (newPixel < boundRect->right)
			{
			cumVal += pix[y][newPixel].ch[0];
			hits++;
			}
		if (hits != 0 && x >= boundRect->left)
		   {
		   newPixVal[index] = fast_ftol((double)cumVal / hits);
           index ++;
		   }
		}
	for (int x = boundRect->left; x < boundRect->right; x++)
		pix[y][x].ch[0] = (Byte)newPixVal[x - boundRect->left];
	}
// step 2  - vert
delete []newPixVal;
newPixVal = new int[boundRect->Height()];
memset(newPixVal, 0, boundRect->Height()*sizeof(int));
for (x = boundRect->left; x < boundRect->right; x++)
	{
	hits = 0;
	cumVal = 0;
    index = 0;
	for (y = boundRect->top - radius; y < boundRect->bottom; y++)
		{
		oldPixel = y - radius - 1;
		if (oldPixel >= boundRect->top)
			{
			cumVal -= pix[oldPixel][x].ch[0];
			hits--;
			}
		newPixel = y + radius;
		if (newPixel < boundRect->bottom)
			{
			cumVal += pix[newPixel][x].ch[0];
			hits++;
			}
		if (hits != 0 && y >= boundRect->top)
		   {
		   newPixVal[index] = fast_ftol((double)cumVal / hits);
           index++;
		   }
		}
	for (y = boundRect->top; y < boundRect->bottom; y++)
		pix[y][x].ch[0] = (Byte)newPixVal[y - boundRect->top];
	}
delete []newPixVal;
}
//---------------------------------------------------------------------------
void spCommon::CollectCCD(spImage<ptRGB> *im, TRect *boundRect, double *cumulBGR, double *distanceBGR, spImage<ptGray> *mask)
{
int z, i, j, k;
double d;
z = boundRect->Width() * boundRect->Height();
//
cumulBGR[0] = cumulBGR[1] = cumulBGR[2] = 0;
//
for (i = boundRect->top; i <  boundRect->bottom; i++)
	{
	for (j = boundRect->left; j < boundRect->right; j++)
		{
		if (mask)
		   {
		   if (mask->Pixel(i, j).ch[0] == 0x00)
			  {
			  z--;
			  continue;
			  }
		   }
		for (k = 0; k < 3; k++)
			cumulBGR[k] += (double)im->Pixel(i, j).ch[k];
		}
	}
if (z <= 0)	// it was mask and mask was empty
   return;
for (k = 0; k < 3; k++)
	cumulBGR[k] = cumulBGR[k] / (double)z;
//
distanceBGR[0] = distanceBGR[1] = distanceBGR[2] = 0;
for (i = boundRect->top; i <  boundRect->bottom; i++)
	{
	for (j = boundRect->left; j < boundRect->right; j++)
		{
		if (mask)
		   {
		   if (mask->Pixel(i, j).ch[0] == 0x00)
			  continue;
		   }
		for (k = 0; k < 3; k++)
			{
			d = (double)im->Pixel(i, j).ch[k] - cumulBGR[k];
			distanceBGR[k] += (d * d);
			}
		}
	}
for (k = 0; k < 3; k++)
	{
	if (distanceBGR[k] > 0)
		distanceBGR[k] = sqrt(distanceBGR[k] / z);
	}
}
//---------------------------------------------------------------------------
// Partitions list (essentially "sorting")  - private
int spCommon::qsPartition(double theList[], int assocList[], int start, int end)
{
double pivot = theList[end];
int assoc_pivot;
if (assocList != NULL)
	assoc_pivot = assocList[end];
int bottom = start-1;
int top = end;
bool notdone = true;
while (notdone)
	{
	while (notdone)
		{
		bottom += 1;
		if (bottom == top)
		   {
		   notdone = false;
		   break;
		   }
		if (theList[bottom] > pivot)
		   {
		   theList[top] = theList[bottom];
		   if (assocList != NULL)
			  assocList[top] = assocList[bottom];
		   break;
		   }
		}
	while (notdone)
		{
		top = top-1;
		if (top == bottom)
		   {
		   notdone = false;
		   break;
		   }
		if (theList[top] < pivot)
		   {
		   theList[bottom] = theList[top];
		   if (assocList != NULL)
			  assocList[bottom] = assocList[top];
		   break;
		  }
	   }
	}
theList[top] = pivot;
if (assocList != NULL)
	assocList[top] = assoc_pivot;
return top;
}
//---------------------------------------------------------------------------
// Actual function call within program - public
int spCommon::QuickSort(double theList[], int assocList[], int start, int end)
{
if (start < end)
   {
   int split = qsPartition(theList, assocList, start, end);   //recursion
   QuickSort(theList, assocList, start, split-1);
   QuickSort(theList, assocList, split+1, end);
   }
else
   {
   return 0;
   }
return 1;
}
//---------------------------------------------------------------------------
void spCommon::CreateLuminanceData(double *palLuminance, int *palIndex,  ptRGB *palEntry, int palCount)
{
int i, j, k, m, pal_index;
double pix_lumina;
// prepare pallet for sorting
for (i = 0; i < palCount; i++)
	{
	palLuminance[i] = 0;
	for (k = 0; k < 3; k++)
		palLuminance[i] += palEntry[i].ch[k] * spe_D_LUMINANCE[k];
	palIndex[i] = i;
	}
QuickSort(palLuminance, palIndex, 0, palCount-1);
}
//---------------------------------------------------------------------------
// Blend two images into output
//---------------------------------------------------------------------------
void spCommon::BlendImages(spImage<ptRGB> *imgIn1, spImage<ptRGB> *imgIn2, int nIn2, spImage<ptRGB> *imgOut)
{
int nIn1 = spe_SP_NORM - nIn2;
TRect in1Rect = imgIn1->GetRoi();
TRect in2Rect = imgIn2->GetRoi();
TRect outRect = imgOut->GetRoi();
ptRGB in1Pix, in2Pix, outPix;
int i1, j1;
int i2 = in2Rect.top, oi = outRect.top;
int j2, oj;
for (i1 = in1Rect.top; i1 < in1Rect.bottom; i1++)
	{
	if (i2 >= in2Rect.bottom || oi >= outRect.bottom)
	   break;
	j2 = in2Rect.left;
	oj = outRect.left;
	for (j1 =  in1Rect.left; j1 < in1Rect.right; j1++)
		{
		if (j2 >= in2Rect.right || oj >= outRect.right)
		   continue;
		in1Pix = imgIn1->Pixel(i1, j1);
		in2Pix = imgIn2->Pixel(i2, j2);
		outPix.ch[0] = (in1Pix.ch[0] * nIn1 + in2Pix.ch[0] * nIn2)>>spe_SP_SHIFT;
		outPix.ch[1] = (in1Pix.ch[1] * nIn1 + in2Pix.ch[1] * nIn2)>>spe_SP_SHIFT;
		outPix.ch[2] = (in1Pix.ch[2] * nIn1 + in2Pix.ch[2] * nIn2)>>spe_SP_SHIFT;
		//outPix = in1Pix * nIn1 + in2Pix * nIn2;
		imgOut->Pixel(oi, oj) = outPix;
		++j2;
		++oj;
		}
	++i2;
	++oi;
	}
}
//---------------------------------------------------------------------------
// Blend image with color under mask and set result to output image (same size as input)
//---------------------------------------------------------------------------
void spCommon::BlendImageMaskColor(spImage<ptRGB> *imgIn, spImage<ptGray> *imgMask, ptRGB color, spImage<ptRGB> *imgOut)
{
TRect inRect = imgIn->GetRoi();
ptRGB inPix, outPix;
int maskPerc, holePerc;
for (int i = inRect.top; i < inRect.bottom; i++)
	{
	for (int j =  inRect.left; j < inRect.right; j++)
		{
		inPix  = imgIn->Pixel(i, j);
		maskPerc = (((int)imgMask->Pixel(i, j).ch[0])<<spe_SP_SHIFT)>>8;
		holePerc = spe_SP_NORM - maskPerc;
		outPix.ch[0] = (inPix.ch[0] * maskPerc + color.ch[0] * holePerc)>>spe_SP_SHIFT;
		outPix.ch[1] = (inPix.ch[1] * maskPerc + color.ch[1] * holePerc)>>spe_SP_SHIFT;
		outPix.ch[2] = (inPix.ch[2] * maskPerc + color.ch[2] * holePerc)>>spe_SP_SHIFT;
		//outPix = in1Pix * nIn1 + in2Pix * nIn2;
		imgOut->Pixel(i, j) = outPix;
		}
	}
}
//---------------------------------------------------------------------------
// Past image onto image (usually small image input - nozzle)
//---------------------------------------------------------------------------
void spCommon::PasteImage(spImage<ptRGB> *imgIn, spImage<ptGray> *alphaIn, spImage<ptRGB> *imgOut, spImage<ptGray> *alphaOut, int nIn)
{
// NOTE: input roi must have correct boundaries.
int blendOut = spe_SP_NORM - nIn, blendIn;
TRect inRect = imgIn->GetRoi();
TRect outRect = imgOut->GetRoi();
ptRGB inPix, outPix;
Byte aInByte, aOutByte;
int i, j;
int oi = outRect.top;
int oj;
// check borders manually
for (i = inRect.top; i < inRect.bottom; i++)
	{
	if (oi >= outRect.bottom || oi >= imgOut->Height())
	   break;
	if (oi < 0)
	   {
	   ++oi;
	   continue;
	   }
	oj = outRect.left;
	for (j =  inRect.left; j < inRect.right; j++)
		{
		if (oj >= outRect.right ||oj >= imgOut->Width())
		   break;
		if (oj < 0)
		   {
		   ++oj;
		   continue;
		   }
		inPix  = imgIn->Pixel(i, j);
		outPix = imgOut->Pixel(oi, oj);
		// test
		aInByte  = alphaIn ? alphaIn->Pixel(i, j).ch[0] : 0xff;
		aOutByte = alphaOut ? alphaOut->Pixel(oi, oj).ch[0] : 0xff;
		if (aInByte == 0x00)
		   {
		   ++oj;
		   continue;
		   }
		blendIn =  (nIn * aInByte)>>8;
		blendOut = ((spe_SP_NORM - blendIn) * aOutByte)>>8;
		outPix.ch[0] = (inPix.ch[0] * blendIn + outPix.ch[0] * blendOut)>>spe_SP_SHIFT;
		outPix.ch[1] = (inPix.ch[1] * blendIn + outPix.ch[1] * blendOut)>>spe_SP_SHIFT;
		outPix.ch[2] = (inPix.ch[2] * blendIn + outPix.ch[2] * blendOut)>>spe_SP_SHIFT;
		imgOut->Pixel(oi, oj) = outPix;
		// merge alpha output
		if (alphaOut)
		   {
		   aOutByte = aInByte > aOutByte  ? aInByte : aOutByte;
		   alphaOut->Pixel(oi, oj).ch[0] = aOutByte;
		   }
		++oj;
		}
	++oi;
	}
}
//---------------------------------------------------------------------------
void spCommon::UpdateMaskHigh(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut, TRect *boundRect)
{
for (int i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (int j = boundRect->left; j < boundRect->right; j++)
		{
		if (imgIn->Pixel(i, j).ch[0] > imgOut->Pixel(i, j).ch[0])
			imgOut->Pixel(i, j).ch[0] = imgIn->Pixel(i, j).ch[0];
		}
	}
}
//---------------------------------------------------------------------------
void spCommon::HardMask(spImage<ptGray> *mask, TRect *boundRect)
{
for (int i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (int j = boundRect->left; j < boundRect->right; j++)
		{
		if (mask->Pixel(i, j).ch[0] > 0x00)
			mask->Pixel(i, j).ch[0] = 0xff;
		}
	}
}
//---------------------------------------------------------------------------
void spCommon::ResetBigBound(void)
{
bigBoundRect.left = 99999;
bigBoundRect.right = 0;
bigBoundRect.top = 99999;
bigBoundRect.bottom = 0;
}
//---------------------------------------------------------------------------
void spCommon::UpdateBigBound(TRect *boundRect)
{
if (boundRect->left < bigBoundRect.left)
   bigBoundRect.left = boundRect->left;
if (boundRect->top < bigBoundRect.top)
   bigBoundRect.top = boundRect->top;
if (boundRect->right > bigBoundRect.right)
   bigBoundRect.right = boundRect->right;
if (boundRect->bottom > bigBoundRect.bottom)
   bigBoundRect.bottom = boundRect->bottom;
}
//---------------------------------------------------------------------------
TRect spCommon::GetPolyBound(TPoint *polyPts, int polyCount)
{
TRect boundRect;
int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
for (int i = 0; i < polyCount; i++)
	{
	if (polyPts[i].x < minX)
		minX = polyPts[i].x;
	if (polyPts[i].y < minY)
	   minY = polyPts[i].y;
	if (polyPts[i].x > maxX)
	   maxX = polyPts[i].x;
	if (polyPts[i].y > maxY)
	   maxY = polyPts[i].y;
	}
boundRect.left = minX;
boundRect.top  = minY;
boundRect.right = maxX;
boundRect.bottom = maxY;
return boundRect;
}
//---------------------------------------------------------------------------
TRect spCommon::GetBigBound(int iW, int iH)
{
if (bigBoundRect.left < 0)
   bigBoundRect.left = 0;
if (bigBoundRect.top < 0)
   bigBoundRect.top = 0;
if (bigBoundRect.right > iW)
   bigBoundRect.right = iW;
if (bigBoundRect.bottom > iH)
   bigBoundRect.bottom = iH;
return bigBoundRect;
}
//---------------------------------------------------------------------------
// rotate image around center
void spCommon::RotateBilinearGray(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut, double angle)
{
// angle in radians
int h = imgIn->Height();
int w = imgIn->Width();
int h2 = imgOut->Height();
int w2 = imgOut->Width();
int oy = (h>>1);
int ox = (w>>1);
int x, y, gray;
double xd, yd;
double A, B, C, D;
double x_diff, y_diff, ya, yb ;
double cv = cos(angle);
double sv = sin(angle);
imgOut->Zero();
for (int i = 0; i < h2; i++)
	{
	for (int j = 0; j < w2; j++)
		{
		xd = (ox + (j - ox)*cv - (i - oy)*sv);
		yd = (oy + (j - ox)*sv + (i - oy)*cv);
		x = floor(xd);
		y = floor(yd);
		if (x < 0 || x >= (w - 1) || y < 0 || y >= (h - 1))
		   continue;
		x_diff =  xd - x ;
		y_diff =  yd - y ;
		A = imgIn->Pixel(y, x).ch[0];
		B = imgIn->Pixel(y, x + 1).ch[0];
		C = imgIn->Pixel(y + 1, x).ch[0];
		D = imgIn->Pixel(y + 1, x + 1).ch[0];
		gray = fast_ftol(A*(1-x_diff)*(1-y_diff) +  B*(x_diff)*(1-y_diff) +
					C*(y_diff)*(1-x_diff)   +  D*(x_diff*y_diff));
		imgOut->Pixel(i, j).ch[0] = (Byte)gray;
		//imgOut->Pixel(i, j).ch[0] = imgIn->Pixel(y, x).ch[0];  // test no interpolation
		}
	}
}
//---------------------------------------------------------------------------
// resize stamp and maintain aspect ratio
void spCommon::ResizeStampMar(spImage<ptGray> *imgIn, spImage<ptGray> *imgOut, int size)
{
double fw = 1.0 , fh = 1.0;
int shiftx = 0, shifty = 0;
if (imgIn->Width() > imgIn->Height())
   {
   fh = (double)imgIn->Height() / imgIn->Width();
   shifty = (fast_ftol(size * (fw - fh))) >> 1;
   }
else
   {
   fw = (double)imgIn->Width() / imgIn->Height();
   shiftx = (fast_ftol(size * (fh - fw))) >> 1;
   }
spImage<ptGray> *temp = new spImage<ptGray>(fast_ftol(size * fw), fast_ftol(size * fh));
spCommon::ResizeBilinearGray(imgIn, temp);
imgOut->Zero();
imgOut->SetRoi(shiftx, shifty, size - shiftx, size - shifty);
imgOut->CopyRoi(temp);
imgOut->SetRoi();
delete temp;
}







