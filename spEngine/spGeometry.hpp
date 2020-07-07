//Copyright (C) 2016 Siniša Petriæ
#ifndef SP_GEOMETRY_H
#define SP_GEOMETRY_H
#include "ASMVars.h"
#include <Types.hpp>
#include "VoronoiDiagramGenerator.h"
class spGeometry
  {
  public:
  typedef double TFloat;
  struct TLine
	 {
	 int x1;
	 int y1;
	 int x2;
	 int y2;
	 };
  struct TPointDP
	 {
	 int x;
	 int y;
	 int maxI;
	 };
  //---------------------------------------------------------------------------
  static void RotatePoints(TPoint *points, int points_number, TPoint *origin, double angle)
	{
	int ox, oy, x1, y1;
	ox = origin->x;
	oy = origin->y;
	double cv = cos(angle);
	double sv = sin(angle);
	for (int i = 0; i < points_number; i++)
		{
		x1 = points[i].x;
		y1 = points[i].y;
		points[i].x = fast_ftol(ox + (x1 - ox)*cv - (y1 - oy)*sv);
		points[i].y = fast_ftol(oy + (x1 - ox)*sv + (y1 - oy)*cv);
		}
	}
  //---------------------------------------------------------------------------
  static void GetRotateData(TPoint *point, TPoint *origin, double angle, TPoint *rotoData, bool relative)
	{
	int ox, oy, x1, y1;
	ox = origin->x;
	oy = origin->y;
	x1 = point->x;
	y1 = point->y;
	rotoData->x = fast_ftol(ox + (x1 - ox)*cos(angle) - (y1 - oy)*sin(angle));
	rotoData->y = fast_ftol(oy + (x1 - ox)*sin(angle) + (y1 - oy)*cos(angle));
	if (relative)
	   {
	   rotoData->x = rotoData->x - x1;
	   rotoData->y = rotoData->y - y1;
	   }
	}
  //---------------------------------------------------------------------------
  static void GetRotateData(TPoint *point, TPoint *origin, double angle, double *rotoX, double *rotoY, bool relative)
	{
	double ox, oy, x1, y1;
	ox = (double)origin->x;
	oy = (double)origin->y;
	x1 = (double)point->x;
	y1 = (double)point->y;
	*rotoX = ox + (x1 - ox)*cos(angle) - (y1 - oy)*sin(angle);
	*rotoY = oy + (x1 - ox)*sin(angle) + (y1 - oy)*cos(angle);
	if (relative)
	   {
	   *rotoX = *rotoX - x1;
	   *rotoY = *rotoY - y1;
	   }
	}
  //---------------------------------------------------------------------------
  static void TranslatePoints(TPoint *points, int points_number, TPoint *trans)
	{
	for (int i = 0; i < points_number; i++)
		{
		points[i].x += trans->x;
		points[i].y += trans->y;
		}
	}
  //---------------------------------------------------------------------------
  static void RotateOneTranslateAll(TPoint *points, int points_number, TPoint *origin, double angle)
	{
	int ox, oy, x1, y1;
	ox = origin->x;
	oy = origin->y;
	x1 = points[0].x;
	y1 = points[0].y;
	// rotate first
	points[0].x = fast_ftol(ox + (x1 - ox)*cos(angle) - (y1 - oy)*sin(angle));
	points[0].y = fast_ftol(oy + (x1 - ox)*sin(angle) + (y1 - oy)*cos(angle));
	//
	x1 = points[0].x - x1;
	y1 = points[0].y - y1;
	for (int i = 1; i < points_number; i++)
		{
		points[i].x += x1;
		points[i].y += y1;
		}
	}
  //---------------------------------------------------------------------------
  static void ReflectPoints(TPoint *points, int points_number, TPoint *origin, double angle)
	{
	int ox, oy, x1, y1;
	ox = origin->x;
	oy = origin->y;
	double cv = cos(2.0*angle);
	double sv = sin(2.0*angle);
	for (int i = 0; i < points_number; i++)
		{
		x1 = points[i].x;
		y1 = points[i].y;
		points[i].x = fast_ftol(ox + (x1 - ox)*cv + (y1 - oy)*sv);
		points[i].y = fast_ftol(oy + (x1 - ox)*sv - (y1 - oy)*cv);
		}
	}
  //---------------------------------------------------------------------------
  static void ChangeLineLength(TPoint *pointS, TPoint *pointE, int nLength)
	{
	int x = pointE->x - pointS->x, y = pointE->y - pointS->y;
	double hypo = sqrt((double)(x*x + y*y));
	x = fast_ftol(1.0 * x * nLength/hypo);
	y = fast_ftol(1.0 * y * nLength/ hypo);
	pointE->x = x;
	pointE->y = y;
	}
  //---------------------------------------------------------------------------
  static void GetLineData(double *OutRadius, double *OutAngle, int x1, int y1, int x2, int y2)
	{
	//
	//  Let's find x and y and hypo.
	//  Then, let's find an angle.
	//
	int x_ord, y_ord;
	double hypo, angle;
	y_ord = y1 - y2;
	x_ord = x2 - x1;
	hypo = sqrt((double)(y_ord*y_ord + x_ord*x_ord));
	if (hypo == 0)
		{
		*OutAngle = 0;
		*OutRadius = 0;
		return;
		}
	// everything to first quadrant
	angle = asin((double)abs(y_ord) / hypo);
	//angle = 180.0*angle/M_PI; u stupnjevin
	//
	// quadrant?
	//
	// first or second
	if (y_ord >= 0)
		{
		if (x_ord < 0)
		   angle = M_PI - angle;
			//angle = 180.0 - angle;
		}
	// thirs or fourth
	else
		{
		angle = - angle;
		if (x_ord < 0)
			//angle = -180.0 - angle;
			angle = -M_PI - angle;
		}
	*OutAngle = angle;
	*OutRadius = hypo;
	//
	// What if coordinates are bad?
	//
	}
  //---------------------------------------------------------------------------
  static TPoint PointSub2D(TPoint &A, TPoint &B)
	{
	// Result = A - B
	TPoint Result;
	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;
	return Result;
	}
  //---------------------------------------------------------------------------
  static TFloat PointDotProd2D(TPoint &A, TPoint &B)
	{
	 // Dotproduct = A * B
	TFloat Result;
	Result = (TFloat)(A.X * B.X + A.Y * B.Y);
	return Result;
	}
  //---------------------------------------------------------------------------
  static double PointNormSquared2D(TPoint A)
	{
	// Square of the norm |A|
	TFloat Result;
	Result = (TFloat)(A.X * A.X + A.Y * A.Y);
	return Result;
	}
  //---------------------------------------------------------------------------
  static TFloat PointDistSquared2D(TPoint &A, TPoint &B)
	{
	// Square of the distance from A to B
	TFloat Result;
	Result = PointNormSquared2D(PointSub2D(A, B));
	return Result;
	}
  //---------------------------------------------------------------------------
  static void DouglasPeucker2D(TFloat Tol2, TPoint *Orig,  std::vector<TPointDP> &simplifiedPoints, int j, int k)
	// Simplify polyline in OrigList between j and k. point index will be added to std::vector
	// for each point that must be included
	{
	int i, MaxI;
	TFloat MaxD2, CU, CW, B, DV2;
	TPoint P0, P1, PB, U, W;
	TPointDP outPoint;
	// Is there anything to simplify?
	if (k <= j + 1)
	   return;
	P0 = Orig[j];
	P1 = Orig[k];
	U  = PointSub2D(P1, P0); // Segment vector
	CU = PointDotProd2D(U, U);  // Segment length squared
	MaxD2 = 0;
	MaxI  = 0;

	// Loop through points and detect the one furthest away
	for (i = j; i < k; i++)
		{
		W  = PointSub2D(Orig[i], P0);
		CW = PointDotProd2D(W, U);
		// Distance of point Orig[i] from segment
		if (CW <= 0) 	  // Before segment
		  DV2 = PointDistSquared2D(Orig[i], P0);
		else if (CW > CU) // Past segment
			DV2 = PointDistSquared2D(Orig[i], P1);
		else
			{
			// Fraction of the segment
			try {
			  B = CW / CU;
			  }
			catch(...) {
			  B = 0; // in case CU = 0
			  }
			PB.X = fast_ftol(P0.X + B * U.X);
			PB.Y = fast_ftol(P0.Y + B * U.Y);
			DV2 = PointDistSquared2D(Orig[i], PB);
			}
		// test with current max distance squared
		if (DV2 > MaxD2)
		   {
		   // Orig[i] is a new max vertex
		   MaxI  = i;
		   MaxD2 = DV2;
		   }
		}
	// If the furthest point is outside tolerance we must split
	if (MaxD2 > Tol2) // error is worse than the tolerance
	   {
	   // split the polyline at the farthest vertex from S
	   // Marker[MaxI] = true;  // mark Orig[maxi] for the simplified polyline
	   outPoint.x = Orig[MaxI].X;
	   outPoint.y = Orig[MaxI].Y;
	   outPoint.maxI = MaxI;
	   simplifiedPoints.push_back(outPoint);
	   //IndexList->AddItem(pxcf_PadLeft(IntToStr(MaxI), 10, '0'), listPoint);
	   // recursively simplify the two subpolylines at Orig[maxi]
	   DouglasPeucker2D(Tol2, Orig, simplifiedPoints, j, MaxI); // polyline Orig[j] to Orig[maxi]
	   DouglasPeucker2D(Tol2, Orig, simplifiedPoints, MaxI, k); // polyline Orig[maxi] to Orig[k]
	   }
  }
  //---------------------------------------------------------------------------
  static void VoronoiGenerate(std::vector<TPoint> &inpPoints, std::vector<TLine> &outLines, std::vector<Triangle> &outTriples,
							  int width, int height, float minDistance)
	{
	int points_number = inpPoints.size();
	float *xValues, *yValues;
	xValues = new float[points_number];
	yValues = new float[points_number];
	for (int i = 0; i < points_number; i++)
		{
		xValues[i] = inpPoints[i].x;
		yValues[i] = inpPoints[i].y;
		}
	VoronoiDiagramGenerator vdg;
	vdg.generateVoronoi(xValues, yValues, points_number, 0, width, 0, height, minDistance);
	delete []xValues;
	delete []yValues;
	vdg.resetIterator();
	TLine line;
	float x1, y1, x2, y2;
	while (vdg.getNext(x1, y1, x2, y2))
		{
		line.x1 = fast_ftol(x1);
		line.y1 = fast_ftol(y1);
		line.x2 = fast_ftol(x2);
		line.y2 = fast_ftol(y2);
		outLines.push_back(line);
		}
	// output Delaunay triangulation
	for (size_t i = 0; i < vdg.tripleList.size(); i++)
		outTriples.push_back(vdg.tripleList[i]);  // test
	}
  private:
	inline bool static yAscTPoint(TPoint lhs, TPoint rhs) { return lhs.y < rhs.y;}
  };
#endif
