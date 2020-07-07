//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushSpray.h"
#include "ASMVars.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushSpray::TBrushSpray(void):
			 TBrushBasic()
{
CommonParams->Type = spe_brshSpray;
SpecificParams = new structSpecificParams;
setDefaultSpecificParams();
}
//---------------------------------------------------------------------------
// destructor
TBrushSpray::~TBrushSpray(void)
{
delete SpecificParams;
}
//---------------------------------------------------------------------------
TBrushBasic* TBrushSpray::Clone(void)
{
TBrushSpray *clonedBrush = new TBrushSpray();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->Thickness = par->Thickness;
clonedBrush->SpecificParams->IntVary = par->IntVary;
clonedBrush->SpecificParams->Density = par->Density;
clonedBrush->SpecificParams->Gravity = par->Gravity;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushSpray::setDefaultSpecificParams(void)
{
structSpecificParams *par = SpecificParams;
par->Thickness = 1;
par->IntVary = 0;
par->Density = 50;
par->Gravity = 1;
}
//---------------------------------------------------------------------------
UnicodeString TBrushSpray::ClassName(void)
{
return "TBrushSpray";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushSpray::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
State->cpROItol += (SpecificParams->Thickness)>>1;
pointsNum = fast_ftol(CommonParams->Size * SpecificParams->Density / (20.0 * SpecificParams->Thickness));
_surfRight->SetContext(255 , 255, SpecificParams->Thickness);
}
//---------------------------------------------------------------------------
void TBrushSpray::DrawShape(TRect *boundRect)
{
DrawVariations();
int i, lp, r_squared, half_size, dx, dy, radius;
//bool get_out = false;
int sign;
//int full_size = State->cpSize;
half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
/*
int half_size_squared = fast_ftol(State->cpHalfFloatSquared);
if (half_size_squared <= 0)
   half_size_squared = 2;
*/
radius = (SpecificParams->Thickness)>>1;
float gravity;
//int area_half, area_full;
//area_half = half_size;
//area_full = full_size;
/* stari naèin
for (lp = 0; lp < LineParams->Count; lp++)
	{
	for (i = 0; i < pointsNum; i++)
		{
		if (SpecificParams->Gravity > 1)
		   {
		   gravity = (random(SpecificParams->Gravity) + 1.0) * (random(SpecificParams->Gravity) + 1.0);
		   area_half = fast_ftol(half_size * 100.0/ gravity);
		   if (area_half > half_size)
			   area_half = half_size;
		   area_full = area_half*2;
		   }
		do
		  {
		  dx = area_half - random(area_full);
		  dy = area_half - random(area_full);
		  r_squared = dx*dx + dy*dy;
		  }
		while (r_squared > half_size_squared);
		if (SpecificParams->IntVary)
		   _surfRight->BrushColor = 255 - random(SpecificParams->IntVary);
		if (SpecificParams->Thickness > 1)
		   _surfRight->EllipseFilled(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy, radius, radius);
		else
		   _surfRight->BrushCoord(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy);
		}
	}
*/
// nov naèin - dobar!
double angle, fRad;
for (lp = 0; lp < LineParams->Count; lp++)
	{
	for (i = 0; i < pointsNum; i++)
		{
		angle = random(100) * M_PI*2.0/100.0;      // [0, Pi)
		fRad =  random(101)/100.0;				   // [0, 1]
		if (SpecificParams->Gravity == 0)
			fRad =  sqrt(fRad);   						// no gravity
		else if (SpecificParams->Gravity != 0.5)
			fRad =  pow(fRad, SpecificParams->Gravity + 0.5); // gravity
		dx = fast_ftol(fRad*cos(angle)* half_size);
		dy = fast_ftol(fRad*sin(angle)* half_size);
		if (SpecificParams->IntVary)
		   _surfRight->BrushColor = 255 - random(SpecificParams->IntVary);
		if (SpecificParams->Thickness > 1)
		   _surfRight->EllipseFilled(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy, radius, radius);
		else
		   _surfRight->BrushCoord(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy);
		}
	}
}
//---------------------------------------------------------------------------
void TBrushSpray::DrawTip(TRect *boundRect, int lp)
{
DrawVariations();
int i, r_squared, half_size, dx, dy, radius;
//bool get_out = false;
//int full_size = State->cpSize;
half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
/*
int half_size_squared = fast_ftol(State->cpHalfFloatSquared);
if (half_size_squared <= 0)
   half_size_squared = 2;
*/
radius = (SpecificParams->Thickness)>>1;
float gravity;
/*
int area_half, area_full;
area_half = half_size;
area_full = full_size;
*/
/* old way
for (i = 0; i < pointsNum; i++)
	{
	if (SpecificParams->Gravity > 1)
	   {
	   gravity = (random(SpecificParams->Gravity) + 1.0) * (random(SpecificParams->Gravity) + 1.0);
	   area_half = fast_ftol(half_size * 100.0/ gravity);
	   if (area_half > half_size)
		   area_half = half_size;
	   area_full = area_half*2;
	   }
	do
	  {
	  dx = half_size - random(full_size);
	  dy = half_size - random(full_size);
	  if (SpecificParams->Gravity > 1)
		 {
		 dx = fast_ftol(dx*1.0/ (random(SpecificParams->Gravity) + 1));
		 dy = fast_ftol(dy*1.0/ (random(SpecificParams->Gravity) + 1));
		 }
	  r_squared = dx*dx + dy*dy;
	  }
	while (r_squared > half_size_squared);

	if (SpecificParams->IntVary)
	   _surfRight->BrushColor = 255 - random(SpecificParams->IntVary);
	if (SpecificParams->Thickness > 1)
	   _surfRight->EllipseFilled(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy, radius, radius);
	else
	   _surfRight->BrushCoord(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy);
	}
*/
double angle, fRad;
for (i = 0; i < pointsNum; i++)
	{
	angle = random(100) * M_PI*2.0/100.0;      // [0, Pi)
	fRad =  random(101)/100.0;				   // [0, 1]
	if (SpecificParams->Gravity == 0)
		fRad =  sqrt(fRad);   						// no gravity
	else if (SpecificParams->Gravity != 0.5)
		fRad =  pow(fRad, SpecificParams->Gravity + 0.5); // gravity
	dx = fast_ftol(fRad*cos(angle)* half_size);
	dy = fast_ftol(fRad*sin(angle)* half_size);
	if (SpecificParams->IntVary)
	   _surfRight->BrushColor = 255 - random(SpecificParams->IntVary);
	if (SpecificParams->Thickness > 1)
	   _surfRight->EllipseFilled(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy, radius, radius);
	else
	   _surfRight->BrushCoord(LineParams->Point[lp].x + dx, LineParams->Point[lp].y + dy);
	}
}
//---------------------------------------------------------------------------


