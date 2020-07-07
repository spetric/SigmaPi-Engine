//---------------------------------------------------------------------------
#pragma hdrstop
#include "grapPoly.h"
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TGrapPoly::TGrapPoly(void)
		   : TGrapLine()
{
_maxPts = 0;
_dpTolerance = 0;
_simplify = false;
//FFillShape = fillPoly;
}
//---------------------------------------------------------------------------
// destructor
TGrapPoly::~TGrapPoly(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TGrapPoly::ClassName(void)
{
return "TGrapPoly";
}
//---------------------------------------------------------------------------
TGrapBasic* TGrapPoly::Clone(void)
{
TGrapPoly *shape = new TGrapPoly();
return shape;
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TGrapPoly::DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed)
{
// simplify
if (_simplify)
   {
   if (_maxPts == 0)
	  _maxPts = polyCount;
   simplifyPolyline(polyPts, polyCount);
   }
TGrapLine::DrawPoly(boundRect, polyPts, polyCount);
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TGrapPoly::DrawPoly(std::vector<TPoint> &polyList)
{
// simplify
if (_simplify)
   {
   int polyCount = (int)polyList.size();
   TPoint *polyPts = new TPoint[polyCount];
   for (int i = 0; i < polyCount; i++)
	   {
	   polyPts[i].x = polyList[i].x;
	   polyPts[i].y = polyList[i].y;
	   }
   DrawPoly(0, polyPts, polyCount);
   delete []polyPts;
   }
else
   {
   for (int i = 1; i < polyList.size(); i++)
	   DrawCoords(polyList[i-1].x, polyList[i-1].y, polyList[i].x, polyList[i].y);
   }
}
//---------------------------------------------------------------------------
void TGrapPoly::SetSimplifyRule(bool simplify, int dpTolerance, int maxPts)
{
_simplify = simplify;
_dpTolerance = dpTolerance;
_maxPts = maxPts;
}
//---------------------------------------------------------------------------
bool TGrapPoly::MouseDown(TShiftState Shift, int X, int Y)
{
if (!TGrapBasic::MouseDown(Shift, X, Y))
   return false;
TPoint pt;
_polyList.clear();
pt.X = X;
pt.Y = Y;
_polyList.push_back(pt);
return true;
}
//---------------------------------------------------------------------------
bool TGrapPoly::MouseMove(TShiftState Shift, int X, int Y)
{
if (!TGrapBasic::MouseMove(Shift, X, Y))
   return false;
TPoint pt;
pt.X = X;
pt.Y = Y;
_polyList.push_back(pt);
return true;
}
//---------------------------------------------------------------------------
bool TGrapPoly::MouseUp(TShiftState Shift, int X, int Y)
{
if (!TGrapBasic::MouseUp(Shift, X, Y))
   return false;
DrawPoly(_polyList);
return true;
}
//---------------------------------------------------------------------------
void TGrapPoly::GetSimplifiedPoints(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts)
{
_dpTolerance = dpTolerance;
_maxPts = maxPts;
if (_maxPts == 0)
   _maxPts = polyCount;
simplifyPolyline(polyPts, polyCount);
}
//---------------------------------------------------------------------------
// protected
//---------------------------------------------------------------------------
void TGrapPoly::simplifyPolyline(TPoint * polyPts, int &polyCount)
{
// Douglas-Peucker
std::vector<spGeometry::TPointDP> simplifiedPoints;
int simplifiedCount;
do
  {
  simplifiedPoints.clear();
  spGeometry::DouglasPeucker2D(_dpTolerance, polyPts, simplifiedPoints, 0, polyCount);
  simplifiedCount = simplifiedPoints.size();
  sort(simplifiedPoints.begin(), simplifiedPoints.end(), ascPointDP);
  for (int i = 0; i < simplifiedCount; i++)
	  {
	  polyPts[i+1].x = simplifiedPoints[i].x;
	  polyPts[i+1].y = simplifiedPoints[i].y;
	  }
  polyPts[simplifiedCount + 1].x = polyPts[polyCount - 1].x;
  polyPts[simplifiedCount + 1].y = polyPts[polyCount - 1].y;
  polyCount = simplifiedCount + 2;
  if (_maxPts == 0)
     break;
  }
while (simplifiedCount > _maxPts);
}


