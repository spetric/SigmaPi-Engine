//---------------------------------------------------------------------------
#ifndef grapRectangleH
#define grapRectangleH
#include "grapLine.h"
//---------------------------------------------------------------------------
class TGrapRectangle : public TGrapLine
{
protected:
	//
	TPoint RectPoint[4];
public:
	TGrapRectangle(void);
	virtual ~TGrapRectangle(void);
	//
	UnicodeString ClassName(void);
	TGrapBasic* Clone(void);
    //
	bool DrawBound(TRect *boundRect);
	bool MouseUp(TShiftState Shift, int X, int Y);
};
#endif
