//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushCml.h"
#include "ASMVars.h"
#include <math.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushCml::TBrushCml(void):
		   TBrushBasic()
{
CommonParams->Type = spe_brshCml;
SpecificParams = new structSpecificParams;
setDefaultSpecificParams();
grapCmlLine  = new TGrapLine(); // ne treba nam imagecontainer, jer uzimamo raw points
cml = new TAutomataCml();
}
//---------------------------------------------------------------------------
// destructor
TBrushCml::~TBrushCml(void)
{
delete SpecificParams;
delete grapCmlLine;
delete cml;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushCml::setDefaultSpecificParams(void)
{
structSpecificParams *par = SpecificParams;
par->MinIntensity = 10;
par->MaxIntensity = 255;
par->LatticeType = 0;
par->SeedIntensity = 100;
par->EpsilonGrow = 0.03;
par->EpsilonDecay = 0.03;
par->Iterations = 2;
par->CreateProbability = 50;
par->DecayProbability = 100;
par->GrowProbability = 100;
par->Reborn = false;
par->MatrixMes = false;
par->DelayCreation = false;
par->RandomSplat = false;
par->SequentialScan = false;
par->GrowFunc = 0;
par->DecayFunc = 0;
par->EpsilonWeight = 0.5;
par->CreationMutation = 0;
par->DestructionMutation = 0;
for (int i = 0; i < 9; i++)
	{
	par->CreationOps[i] = 0;          // OR
	par->DestructionOps[i] = 1;       // AND
	}
}
//---------------------------------------------------------------------------
UnicodeString TBrushCml::ClassName(void)
{
return "TBrushCml";
}
//---------------------------------------------------------------------------
TBrushBasic* TBrushCml::Clone(void)
{
TBrushCml *clonedBrush = new TBrushCml();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->MinIntensity = par->MinIntensity;
clonedBrush->SpecificParams->MaxIntensity = par->MaxIntensity;
clonedBrush->SpecificParams->SeedIntensity = par->SeedIntensity;
clonedBrush->SpecificParams->LatticeType = par->LatticeType;
clonedBrush->SpecificParams->EpsilonGrow  = par->EpsilonGrow;
clonedBrush->SpecificParams->EpsilonDecay = par->EpsilonDecay;
clonedBrush->SpecificParams->Iterations =  par->Iterations;
clonedBrush->SpecificParams->CreateProbability =  par->CreateProbability;
clonedBrush->SpecificParams->DecayProbability = par->DecayProbability;
clonedBrush->SpecificParams->GrowProbability =  par->GrowProbability;
clonedBrush->SpecificParams->Reborn = par->Reborn;
clonedBrush->SpecificParams->MatrixMes = par->MatrixMes;
clonedBrush->SpecificParams->DelayCreation = par->DelayCreation;
clonedBrush->SpecificParams->RandomSplat = par->RandomSplat;
clonedBrush->SpecificParams->SequentialScan = par->SequentialScan;
clonedBrush->SpecificParams->GrowFunc = par->GrowFunc;
clonedBrush->SpecificParams->DecayFunc = par->DecayFunc;
clonedBrush->SpecificParams->EpsilonWeight = par->EpsilonWeight;
//
clonedBrush->SpecificParams->CreationMutation = par->CreationMutation;
clonedBrush->SpecificParams->DestructionMutation = par->DestructionMutation;
//
for (int i = 0; i < 9; i++)
	{
	clonedBrush->SpecificParams->CreationOps[i]    = par->CreationOps[i];
	clonedBrush->SpecificParams->DestructionOps[i] = par->DestructionOps[i];
	}
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushCml::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
_brushTipTol = 1;
_beginHalfSize = half_size + _brushTipTol;
_stampOriginal = new spSurface<ptGray>(_beginHalfSize  * 2, _beginHalfSize * 2, true);
_stampOriginal->Zero();
_stampOriginal->SetContext(255, 255, _brushTipTol);
if (half_size > 1)
   _stampOriginal->EllipseFilled(_beginHalfSize, _beginHalfSize, half_size, half_size);
else
   _stampOriginal->PenCoord(half_size, half_size);
_stampWork = new spSurface<ptGray>(_stampOriginal);
cml->Prepare(_surfLeft, _surfRight, reinterpret_cast <TAutomataCml::structSpecificParams*> (SpecificParams));
}
//---------------------------------------------------------------------------
void TBrushCml::DrawShape(TRect *boundRect)
{
if (!_stampWork)
   return;
DrawVariations();
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
half_size += _brushTipTol;
ResizeStamper(half_size, _invertStamp);
for (int lp = 0; lp < LineParams->Count; lp++)
	{
	_brushTipRect = GetTipBoundRectClear(LineParams->Point[lp].x, LineParams->Point[lp].y,
										_beginHalfSize, _surfRight->Width, _surfRight->Height);
	CopyStampToMask(_stampWork, cml->Mask, &_brushTipRect);
	}
cml->ExecuteShape(boundRect, LineParams->Point, LineParams->Count);
}
//---------------------------------------------------------------------------
void TBrushCml::DrawTip(TRect *boundRect, int lp)
{
DrawVariations();
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
half_size += _brushTipTol;
ResizeStamper(half_size, _invertStamp);
_brushTipRect = GetTipBoundRectClear(LineParams->Point[lp].x, LineParams->Point[lp].y,
									 _beginHalfSize, _surfRight->Width, _surfRight->Height);
CopyStampToMask(_stampWork, cml->Mask, &_brushTipRect);

cml->ExecuteTip(&_brushTipRect, &(LineParams->Point[lp]));
}
//---------------------------------------------------------------------------
void TBrushCml::End(int param)
{
cml->Update(param);
}
//---------------------------------------------------------------------------

