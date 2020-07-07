//---------------------------------------------------------------------------
#ifndef grapSplineCRH
#define grapSplineCRH
#include "grapPoly.h"
//---------------------------------------------------------------------------
class TGrapSplineCR : public TGrapPoly
{
private:
	 struct structSplinePoint
		 {
		 double x, y;
		 };
	 TPoint *splinePts;
     int splineCount;
	 int Resolution;
	 structSplinePoint  A, B, C, D, *P;
	 int computeSpline(TPoint *poly_pts, TPoint *spline_pts, int poly_count, bool closed);
	 void computePoint(double t, structSplinePoint *Point);
	 void computeCoeffs(int n);
protected:
	//
public:
	TGrapSplineCR(void);
	virtual ~TGrapSplineCR(void);
	virtual UnicodeString ClassName(void);
    virtual TGrapBasic* Clone(void);
	//
	void DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed = false);
	void DrawPoly(std::vector<TPoint> &polyList);
	void GetSplinePoints(TPoint *polyPts, int &polyCount, bool closed = false);
};

#endif
