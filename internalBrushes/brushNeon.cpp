//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushNeon.h"
#include "ASMVars.h"
#include <math.h>
#include "spGlobals.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushNeon::TBrushNeon(void):
			TBrushBasic()
{
CommonParams->Type = spe_brshNeon;
SpecificParams = new structSpecificParams;
setDefaultSpecificParams();
}
//---------------------------------------------------------------------------
// destructor
TBrushNeon::~TBrushNeon(void)
{
delete SpecificParams;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
TBrushBasic* TBrushNeon::Clone(void)
{
TBrushNeon *clonedBrush = new TBrushNeon();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->FalloffIndex = par->FalloffIndex;
clonedBrush->SpecificParams->LinearValue = par->LinearValue;
clonedBrush->SpecificParams->TrigonometricValue = par->TrigonometricValue;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushNeon::setDefaultSpecificParams(void)
{
structSpecificParams *par = SpecificParams;
par->FalloffIndex = 0;
par->LinearValue = 100;
par->TrigonometricValue = 32;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
UnicodeString TBrushNeon::ClassName(void)
{
return "TBrushNeon";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushNeon::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
int i, col_index, half_size;
double f_index, cor_size, norm_fact;
bool get_out = false;
half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
    half_size = 1;
cor_size  = (double)half_size*(SpecificParams->LinearValue)/100.0;
if (cor_size <= 0)
    cor_size = 1.0;
norm_fact = (double)half_size/cor_size;
_brushTipTol = 0;
_beginHalfSize = half_size + _brushTipTol;
_stampOriginal = new spSurface<ptGray>(_beginHalfSize  * 2, _beginHalfSize * 2, true);
_stampOriginal->Zero();
_stampOriginal->SetContext();
for (i = half_size; i >= 0; i--)
	{
	f_index = BrushArith((double)i , half_size, cor_size, norm_fact);
	col_index = fast_ftol(f_index*255.0);
	if (col_index > 255)
	   {
	   col_index = 255;
	   get_out = true;
	   }
	_stampOriginal->BrushColor = col_index;
	if (i)
		_stampOriginal->EllipseFilled(half_size, half_size, i, i);
	else
		_stampOriginal->BrushCoord(half_size, half_size);
	if (get_out)
		break;
	}
_stampWork = new spSurface<ptGray>(_stampOriginal);
}
//---------------------------------------------------------------------------
double TBrushNeon::BrushArith(double x_var, double half_fact, double cor_fact, double add_const)
{
double y_var;
switch (SpecificParams->FalloffIndex)
   {
   case 0:      // linear
		x_var = fabs(x_var);
		y_var = -x_var/cor_fact + add_const;
		break;
   case 1:      // quadratic
		y_var = 1.0 - x_var*x_var/(half_fact*half_fact);
		if (y_var < 0)
			y_var = 0;
		break;
   case 2:      // trigono
        y_var = 0.5 - cos(SpecificParams->TrigonometricValue * x_var *M_PI / half_fact)/2.0;
        break;
   default:
        y_var = x_var;
        break;
   }
return y_var;
}
//---------------------------------------------------------------------------
