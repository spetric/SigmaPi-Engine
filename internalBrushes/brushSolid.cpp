//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushSolid.h"
#include "ASMVars.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushSolid::TBrushSolid(void):
			 TBrushBasic()
{
CommonParams->Type = spe_brshSolid;
SpecificParams = new structSpecificParams;
setDefaultSpecificParams();
}
//---------------------------------------------------------------------------
// destructor
TBrushSolid::~TBrushSolid(void)
{
delete SpecificParams;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushSolid::setDefaultSpecificParams(void)
{
structSpecificParams *par = SpecificParams;
par->Shape = 0;
par->Thickness = 1;
par->Filled = true;
par->Angle = 0;
par->RandomAngle = true;
par->BlurRadius = 0;
}
//---------------------------------------------------------------------------
UnicodeString TBrushSolid::ClassName(void)
{
return "TBrushSolid";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
TBrushBasic* TBrushSolid::Clone(void)
{
TBrushSolid *clonedBrush = new TBrushSolid();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->Shape = par->Shape;
clonedBrush->SpecificParams->Thickness = par->Thickness;
clonedBrush->SpecificParams->Filled = par->Filled;
clonedBrush->SpecificParams->Angle = par->Angle;
clonedBrush->SpecificParams->RandomAngle = par->RandomAngle;
clonedBrush->SpecificParams->BlurRadius = par->BlurRadius;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushSolid::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
int ps =  SpecificParams->Filled ? 1 : SpecificParams->Thickness;
_brushTipTol = ps   +  (SpecificParams->BlurRadius > 0 ? SpecificParams->BlurRadius*2 + 1 : 0);
_beginHalfSize = half_size + _brushTipTol;
_stampOriginal = new spSurface<ptGray>(_beginHalfSize  * 2, _beginHalfSize * 2, true);
_stampOriginal->Zero();
_stampOriginal->SetContext(255, 255, SpecificParams->Thickness);
if (half_size > 1)
   {
   switch (SpecificParams->Shape)
	  {
	  case 1:
		   if (SpecificParams->Filled)
			   _stampOriginal->RectangleFilled(_brushTipTol, _brushTipTol, half_size*2 + _brushTipTol, half_size*2 + _brushTipTol);
		   else
			   _stampOriginal->Rectangle(_brushTipTol, _brushTipTol, half_size*2 + _brushTipTol - ps, half_size*2 + _brushTipTol - ps);
		   break;
	  case 2:
		   {
		   TPoint tr[3];
		   double angle;
		   if (SpecificParams->RandomAngle)
			  angle = random(100) * M_PI*2.0/100.0;
		   else
			  angle = SpecificParams->Angle * M_PI/180.0;
		   float incr = M_PI/1.5;
		   for (int k = 0; k < 3; k++)
			   {
			   tr[k].x = fast_ftol(cos(angle + k*incr)* half_size) + _beginHalfSize;
			   tr[k].y = fast_ftol(sin(angle + k*incr)* half_size) + _beginHalfSize;
			   }
		   if (SpecificParams->Filled)
			   _stampOriginal->TriangleFilled(tr[0].x, tr[0].y, tr[1].x, tr[1].y, tr[2].x, tr[2].y, false); // ok
		   else
			   _stampOriginal->Triangle(tr[0].x, tr[0].y, tr[1].x, tr[1].y, tr[2].x, tr[2].y); //ok
		   }
		   break;
	  default:    // case 0
		   if (SpecificParams->Filled)
			   _stampOriginal->EllipseFilled(_beginHalfSize, _beginHalfSize, half_size, half_size);
			   //_stampOriginal->EllipseFilled(_beginHalfSize, _beginHalfSize, _beginHalfSize, _beginHalfSize);
		   else
			   _stampOriginal->Ellipse(_beginHalfSize, _beginHalfSize, half_size - ps, half_size - ps);
		   break;
	  }
   }
else
   _stampOriginal->PenCoord(half_size, half_size);
State->cpROItol += _brushTipTol;
if (SpecificParams->BlurRadius > 0)
   spCommon::BoxBlurGray(_stampOriginal, SpecificParams->BlurRadius);
//_stampOriginal->Rectangle(1, 1, _stampOriginal->WidthM1, _stampOriginal->HeightM1);
_stampWork = new spSurface<ptGray>(_stampOriginal);
}
//---------------------------------------------------------------------------
