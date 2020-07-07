//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushHair.h"
#include "ASMVars.h"
#include <math.h>
//#include "spGlobals.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushHair::TBrushHair(void):
			TBrushBasic()
{
CommonParams->Type = spe_brshHair;
SpecificParams = new structSpecificParams;
setDefaultSpecificParams();
}
//---------------------------------------------------------------------------
// destructor
TBrushHair::~TBrushHair(void)
{
delete SpecificParams;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
TBrushBasic* TBrushHair::Clone(void)
{
TBrushHair *clonedBrush = new TBrushHair();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->Thickness = par->Thickness;
clonedBrush->SpecificParams->IntVary = par->IntVary;
clonedBrush->SpecificParams->Density = par->Density;
clonedBrush->SpecificParams->BlurRadius = 0;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushHair::setDefaultSpecificParams(void)
{
structSpecificParams *par = SpecificParams;
par->Thickness = 1;
par->IntVary = 0;
par->Density = 30;
par->BlurRadius = 0;
}
//---------------------------------------------------------------------------
UnicodeString TBrushHair::ClassName(void)
{
return "TBrushHair";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushHair::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
int dx, dy, pointsNum, radius, r_squared, half_size_squared, half_size = fast_ftol(State->cpHalfFloat);
if (half_size == 0)
   half_size = 1;
half_size_squared = half_size * half_size;
if (half_size_squared == 1)
   half_size_squared = 2;
pointsNum = fast_ftol(CommonParams->Size * SpecificParams->Density / (10.0 * SpecificParams->Thickness));
if (pointsNum == 0)
   pointsNum++;
radius = (SpecificParams->Thickness)>>1;
_brushTipTol = SpecificParams->Thickness + (SpecificParams->BlurRadius > 0 ? SpecificParams->BlurRadius*2 + 1 : 0);
_beginHalfSize = half_size + _brushTipTol;
_stampOriginal = new spSurface<ptGray>(_beginHalfSize  * 2, _beginHalfSize * 2, true);
_stampOriginal->Zero();
_stampOriginal->SetContext();
for (int i = 0; i < pointsNum; i++)
	{
	do
	  {
	  dx = half_size - random(State->cpSize);
	  dy = half_size - random(State->cpSize);
	  r_squared = dx*dx + dy*dy;
	  }
	while (r_squared > half_size_squared);
	dx += _beginHalfSize;
	dy += _beginHalfSize;
	if (SpecificParams->IntVary)
	   _stampOriginal->BrushColor = 255 - random(SpecificParams->IntVary);
	else
	   _stampOriginal->BrushColor = 255;
	if (SpecificParams->Thickness > 1)
		_stampOriginal->EllipseFilled(dx, dy, radius, radius);
	else
		_stampOriginal->BrushCoord(dx, dy);
	}
//State->cpROItol += (SpecificParams->Thickness)>>1;
State->cpROItol += _brushTipTol;
if (SpecificParams->BlurRadius > 0)
   spCommon::BoxBlurGray(_stampOriginal, SpecificParams->BlurRadius);
_stampWork = new spSurface<ptGray>(_stampOriginal);
}
//---------------------------------------------------------------------------

