//Copyright (C) 2016 Siniša Petriæ
#ifndef SP_PIXEL_ARITHMETICS_H
#define SP_PIXEL_ARITHMETICS_H
#include "spGlobals.h"
//#include "spImage.hpp"
//#include "spPixelTypes.hpp"
#include "spByteLogic.h"
//#include "brushBasic.h"
#include <vector>
#include <algorithm>
#include "ASMVars.h"
class spPixelArithmetics
  {
  public:
	// structures
	template <class T>
	struct spsFilter
	   {
	   // 3x3 filter sutructure
	   bool wrap;
	   bool chanOn[3];
	   float kernel[9];
	   float div;
	   float bias;
	   T nbh[9];
	   // logic filter
	   int  logic[9];
	   int  sign[9];
	   };
	template <class T>
	struct spsArith
	   {
	   // standard arithmetics
	   T topVal;
	   T botVal;
	   bool invertPixel;
	   int bitShift;
	   // bumping paremeters
	   T bumpVal;
	   float bumpScale;
	   // erase match color
	   T testVal;
	   T repVal;
	   int tolerance;
	   bool matchAll;
	   // apply on desired channel - external filter (ipp)
	   bool chanOn[3];
	   };
	template <class T>
	struct spsColor
	   {
	   // color casting
	   T castVal[3];
	   bool castOn[3];
	   float castRange[2];
	   // color mapping
	   double mapFactor[3];
	   bool  mapRelative;
	   // color scaling
	   float scaleRange[3][2];
	   // color correction
	   double SrcCumul[3];
	   double SrcDistance[3];
	   double DstCumul[3];
	   double DstDistance[3];
	   };
	template <class T>
	struct Execute
		{
		typedef void (*ArithFunc)(const T &, const T &, T &, spsArith<T> *);
		typedef void (*FilterFunc)(const T &, const T &, T &, spsFilter<T> *);
		typedef void (*ColorFunc)(const T &, const T &, T &, spsColor<T> *);
		};
  //---------------------------------------------------------------------------
  // color normalization - standard
  //---------------------------------------------------------------------------
  static Byte NormColor(int colNew, Byte colBot, Byte colTop)
	{
	//
	// 	Function for color normalization:
	//	ColNew < colBot    => ColNew = colBot
	//	ColNew > colTop    => ColNew = colTop
	//
	if (colTop <= colBot)
		return 0x00;
	if (colNew > ((int)colTop))
		return colTop;
	if (colNew < ((int)colBot))
		return colBot;
	return (Byte)colNew;
	}
  //---------------------------------------------------------------------------
  // color normalization - standard (template) - maybe it can come hany
  //---------------------------------------------------------------------------
  template <class T>
  static void NormColor(T &outVal, T &inpVal, T &botVal, T &topVal)
	{
	//
	// 	Function for color normalization:
	//	outVal < colBot    => outVal = colBot
	//	outVal > colTop    => outVal = colTop
	//
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (topVal.ch[k] <= botVal.ch[k])
			outVal.ch[k] =  0;
		else if (inpVal.ch[k] > topVal.ch[k])
			outVal.ch[k] = topVal.ch[k];
		else if (inpVal.ch[k] < botVal.ch[k])
			outVal.ch[k] = botVal.ch[k];
		else
			outVal.ch[k] = inpVal.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  // color normalization - circular
  //---------------------------------------------------------------------------
  static Byte CircColor(int colNew, Byte &colBot, Byte &colTop)
	{
	//
	//	Function for color normalization:
	//	colNew < colBot   => colTop - diff until reaches the limit.
	//	colNew > colTop   => colBot + diff until reaches the limit.
	//
	if (colTop <= colBot)
		return 0x00;
	int interval, colLow, colHig;
	Byte colPix;
	interval = (int)colTop - (int)colBot;
	if (colNew > ((int)colTop))
		{
		colHig = colNew - (int)colTop;
		colNew = (int) colBot + (colHig % interval);
		}
	if (colNew < ((int)colBot))
		{
		colLow = (int)colBot - colNew;
		colNew = (int)colTop - (colLow % interval);
		}
	colPix = (BYTE)colNew;
	return colPix;
	}
  //---------------------------------------------------------------------------
  // color normalization - swap
  //---------------------------------------------------------------------------
  static Byte SwapColor(int colNew, Byte colBot, Byte colTop)
	{
	//
	//	Function for color normalization:
	//	colNew < colBot	=> colNew = colTop
	//	colNew > colTop	=> colNew = colBot
	//
	if (colTop <= colBot)
		return 0x00;
	Byte colPix;
	if (colNew > ((int)colTop)) colPix = colBot;
	else if (colNew < ((int)colBot)) colPix = colTop;
	else colPix = (BYTE)colNew;
	return colPix;
	}
  //---------------------------------------------------------------------------
  // color normalization - swap
  //---------------------------------------------------------------------------
  static Byte ScaleColor(int colNew, Byte colBot, Byte colTop)
	{
	if (colBot == 0x00 && colTop == 0xff)
	   return NormColor(colNew, colBot, colTop);
	//
	//	Function for color scaling:
	//
	//
	//  	    (top - bot)ch
	//  f(x) = --------------  + bot
	//		      255.0
	//
	if (colTop <= colBot)
		return 0x00;
	Byte colPix;
	colPix = (Byte)fast_ftol(((float)colTop - (float)colBot) * (float)colNew / 255.0 + (float)colBot);
	return colPix;
	}
  //---------------------------------------------------------------------------
  // RGB to HSV conversion
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_RGB_HSV(T &val, float *hsv)
	{
	float v, s, h, delta, r, g, b, rgb_max, rgb_min;
	r = (float)val.ch[2]  / 255.0;
	g = (float)val.ch[1]  / 255.0;
	b = (float)val.ch[0]  / 255.0;
	rgb_max = std::max(std::max(r, g), b);
	rgb_min = std::min(std::min(r, g), b);
	delta = rgb_max - rgb_min;
	v = rgb_max;
	hsv[2] = v;
	if ((delta > 0)&&(rgb_max >0))
		s = delta / rgb_max;
	else
	   {
	   s = 0;
	   h = -1;                    // undefined
	   hsv[0] = h;
	   hsv[1] = s;
	   return;
	   }
	if (r == rgb_max)
		h = (g - b)/delta;
	else if (g == rgb_max)
		h = 2 + (b - r)/delta;
	else
		h = 4 + ( r - g ) / delta;
	h = h*60;
	if (h < 0)
		h += 360;
	hsv[0] = h;
	hsv[1] = s;
	}
  //---------------------------------------------------------------------------
  // RGB to HSV conversion
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_HSV_RGB(T &val, float *hsv)
	{
	int k, i;
	float f, p, q, t, r, g, b, h, s, v;
	h = hsv[0];
	s = hsv[1];
	v = hsv[2];
	if ( s == 0 && h == -1)  // undefined
	  {
	  for (k = 0; k < 3; k++)
		   val.ch[k] = (Byte)fast_ftol(v*255.0);
	  return;
	  }
	h /= 60;                        // sector 0 to 5
	i = floor( h );
	f = h - i;                      // factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i )
		{
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
	   case 2:
			r = p;
			g = v;
			b = t;
			break;
	   case 3:
			r = p;
			g = q;
			b = v;
			break;
	   case 4:
			r = t;
			g = p;
			b = v;
			break;
	   default:
			r = v;
			g = p;
			b = q;
			break;
	   }
	val.ch[0] = (Byte)fast_ftol(b*255.0);
	val.ch[1] = (Byte)fast_ftol(g*255.0);
	val.ch[2] = (Byte)fast_ftol(r*255.0);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Temperature_RGB(double temperature, T &val)
	{
	/*
	Algorithm: Tanner Helland, http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
	Start with a temperature, in Kelvin, somewhere between 1000 and 40000.  (Other values may work,
	but I can't make any promises about the quality of the algorithm's estimates above 40000 K.)
	Note also that the temperature and color variables need to be declared as floating-point.
	*/
	temperature = temperature / 100.0;
	double red, green, blue;
	// calculate red
	if (temperature <= 66)
		red = 255;
	else
		{
		red = temperature - 60.0;
		red = 329.698727446 * pow(red, -0.1332047592);
		if (red< 0)
		   red = 0;
		else if (red > 255)
		   red = 255;
		}
	// calculate green
	if (temperature <= 66)
	   {
	   green = temperature;
	   green = 99.4708025861 * log(green) - 161.1195681661;
	   if (green < 0)
		   green = 0;
	   else if (green > 255)
		   green = 255;
	   }
	else
	   {
	   green = temperature - 60;
	   green = 288.1221695283 * pow(green, -0.0755148492);
	   if (green < 0)
		  green = 0;
	   if (green > 255)
		  green = 255;
	   }
	// calculate blue
	if (temperature >= 66)
	   blue = 255;
	else
	   {
	   if (temperature <= 19)
		  blue = 0;
	   else
		  {
		  blue = temperature - 10;
		  blue = 138.5177312231 * log(blue) - 305.0447927307;
		  if (blue < 0)
			 blue = 0;
		  if (blue > 255)
			 blue = 255;
		  }
	   }
	val.ch[0] = (Byte)fast_ftol(blue);
	val.ch[1] = (Byte)fast_ftol(green);
	val.ch[2] = (Byte)fast_ftol(red);
	}
  //---------------------------------------------------------------------------
  // pixel arithmetics block
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_NORM(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	// NOP
	outVal = iVal2;
	}
    //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_NORM_CHAN(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (arith->chanOn[k])
			outVal.ch[k] = iVal2.ch[k];
		else
			outVal.ch[k] = iVal1.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_SCALED(const T &iVal1, const T &iVal2, T &outVal,  spsArith<T> *arith)
	{
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_XOR(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith) ^ (int)iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_AND(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith) & (int)iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_OR(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith) | (int)iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_ADD(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith) + (int)iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_SUB(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith) - (int)iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_MULT(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith) * (int)iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_MULT_nA(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	size_t  k, i;
	Byte max_value = 0x00;
	for (k = 0; k < sizeof(T); k++)
		{
		if (iVal1.ch[k] > max_value)
		   max_value = iVal1.ch[k];
		}
	for (k = 0, i = 3; k < 3; k++, i++)
		{
		if (max_value > 0x00)
		   outVal.ch[k] = ScaleColor(fast_ftol((float)BitShiftInvert(iVal1.ch[k], arith) * (float)iVal2.ch[k] / (float) max_value), arith->botVal.ch[k], arith->topVal.ch[k]);
		else
		   outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_MULT_nB(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	size_t  k, i;
	Byte max_value = 0x00;
	for (k = 0; k < sizeof(T); k++)
		{
		if (iVal2.ch[k] > max_value)
		   max_value = iVal2.ch[k];
		}
	for (k = 0, i = 3; k < 3; k++, i++)
		{
		if (max_value > 0x00)
		   outVal.ch[k] = ScaleColor(fast_ftol((float)BitShiftInvert(iVal1.ch[k], arith) * (float)iVal2.ch[k] / (float) max_value), arith->botVal.ch[k], arith->topVal.ch[k]);
		else
		   outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Higher(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (iVal1.ch[k] > iVal2.ch[k])
		   outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith), arith->botVal.ch[k], arith->topVal.ch[k]);
		else
		   outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Lower(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (iVal1.ch[k] < iVal2.ch[k])
		   outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith), arith->botVal.ch[k], arith->topVal.ch[k]);
		else
		   outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Mean(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(fast_ftol(((float)BitShiftInvert(iVal1.ch[k], arith) + (float)iVal2.ch[k])/2.0), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_MinMax(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	Byte b_max_value, e_max_value, b_min_value, e_min_value ;
	size_t k, e_max_elem, e_min_elem;
	bool gotMinMax;
	b_max_value = e_max_value = 0x00;
	b_min_value = e_min_value = 0xff;
	for (k = 0; k < sizeof(T); k++)
		{
		// maxs
		if (iVal1.ch[k] > b_max_value)
			b_max_value = iVal1.ch[k];
		if (iVal2.ch[k] > e_max_value)
		   {
		   e_max_value = iVal2.ch[k];
		   e_max_elem = k;
		   }
		// mins
		if (iVal1.ch[k] < b_min_value)
			b_min_value = iVal1.ch[k];
		if (iVal2.ch[k] < e_min_value)
		   {
		   e_min_value = iVal2.ch[k];
		   e_min_elem = k;
		   }
		}
	for (k = 0;  k < sizeof(T); k++)
		{
		gotMinMax = ((e_max_value < b_max_value) && (e_max_elem == k)) || ((e_min_value > b_min_value) && (e_min_elem == k));
		if (gotMinMax)
		   outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith), arith->botVal.ch[k], arith->topVal.ch[k]);
		else
		   outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Random(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	if (random(2) == 0)
	   {
	   outVal = iVal2;
	   return;
	   }
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Equalize(const T &iVal1, const T &iVal2,  T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float intensityA = 0.0, intensityB = 0.0;
	size_t k;
	float aByte;
	for (k = 0; k < sizeof(T); k++)
		{
		intensityA += (float)iVal1.ch[k];
		intensityB += (float)iVal2.ch[k];
		}
	intensityA = intensityA/(3*255.0);
	intensityB = intensityB/(3*255.0);
	for (k = 0; k < sizeof(T); k++)
		{
		aByte = BitShiftInvert(iVal1.ch[k], arith) *intensityB;
		aByte += (float)iVal2.ch[k] * intensityA;
		outVal.ch[k] = ScaleColor(fast_ftol(aByte),  arith->botVal.ch[k], arith->topVal.ch[k]);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Burner(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float intensityA = 0.0, intensityB = 0.0;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		intensityA += (float)iVal1.ch[k];
		intensityB += (float)iVal2.ch[k];
		}
	if (intensityB > 0)
	   intensityB = intensityA/intensityB;
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(fast_ftol((float)BitShiftInvert(iVal1.ch[k], arith) *intensityB), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_IntensityB(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float intensityB = 0;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		intensityB += (float)iVal2.ch[k];
	intensityB = intensityB /(3*255.0);
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(fast_ftol((float)BitShiftInvert(iVal1.ch[k], arith) *intensityB), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_IntensityA(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float intensityA = 0;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		intensityA += (float)BitShiftInvert(iVal1.ch[k], arith);
	intensityA = intensityA /(3*255.0);
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(fast_ftol((float)iVal2.ch[k] * intensityA), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_A_MOD_B(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (iVal2.ch[k] > 0x00)
			outVal.ch[k] = ScaleColor(BitShiftInvert(iVal1.ch[k], arith) % (int)iVal2.ch[k], arith->botVal.ch[k], arith->topVal.ch[k]);
		else
        	outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_AxB(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	int a_vect[3], b_vect[3], vect_comp[3];
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		a_vect[k] = BitShiftInvert(iVal1.ch[k], arith);
		b_vect[k] = (int)iVal2.ch[k];
		}
	vect_comp[2] = a_vect[1] * b_vect[0] - a_vect[0] *  b_vect[1];
	vect_comp[1] = a_vect[0] * b_vect[2] - a_vect[2] *  b_vect[0];
	vect_comp[0] = a_vect[2] * b_vect[1] - a_vect[1] *  b_vect[2];
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (outVal.ch[k] > 0x00)
			outVal.ch[k] = ScaleColor(vect_comp[k] > 0 ? vect_comp[k] : -vect_comp[k], arith->botVal.ch[k], arith->topVal.ch[k]);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Bump(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	if (arith->bumpScale < 0.001f)
	   return;
	float from_len = 0.0f, color_len = 0.0f;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		from_len  = max(from_len,  (float)BitShiftInvert(iVal1.ch[k], arith));
		color_len = max(color_len, (float)iVal2.ch[k]); // shift color
		}
	from_len  *= arith->bumpScale;
	color_len *= arith->bumpScale;
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(fast_ftol((float)arith->bumpVal.ch[k] + (from_len - color_len)), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_BumpColor(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	if (arith->bumpScale < 0.001f)
	   return;
	float from_len = 0.0f, color_len = 0.0f;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		// možda obratno...radi efekta shift-anja
		from_len  = max(from_len,  (float)BitShiftInvert(iVal1.ch[k], arith));
		color_len = max(color_len, (float)iVal2.ch[k]); // shift color
		}
	from_len  *= arith->bumpScale;
	color_len *= arith->bumpScale;
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(fast_ftol((float)iVal2.ch[k] + (from_len - color_len)), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_BurnLinear(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor((int)iVal2.ch[k] + BitShiftInvert(iVal1.ch[k], arith) - 255,  arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_SqrtAB(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float aByte;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		aByte  =  (float)BitShiftInvert(iVal1.ch[k], arith);
		aByte *= aByte;
		aByte += (float)iVal2.ch[k] * (float)iVal2.ch[k];
		outVal.ch[k] = ScaleColor(fast_ftol(sqrt(aByte)),  arith->botVal.ch[k], arith->topVal.ch[k]);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_DodgeColor(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float aByte;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		outVal.ch[k] = iVal2.ch[k];
		if (iVal1.ch[k] == 0xff)
		   continue;
		//aByte = (float)pt_maper[k] /(255.0 -  (float)pt_bgr[k]);
		aByte = (float)iVal2.ch[k] /(255.0 -  (float)BitShiftInvert(iVal1.ch[k], arith));
		outVal.ch[k] = ScaleColor(fast_ftol(aByte),  arith->botVal.ch[k], arith->topVal.ch[k]);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Screen(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float aByte;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		aByte = 255.0 - (255.0 - (float)BitShiftInvert(iVal1.ch[k], arith)) * (255.0 -  (float)iVal2.ch[k]);
		outVal.ch[k] = ScaleColor(fast_ftol(aByte),  arith->botVal.ch[k], arith->topVal.ch[k]);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_AbsSub(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		outVal.ch[k] = ScaleColor(abs(BitShiftInvert(iVal1.ch[k], arith) - (int)iVal2.ch[k]), arith->botVal.ch[k], arith->topVal.ch[k]);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Exclusion(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	float aByte;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		outVal.ch[k] = iVal2.ch[k];
		if (iVal1.ch[k] == 0xff)
		   continue;
		aByte = 127.0 - 2.0*((float)BitShiftInvert(iVal1.ch[k], arith) - 127.0) * ((float)iVal2.ch[k] - 127.0);
		outVal.ch[k] = ScaleColor(fast_ftol(aByte),  arith->botVal.ch[k], arith->topVal.ch[k]);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_DIV(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (iVal2.ch[k] > 0x00)
			outVal.ch[k] = ScaleColor(fast_ftol((float)BitShiftInvert(iVal1.ch[k], arith) / (float)iVal2.ch[k]), arith->botVal.ch[k], arith->topVal.ch[k]);
		else
			outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_DIV_nA(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsArith<T> *arith = static_cast <spsArith<T>*>(data);
	Byte max_value = 0x00;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		if (iVal1.ch[k] > max_value)
			max_value = iVal1.ch[k];
		}
	for (k = 0; k < sizeof(T); k++)
		{
		if (iVal2.ch[k] > 0x00)
		   outVal.ch[k] = ScaleColor(fast_ftol((double)BitShiftInvert(iVal1.ch[k], arith) * (double)max_value / (double)iVal2.ch[k]), arith->botVal.ch[k], arith->topVal.ch[k]);
		else
		   outVal.ch[k] = iVal2.ch[k];
		}
	}
  //---------------------------------------------------------------------------
  // pixel color block
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Erase_MatchColor(const T &iVal1, const T &iVal2, T &outVal, spsArith<T> *arith)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	int  pEdit, pTest;
	if (arith->matchAll)
	   {
	   outVal = arith->repVal;
	   return;
	   }
	bool match = true;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		pEdit = (int)iVal1.ch[k];
		pTest = (int)arith->testVal.ch[k];
		match = match && (pEdit <= (pTest + arith->tolerance) && pEdit >= (pTest - arith->tolerance));
		}
	if (match)
	   outVal = arith->repVal;
	else
	   outVal = iVal1;
	}
  template <class T>
  //---------------------------------------------------------------------------
  static void Pixel_Color_NORM(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	// NOP
	outVal = iVal2;
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MaxRG(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	if (outVal.ch[2] > outVal.ch[1])
		outVal.ch[1] = outVal.ch[2];
	else
		outVal.ch[2] = outVal.ch[1];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MaxGB(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	if (outVal.ch[1] > outVal.ch[0])
		outVal.ch[0] = outVal.ch[1];
	else
		outVal.ch[1] = outVal.ch[0];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MaxBR(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	if (outVal.ch[2] > outVal.ch[0])
		outVal.ch[0] = outVal.ch[2];
	else
		outVal.ch[2] = outVal.ch[0];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MaxRGB(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	Byte max_byte = 0x00;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		if (iVal1.ch[k] > max_byte)
		   max_byte = iVal1.ch[k];
		}
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = max_byte;
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MinRG(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	if (outVal.ch[2] < outVal.ch[1])
		outVal.ch[1] = outVal.ch[2];
	else
		outVal.ch[2] = outVal.ch[1];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MinGB(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	if (outVal.ch[1] < outVal.ch[0])
		outVal.ch[0] = outVal.ch[1];
	else
		outVal.ch[1] = outVal.ch[0];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MinBR(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	if (outVal.ch[2] < outVal.ch[0])
		outVal.ch[0] = outVal.ch[2];
	else
		outVal.ch[2] = outVal.ch[0];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_MinRGB(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	Byte min_byte = 0xff;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		if (iVal1.ch[k] < min_byte)
		   min_byte = iVal1.ch[k];
		}
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = min_byte;
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_ExcRG(const T &iVal1, const T &iVal2,  T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	outVal.ch[1] = outVal.ch[1]^outVal.ch[2];
	outVal.ch[2] = outVal.ch[1]^outVal.ch[2];
	outVal.ch[1] = outVal.ch[1]^outVal.ch[2];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_ExcGB(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	outVal.ch[0] = outVal.ch[0]^outVal.ch[1];
	outVal.ch[1] = outVal.ch[0]^outVal.ch[1];
	outVal.ch[0] = outVal.ch[0]^outVal.ch[1];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_ExcBR(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	outVal.ch[0] = outVal.ch[0]^outVal.ch[2];
	outVal.ch[2] = outVal.ch[0]^outVal.ch[2];
	outVal.ch[0] = outVal.ch[0]^outVal.ch[2];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_ExcRGB(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	outVal = iVal1;
	Byte temp_byte = outVal.ch[2];
	outVal.ch[2] = outVal.ch[1];
	outVal.ch[1] = outVal.ch[0];
	outVal.ch[0] = temp_byte;
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_ExcMinMax(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	T tempVal;
	Byte max_byte = 0x00, min_byte = 0xff;
	int  max_elem = 0, min_elem = 0;
	size_t k;
	for (k = 0; k < sizeof(T); k ++)
		{
		tempVal.ch[k] = iVal1.ch[k];
		if (tempVal.ch[k] > max_byte)
		   {
			max_byte = tempVal.ch[k];
			max_elem = k;
		   }
		if (tempVal.ch[k] < min_byte)
		   {
			min_byte = tempVal.ch[k];
			min_elem = k;
		   }
		}
	tempVal.ch[max_elem] = tempVal.ch[max_elem]^tempVal.ch[min_elem];
	tempVal.ch[min_elem] = tempVal.ch[max_elem]^tempVal.ch[min_elem];
	tempVal.ch[max_elem] = tempVal.ch[max_elem]^tempVal.ch[min_elem];
	for (k = 0; k < sizeof(T); k++)
		outVal.ch[k] = tempVal.ch[k];
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_Brightness(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	double adjust;
	/* from old pixopedia - check it!
		factor = ((float)pt_bgr[k] - 100.0) *px_SP_NORM /100.0;
	if (pt_bgr[3])
	   adjust = (adjust << px_SP_SHIFT) + fast_ftol(adjust * factor);
	else
	   adjust = (adjust << px_SP_SHIFT) +  fast_ftol(255.0 * factor);
	adjust = adjust >> px_SP_SHIFT;
	*/
	for (size_t k = 0; k < sizeof(T); k++)
		{
		adjust = (double)iVal1.ch[k];
		if (color->mapRelative)
		   adjust = adjust + adjust * color->mapFactor[k];
		else
		   adjust = adjust + 255.0 * color->mapFactor[k];
		outVal.ch[k] = NormColor(fast_ftol(adjust), 0x00, 0xff);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_Stretch(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	double adjust;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		adjust = (double)iVal1.ch[k];
		if (color->mapRelative)
		   adjust = adjust + adjust * color->mapFactor[k] - (255.0 * color->mapFactor[k]/2.0);
		else
		   adjust = adjust + 255.0 * color->mapFactor[k] - (255.0 * color->mapFactor[k]/2.0);
		outVal.ch[k] = NormColor(fast_ftol(adjust), 0x00, 0xff);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_LMH(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	double factor, dbl_bgr[3], max_elem = 0.0;
	size_t  k, index;
	for (k = 0; k < sizeof(T); k++)
		{
		dbl_bgr[k] = (double)iVal1.ch[k];
		if (dbl_bgr[k] > max_elem)
		   max_elem = dbl_bgr[k];
		}
	if (max_elem < 85)
		index = 2;
	else if ((max_elem >= 85) && (max_elem < 170))
		index = 1;
	else
		index = 0;
	for (k = 0; k < sizeof(T); k++)
		{
		if (color->mapRelative)
		   dbl_bgr[k] = dbl_bgr[k] + dbl_bgr[k] * color->mapFactor[index];
		else
		   dbl_bgr[k] = dbl_bgr[k] + 255.0 * color->mapFactor[index];
		outVal.ch[k] = NormColor(fast_ftol(dbl_bgr[k]), 0x00, 0xff);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_HSV(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	float hsv[3];
	size_t  k;
	Pixel_RGB_HSV(iVal1, hsv);
	if (hsv[0] != -1 && hsv[1] != 0)
	   {
	   for (size_t k = 0; k < sizeof(T); k++)
		   {
		   if (color->mapRelative)
			  hsv[k] += (float)(hsv[k] * color->mapFactor[k]);
		   else
			  {
			  if (k == 0)
				 hsv[k] += (float)(color->mapFactor[k] * 360.0);
			  else
				 hsv[k] += (float)color->mapFactor[k];
			  }
		   hsv[k] = hsv[k] < 0.0 ? 0 : hsv[k];
		   if (k == 0)
			  hsv[k] = hsv[k] > 360.0 ? 360.0 : hsv[k];
		   else
			  hsv[k] = hsv[k] > 1 ? 1 : hsv[k];
		   }
	   }
	Pixel_HSV_RGB(outVal, hsv);
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_Gamma(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	size_t k;
	double fl_maper, fl_color;
	for (int k = 0; k < 3; k++)
		{
		if (color->mapFactor[k] == 0)
		   {
		   outVal.ch[k] = iVal1.ch[k];
		   continue;
		   }
		fl_maper = (double)iVal1.ch[k]/255.0;
		fl_color = pow( fl_maper, 1.0/ color->mapFactor[k])*255.0;
		outVal.ch[k] = NormColor(fast_ftol(fl_color), 0x00, 0xff);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_Cast(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	float intensity = 0.0f;
	// input pixel intensity
	for (size_t k = 0; k < sizeof(T); k++)
		intensity += (float)iVal1.ch[k];
	intensity /= 3.0;
	// napravi cast
	if (color->castOn[0] && intensity <= color->castRange[0])
	   outVal = color->castVal[0];
	else if (color->castOn[1] && intensity > color->castRange[0] && intensity <= color->castRange[1])
	   outVal = color->castVal[1];
	else if (color->castOn[2] && intensity > color->castRange[1])
	   outVal = color->castVal[2];
	else
	   outVal = iVal1;
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_Scale(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	float scaledChan;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		scaledChan = (float)iVal1.ch[k] * (color->scaleRange[k][1] - color->scaleRange[k][0])/255.0 + color->scaleRange[k][0];
		outVal.ch[k] = NormColor(fast_ftol(scaledChan), 0x00, 0xff);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_Luminance(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	//spsColor<T> *color = static_cast <spsColor<T>*>(data);
	// castVal as color input
	float luminaLeft = 0.0f, luminaRight = 0.0f, adjChan;
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		luminaLeft  += (float)iVal1.ch[k] * spe_F_LUMINANCE[2];
		luminaRight += (float)iVal2.ch[k] * spe_F_LUMINANCE[2];
		}
	if (luminaRight <= 0.0f)
	   {
	   outVal = iVal2;
	   return;
	   }
	for (k = 0; k < sizeof(T); k++)
		{
		adjChan = (float)iVal2.ch[k] * luminaLeft / luminaRight;
		outVal.ch[k] = NormColor(fast_ftol(adjChan), 0x00, 0xff);
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void Pixel_Color_Correction(const T &iVal1, const T &iVal2, T &outVal, spsColor<T> *color)
	{
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		{
		if (color->SrcDistance[k] > 0)
		   {
		   outVal.ch[k] = NormColor(fast_ftol(color->DstCumul[k] + (iVal1.ch[k] - color->SrcCumul[k]) * color->DstDistance[k] / color->SrcDistance[k]), 0x00, 0xff);
		   //or  floor instead of fast_ftol
		   }
	   }
	}
  //---------------------------------------------------------------------------
  // linear filter 3x3 (cross correlation)
  //---------------------------------------------------------------------------
  template <class T>
  static void Filter33_Linear(const T &iVal1, const T &iVal2, T &outVal, spsFilter<T> *filter)
	{
	int m;
	float fPixel;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (filter->chanOn[k])
		   {
		   fPixel = 0.0f;
		   for (m = 0; m < 9; m++)
			   fPixel += (float)filter->nbh[m].ch[k] * filter->kernel[m];
		   fPixel = fPixel / filter->div + filter->bias;
		   outVal.ch[k] = NormColor(fast_ftol(fPixel), 0x00, 0xff);
		   }
		else
		   outVal.ch[k] = iVal1.ch[k]; //(Byte)filter->nbh[4].ch[k];
	   }
	}
  //---------------------------------------------------------------------------
  // closest filter 3x3
  //---------------------------------------------------------------------------
  template <class T>
  static void Filter33_Closest(const T &iVal1, const T &iVal2, T &outVal, spsFilter<T> *filter)
	{
	int m, index;
	float fPixel, epsilon, epsilon_test;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (filter->chanOn[k])
		   {
		   fPixel = 0.0f;	 // konvolucija
		   for (m = 0; m < 9; m++)
			   fPixel += (float)filter->nbh[m].ch[k] * filter->kernel[m];
		   fPixel = fPixel / filter->div;
		   // channels closest to cross correlation
		   epsilon = 9999.0f;
		   index = 0;
		   for (m = 0; m < 9; m++)
			   {
			   epsilon_test = fabs(filter->nbh[m].ch[k] - fabs(fPixel));
			   if (epsilon_test  < epsilon )
				   {
				   epsilon = epsilon_test;
				   index  = m;
				   }
				}
		   fPixel = filter->nbh[index].ch[k] + filter->bias;
		   outVal.ch[k] = NormColor(fast_ftol(fPixel), 0x00, 0xff);
		   }
		else
		   outVal.ch[k] = iVal1.ch[k]; //(Byte)filter->nbh[4].ch[k];
	   }
	}
  //---------------------------------------------------------------------------
  // maxdiff filter 3x3
  //---------------------------------------------------------------------------
  template <class T>
  static void Filter33_Diff(const T &iVal1, const T &iVal2, T &outVal, spsFilter<T> *filter)
	{
	int m, index;
	float fPixel, mid_convo, epsilon_test;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (filter->chanOn[k])
		   {
		   fPixel = 0.0f;	 // cross correalation
		   mid_convo = filter->nbh[4].ch[k] * filter->kernel[4];
		   for (m = 0; m < 9; m++)
			   {
			   if (m != 4)
				  {
				  epsilon_test = filter->nbh[m].ch[k] * filter->kernel[m];
				  if (fabs(fabs(epsilon_test) - fabs(mid_convo))  > fabs(fPixel))
					 fPixel = mid_convo - epsilon_test;
				  }
			   }
		   fPixel = fPixel / filter->div + filter->bias;;
		   outVal.ch[k] = NormColor(fast_ftol(fPixel), 0x00, 0xff);
		   }
		else
		   outVal.ch[k] = iVal1.ch[k]; //(Byte)filter->nbh[4].ch[k];
	   }
	}
  //---------------------------------------------------------------------------
  // minmax filter 3x3
  //---------------------------------------------------------------------------
  template <class T>
  static void Filter33_MinMax(const T &iVal1, const T &iVal2, T &outVal, spsFilter<T> *filter)
	{
	int m;
	float fPixel, convo[9], min_value, max_value, divisor, min_fact, max_fact;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (filter->chanOn[k])
		   {
		   divisor = 0;
		   convo[k] = 0.0f;
		   min_value = 99999.0f;
		   max_value = -99999.0f;
		   for (m = 0; m < 9; m++)
			   {
			   convo[k] += filter->nbh[m].ch[k] * filter->kernel[m];
			   divisor  += filter->kernel[m];
			   if (filter->nbh[m].ch[k] < min_value)
				  {
				  min_fact  = filter->kernel[m];
				  min_value = filter->nbh[m].ch[k];
				  }
			   if (filter->nbh[m].ch[k] > max_value)
				  {
				  max_fact  = filter->kernel[m];
				  max_value = filter->nbh[m].ch[k];
				  }
			   }
		   convo[k] = convo[k] -  max_value*max_fact - min_value*min_fact;
		   divisor = divisor - max_fact - min_fact;
		   convo[k] =  (fast_ftol(divisor)  == 0 ? convo[k] :  convo[k] / divisor);
		   fPixel =  (max_value - convo[k]) > (convo[k] - min_value) ? max_value : min_value;
		   fPixel = fPixel/filter->div + filter->bias;
		   outVal.ch[k] = NormColor(fast_ftol(fPixel), 0x00, 0xff);
		   }
		else
		   outVal.ch[k] = iVal1.ch[k]; //(Byte)filter->nbh[4].ch[k];
	   }
	}
  //---------------------------------------------------------------------------
  // closest filter 3x3
  //---------------------------------------------------------------------------
  template <class T>
  static void Filter33_Logic(const T &iVal1, const T &iVal2, T &outVal, spsFilter<T> *filter)
	{
	int m;
	float fPixel;
	Byte ker_result;
	for (size_t k = 0; k < sizeof(T); k++)
		{
		if (filter->chanOn[k])
		   {
		   fPixel = 0.0f;	 // cross correlation
		   for (m = 0; m < 9; m++)
			   {
			   ker_result = (Byte)fast_ftol(filter->kernel[m]);
			   ker_result = bLogic[filter->logic[m]] (filter->nbh[m].ch[k], ker_result);
			   fPixel += (float)ker_result * (float)filter->sign[m];
		       }
		   fPixel = fPixel / filter->div + filter->bias;;
		   outVal.ch[k] = NormColor(fast_ftol(fPixel), 0x00, 0xff);
		   }
		else
		   outVal.ch[k] = iVal1.ch[k]; //(Byte)filter->nbh[4].ch[k];
	   }
	}
  private:
  //---------------------------------------------------------------------------
  // shift/invert pixel
  //---------------------------------------------------------------------------
  template <class T>
  static int BitShiftInvert(Byte inPix, spsArith<T> *arith)
	{
	int outPix = (int)inPix;
	if (arith->invertPixel)
	   outPix = 255 - outPix;
	if (arith->bitShift == 0)
	   return outPix;
	arith->bitShift > 0 ? outPix = outPix<<arith->bitShift : outPix = outPix>>abs(arith->bitShift);
	return outPix;
	}
  };
#endif

