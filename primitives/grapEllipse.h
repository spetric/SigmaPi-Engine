//---------------------------------------------------------------------------
#ifndef grapEllipseH
#define grapEllipseH
#include "grapLine.h"
//---------------------------------------------------------------------------
class TGrapEllipse : public TGrapLine
{
private:
	std::vector<structPtChunk*> chunkList[4];
	size_t chunkPosition[4];
    bool isPoly;
protected:
    //
public:
	TGrapEllipse(void);
	virtual ~TGrapEllipse(void);
	//
	UnicodeString ClassName(void);
	TGrapBasic* Clone(void);
	void DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed = false);
    //
	bool DrawBound(TRect *boundRect);
	void DrawCoords(int xc, int yc, int rx, int ry);
	void Rotate(TRect *boundRect, TPoint *origin, double angle);
	void GetStepPoints(int lineStep);
	void GetRawPoints(void);
	void Reset(void);
	bool MouseUp(TShiftState Shift, int X, int Y);
};
#endif
