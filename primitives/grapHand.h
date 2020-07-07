//---------------------------------------------------------------------------
#ifndef grapHandH
#define grapHandH
#include "grapLine.h"
//---------------------------------------------------------------------------
class TGrapHand : public TGrapLine
{
private:
protected:
    //
public:
	TGrapHand(void);
	virtual ~TGrapHand(void);
	//
	UnicodeString ClassName(void);
    TGrapBasic* Clone(void);
	bool MouseMove(TShiftState Shift, int X, int Y);
	bool MouseUp(TShiftState Shift, int X, int Y);
};
#endif
