//---------------------------------------------------------------------------
#pragma hdrstop
#include "grapSplineCR.h"
#include "ASMVars.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TGrapSplineCR::TGrapSplineCR(void)
		   : TGrapPoly()
{
Resolution = 20;
_maxPts = 150;
_dpTolerance = 200;
}
//---------------------------------------------------------------------------
// destructor
TGrapSplineCR::~TGrapSplineCR(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TGrapSplineCR::ClassName(void)
{
return "TGrapSplineCR";
}
//---------------------------------------------------------------------------
TGrapBasic* TGrapSplineCR::Clone(void)
{
TGrapSplineCR *shape = new TGrapSplineCR();
return shape;
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TGrapSplineCR::DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed)
{
bool saveSimplify = _simplify;
if (_maxPts == 0)
   _maxPts = polyCount;
simplifyPolyline(polyPts, polyCount);
_simplify = false;
// create spline points
splinePts = new TPoint[polyCount * Resolution];
P = new structSplinePoint[(polyCount + (closed ? 1: 0))* Resolution];
splineCount = computeSpline(polyPts, splinePts, polyCount, closed);
_simplify = false;
TGrapPoly::DrawPoly(boundRect, splinePts, splineCount);
// delete spline points
delete []P;
delete []splinePts;
_simplify = saveSimplify;
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TGrapSplineCR::DrawPoly(std::vector<TPoint> &polyList)
{
// create spline points
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
//---------------------------------------------------------------------------
void TGrapSplineCR::GetSplinePoints(TPoint *polyPts, int &polyCount, bool closed)
{
// create spline points
splinePts = new TPoint[polyCount * Resolution];
P = new structSplinePoint[(polyCount + (closed ? 1: 0)) * Resolution];
splineCount = computeSpline(polyPts, splinePts, polyCount, closed);
polyCount = splineCount;
for (int i = 0; i <= splineCount; i++)
    {
	polyPts[i].x = splinePts[i].x;
	polyPts[i].y = splinePts[i].y;
	}
// delete spline points
delete []P;
delete []splinePts;
}
//---------------------------------------------------------------------------
// private
//---------------------------------------------------------------------------
int TGrapSplineCR::computeSpline(TPoint *poly_pts, TPoint *spline_pts, int poly_count, bool closed)
{
int i, Segment, seg_count;
structSplinePoint Current, Next;
for (i = 0; i < poly_count; i++)
    {
	P[i+1].x = (double)(poly_pts[i].x);
    P[i+1].y = (double)(poly_pts[i].y);
	}
if (closed)
   P[++poly_count] = P[2];
//Duplicate start and end points
P [0]          = P [1];
P [poly_count+1] = P [poly_count];
//Loop along all the points, drawing a line to the next point
seg_count = 0;
for (i = 1; i < poly_count - closed ? 1 : 0; i++)
  {
  //Calculate coefficients for this point
  computeCoeffs (i);
  //Calculate the start point for the first segment
  computePoint (0, &Current);
  //Split into smaller segments
  for (Segment = 1; Segment <= Resolution; Segment++)
      {
      //Calculate end point
      computePoint ((double)Segment / Resolution, &Next);
      // save segment
      spline_pts[seg_count].x = fast_ftol(Current.x);
      spline_pts[seg_count].y = fast_ftol(Current.y);
      seg_count++;
      spline_pts[seg_count].x = fast_ftol(Next.x);
      spline_pts[seg_count].y = fast_ftol(Next.y);
      Current = Next;
      }
  }
return seg_count;
}
//---------------------------------------------------------------------------
void TGrapSplineCR::computePoint(double t, structSplinePoint *Point)
{
  double t2, t3;
  //Square and cube of t
  t2 = t * t;
  t3 = t2 * t;
  //Calculate coordinates
  Point->x = ((A.x * t3) + (B.x * t2) + (C.x * t) + D.x) / 2;
  Point->y = ((A.y * t3) + (B.y * t2) + (C.y * t) + D.y) / 2;
}
//---------------------------------------------------------------------------
void TGrapSplineCR::computeCoeffs(int n)
{
  /*
  Computes coefficients for point n.
  This is a matrix transform:
	-1  3 -3  1
	 2 -5  4 -1
	-1  0  1  0
	 0  2  0  0
  */
  //x-coefficients
  A.x =    -P [n - 1].x + 3 * P [n].x - 3 * P [n + 1].x + P [n + 2].x;
  B.x = 2 * P [n - 1].x - 5 * P [n].x + 4 * P [n + 1].x - P [n + 2].x;
  C.x =    -P [n - 1].x                   + P [n + 1].x;
  D.x =                   2 * P [n].x;

  //y-coefficients
  A.y =    -P [n - 1].y + 3 * P [n].y - 3 * P [n + 1].y + P [n + 2].y;
  B.y = 2 * P [n - 1].y - 5 * P [n].y + 4 * P [n + 1].y - P [n + 2].y;
  C.y =    -P [n - 1].y                   + P [n + 1].y;
  D.y =                   2 * P [n].y;
}




