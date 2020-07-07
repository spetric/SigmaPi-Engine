#pragma hdrstop
#include "WuLine.h"
#include <math.h>
#include "ASMVars.h"
//-----------------------------------------------
// Get the integer part of x
//-----------------------------------------------
static int ipart(float x)
  {
  return static_cast<int>(x);
  }
//-----------------------------------------------
// Round the number
//-----------------------------------------------
static int round(float x)
  {
	return ipart(x + 0.5f);
  }
//-----------------------------------------------
// Get the fractional part of x
//-----------------------------------------------
static float fpart(float x)
  {
  return x - ipart(x);
  }
//-----------------------------------------------
// Get 1 - fractional part of x
//-----------------------------------------------
static float rfpart(float x)
  {
	return 1 - fpart(x);
  }
//-----------------------------------------------
// Add a vertex to the vertex array
//-----------------------------------------------
static void plot(std::vector<TVertex> &va, int x, int y, float c, const Byte *fColor, const Byte *bColor, const int chans)
  {
  TVertex vt;
  vt.x = x;
  vt.y = y;
  for (int k = 0; k < chans; k++)
	  vt.bgr[k] = fast_ftol((float)bColor[k] * (1.0 - c) + (float)fColor[k] * c);
  va.push_back(vt);
  }
//-----------------------------------------------
// Draw antialiased line
//-----------------------------------------------
void drawWuLine(std::vector<TVertex> &va, float x1, float y1, float x2, float y2, const Byte *fColor, const Byte *bColor, const int chans)
	{

	float dx = x2 - x1;
	float dy = y2 - y1;

	// Store all points in vector

	if (std::fabs(dx) > std::fabs(dy))
	   {
	   if (x2 < x1)
		  {
		  std::swap(x1, x2);
		  std::swap(y1, y2);
		  }

		float gradient;
		dx == 0.0 ? gradient = 0 : gradient = dy / dx;
		float xend = static_cast<float>(round(x1));
		float yend = y1 + gradient * (xend - x1);
		float xgap = rfpart(x1 + 0.5f);
		
		int xpxl1 = static_cast<int>(xend);
		int ypxl1 = ipart(yend);

		// Add the first endpoint
		plot(va, xpxl1, ypxl1, rfpart(yend) * xgap, fColor, bColor, chans);
		plot(va, xpxl1, ypxl1 + 1, fpart(yend) * xgap, fColor, bColor, chans);
		
		float intery = yend + gradient;
 
		xend = static_cast<float>(round(x2));
		yend = y2 + gradient * (xend - x2);
		xgap = fpart(x2 + 0.5f);
		
		int xpxl2 = static_cast<int>(xend);
		int ypxl2 = ipart(yend);
		
		// Add the second endpoint
		plot(va, xpxl2, ypxl2, rfpart(yend) * xgap, fColor, bColor, chans);
		plot(va, xpxl2, ypxl2 + 1, fpart(yend) * xgap, fColor, bColor, chans);

		// Add all the points between the endpoints
		for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
			{
			 plot(va, x, ipart(intery), rfpart(intery), fColor, bColor, chans);
			 plot(va, x, ipart(intery) + 1, fpart(intery), fColor, bColor, chans);
			 intery += gradient;
			}
		}
	else
		{
		if (y2 < y1)
			{
			std::swap(x1, x2);
			std::swap(y1, y2);
			}
		float gradient;
		dy == 0.0 ? gradient = 0 : gradient = dx / dy;
		float yend = static_cast<float>(round(y1));
		float xend = x1 + gradient * (yend - y1);
		float ygap = rfpart(y1 + 0.5f);
		
		int ypxl1 = static_cast<int>(yend);
		int xpxl1 = ipart(xend);

		// Add the first endpoint
		plot(va, xpxl1, ypxl1, rfpart(xend) * ygap, fColor, bColor, chans);
		plot(va, xpxl1, ypxl1 + 1, fpart(xend) * ygap, fColor, bColor, chans);
		
		float interx = xend + gradient;
 
		yend = static_cast<float>(round(y2));
		xend = x2 + gradient * (yend - y2);
		ygap = fpart(y2 + 0.5f);
		
		int ypxl2 = static_cast<int>(yend);
		int xpxl2 = ipart(xend);

		// Add the second endpoint
		plot(va, xpxl2, ypxl2, rfpart(xend) * ygap, fColor, bColor, chans);
		plot(va, xpxl2, ypxl2 + 1, fpart(xend) * ygap, fColor, bColor, chans);

		// Add all the points between the endpoints
		for (int y = ypxl1 + 1; y <= ypxl2 - 1; ++y)
			{
			 plot(va, ipart(interx), y, rfpart(interx), fColor, bColor, chans);
			 plot(va, ipart(interx) + 1, y, fpart(interx), fColor, bColor, chans);
			 interx += gradient;
			}
		}
   }
//-----------------------------------------------


