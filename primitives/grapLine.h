//---------------------------------------------------------------------------
#ifndef grapLineH
#define grapLineH
#include "grapBasic.h"
//---------------------------------------------------------------------------
class TGrapLine : public TGrapBasic
{
private:
protected:
    //
public:
	TGrapLine(void);
	virtual ~TGrapLine(void);
	virtual UnicodeString ClassName(void);
    virtual TGrapBasic* Clone(void);
	//
	virtual void DrawCoords(int x0, int y0, int x1, int y1);
	virtual void DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed = false);
	//virtual bool MouseUp(TShiftState Shift, int X, int Y);
};
#endif
