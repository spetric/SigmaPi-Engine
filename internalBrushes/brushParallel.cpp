//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushParallel.h"
#include "ASMVars.h"
#include <math.h>
#include "spGeometry.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushParallel::TBrushParallel(void):
						   TBrushBasic()
{
CommonParams->Type = spe_brshParallel;
SpecificParams = new structSpecificParams;
setDefaultSpecificParams();
grapParaLine  = new TGrapLine(); // no need for image continer - raw points
}
//---------------------------------------------------------------------------
// destructor
TBrushParallel::~TBrushParallel(void)
{
delete SpecificParams;
delete grapParaLine;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
TBrushBasic* TBrushParallel::Clone(void)
{
TBrushParallel *clonedBrush = new TBrushParallel();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->Thickness = par->Thickness;
clonedBrush->SpecificParams->IntVary = par->IntVary;
clonedBrush->SpecificParams->Spacing = par->Spacing;
clonedBrush->SpecificParams->Angle = par->Angle;
clonedBrush->SpecificParams->BlurRadius = par->BlurRadius;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushParallel::setDefaultSpecificParams(void)
{
structSpecificParams *par = SpecificParams;
par->Shape = 0;
par->Thickness = 2;
par->IntVary = 0;
par->Spacing = 4;
par->Angle = 90;
par->BlurRadius = 0;
}
//---------------------------------------------------------------------------
UnicodeString TBrushParallel::ClassName(void)
{
return "TBrushParallel";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushParallel::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
structStateParams *par = State;
//int midPos, stepper, i, half_size = fast_ftol(State->cpHalfFloat);
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size == 0)
   half_size = 1;
TPoint points[2];
TPoint origin;
points[0].x =  0;
points[0].y = half_size;
points[1].x = State->cpSize;
points[1].y = half_size;
origin.x = half_size;
origin.y = half_size;
double radians = SpecificParams->Angle *  M_PI/180.0;
//
_radius = (SpecificParams->Thickness)>>1;
_brushTipTol = SpecificParams->Thickness + (SpecificParams->BlurRadius > 0 ? SpecificParams->BlurRadius*2 + 1 : 0);
_beginHalfSize = half_size + _brushTipTol + 2; //SpecificParams->Thickness + 2; // corrections because of rounding
_stampOriginal = new spSurface<ptGray>(_beginHalfSize  * 2, _beginHalfSize * 2, true);
_stampOriginal->Zero();
_stampOriginal->SetContext();
_stampOriginal->BrushColor = 255;
//
if (SpecificParams->Shape == 2) // circular rotation / tube
   {
   spGeometry::RotatePoints(points, 2, &origin, radians);
   double rotoStep = (double)SpecificParams->Spacing / (double)half_size;
   for (double astep = 0; astep < 2.0*M_PI; astep = astep + rotoStep)
	   {
	   if (SpecificParams->IntVary)
		  _stampOriginal->BrushColor = 255 - random(SpecificParams->IntVary);
	   if (SpecificParams->Thickness > 1)
		  _stampOriginal->EllipseFilled(points[1].x, points[1].y, _radius, _radius);
	   else
		  _stampOriginal->BrushCoord(points[1].x, points[1].y);
	   spGeometry::RotatePoints(points, 2, &origin, rotoStep);
	   }
   }
else  // simple
   {
	spGeometry::RotatePoints(points, 2, &origin, radians);
	grapParaLine->Reset();
	grapParaLine->DrawCoords(points[0].x, points[0].y, points[1].x, points[1].y);
	grapParaLine->GetRawPoints();
	prepareStamp(origin);
	if (SpecificParams->Shape == 1)    // cross (add points rotated by 90
	   {
	   radians = -M_PI_2;
	   spGeometry::RotatePoints(points, 2, &origin, radians);
	   grapParaLine->Reset();
	   grapParaLine->DrawCoords(points[0].x, points[0].y, points[1].x, points[1].y);
	   grapParaLine->GetRawPoints();
	   prepareStamp(origin);
	   }
   }
/*
if (SpecificParams->Shape > 1)
  {
   radians = -M_PI_4;
   spGeometry::RotatePoints(points, 2, &origin, radians);
   grapParaLine->Reset();
   grapParaLine->DrawCoords(points[0].x, points[0].y, points[1].x, points[1].y);
   grapParaLine->GetRawPoints();
   prepareStamp(origin);
   radians = -M_PI_2;
   spGeometry::RotatePoints(points, 2, &origin, radians);
   grapParaLine->Reset();
   grapParaLine->DrawCoords(points[0].x, points[0].y, points[1].x, points[1].y);
   grapParaLine->GetRawPoints();
   prepareStamp(origin);
  }
*/
//
State->cpROItol += _brushTipTol; //(SpecificParams->Thickness)>>1;
if (SpecificParams->BlurRadius > 0)
   spCommon::BoxBlurGray(_stampOriginal, SpecificParams->BlurRadius);
_stampWork = new spSurface<ptGray>(_stampOriginal);
}
//---------------------------------------------------------------------------
void TBrushParallel::prepareStamp(TPoint origin)
{
int midPos, stepper, i;
midPos = (grapParaLine->CountRaw)>>1;
// first half
stepper = -1;
for (i = midPos; i >= 0; i--)
	{
	origin = grapParaLine->PointRaw[i];
	if (++stepper <  SpecificParams->Spacing)
	   continue;
	if (SpecificParams->IntVary)
	   _stampOriginal->BrushColor = 255 - random(SpecificParams->IntVary);
	if (SpecificParams->Thickness > 1)
	   _stampOriginal->EllipseFilled(origin.x, origin.y, _radius, _radius);
	else
	   _stampOriginal->BrushCoord(origin.x, origin.y);
	stepper = 0;
	}
// second half
stepper = SpecificParams->Spacing;
for (i = midPos; i < grapParaLine->CountRaw; i++)
	{
	origin = grapParaLine->PointRaw[i];
	if (++stepper <  SpecificParams->Spacing)
	   continue;
	if (SpecificParams->IntVary)
	   _stampOriginal->BrushColor = 255 - random(SpecificParams->IntVary);
	if (SpecificParams->Thickness > 1)
	   _stampOriginal->EllipseFilled(origin.x, origin.y, _radius, _radius);
	else
	   _stampOriginal->BrushCoord(origin.x, origin.y);
	stepper = 0;
	}

}
//---------------------------------------------------------------------------
