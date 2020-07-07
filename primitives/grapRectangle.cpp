//---------------------------------------------------------------------------
#pragma hdrstop
#include "grapRectangle.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TGrapRectangle::TGrapRectangle(void)
		   : TGrapLine()
{
//FFillShape = fillRectangle;
FClosed = true;
}
//---------------------------------------------------------------------------
// destructor
TGrapRectangle::~TGrapRectangle(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TGrapRectangle::ClassName(void)
{
return "TGrapRectangle";
}
//---------------------------------------------------------------------------
TGrapBasic* TGrapRectangle::Clone(void)
{
TGrapRectangle *shape = new TGrapRectangle();
return shape;
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
bool TGrapRectangle::DrawBound(TRect *boundRect)
{
if (boundRect->Width() == 0 || boundRect->Height() == 0)
   return false;
boundRect->NormalizeRect();
RectPoint[0].x = boundRect->left;
RectPoint[0].y = boundRect->top;
RectPoint[1].x = boundRect->right;
RectPoint[1].y = boundRect->top;
RectPoint[2].x = boundRect->right;
RectPoint[2].y = boundRect->bottom;
RectPoint[3].x = boundRect->left;
RectPoint[3].y = boundRect->bottom;
for (int i = 1; i < 4; i++)
	DrawCoords(RectPoint[i-1].x, RectPoint[i-1].y, RectPoint[i].x, RectPoint[i].y);
//spoji kraj
DrawCoords(RectPoint[3].x, RectPoint[3].y, RectPoint[0].x, RectPoint[0].y);
return true;
}
//---------------------------------------------------------------------------
bool TGrapRectangle::MouseUp(TShiftState Shift, int X, int Y)
{
if (!TGrapBasic::MouseUp(Shift, X, Y))
   return false;
TRect boundRect;
boundRect.left    = _mouseData.X_down;
boundRect.top     = _mouseData.Y_down;
boundRect.right   = _mouseData.X_up;
boundRect.bottom  = _mouseData.Y_up;
boundRect.NormalizeRect();
RectPoint[0].x = boundRect.left;
RectPoint[0].y = boundRect.top;
RectPoint[1].x = boundRect.right;
RectPoint[1].y = boundRect.top;
RectPoint[2].x = boundRect.right;
RectPoint[2].y = boundRect.bottom;
RectPoint[3].x = boundRect.left;
RectPoint[3].y = boundRect.bottom;
for (int i = 1; i < 4; i++)
	DrawCoords(RectPoint[i-1].x, RectPoint[i-1].y, RectPoint[i].x, RectPoint[i].y);
//spoji kraj
DrawCoords(RectPoint[3].x, RectPoint[3].y, RectPoint[0].x, RectPoint[0].y);
return true;
}


