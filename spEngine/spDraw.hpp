//Copyright (C) 2016 Siniša Petriæ
#ifndef SP_DRAW_H
#define SP_DRAW_H
#include "spImage.hpp"
#include "spPixelTypes.hpp"
#include "WuLine.h"
#include <vector>
#include <algorithm>
#include <stack>
#include "ASMVars.h"
using namespace std;
enum speDotType {dtCircle, dtSquare};
//---------------------------
template <class T>
class spLineWalk
  {
  private:
	int x0, y0, x1, y1;
	T color0, color1;
	// color step
	float cStep[3], colorOut[3];
	int dx, dy, sx, sy, err, e2;
  public:
	int x;
	int y;
	T color;
	// trivial constructor
	spLineWalk()  {}
	virtual ~spLineWalk() {}
	// set line coordinates
	void Set(int xs, int ys, int xe, int ye)
	  {
	  x0 = xs;
	  y0 = ys;
	  x1 = xe;
	  y1 = ye;
	  x = x0;
	  y = y0;
	  // Breshenham's algorithm - set
	  dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	  dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	  err = dx + dy;
	  }
	// set start-end colors only
	void Set(T colors, T colore)
	  {
	  color0 = colors;
	  color1 = colore;
	  color = color0;
	  size_t k;
	  float length = sqrt((float)((x1-x0)*(x1-x0) + (y1-y0)*(y1-y0)));
	  for (k = 0; k < sizeof(T); k++)
		  {
		  cStep[k] = length > 0 ? ((float)color1.ch[k] - (float)color0.ch[k])/length : 0.0f;
		  colorOut[k] = (float)color0.ch[k];
		  }
	  }
	// set line coordinates and start-end colors
	void Set(int xs, int ys, int xe, int ye, T colors, T colore)
	  {
	  Set(xs, ys, xe, ye);
	  Set(colors, colore);
	  }
	// walk the line
	bool Walk()
	  {
	  // Breshenham's algorithm - silent walk
	  if (x0 == x1 && y0 == y1)
		 return false;  // end of line
	  // output data
	  x = x0; y = y0;
	  // step calc
	  e2 = (err<<1);
	  if (e2 >= dy) { err += dy; x0 += sx; }
	  if (e2 <= dx) { err += dx; y0 += sy; }
	  return true; // more steps to go
	  }
	// walk the color line
	bool ColorWalk()
	  {
	  // Breshenham's algorithm - silent walk
	  if (x0 == x1 && y0 == y1)
		 return false;  // end of line
	  // output data
	  x = x0; y = y0;
	  size_t k;
	  for (k = 0; k < sizeof(T); k++)
		  {
		  color.ch[k] = (Byte)fast_ftol(colorOut[k]);
		  //color.ch[k] = (Byte)ceil(colorOut[k]);
		  colorOut[k] += cStep[k];
		  }
	  // step calc
	  e2 = (err<<1);
	  if (e2 >= dy) { err += dy; x0 += sx; }
	  if (e2 <= dx) { err += dx; y0 += sy; }
	  return true; // more steps to go
	  }
};
//---------------------------
class spDraw
  {
  public:
	template <class T>
	struct spsColorTriangle
	   {
	   int x[3];
	   int y[3];
	   T val[3];
	   //unsigned char rgb[3][3];
	   };
	template <class T>
	struct spsColorPoint   //TODO: gradient line
	   {
	   int x;
	   int y;
	   T val;
	   //unsigned char rgb[3];
	   spsColorPoint() {}
	   spsColorPoint(int _x, int _y, T _val)
		  {
		  x = _x;
		  y = _y;
		  val = _val;
		  }
	   };
  //---------------------------------------------------------------------------
  // draw line
  //---------------------------------------------------------------------------
  template <class T>
  static void Line(spImage<T> *img, T val, int x0, int y0, int x1, int y1, unsigned int size = 1, speDotType dType = dtCircle)
	{
	// Breshenham's algorithm
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx + dy, e2;

	typename TExecute<T>::PointFunc putFunc;
	if (size == 1)
	   putFunc = putPixel;
	else
	   {
	   if (dType == dtCircle)
		   putFunc = putCircle;
	   else
		   putFunc = putSquare;
	   }
	for(;;)
	   {
	   putFunc(img, val, x0, y0, size);
	   if (x0==x1 && y0==y1) break;
	   e2 = (err<<1);
	   //e2 = 2*err;
	   if (e2 >= dy) { err += dy; x0 += sx; }
	   if (e2 <= dx) { err += dx; y0 += sy; }
	   }
	}
  //---------------------------------------------------------------------------
  // draw scanline
  //---------------------------------------------------------------------------
  template <class T>
  static void ScanLine(spImage<T> *img, T val, int y, int x1, int x2)
	{
	if (y < 0 || y > img->Height()-1) return;
	if ((x1 < 0 && x2 < 0) || (x1 > img->Width()-1 && x2 > img->Width()-1 )) return;
	if (x1 > x2)
	   {
	   x1 = x1^x2;
	   x2 = x1^x2;
	   x1 = x1^x2;
	   }
	if (x1 < 0) x1 = 0;
	if (x2 > img->Width()-1) x2 = img->Width()-1;
	for (int j = x1; j <= x2; j++)
		img->Pixel(y, j) = val;     // draw point
	}
  //---------------------------------------------------------------------------
  // draw multicolor scanline
  //---------------------------------------------------------------------------
  template <class T>
  static void ScanLine(spImage<T> *img, T val1, T val2, int y, int x1, int x2)
	{
	if (y < 0 || y > img->Height()-1) return;
	if ((x1 < 0 && x2 < 0) || (x1 > img->Width()-1 && x2 > img->Width()-1 )) return;
	if (x1 > x2)
	   {
	   x1 = x1^x2;
	   x2 = x1^x2;
	   x1 = x1^x2;
	   val1 = val1^val2;
	   val2 = val1^val2;
	   val1 = val1^val2;
	   }
	if (x1 < 0) x1 = 0;
	if (x2 > img->Width()-1) x2 = img->Width()-1;
	float colorVal[3], colorStep[3];
	size_t k;
	for (k = 0; k < sizeof(T); k++)
		colorVal[k] = val1.ch[k];
	float length = x2 - x1;
	if (length > 0)
	   {
	   for (k = 0; k < sizeof(T); k++)
		   colorStep[k] = (val2.ch[k] - val1.ch[k]) / length;
	   }
	else
	  colorStep[0] = colorStep[1] = colorStep[2] = 0.0f;
	for (int j = x1; j <= x2; j++)
		{
		for (k = 0; k < sizeof(T); k++)
			{
			img->Pixel(y, j).ch[k] = fast_ftol(colorVal[k]);     // draw point
			colorVal[k] += colorStep[k];
			}
		}
	}
  //---------------------------------------------------------------------------
  // draw ellipse
  //---------------------------------------------------------------------------
  template <class T>
  static void Ellipse(spImage<T> *img, T val, int xc, int yc, int rx, int ry, unsigned int size = 1)
	{
	typename TExecute<T>::PointFunc putFunc;
	if (size == 1)
	   putFunc = putPixel;
	else
	   putFunc = putCircle;
	int x = 0, y = ry;
	int p =(ry*ry)-(rx*rx*ry)+((rx*rx)/4);
	int xe, ye;
	while ((2*x*ry*ry)<(2*y*rx*rx))
	   {
	   xe = xc+x, ye = yc-y; putFunc(img, val, xe, ye, size);  // 1. quadrant
	   xe = xc-x, ye = yc-y; putFunc(img, val, xe, ye, size);  // 2. quadant
	   xe = xc-x, ye = yc+y; putFunc(img, val, xe, ye, size);  // 3. quadrant
	   xe = xc+x, ye = yc+y; putFunc(img, val, xe, ye, size);  // 4. qaudrant
	   if ( p < 0)
		  {
		  x = x + 1;
		  p = p +(2*ry*ry*x)+(ry*ry);
		  }
	   else
		  {
		  x = x + 1;
		  y = y - 1;
		  p = p + (2*ry*ry*x+ry*ry)-(2*rx*rx*y);
		  }
	   }
	   p = ((float)x+0.5)*((float)x+0.5)*ry*ry+(y-1)*(y-1)*rx*rx-rx*rx*ry*ry;
	while( y >= 0)
	   {
	   xe = xc+x, ye = yc-y; putFunc(img, val, xe, ye, size);  // 1. quadrant
	   xe = xc-x, ye = yc-y; putFunc(img, val, xe, ye, size);  // 2. quadant
	   xe = xc-x, ye = yc+y; putFunc(img, val, xe, ye, size);  // 3. quadrant
	   xe = xc+x, ye = yc+y; putFunc(img, val, xe, ye, size);  // 4. qaudrant
	   if (p > 0)
		  {
		  y = y-1;
		  p = p-(2*rx*rx*y)+(rx*rx);
		  }
	   else
		  {
		  y = y-1;
		  x = x+1;
		  p = p+(2*ry*ry*x)-(2*rx*rx*y)-(rx*rx);
		  }
	  }
	}
  //---------------------------------------------------------------------------
  // draw ellipse fileld
  //---------------------------------------------------------------------------
  template <class T>
  static void EllipseFilled(spImage<T> *img, T val, int xc, int yc, int rx, int ry)
	{
	int x = 0, y = ry;
	int p =(ry*ry)-(rx*rx*ry)+((rx*rx)/4);
	while ((2*x*ry*ry)<(2*y*rx*rx))
	   {
	   ScanLine(img, val, yc-y, xc-x, xc+x);     // draw scanline 1. and 2. quadrant
	   ScanLine(img, val, yc+y, xc-x, xc+x);	 // draw scanline 3. and 4. quadnant
	   if ( p < 0)
		  {
		  x = x + 1;
		  p = p +(2*ry*ry*x)+(ry*ry);
		  }
	   else
		  {
		  x = x + 1;
		  y = y - 1;
		  p = p + (2*ry*ry*x+ry*ry)-(2*rx*rx*y);
		  }
	   }
	   p = ((float)x+0.5)*((float)x+0.5)*ry*ry+(y-1)*(y-1)*rx*rx-rx*rx*ry*ry;
	while( y >= 0)
	   {
	   ScanLine(img, val, yc-y, xc-x, xc+x);     // draw scanline 1. and 2. quadrant
	   ScanLine(img, val, yc+y, xc-x, xc+x);	 // draw scanline 3. and 4. quadnant
	   if (p > 0)
		  {
		  y = y-1;
		  p = p-(2*rx*rx*y)+(rx*rx);
		  }
	   else
		  {
		  y = y-1;
		  x = x+1;
		  p = p+(2*ry*ry*x)-(2*rx*rx*y)-(rx*rx);
		  }
	  }
	}
  //---------------------------------------------------------------------------
  // draw rectangle
  //---------------------------------------------------------------------------
  template <class T>
  static void Rectangle(spImage<T> *img, T val, int x1, int y1, int x2, int y2, unsigned int size = 1)
	{
	if (x1 > x2)
	   {
	   x1 = x1^x2;
	   x2 = x1^x2;
	   x1 = x1^x2;
	   }
	if (y1 > y2)
	   {
	   y1 = y1^y2;
	   y2 = y1^y2;
	   y1 = y1^y2;
	   }
	// clockwise
	Line(img, val, x1, y1, x2, y1, size, dtSquare);
	Line(img, val, x2, y1, x2, y2, size, dtSquare);
	Line(img, val, x2, y2, x1, y2, size, dtSquare);
	Line(img, val, x1, y2, x1, y1, size, dtSquare);
	}
  //---------------------------------------------------------------------------
  // draw rectangle filled
  //---------------------------------------------------------------------------
  template <class T>
  static void RectangleFilled(spImage<T> *img, T val, int x1, int y1, int x2, int y2)
	{
	if (x1 > x2)
	   {
	   x1 = x1^x2;
	   x2 = x1^x2;
	   x1 = x1^x2;
	   }
	if (y1 > y2)
	   {
	   y1 = y1^y2;
	   y2 = y1^y2;
	   y1 = y1^y2;
	   }
	// down
	for (int i = y1; i <= y2; i++)
		ScanLine(img, val, i, x1, x2);
	}
  //---------------------------------------------------------------------------
  // draw triangle
  //---------------------------------------------------------------------------
  template <class T>
  static void Triangle(spImage<T> *img, T val, int x1, int y1, int x2, int y2, int x3, int y3, unsigned int size = 1)
	{
	Line(img, val, x1, y1, x2, y2, size);
	Line(img, val, x2, y2, x3, y3, size);
	Line(img, val, x3, y3, x1, y1, size);
	}
  //---------------------------------------------------------------------------
  // draw triangle filled
  //---------------------------------------------------------------------------
  template <class T>
  static void TriangleDeprFilled(spImage<T> *img, T val, int x1, int y1, int x2, int y2, int x3, int y3)
	{
	spFPoint pt;
	pt.x = x1;
	pt.y = y1;
	vector <spFPoint> triangle;
	triangle.push_back(pt);
	pt.x = x2;
	pt.y = y2;
	triangle.push_back(pt);
	pt.x = x3;
	pt.y = y3;
	triangle.push_back(pt);
	stable_sort(triangle.begin(), triangle.end(), yAscFPoint);
	spFPoint A, B, C;
	float	dx1, dx2, sx1, sx2, y;
	A = triangle[0];
	B = triangle[1];
	C = triangle[2];
	B.y-A.y > 0 ? dx1 = (B.x-A.x)/(B.y-A.y) : dx1=0;
	C.y-A.y > 0 ? dx2 = (C.x-A.x)/(C.y-A.y) : dx2=0;
	sx1 = sx2 = A.x;
	y = A.y;
	// upper halft
	for (; y <= B.y; y++, sx1+=dx1, sx2+=dx2)
		ScanLine(img, val, y, sx1, sx2);
	// lower half
	C.y-B.y > 0 ? dx1 = (C.x-B.x)/(C.y-B.y) : dx1=0;
	sx1 = B.x;
	y   = B.y;
	for (; y <= C.y; y++, sx1+=dx1, sx2+=dx2)
		ScanLine(img, val, y, sx1, sx2);
	//triangle.clear();
	}
  //---------------------------------------------------------------------------
  // draw triangle filled (silent Breshenhaim)
  //---------------------------------------------------------------------------
  template <class T>
  static void TriangleFilled(spImage<T> *img, T val, int x1, int y1, int x2, int y2, int x3, int y3)
	{
	vector <TPoint> triangle;
	triangle.push_back(TPoint(x1, y1));
	triangle.push_back(TPoint(x2, y2));
	triangle.push_back(TPoint(x3, y3));
	stable_sort(triangle.begin(), triangle.end(), yAscTPoint);
	TPoint A, B, C;
	A = triangle[0];
	B = triangle[1];
	C = triangle[2];
	spLineWalk<T> l1, l2;
	l1.Set(A.x, A.y, B.x, B.y);
	l2.Set(A.x, A.y, C.x, C.y);
	// upper halft
	simpleWalk(img, l1, l2, val);
	// lower half
	l1.Set(B.x, B.y , C.x, C.y);
	simpleWalk(img, l1, l2, val);
	}
  //---------------------------------------------------------------------------
  // draw triangle filled (silent Breshenhaim)
  //---------------------------------------------------------------------------
  template <class T>
  static void TriangleGouraud(spImage<T> *img, T val1, T val2, T val3, int x1, int y1, int x2, int y2, int x3, int y3)
	{
	//typedef spsColorPoint <T> Z;
	vector <spsColorPoint <T> > triangle;
	//vector <Z> triangle;
	triangle.push_back(spsColorPoint<T>(x1, y1, val1));
	triangle.push_back(spsColorPoint<T>(x2, y2, val2));
	triangle.push_back(spsColorPoint<T>(x3, y3, val3));
	stable_sort(triangle.begin(), triangle.end(), yAscColorPoint<T>);
	spsColorPoint<T> A, B, C; //, BB,CC;
	A = triangle[0];
	B = triangle[1];
	C = triangle[2];
	spLineWalk<T> l1, l2;
	l1.Set(A.x, A.y, B.x, B.y, A.val, B.val);
	l2.Set(A.x, A.y, C.x, C.y, A.val, C.val);
	// upper halft
	colorWalk(img, l1, l2);
	// lower half
	l1.Set(B.x, B.y, C.x, C.y, l1.color, C.val);
	colorWalk(img, l1, l2);
	}
  //---------------------------------------------------------------------------
  // draw triangle filled
  //---------------------------------------------------------------------------
  template <class T>
  static void TriangleDeprGouraud(spImage<T> *img, T val1, T val2, T val3, int x1, int y1, int x2, int y2, int x3, int y3)
	{
	T val;
	spFColorPoint pt;
	pt.x = x1;
	pt.y = y1;
	if (typeid(T) == typeid(ptRGB))
	   {
	   pt.r = val1.ch[2];
	   pt.g = val1.ch[1];
	   }
	pt.b = val1.ch[0];
	vector <spFColorPoint> triangle;
	triangle.push_back(pt);
	pt.x = x2;
	pt.y = y2;
	if (typeid(T) == typeid(ptRGB))
	   {
	   pt.r = val2.ch[2];
	   pt.g = val2.ch[1];
	   }
	pt.b = val2.ch[0];
	triangle.push_back(pt);
	pt.x = x3;
	pt.y = y3;
	if (typeid(T) == typeid(ptRGB))
	   {
	   pt.r = val3.ch[2];
	   pt.g = val3.ch[1];
	   }
	pt.b = val3.ch[0];
	triangle.push_back(pt);
	stable_sort(triangle.begin(), triangle.end(), yAscColorFPoint);
	// gouraud filler
	spFColorPoint A, B, C, S, E, P;
	A = triangle[0];
	B = triangle[1];
	C = triangle[2];
	float dx1, dx2;
	float dr1, dg1, db1, dr2, dg2, db2, dr, dg, db;
	float sign;
	if (B.y-A.y > 0)
	   {
	   dx1 = (B.x-A.x)/(B.y-A.y);
	   dr1 = (B.r-A.r)/(B.y-A.y);
	   dg1 = (B.g-A.g)/(B.y-A.y);
	   db1 = (B.b-A.b)/(B.y-A.y);
	   }
	else
	   dx1=dr1=dg1=db1 = 0.0f;
	if (C.y-A.y > 0)
	   {
	   dx2 = (C.x-A.x)/(C.y-A.y);
	   dr2 = (C.r-A.r)/(C.y-A.y);
	   dg2 = (C.g-A.g)/(C.y-A.y);
	   db2 = (C.b-A.b)/(C.y-A.y);
	   }
	else
	   dx2=dr2=dg2=db2=0.0f;
	S = E = A;
	// upper halft
	for (; S.y <= B.y; S.y++, S.x+=dx1, E.x+=dx2, S.r+=dr1, S.g+=dg1, S.b+=db1, E.r+=dr2, E.g+=dg2, E.b+=db2)
		{
		sign = E.x-S.x < 0 ? -1 : 1;
		if (fabs(E.x-S.x) > 0)
		   {
		   dr = sign*(E.r-S.r)/(E.x-S.x);
		   dg = sign*(E.g-S.g)/(E.x-S.x);
		   db = sign*(E.b-S.b)/(E.x-S.x);
		   }
		else
		   dr=dg=db=0;
		P = S;
		if (P.x > E.x)
			{
			P.x = (float)ceil(P.x);
			for (; P.x >= floor(E.x); P.x--, P.r+=dr, P.g+=dg, P.b+=db)
				{
				limitPixelColor(img, P, dr, dg, db);
				putPixel(img, P);
				}
			}
		else
		   {
		   P.x = (float)floor(P.x);
		   for (; P.x <= ceil(E.x); P.x++, P.r+=dr, P.g+=dg, P.b+=db)
			   {
			   limitPixelColor(img, P, dr, dg, db);
			   putPixel(img, P);
			   }
		   }
		}
	// lower half
	if (C.y-B.y > 0)
	   {
	   dx1 = (C.x-B.x)/(C.y-B.y);
	   dr1 = (C.r-B.r)/(C.y-B.y);
	   dg1 = (C.g-B.g)/(C.y-B.y);
	   db1 = (C.b-B.b)/(C.y-B.y);
	   }
	else
	   dx1=dr1=dg1=db1 = 0.0f;
	S = B;
	for (; S.y <= C.y; S.y++, S.x+=dx1, E.x+=dx2, S.r+=dr1, S.g+=dg1, S.b+=db1, E.r+=dr2, E.g+=dg2, E.b+=db2)
		{
		sign = E.x-S.x < 0 ? -1 : 1;
		if (fabs(E.x-S.x) > 0)
		   {
		   dr = sign*(E.r-S.r)/(E.x-S.x);
		   dg = sign*(E.g-S.g)/(E.x-S.x);
		   db = sign*(E.b-S.b)/(E.x-S.x);
		   }
		else
		   dr=dg=db=0;
		P = S;
		if (P.x > E.x)
		   {
		   P.x = (float)ceil(P.x);
		   for (; P.x >= floor(E.x); P.x--, P.r+=dr, P.g+=dg, P.b+=db)
			   {
			   limitPixelColor(img, P, dr, dg, db);
			   putPixel(img, P);
			   }
		   }
		else
		   {
		   P.x = (float)floor(P.x);
		   for (; P.x <= ceil(E.x); P.x++, P.r+=dr, P.g+=dg, P.b+=db)
			   {
			   limitPixelColor(img, P, dr, dg, db);
			   putPixel(img, P);
			   }
		   }
		}
	//TODO: int values for coordinates + Breshenham's algorithm
	}
  //---------------------------------------------------------------------------
  // draw ellipse within bounding rectangle
  //---------------------------------------------------------------------------
  template <class T>
  static void Ellipse (spImage<T> *img, T val, TRect rect, unsigned int size = 1)
	{
	int rx = (rect.Width()>>1);
	int ry = (rect.Height()>>1);
	Ellipse (img, val, rect.left + rx, rect.top + ry, rx, ry, size);
	}
  //---------------------------------------------------------------------------
  // draw ellipse fileld within bounding rectangle
  //---------------------------------------------------------------------------
  template <class T>
  static void EllipseFilled(spImage<T> *img, T val, TRect rect)
	{
	rect.NormalizeRect();
	int rx = (rect.Width()>>1);
	int ry = (rect.Height()>>1);
	EllipseFilled (img, val, rect.left + rx, rect.top + ry, rx, ry);
	}
  //---------------------------------------------------------------------------
  // draw rectangle within bounding rectangle
  //---------------------------------------------------------------------------
  template <class T>
  static void Rectangle(spImage<T> *img, T val, TRect rect, unsigned int size = 1)
	{
	Rectangle(img, val, rect.left, rect.top, rect.right, rect.bottom, size);
	}
  //---------------------------------------------------------------------------
  // draw rectangle filled within bounding rectangle
  //---------------------------------------------------------------------------
  template <class T>
  static void RectangleFilled(spImage<T> *img, T val, TRect rect)
	{
	RectangleFilled(img, val, rect.left, rect.top, rect.right, rect.bottom);
	}
  //---------------------------------------------------------------------------
  // draw triangle
  //---------------------------------------------------------------------------
  template <class T>
  static void Triangle(spImage<T> *img, spsColorTriangle<T> inpT, unsigned int size = 1, int colorChan = 0)
	{
	if (sizeof(T) == 3 && colorChan < 0)
	   {
	   for (int k = 0; k < 3; k++)
			Line(img, inpT.val[k], inpT.x[0], inpT.y[0], inpT.x[1], inpT.y[1], size);
	   }
	else
	   {
	   Line(img, inpT.val[colorChan], inpT.x[0], inpT.y[0], inpT.x[1], inpT.y[1], size);
	   Line(img, inpT.val[colorChan], inpT.x[1], inpT.y[1], inpT.x[2], inpT.y[2], size);
	   Line(img, inpT.val[colorChan], inpT.x[2], inpT.y[2], inpT.x[0], inpT.y[0], size);
	   }
	}
  //---------------------------------------------------------------------------
  // draw filled triangle
  //---------------------------------------------------------------------------
  template <class T>
  static void TriangleFilled(spImage<T> *img, spsColorTriangle<T> inpT, int colorChan = 0)
	{
	T val = inpT.val[colorChan];
	if (sizeof(T) == 3 && colorChan < 0)
	   {
	   for (int k = 0; k < 3; k++)
		   val.ch[k] = fast_ftol(((float)inpT.val[0].ch[k] + (float)inpT.val[1].ch[k] + (float)inpT.val[1].ch[k]) / 3.0f);
	   }
	TriangleFilled(img, val, inpT.x[0], inpT.y[0], inpT.x[1], inpT.y[1], inpT.x[2], inpT.y[2]);
	}
  //---------------------------------------------------------------------------
  // draw triangle with Gouraud shading
  //---------------------------------------------------------------------------
  template <class T>
  static void TriangleGouraud(spImage<T> *img, spsColorTriangle<T> inpT)
	{
	/*
	T val1, val2, val3;
	val1.ch[0] = inpT.rgb[0][2];
	val2.ch[0] = inpT.rgb[1][2];
	val3.ch[0] = inpT.rgb[2][2];
	if (typeid(T) == typeid(ptRGB))
	   {
	   val1.ch[1] = inpT.rgb[0][1];
	   val1.ch[2] = inpT.rgb[0][0];
	   val2.ch[1] = inpT.rgb[1][1];
	   val2.ch[2] = inpT.rgb[1][0];
	   val3.ch[1] = inpT.rgb[2][1];
	   val3.ch[2] = inpT.rgb[2][0];
	   }
	TriangleGouraud(img, val1, val2, val3, inpT.x[0], inpT.y[0], inpT.x[1], inpT.y[1], inpT.x[2], inpT.y[2]);
	*/
	TriangleGouraud(img, inpT.val[0], inpT.val[1], inpT.val[2], inpT.x[0], inpT.y[0], inpT.x[1], inpT.y[1], inpT.x[2], inpT.y[2]);
	}
  //---------------------------------------------------------------------------
  // draw polyline
  //---------------------------------------------------------------------------
  template <class T>
  static void Polyline(spImage<T> *img, T val,  TPoint *polyPts, int polyCount, unsigned int size = 1)
	{
	for (int i = 1; i < polyCount; i++)
		Line(img, val, polyPts[i-1].x, polyPts[i-1].y, polyPts[i].x, polyPts[i].y, size, dtCircle);
	}
  //---------------------------------------------------------------------------
  // draw polygon
  //---------------------------------------------------------------------------
  template <class T>
  static void Polygon(spImage<T> *img, T val,  TPoint *polyPts, int polyCount, unsigned int size = 1)
	{
	Polyline(img, val, polyPts, polyCount, size);
	Line(img, val, polyPts[polyCount-1].x, polyPts[polyCount-1].y, polyPts[0].x, polyPts[0].y, size, dtCircle);
	}
  //---------------------------------------------------------------------------
  // draw polygon filled
  //---------------------------------------------------------------------------
  template <class T>
  static void PolygonFilled(spImage<T> *img, T val,  TPoint *polyPts, int polyCount)
	{
	spFPoint pt;
	vector<int> nodeX;
	int tNode, i, j, y, yMin = INT_MAX, yMax = 0;
	double div, mult;
	for (i = 0; i < polyCount; i++)
		{
		if (polyPts[i].y < yMin)
			yMin = polyPts[i].y;
		if (polyPts[i].y > yMax)
			yMax = polyPts[i].y;
		}
	//  Loop through the rows
	for (y = yMin; y <= yMax; y++)
		{
		//  Build a list of nodes.
		nodeX.clear();
		for (i = 0, j = 1; i < polyCount; i++, j++)
			{
			if (j == polyCount)
			   j = 0;
			if ((polyPts[i].y < y && polyPts[j].y >= y) ||  (polyPts[j].y < y && polyPts[i].y >= y))
			   {
			   div  = (double)(polyPts[j].y - polyPts[i].y);
			   mult = (double)(polyPts[j].x - polyPts[i].x);
			   tNode = fast_ftol((double)polyPts[i].x + (double)(y - polyPts[i].y)/div * mult);
			   nodeX.push_back(tNode);
			   }
			}
		// Sort the nodes
		sort(nodeX.begin(), nodeX.end());
		//  Fill the pixels between node pairs.
		for (int i = 0; i < nodeX.size(); i +=2)
			ScanLine(img, val, y, nodeX[i], nodeX[i+1]);
	   }
	}
  //---------------------------------------------------------------------------
  // flood fill with mask recursive
  //---------------------------------------------------------------------------
  template <class T>
  static void FloodFillRecursive(spImage <ptGray> *mask, spImage<T> *img, int xc, int yc, T val)
	{
	// four way flood fill recursive
	if (yc < 0 || yc > mask->Height()-1	|| xc < 0 || xc > mask->Width()-1)
	   return;
	if (mask->Pixel(yc, xc).ch[0] == 0x00)
	   {
	   mask->Pixel(yc, xc).ch[0] = 0xff;
	   img->Pixel(yc, xc) = val;     // draw point
	   FloodFillRecursive(mask, img, xc-1, yc, val);
	   FloodFillRecursive(mask, img, xc+1, yc, val);
	   FloodFillRecursive(mask, img, xc, yc-1, val);
	   FloodFillRecursive(mask, img, xc, yc+1, val);
	   }
	}
  //---------------------------------------------------------------------------
  // flood fill with mask non-recursive (stack)
  //---------------------------------------------------------------------------
  template <class T>
  static void FloodFill(spImage <ptGray> *mask, spImage<T> *img, int xc, int yc, T val)
	{
	// four way flood fill non-recursive
	if (yc < 0 || yc > mask->Height()-1	|| xc < 0 || xc > mask->Width()-1)
	   return;
	TPoint pt;
	pt.x = xc;
	pt.y = yc;
	std::stack<TPoint> ptStack;
	ptStack.push(pt);
	while (ptStack.size() > 0)
		{
		pt = ptStack.top();
		ptStack.pop();
		int x = pt.x;
		int y = pt.y;
		if (y < 0 || y > mask->Height()-1	|| x < 0 || x > mask->Width()-1)
		   continue;
		if (mask->Pixel(y, x).ch[0] == 0x00)
		   {
		   mask->Pixel(y, x).ch[0] = 0xff;
		   img->Pixel(y, x) = val;     // draw point
		   ptStack.push(TPoint(x-1, y));
		   ptStack.push(TPoint(x+1, y));
		   ptStack.push(TPoint(x, y-1));
		   ptStack.push(TPoint(x, y+1));
		   }
		}
	}
  //---------------------------------------------------------------------------
  // flood fill with mask non-recursive convex
  //---------------------------------------------------------------------------
  template <class T>
  static void FloodFillConvex(spImage <ptGray> *mask, spImage<T> *img, int xc, int yc, T val, bool fillEdge)
	{
	// scanline flood fill convex
	int leftX,rightX, udY;
	bool status;
	// up
	status = true;
	udY = yc;
	leftX = xc;
	rightX = xc + 1;
	for(;;)
	   {
	   if (status)
		  {
		  floodLeftRight(mask, img, leftX, rightX, udY, val);
		  if (fillEdge) floodEdge(mask, img, leftX, rightX, udY, val);
		  leftX = leftX + ((rightX - leftX)>>1);  // scanline by half
		  rightX = leftX + 1;
		  }
	   status = false;
	   if (udY > 0)
		  status = (mask->Pixel(--udY, leftX).ch[0] == 0x00);
	   if (!status)
		  {
		  if (fillEdge) floodEdge(mask, img, leftX, rightX, udY, val);
		  break;
		  }
	   }
	// down
	status = true;
	udY = yc + 1;
	if (udY  > img->Height()-1)
	   return;
	leftX = xc;
	rightX = xc + 1;
	for(;;)
	   {
	   if (status)
		  {
		  floodLeftRight(mask, img, leftX, rightX, udY, val);
		  if (fillEdge) floodEdge(mask, img, leftX, rightX, udY, val);
		  leftX = leftX + ((rightX - leftX)>>1);  // scanline by half
		  rightX = leftX + 1;
		  }
	   status = false;
	   if (udY < mask->Height()-1)
		  status = (mask->Pixel(++udY, leftX).ch[0] == 0x00);
	   if (!status)
		  {
		  if (fillEdge) floodEdge(mask, img, leftX, rightX, udY, val);
		  break;
		  }
	   }
	}
  //---------------------------------------------------------------------------
  // draw Wu line (antialiased) 1 pixel
  //---------------------------------------------------------------------------
  template <class T>
  static void WuLine(spImage<T> *img, T fVal, T bVal, int x0, int y0, int x1, int y1)
	{
	std::vector<TVertex> va;
	T WuVal;
	int x, y;
	unsigned int size = 1;
	drawWuLine(va, (float)x0, (float) y0, (float) x1, (float) y1, fVal.ch, bVal.ch, sizeof(T));
	for (size_t i = 0; i < va.size(); i++)
		{
		x = va[i].x;
		y = va[i].y;
		WuVal.ch[0] = (Byte)fast_ftol(va[i].bgr[0]);
		if (sizeof(T) == 3)
		   {
		   WuVal.ch[1] = (Byte)fast_ftol(va[i].bgr[1]);
		   WuVal.ch[2] = (Byte)fast_ftol(va[i].bgr[2]);
		   }
		putPixel(img, WuVal, x, y, size);
		}
	va.clear();
	}
  //---------------------------------------------------------------------------
  // draw line with subpixel accuracy
  //---------------------------------------------------------------------------
  template <class T>
  static void SpaLine(spImage<T> *img, T val, int x0, int y0, int x1, int y1, unsigned int size = 1, speDotType dType = dtCircle)
	{
	// Breshenham's algorithm with subpixel accuracy
	int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int e2;
	int e2dx = (dx<<8);
	int e2dy = (dy<<8);
	int err = dx + dy;

	typename TExecute<T>::PointFunc putFunc;
	if (size == 1)
	   putFunc = putPixel;
	else
	   {
	   if (dType == dtCircle)
		   putFunc = putCircle;
	   else
		   putFunc = putSquare;
	   }
	for(;;)
	   {
	   putFunc(img, val, x0, y0, size);
	   if (x0==x1 && y0==y1) break;
	   e2 = (err<<9);
	   if (e2 >= e2dy) { err += dy; x0 += sx; }
	   if (e2 <= e2dx) { err += dx; y0 += sy; }
	   }
	}
  //---------------------------------------------------------------------------
  // static variables - connected to surface
  //---------------------------------------------------------------------------
  static void *PenColor;
  static unsigned int PenSize;
  static speDotType PenType;
  static void *BrushColor;
  static void *Surface;
  static unsigned int Height;
  static unsigned int Width;
  //static spLineWalk<ptRGB> l1, l2;
  //---------------------------------------------------------------------------
  private:
  struct spFPoint
	{
	float x;
	float y;
	};
  struct spFColorPoint
	{
	float x;
	float y;
	float r;
	float g;
	float b;
	};
  //
  template <class T>
  struct TExecute
	{
	typedef void (*PointFunc)(spImage<T> *, T&, int&, int&, unsigned int&);
	};
  //typedef void (*TPutFunc)(spImage<T> *, T , int, int, unsigned int);
  // (__closure *TKeyUDEvent)(TObject *Sender, Word &Key, TShiftState Shift);
  //---------------------------------------------------------------------------
  template <class T>
  static void putPixel(spImage<T> *img, T &val, int &x, int &y, unsigned int &size)
	{
	if (y < 0) return;
	if (y > img->Height()-1) return;
	if (x < 0) return;
	if (x > img->Width()-1) return;
	img->Pixel(y, x) = val;     // draw point
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void putPixel(spImage<T> *img, spFColorPoint &P)
	{
	int y = fast_ftol(P.y);
	int x = fast_ftol(P.x);
	if (y < 0) return;
	if (y > img->Height()-1) return;
	if (x < 0) return;
	if (x > img->Width()-1) return;
	T val;
	if (typeid(T) == typeid(ptRGB))
	   {
	   val.ch[2] = P.r < 0.0f ? 0 : fast_ftol(P.r);  // fast_ftol -> P.r > 255.0f ? 255 : fast_ftol...???
	   val.ch[1] = P.g < 0.0f ? 0 : fast_ftol(P.g);
	   }
	val.ch[0] = P.b < 0.0f ? 0 : fast_ftol(P.b);
	img->Pixel(y, x) = val;     // draw point
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void putCircle(spImage<T> *img, T &val, int &x, int &y, unsigned int &size)
	{
	if (y < 0) return;
	if (y > img->Height()-1) return;
	if (x < 0) return;
	if (x > img->Width()-1) return;
	//size = size>>1;
	EllipseFilled(img, val, x, y, size>>1, size>>1);   // draw circle
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void putSquare(spImage<T> *img, T &val, int &x, int &y, unsigned int &size)
	{
	if (y < 0) return;
	if (y > img->Height()-1) return;
	if (x < 0) return;
	if (x > img->Width()-1) return;
	RectangleFilled(img, val, x-(size>>1), y-(size>>1), x+(size>>1), y+(size>>1));  // draw square
	}
  //---------------------------------------------------------------------------
  template <class T>
  static void limitPixelColor(spImage<T> *img, spFColorPoint &P, float &dr, float &dg, float &db)
	{
	if (P.r < 0 || P.g < 0 || P.b < 0 || P.r > 255 || P.g > 255 || P.b > 255)
	   {
	   P.r -= dr;
	   P.g -= dg;
	   P.b -= db;
	   }
	}
  //---------------------------------------------------------------------------
  template <class T>
  inline static void floodLeftRight(spImage <ptGray> *mask, spImage<T> *img, int &leftX, int &rightX, int yc, T &val)
	{
	// go left
	for (; leftX >= 0 && mask->Pixel(yc, leftX).ch[0] == 0x00; leftX--)
		{
		mask->Pixel(yc, leftX).ch[0] = 0xff;
		img->Pixel(yc, leftX) = val;     // draw point
		}
	// go right
	for (; rightX < mask->Width() && mask->Pixel(yc, rightX).ch[0] == 0x00; rightX++)
		{
		mask->Pixel(yc, rightX).ch[0] = 0xff;
		img->Pixel(yc, rightX) = val;    // draw point
		}
	}
  //---------------------------------------------------------------------------
  template <class T>
  inline static void floodEdge(spImage <ptGray> *mask, spImage<T> *img, int &leftX, int &rightX, int yc, T &val)
	{
	if (leftX > 0 /*&& leftEdge*/)
	   img->Pixel(yc, leftX) = val;  // draw point
	if (rightX < mask->Width()-1 /*&& rightEdge*/)
	   img->Pixel(yc, rightX) = val;    // draw point
	}
  //---------------------------------------------------------------------------
  inline static bool yAscFPoint(spFPoint lhs, spFPoint rhs) { return lhs.y < rhs.y;}
  inline static bool yAscTPoint(TPoint lhs, TPoint rhs) { return lhs.y < rhs.y;}
  inline static bool yAscColorFPoint(spFColorPoint lhs, spFColorPoint rhs) { return lhs.y < rhs.y;}
  //inline static bool yAscRGBPoint(spsColorPoint <ptRGB>lhs, spsColorPoint<ptRGB> rhs) { return lhs.y < rhs.y;}
  //inline static bool yAscGrayPoint(spsColorPoint <ptGray>lhs, spsColorPoint<ptGray> rhs) { return lhs.y < rhs.y;}
  template <class T>
  inline static bool yAscColorPoint( spsColorPoint<T> lhs, spsColorPoint<T> rhs) { return lhs.y < rhs.y;}
  //---------------------------------------------------------------------------
  template <class T>
  inline static void simpleWalk(spImage<T> *img, spLineWalk<T> &l1, spLineWalk<T> &l2, T &val)
	{
	int y_prev = -99999;
	// Trade: precission vs. speed
	while (l1.Walk()) // note: last pixel is not drawn - trade
	   {
	   if (y_prev != l1.y )
		  {
		  while (l1.y > l2.y)
			{
			if (!l2.Walk())
			   break;
			}
		  ScanLine(img, val, l1.y, l1.x, l2.x);
		  y_prev = l1.y;
		  }
	   }
	}
  //---------------------------------------------------------------------------
  template <class T>
  inline static void colorWalk(spImage<T> *img, spLineWalk<T> &l1, spLineWalk<T> &l2)
	{
	int y_prev = -99999;
	// Trade: precission vs. speed
	while (l1.ColorWalk()) // note: last pixel is not drawn - trade
	   {
	   if (y_prev != l1.y )
		  {
		  while (l1.y > l2.y)
			{
			if (!l2.ColorWalk())
			   break;
			}
		  ScanLine(img, l1.color, l2.color, l1.y, l1.x, l2.x);
		  y_prev = l1.y;
		  }
	   }
	}
  };
/*
void *spDraw::PenColor = 0;
unsigned int spDraw::PenSize = 1;
speDotType spDraw::PenType = dtCircle;
void *spDraw::BrushColor = 0;
void *spDraw::Surface = 0;
unsigned int spDraw::Height = 0;
unsigned int spDraw::Width = 0;
*/
//spLineWalk<ptRGB> spDraw::l1;
//spLineWalk<ptRGB> spDraw::l2;
#endif
