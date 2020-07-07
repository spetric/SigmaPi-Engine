//---------------------------------------------------------------------------
#pragma hdrstop
#include "grapHand.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TGrapHand::TGrapHand(void)
		   : TGrapLine()
{
}
//---------------------------------------------------------------------------
// destructor
TGrapHand::~TGrapHand(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TGrapHand::ClassName(void)
{
return "TGrapHand";
}
//---------------------------------------------------------------------------
TGrapBasic* TGrapHand::Clone(void)
{
TGrapHand *shape = new TGrapHand();
return shape;
}
//---------------------------------------------------------------------------
bool TGrapHand::MouseMove(TShiftState Shift, int X, int Y)
{
if (!TGrapBasic::MouseMove(Shift, X, Y))
   return false;
Reset();
DrawCoords(_mouseData.X_move_from, _mouseData.Y_move_from, _mouseData.X_move_to, _mouseData.Y_move_to);
return true;
}
//---------------------------------------------------------------------------
bool TGrapHand::MouseUp(TShiftState Shift, int X, int Y)
{
return TGrapBasic::MouseUp(Shift, X, Y);
}

