//---------------------------------------------------------------------------
#pragma hdrstop
#include "grapLine.h"
#include <math.h>
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TGrapLine::TGrapLine(void)
		   : TGrapBasic()
{
}
//---------------------------------------------------------------------------
// destructor
TGrapLine::~TGrapLine(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TGrapLine::ClassName(void)
{
return "TGrapLine";
}
//---------------------------------------------------------------------------
TGrapBasic* TGrapLine::Clone(void)
{
TGrapLine *shape = new TGrapLine();
return shape;
}
//---------------------------------------------------------------------------
void TGrapLine::DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed)
{
for (int i = 1; i < polyCount; i++)
   TGrapLine::DrawCoords(polyPts[i-1].x, polyPts[i-1].y, polyPts[i].x, polyPts[i].y);
if (FClosed)
   TGrapLine::DrawCoords(polyPts[polyCount-1].x, polyPts[polyCount-1].y, polyPts[0].x, polyPts[0].y);
// find bounding rectangle
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
boundRect->left = minX;
boundRect->top  = minY;
boundRect->right = maxX;
boundRect->bottom = maxY;
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TGrapLine::DrawCoords(int x0, int y0, int x1, int y1)
{
 // Breshenham's algorithm
int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
int err = dx + dy, e2;

for(;;)
   {
   savePoint(x0, y0);     // points collector
   if (x0==x1 && y0==y1) break;
   e2 = (err<<1);
   //e2 = 2*err;
   if (e2 >= dy) { err += dy; x0 += sx; }
   if (e2 <= dx) { err += dx; y0 += sy; }
  }
}
/* nema te više Alijaaaaaa!
//---------------------------------------------------------------------------
bool TGrapLine::MouseUp(TShiftState Shift, int X, int Y)
{
if (!TGrapBasic::MouseUp(Shift, X, Y))
   return false;
DrawCoords(_mouseData.X_down, _mouseData.Y_down, _mouseData.X_up, _mouseData.Y_up);
return true;
}
*/


