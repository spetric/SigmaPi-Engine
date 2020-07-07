//---------------------------------------------------------------------------
#ifndef grapPolyH
#define grapPolyH
#include "grapLine.h"
#include "spGeometry.hpp"
//---------------------------------------------------------------------------
class TGrapPoly : public TGrapLine
{
private:
    std::vector<TPoint> _polyList;
protected:
	//
	spGeometry::TFloat _dpTolerance;
	int _maxPts;
	bool _simplify;
	void simplifyPolyline(TPoint *polyPts, int &polyCount);
	inline static bool ascPointDP(spGeometry::TPointDP lhs, spGeometry::TPointDP rhs) { return lhs.maxI < rhs.maxI;}
public:
	TGrapPoly(void);
	virtual ~TGrapPoly(void);
	virtual UnicodeString ClassName(void);
    virtual TGrapBasic* Clone(void);
	//
	virtual void DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed = false);
	virtual void DrawPoly(std::vector<TPoint> &polyList);
	void SetSimplifyRule(bool simplify, int dpTolerance, int maxPts);
	bool MouseDown(TShiftState Shift, int X, int Y);
	bool MouseMove(TShiftState Shift, int X, int Y);
	bool MouseUp(TShiftState Shift, int X, int Y);
	void GetSimplifiedPoints(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts);
};
#endif
