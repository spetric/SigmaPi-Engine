//---------------------------------------------------------------------------
#pragma hdrstop
#include "spWarping.h"
#include "AsmVars.h"
#include "spPixelArithmetics.hpp"
#include "spGeometry.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
void spWarp::pixInterpol(warpDataStruct *warpData, int &i, int &j, double &X_displ, double &Y_displ)
{
int  k, X_shift, Y_shift, new_bgr;
if (X_displ < 0.0)
	X_displ = 0.0;
if (Y_displ < 0.0)
	Y_displ = 0.0;
if (X_displ > (warpData->srcImage->Width() - 1))
	X_displ = warpData->srcImage->Width() - 1;
if (Y_displ > (warpData->srcImage->Height() - 1))
	Y_displ = warpData->srcImage->Height() - 1;
if (!warpData->antialias)
   {
   X_shift	= fast_ftol(X_displ);
   Y_shift	= fast_ftol(Y_displ);
   //
   // without those upper two lines, we get some strange patter...use it for something smart.
   //
   if ((X_shift + Y_shift) > 0)
		{
		for (k = 0; k < 3; k++)
			{
			if (warpData->Chan[k])
				{
				new_bgr =  (int)warpData->srcImage->Pixel(Y_shift, X_shift).ch[k] * warpData->mask_perc
						 + (int)warpData->dstImage->Pixel(i, j).ch[k] * (spe_SP_NORM - warpData->mask_perc);
				new_bgr = new_bgr>>spe_SP_SHIFT;
				warpData->dstImage->Pixel(i, j).ch[k] =  spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
				}
			}
		// alpha channel?
		if (warpData->srcAlpha)
		   {
		   new_bgr = (int)warpData->srcAlpha->Pixel(Y_shift, X_shift).ch[0] * warpData->mask_perc
				   + (int)warpData->dstAlpha->Pixel(i, j).ch[0] * (spe_SP_NORM - warpData->mask_perc);
		   new_bgr = new_bgr>>spe_SP_SHIFT;
		   warpData->dstAlpha->Pixel(i, j).ch[0] = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
		   }
		}
	return;
	}
double X_factor, Y_factor, out_value;
ptRGB Pix_1, Pix_2, Pix_3, Pix_4;
// interpolation on => go!
//TODO: a little optimisation would be nice
// pixels position
X_shift  = floor(X_displ);      // round down
Y_shift  = floor(Y_displ);
X_factor  = X_displ - X_shift;	// ponder
Y_factor  = Y_displ - Y_shift;
// first point (top-left)
Pix_1 =  warpData->srcImage->Pixel(Y_shift, X_shift);
// second point (top-right)
if (X_shift == (warpData->srcImage->Width() - 1))
   Pix_2 = Pix_1;
else
   Pix_2 =  warpData->srcImage->Pixel(Y_shift, X_shift + 1);
// third one (bottom-left)
if (Y_shift == (warpData->srcImage->Height() - 1))
   {
   Pix_3 = Pix_1;
   Pix_4 = Pix_2;
   }
else
   {
   Pix_3  = warpData->srcImage->Pixel(Y_shift + 1, X_shift);
   // fourth point (down-right)
   if (X_shift < (warpData->srcImage->Width() - 1))
	   Pix_4 = warpData->srcImage->Pixel(Y_shift + 1, X_shift + 1);
   }
for (k = 0; k < 3; k++)
	{
	if (warpData->Chan[k])
	   {
	   out_value = (Pix_1.ch[k]*(1.0 - X_factor) + Pix_2.ch[k]*X_factor) * (1.0 - Y_factor) + (Pix_3.ch[k]*(1.0 - X_factor) + Pix_4.ch[k]*X_factor) * Y_factor;
	   new_bgr = fast_ftol(out_value) * warpData->mask_perc
				+  warpData->dstImage->Pixel(i, j).ch[k] * (spe_SP_NORM - warpData->mask_perc);
	   new_bgr = new_bgr>>spe_SP_SHIFT;
	   warpData->dstImage->Pixel(i, j).ch[k] = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
	   }
	}
// alpha channel?
if (warpData->srcAlpha)
   {
   ptGray a_1, a_2, a_3, a_4;
   a_1 =  warpData->srcAlpha->Pixel(Y_shift, X_shift);
   if (X_shift == (warpData->srcAlpha->Width() - 1))
	   a_2 = a_1;
   else
	   a_2 =  warpData->srcAlpha->Pixel(Y_shift, X_shift + 1);
   if (Y_shift == (warpData->srcAlpha->Height() - 1))
	  {
	  a_3 = a_1;
	  a_4 = a_2;
	  }
   else
	  {
	  a_3  = warpData->srcAlpha->Pixel(Y_shift + 1, X_shift);
	  if (X_shift < (warpData->srcAlpha->Width() - 1))
		  a_4 = warpData->srcAlpha->Pixel(Y_shift + 1, X_shift + 1);
	  }
   out_value = (a_1.ch[0]*(1.0 - X_factor) + a_2.ch[0]*X_factor) * (1.0 - Y_factor) + (a_3.ch[k]*(1.0 - X_factor) + a_4.ch[0]*X_factor) * Y_factor;
   new_bgr = fast_ftol(out_value) * warpData->mask_perc
			+  warpData->dstAlpha->Pixel(i, j).ch[0] * (spe_SP_NORM - warpData->mask_perc);
   new_bgr = new_bgr>>spe_SP_SHIFT;
   warpData->dstAlpha->Pixel(i, j).ch[0] = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
   }
}
//---------------------------------------------------------------------------
void spWarp::restorePixel(warpDataStruct *warpData, int &i, int &j)
{
warpData->dstImage->Pixel(i, j) = warpData->srcImage->Pixel(i, j);
if (warpData->srcAlpha)
   warpData->dstAlpha->Pixel(i, j) = warpData->srcAlpha->Pixel(i, j);
}
//---------------------------------------------------------------------------
void spWarp::displBiLinear(double &xd, double &yd, double &xm, double &ym, double &xf, double &yf,  int &i, int &j, double &bf)
{
xd = (double)j - xm * xf*yf;
yd = (double)i - ym * xf*yf;
}
//---------------------------------------------------------------------------
void spWarp::displBiQuadratic(double &xd, double &yd, double &xm, double &ym, double &xf, double &yf,  int &i, int &j, double &bf)
{
xd = (double)j + xm * xf*yf*xf*yf * bf - xm * xf*yf;
yd = (double)i + ym * xf*yf*xf*yf * bf - ym * xf*yf;
}
//---------------------------------------------------------------------------
void spWarp::updateBB(warpDataStruct *WarpData, TRect *boundRect, int &iW, int &iH)
{
if (boundRect->left < WarpData->BoundRect.left)
   WarpData->BoundRect.left = boundRect->left;
if (boundRect->top < WarpData->BoundRect.top)
   WarpData->BoundRect.top = boundRect->top;
if (boundRect->right > WarpData->BoundRect.right)
   WarpData->BoundRect.right = boundRect->right;
if (boundRect->bottom > WarpData->BoundRect.bottom)
   WarpData->BoundRect.bottom = boundRect->bottom;
if (WarpData->BoundRect.left < 0)
   WarpData->BoundRect.left = 0;
if (WarpData->BoundRect.top < 0)
   WarpData->BoundRect.top = 0;
if (WarpData->BoundRect.right > iW)
   WarpData->BoundRect.right = iW;
if (WarpData->BoundRect.bottom > iH)
   WarpData->BoundRect.bottom = iH;
}
//---------------------------------------------------------------------------
// public
//---------------------------------------------------------------------------
void spWarp::Begin(warpDataStruct *WarpData)
{
WarpData->BoundRect.left = 99999;
WarpData->BoundRect.right = 0;
WarpData->BoundRect.top = 99999;
WarpData->BoundRect.bottom = 0;
}
//---------------------------------------------------------------------------
void spWarp::WarpLinearBox(warpDataStruct *WarpData)
{
double X_move, Y_move, X_displ, Y_displ, diag, X_factor, Y_factor;
int w = WarpData->srcImage->Width();
int h = WarpData->srcImage->Height();
TRect boundRect;
TWarpCalcDispl calcDispl;
// izraèunaj pomak
X_move	= (double)(WarpData->X2 - WarpData->X1);
Y_move	= (double)(WarpData->Y2 - WarpData->Y1);
// velièine kvadranata od nove toèkice
if ((fabs(X_move) < 1)&&(fabs(Y_move) < 1))
   {
   boundRect.left = WarpData->X1;
   boundRect.top = WarpData->Y1;
   boundRect.right = WarpData->X2;
   boundRect.bottom = WarpData->Y2;
   updateBB(WarpData, &boundRect, w, h);
   return;
   }
if (WarpData->bi_linear)
   calcDispl = &displBiLinear;
else
   calcDispl = &displBiQuadratic;
//
diag = sqrt(X_move*X_move + Y_move*Y_move) * WarpData->watract;
boundRect.left   = WarpData->X2 - ceil(diag);
boundRect.top    = WarpData->Y2 - ceil(diag);
boundRect.right	 = WarpData->X2 + ceil(diag);
boundRect.bottom = WarpData->Y2 + ceil(diag);
updateBB(WarpData, &boundRect, w, h);
// go and taear down the rectangle
for (int i = WarpData->BoundRect.top ; i < WarpData->BoundRect.bottom; i++)
   {
   for (int j = WarpData->BoundRect.left; j < WarpData->BoundRect.right; j++)
	   {
	   if (WarpData->SelByte & 0x01)
		  {
		  if (WarpData->selImage->Pixel(i, j).ch[0] == 0x00)
			 continue;
		  }
	   Y_factor	= 1.0 - abs(WarpData->Y2 - i)/diag;
	   X_factor = 1.0 - abs(WarpData->X2 - j)/diag;
	   if (X_factor < 0.0)
		   X_factor = 0.0;
	   if (Y_factor < 0.0)
		   Y_factor = 0.0;
	   calcDispl(X_displ, Y_displ, X_move, Y_move, X_factor, Y_factor, i, j, WarpData->bq_factor);
	   if (WarpData->SelByte & 0x02)
		  {
		  if (WarpData->selImage->Pixel((int)Y_displ, (int)X_displ).ch[0] == 0x00)
			 continue;
		  }
	   pixInterpol(WarpData, i, j, X_displ, Y_displ);
	   }
   }
}
//---------------------------------------------------------------------------
void spWarp::WarpLinearImage(warpDataStruct *WarpData)
{
double X_move, Y_move, X_displ, Y_displ, X_factor, Y_factor;
double h_up, h_down, w_left, w_right;
int w = WarpData->srcImage->Width();
int h = WarpData->srcImage->Height();
TRect boundRect;
TWarpCalcDispl calcDispl;
// calculate shift
X_move	= (double)(WarpData->X2 - WarpData->X1);
Y_move	= (double)(WarpData->Y2 - WarpData->Y1);
//D_height = (double)WarpData->srcImage->Height();
//D_width  = (double)WarpData->srcImage->Width();
// velièine kvadranata od nove toèkice
if ((fabs(X_move) < 1)&&(fabs(Y_move) < 1))
   {
   boundRect.left = WarpData->X1;
   boundRect.top = WarpData->Y1;
   boundRect.right = WarpData->X2;
   boundRect.bottom = WarpData->Y2;
   updateBB(WarpData, &boundRect, w, h);
   return;
   }
if (WarpData->bi_linear)
   calcDispl = &displBiLinear;
else
   calcDispl = &displBiQuadratic;
h_up   	 = (double)WarpData->Y2;
h_down	 = (double)abs(h - WarpData->Y2);
w_left	 = (double)WarpData->X2;
w_right  = (double)abs(w - WarpData->X2);
boundRect.left   = 0;
boundRect.top    = 0;
boundRect.right  = w;
boundRect.bottom = h;
updateBB(WarpData, &boundRect, w, h);
// tear down the rectangle
//
// It's better to do scan using boundRect...less operations.
//
for (int i = 0 ; i < h; i++)
   {
   for (int j = 0; j < w; j++)
	   {
	   if (WarpData->SelByte & 0x01)
		  {
		  if (WarpData->selImage->Pixel(i, j).ch[0] == 0x00)
			 continue;
		  }
	   if (i < WarpData->Y2)
		   Y_factor	= 1.0 - abs(WarpData->Y2 - i)/h_up;
	   else
		   Y_factor	= 1.0 - abs(WarpData->Y2 - i)/h_down;
	   if (j < WarpData->X2)
		   X_factor = 1.0 - abs(WarpData->X2 - j)/w_left;
	   else
		   X_factor	= 1.0 - abs(WarpData->X2 - j)/w_right;
	   if (X_factor < 0.0)
		   X_factor = 0.0;
	   if (Y_factor < 0.0)
		   Y_factor = 0.0;
	   calcDispl(X_displ, Y_displ, X_move, Y_move, X_factor, Y_factor, i, j, WarpData->bq_factor);
	   if (WarpData->SelByte & 0x02)
		  {
		  if (WarpData->selImage->Pixel((int)Y_displ, (int)X_displ).ch[0] == 0x00)
			 continue;
		  }
	   pixInterpol(WarpData, i, j, X_displ, Y_displ);
	   }
   }
}
//---------------------------------------------------------------------------
void spWarp::WarpSineSplash(warpDataStruct *WarpData)
{
//
// 	Radial cosine...what a term!
//
double X_move, Y_move, X_norig, Y_norig;
int b_sign;
double wl_move, wl_factor, r_move, r_factor, r_dump;
double X_displ, Y_displ;
int w = WarpData->srcImage->Width();
int h = WarpData->srcImage->Height();
TRect boundRect;
// pomak
X_move	= (double)(WarpData->X2 - WarpData->X1);
Y_move	= (double)(WarpData->Y2 - WarpData->Y1);
// the size of quadrant from new point
if ((fabs(X_move) < 1)&&(fabs(Y_move) < 1))
   {
   boundRect.left = WarpData->X1;
   boundRect.top = WarpData->Y1;
   boundRect.right = WarpData->X2;
   boundRect.bottom = WarpData->Y2;
   updateBB(WarpData, &boundRect, w, h);
   return;
   }
wl_move	  = sqrt(X_move*X_move + Y_move*Y_move);
// what mode is on?
switch (WarpData->mode)
	{
	case spe_WARP_SPLASH:               // splash
		wl_factor = wl_move/M_PI;
		boundRect.left	 = 0;
		boundRect.top    = 0;
		boundRect.right	 = w;
		boundRect.bottom = h;
		break;
	case spe_WARP_BLOB:            // blob
		b_sign = +1;
		wl_factor = wl_move/M_PI_2;
		boundRect.left   = WarpData->X1 - ceil(wl_factor*(M_PI_2));
		boundRect.top    = WarpData->Y1 - ceil(wl_factor*(M_PI_2));
		boundRect.right	 = WarpData->X1 + ceil(wl_factor*(M_PI_2));
		boundRect.bottom = WarpData->Y1 + ceil(wl_factor*(M_PI_2));
		break;
	case spe_WARP_BLUB:            // blub
		b_sign = -1;
		wl_factor = wl_move/M_PI_2;
		boundRect.left   = WarpData->X1 - ceil(wl_factor*(M_PI_2));
		boundRect.top 	 = WarpData->Y1 - ceil(wl_factor*(M_PI_2));
		boundRect.right  = WarpData->X1 + ceil(wl_factor*(M_PI_2));
		boundRect.bottom = WarpData->Y1 + ceil(wl_factor*(M_PI_2));
		break;
	}
updateBB(WarpData, &boundRect, w, h);
// go through bounding rect
for (int i = WarpData->BoundRect.top ; i < WarpData->BoundRect.bottom; i++)
	{
	for (int j = WarpData->BoundRect.left; j < WarpData->BoundRect.right; j++)
		{
		if (WarpData->SelByte & 0x01)
		   {
		   if (WarpData->selImage->Pixel(i, j).ch[0] == 0x00)
			  continue;
		   }
		X_norig	=  (double)(j - WarpData->X1);
		Y_norig	=  (double)(i - WarpData->Y1);
		r_move 	= sqrt(X_norig*X_norig + Y_norig*Y_norig);
		if (WarpData->mode == spe_WARP_SPLASH)
			{
			r_dump	= (r_move/(wl_factor*M_PI))+1;
			r_factor	= cos(r_dump*r_move/wl_factor);
			r_factor 	= r_factor/r_dump;
			}
		else
			{
			if (r_move > wl_move)
				r_factor = 0;
			else
				r_factor = b_sign*cos(r_move/wl_factor);
			}
		//
		X_displ	= (double)j - (X_norig* WarpData->watract)*r_factor;
		Y_displ = (double)i - (Y_norig* WarpData->watract)*r_factor;
		if (WarpData->SelByte & 0x02)
		   {
		   if (WarpData->selImage->Pixel((int)Y_displ, (int)X_displ).ch[0] == 0x00)
			  continue;
		   }
		pixInterpol(WarpData, i, j, X_displ, Y_displ);
		}
	}
}
//---------------------------------------------------------------------------
void spWarp::WarpSineRipple(warpDataStruct *WarpData)
{
//
//  Double cosine methid
//  Quite messy.
//
double X_worig, Y_worig, X_move, Y_move, X_norig, Y_norig;
int b_sign;
double wl_move, wl_factor, X_factor, Y_factor;
double X_displ, Y_displ, D_width, D_height, X_abs_move, Y_abs_move;
int w = WarpData->srcImage->Width();
int h = WarpData->srcImage->Height();

TRect boundRect;
// pick-point
X_worig	= (double)WarpData->X1;
Y_worig	= (double)WarpData->Y1;
// shift
X_move	= (double)(WarpData->X2 - WarpData->X1);
Y_move	= (double)(WarpData->Y2 - WarpData->Y1);
// the size of quadrats from new point
if ((fabs(X_move) < 1)&&(fabs(Y_move) < 1))
   {
   boundRect.left = WarpData->X1;
   boundRect.top = WarpData->Y1;
   boundRect.right = WarpData->X2;
   boundRect.bottom = WarpData->Y2;
   updateBB(WarpData, &boundRect, w, h);
   return;
   }
wl_factor = sqrt(X_move*X_move + Y_move*Y_move)/M_PI;
X_abs_move = fabs(X_move);
Y_abs_move = fabs(Y_move);
// correction
Y_worig += (int)(wl_factor*M_PI_2);
// what mode is on?
switch (WarpData->mode)
	{
	case spe_WARP_RIPPLES:
		boundRect.left	 = 0;
		boundRect.top    = 0;
		boundRect.right  = w;
		boundRect.bottom = h;
		break;
	case spe_WARP_LONG_RIPPLE:
		wl_factor = (double)std::max(h, w);
		wl_factor = wl_factor/(2*M_PI);
		boundRect.left   = 0;
		boundRect.top    = 0;
		boundRect.right	 = w;
		boundRect.bottom = h;
		break;
	case spe_WARP_BALLOON:
		if(X_abs_move > Y_abs_move)
            {
			boundRect.left = fast_ftol(X_worig - ceil(wl_factor*M_PI));
			boundRect.top = fast_ftol(Y_worig - ceil(wl_factor*M_PI));
			boundRect.right   = fast_ftol(X_worig + ceil(wl_factor*M_PI));
            boundRect.bottom   = fast_ftol(Y_worig + ceil(wl_factor*0));
            }
        else
			{
            boundRect.left = fast_ftol(X_worig - ceil(wl_factor*M_PI_2));
            boundRect.top = fast_ftol(Y_worig - ceil(wl_factor*(M_PI+M_PI_2)));
            boundRect.right   = fast_ftol(X_worig + ceil(wl_factor*M_PI_2));
            boundRect.bottom   = fast_ftol(Y_worig + ceil(wl_factor*M_PI_2));
			}
        break;
    case spe_WARP_DOUBLE_BALLOON:
       if(X_abs_move > Y_abs_move)
			{
            boundRect.left = fast_ftol(X_worig - ceil(wl_factor*M_PI));
            boundRect.top = fast_ftol(Y_worig - ceil(wl_factor*(2*M_PI)));
            boundRect.right   = fast_ftol(X_worig + ceil(wl_factor*M_PI));
            boundRect.bottom   = fast_ftol(Y_worig + ceil(wl_factor*M_PI));
            }
        else
            {
			boundRect.left = fast_ftol(X_worig - ceil(wl_factor*(M_PI+M_PI_2)));
			boundRect.top = fast_ftol(Y_worig - ceil(wl_factor*(M_PI+M_PI_2)));
			boundRect.right   = fast_ftol(X_worig + ceil(wl_factor*(M_PI+M_PI_2)));
			boundRect.bottom   = fast_ftol(Y_worig + ceil(wl_factor*M_PI_2));
			}
		break;
	}
updateBB(WarpData, &boundRect, w, h);
// go!!!
for (int i = WarpData->BoundRect.top ; i < WarpData->BoundRect.bottom; i++)
	{
	for (int j = WarpData->BoundRect.left; j < WarpData->BoundRect.right; j++)
		{
		if (i < boundRect.top || i >= boundRect.bottom || j < boundRect.Left || j >= boundRect.right)
		   {
		   restorePixel(WarpData, i, j);
		   continue;
           }
		if (WarpData->SelByte & 0x01)
		   {
		   if (WarpData->selImage->Pixel(i, j).ch[0] == 0x00)
			  continue;
		   }
		X_norig =  j - X_worig;
		Y_norig	=  i - Y_worig;
		if (X_abs_move > Y_abs_move)
		   {
		   X_factor	= sin(X_norig/wl_factor);
		   Y_factor	= sin(Y_norig/wl_factor);
		   }
		else
		   {
		   X_factor	= cos(X_norig/wl_factor);
		   Y_factor	= cos(Y_norig/wl_factor);
		   }
		X_displ	= (double)j - (X_move* WarpData->watract)*X_factor*Y_factor;
		Y_displ = (double)i - (Y_move* WarpData->watract)*X_factor*Y_factor;
		if (WarpData->SelByte & 0x02)
		   {
		   if (WarpData->selImage->Pixel((int)Y_displ, (int)X_displ).ch[0] == 0x00)
			  continue;
		   }
		pixInterpol(WarpData, i, j, X_displ, Y_displ);
		}
    }
}
//---------------------------------------------------------------------------
void spWarp::WarpTwirl(warpDataStruct *WarpData)
{
//TODO: optimise
double PIC_xx, PIC_yy, PIC_diag, RES_diag, ROI_diag, WORK_diag, NEW_diag;
double angle, step_angle;
//double inputAngle;
double radius, X_displ, Y_displ;
TPoint pStart, pEnd, pRoto;
TRect boundRect;
int w = WarpData->srcImage->Width();
int h = WarpData->srcImage->Height();
//
// Calculate twirl params
//
spGeometry::GetLineData(&ROI_diag, &angle, WarpData->X1, WarpData->Y1, WarpData->X2, WarpData->Y2);
if (ROI_diag < 1)
   {
   boundRect.left = WarpData->X1;
   boundRect.top = WarpData->Y1;
   boundRect.right = WarpData->X2;
   boundRect.bottom = WarpData->Y2;
   updateBB(WarpData, &boundRect, w, h);
   return;
   }
pStart.x = WarpData->X1;
pStart.y = WarpData->Y1;
pEnd.x   = WarpData->X2;
pEnd.y   = WarpData->Y2;
if (WarpData->X1 > (w >>1))
	PIC_xx = WarpData->X1 * WarpData->X1;
else
	PIC_xx = (w - 1 - WarpData->X1)*(w -1 -  WarpData->X1);
if (WarpData->Y1 > (h>>1))
	PIC_yy = WarpData->Y1 * WarpData->Y1;
else
	PIC_yy = (h - 1 - WarpData->Y1) * (h - 1 - WarpData->Y1);
PIC_diag = sqrt(PIC_yy + PIC_xx);
RES_diag = PIC_diag - ROI_diag;
ROI_diag = ceil(ROI_diag);
PIC_diag = ceil(PIC_diag);
RES_diag = ceil(RES_diag);
//inputAngle = angle;
switch (WarpData->mode)
	{
	case spe_WARP_INSIDE_TWIRL:
		step_angle = angle/ROI_diag;
		WORK_diag = ROI_diag;
		boundRect.left   = fast_ftol(WarpData->X1 - ROI_diag);
		boundRect.top    = fast_ftol(WarpData->Y1 - ROI_diag);
		boundRect.right  = fast_ftol(WarpData->X1 + ROI_diag);
		boundRect.bottom = fast_ftol(WarpData->Y1 + ROI_diag);
		break;
	case spe_WARP_OUTSIDE_TWIRL:
		step_angle = angle/RES_diag;
		WORK_diag = PIC_diag;
		boundRect.left   = 0;
		boundRect.top    = 0;
		boundRect.right	 = w;
		boundRect.bottom = h;
		break;
	case spe_WARP_FULL_TWIRL:
		step_angle = angle/PIC_diag;
		WORK_diag = PIC_diag;
		boundRect.left    = 0;
		boundRect.top     = 0;
		boundRect.right   = w;
		boundRect.bottom  = h;
        break;
	}
updateBB(WarpData, &boundRect, w, h);
// go!
for (int i = WarpData->BoundRect.top; i < WarpData->BoundRect.bottom; i++)
	{
	for (int j = WarpData->BoundRect.left; j < WarpData->BoundRect.right; j++)
		{
		if (WarpData->SelByte & 0x01)
		   {
		   if (WarpData->selImage->Pixel(i, j).ch[0] == 0x00)
			  continue;
		   }
		spGeometry::GetLineData(&radius, &angle, WarpData->X1, WarpData->Y1, j, i);
		NEW_diag = WORK_diag - radius;
		if (WarpData->mode == spe_WARP_OUTSIDE_TWIRL  && radius <= ROI_diag)
			NEW_diag = RES_diag;
		if (NEW_diag > 0)
			{
			pEnd.x = j;
			pEnd.y = i;
			spGeometry::GetRotateData(&pEnd, &pStart, /*angle*/ + step_angle * NEW_diag, &pRoto, false); //absolute
			X_displ = pRoto.x;
			Y_displ = pRoto.y;
			if (WarpData->SelByte & 0x02)
			   {
			   if (WarpData->selImage->Pixel((int)Y_displ, (int)X_displ).ch[0] == 0x00)
				  continue;
			   }
			pixInterpol(WarpData, i, j, X_displ, Y_displ);
			}
		}
	}
}
//---------------------------------------------------------------------------
void spWarp::WarpDisorder(warpDataStruct *WarpData)
{
int radius, diameter;
double fx, fy, fr, x_comp, y_comp, x_abs_comp, y_abs_comp, x_worig, y_worig, x_norig, y_norig;
//bool all_chans = true;
double ROI_diag, angle;
int interpol;
TRect boundRect;
int w = WarpData->srcImage->Width();
int h = WarpData->srcImage->Height();
spGeometry::GetLineData(&ROI_diag, &angle, WarpData->X1, WarpData->Y1, WarpData->X2, WarpData->Y2);
if (ROI_diag < 1)
   {
   boundRect.left = WarpData->X1;
   boundRect.top = WarpData->Y1;
   boundRect.right = WarpData->X2;
   boundRect.bottom = WarpData->Y2;
   updateBB(WarpData, &boundRect, w, h);
   return;
   }
radius = floor(ROI_diag) + 1; // why + 1?? I forgot it.
diameter = radius*2;
x_comp = WarpData->X2 - WarpData->X1;
y_comp = WarpData->Y2 - WarpData->Y1;
x_abs_comp = fabs(x_comp);
y_abs_comp = fabs(y_comp);
if (x_abs_comp < 0.01)
	x_comp = 1.0;
if (y_abs_comp < 0.01)
	y_comp = 1.0;
x_comp *= WarpData->watract;
y_comp *= WarpData->watract;
x_worig = WarpData->X1;
y_worig = WarpData->Y1;
boundRect.left = 0;
boundRect.top = 0;
boundRect.right = w;
boundRect.bottom = h;
updateBB(WarpData, &boundRect, w, h);
for (int i = 0; i < h; i++)
	{
	for (int j = 0; j < w; j++)
		{
		if (WarpData->SelByte & 0x01)
		   {
		   if (WarpData->selImage->Pixel(i, j).ch[0] == 0x00)
			  continue;
		   }
		switch (WarpData->mode)
		   {
		   case spe_WARP_DISORDER:      //explode uniform
				fr = (double)(radius - random(diameter));
				fr *= WarpData->watract;
				fx = (double)(j + fr);
				fr = (double)(radius - random(diameter));
				fr *= WarpData->watract;
				fy = (double)(i + fr);
				break;
		   case spe_WARP_MOTION:      // motion explode
				fr = (double)random(radius);
				fr *= WarpData->watract;
				if (x_comp < 0)
				   fx = (double)(j + fr*x_abs_comp/radius);
				else
				   fx = (double)(j - fr*x_abs_comp/radius);
				if (y_comp < 0)
				   fy = (double)(i + fr*y_abs_comp/radius);
				else
				   fy = (double)(i - fr*y_abs_comp/radius);
				break;
		   case spe_WARP_WAVES_HORIZONTAL:      // waves horiz
				fx = (double)j + sin(double(i)/y_comp)*x_comp;
				fy = (double)i;
				break;
		   case spe_WARP_WAVES_VERTICAL:      // waves verct
				fx = (double)j;
				fy = (double)i + sin(double(j)/x_comp)*y_comp;
				break;
		   case spe_WARP_EXPLODE:     // central explode
				fr = (double)random(radius);
				fr *= WarpData->watract;
				x_norig =  j - x_worig;
				y_norig	=  i - y_worig;
				fx = (double)(j - fr*x_norig/((w - 1)>>1));
				fy = (double)(i - fr*y_norig/((h - 1)>>1));
				break;
		   case spe_WARP_IMPLODE:     //central implode
				fr = (double)random(radius);
				fr *= WarpData->watract;
				x_norig =  j - x_worig;
				y_norig	=  i - y_worig;
				fx = (double)(j + fr*x_norig/((w - 1)>>1));
				fy = (double)(i + fr*y_norig/((h - 1)>>1));
				// diamond???
				//fx = (float)(j + fr*x_norig*y_norig);
				//fy = (float)(i + fr*y_norig*x_norig);
				break;
		   }
		if (WarpData->SelByte & 0x02)
		   {
		   if (WarpData->selImage->Pixel((int)fy, (int)fx).ch[0] == 0x00)
			  continue;
		   }
		pixInterpol(WarpData, i, j, fx, fy);
		}
	}
}




