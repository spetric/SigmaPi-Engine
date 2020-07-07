//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushStar.h"
#include "ASMVars.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushStar::TBrushStar(void):
			TBrushBasic()
{
CommonParams->Type = spe_brshStar;
SpecificParams = new structSpecificParams;
setDefaultSpecificParams();
}
//---------------------------------------------------------------------------
// destructor
TBrushStar::~TBrushStar(void)
{
delete SpecificParams;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushStar::setDefaultSpecificParams(void)
{
structSpecificParams *par = SpecificParams;
par->Thickness = 1;
par->IntVary = 0;
par->Density = 30;    //TODO: number of points (inch or some other measure)???
}
//---------------------------------------------------------------------------
UnicodeString TBrushStar::ClassName(void)
{
return "TBrushStar";
}
//---------------------------------------------------------------------------
TBrushBasic* TBrushStar::Clone(void)
{
TBrushStar *clonedBrush = new TBrushStar();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->Thickness = par->Thickness;
clonedBrush->SpecificParams->IntVary = par->IntVary;
clonedBrush->SpecificParams->Density = par->Density;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushStar::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
State->cpROItol += (SpecificParams->Thickness)>>1;
pointsNum = fast_ftol(CommonParams->Size * SpecificParams->Density / (100.0 * SpecificParams->Thickness));
if (pointsNum == 0)
   pointsNum++;
_surfRight->SetContext(255 , 255, SpecificParams->Thickness);
}
//---------------------------------------------------------------------------
void TBrushStar::DrawShape(TRect *boundRect)
{
DrawVariations();
int half_size_squared, lp, i, dx, dy, r_squared, full_size, half_size;
full_size = State->cpSize;
half_size = fast_ftol(State->cpHalfFloat);
half_size_squared = fast_ftol(State->cpHalfFloatSquared);
for (lp = 0; lp < LineParams->Count; lp++)
    {
    for (i = 0; i < pointsNum; i++)
        {
        do
          {
          dx = half_size - random(full_size);
          dy = half_size - random(full_size);
          r_squared = dx*dx + dy*dy;
          }
	   //TODO kvadrat while ((r2 > half2)&&(mgl_shape));
	   while (r_squared > half_size_squared);
	   if (SpecificParams->IntVary)
		   _surfRight->PenColor = 255 - random(SpecificParams->IntVary);
	   _surfRight->Line(LineParams->Point[lp].x, LineParams->Point[lp].y, LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy);
       }
    }
}
//---------------------------------------------------------------------------
void TBrushStar::DrawTip(TRect *boundRect, int lp)
{
DrawVariations();
int half_size_squared, i, dx, dy, r_squared, full_size, half_size;
full_size = State->cpSize;
half_size = fast_ftol(State->cpHalfFloat);
half_size_squared = fast_ftol(State->cpHalfFloatSquared);
for (i = 0; i < pointsNum; i++)
    {
    do
      {
      dx = half_size - random(full_size);
      dy = half_size - random(full_size);
      r_squared = dx*dx + dy*dy;
      }
   //TODO kvadrat while ((r2 > half2)&&(mgl_shape));
   while (r_squared > half_size_squared);
   if (SpecificParams->IntVary)
	   _surfRight->PenColor = 255 - random(SpecificParams->IntVary);
   _surfRight->Line(LineParams->Point[lp].x, LineParams->Point[lp].y, LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy);
   }
}
//---------------------------------------------------------------------------

