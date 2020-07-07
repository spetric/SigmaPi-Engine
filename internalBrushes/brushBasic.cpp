//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushBasic.h"
#include <math.h>
#include "ASMVars.h"
#include "spGlobals.h"
#include "spCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TBrushBasic::TBrushBasic(void)
{
//CommonParams = new structCommonParams;
CommonParams = new Tspe_BrushCommonParams;;
CommonParams->Type = spe_brshUndefined;
State = new structStateParams;
LineParams = new structLineParams;
LineParams->Point = 0;
SetDefaultCommonParams();
ColorParams = new TBrushColors();
// set color scheme index to 0
ColorParams->SetSchemeIndex((TspeColorScheme)0);
_stampOriginal = 0;
_stampWork = 0;
_invertStamp = false;
_brushTipTol = 0;
_resizedOnTheFly = false;
_surfLeft = 0;
_surfRight = 0;
_grapShape = 0;
_sizeEnvelope = 0;
_capacityEnvelope = 0;
State->cpEnvelopeSizeFactor = 0;
State->cpEnvelopeCapacityFactor = 0;
State->cpHalfSizeMax = 0;
}
//---------------------------------------------------------------------------
// destructor
TBrushBasic::~TBrushBasic(void)
{
//
delete CommonParams;
delete State;
delete ColorParams;
if (LineParams->Point)
   delete[] (LineParams->Point);
delete LineParams;
if (_stampOriginal)
   {
   delete _stampOriginal;
   _stampOriginal = 0;
   }
if (_stampWork)
   {
   delete _stampWork;
   _stampWork = 0;
   }
if (_sizeEnvelope)
   {
   delete []_sizeEnvelope;
   _sizeEnvelope = 0;
   }
if (_capacityEnvelope)
   {
   delete []_capacityEnvelope;
   _capacityEnvelope = 0;
   }
if (State->cpEnvelopeSizeFactor)
   {
   delete []State->cpEnvelopeSizeFactor;
   State->cpEnvelopeSizeFactor = 0;
   }
if (State->cpEnvelopeCapacityFactor)
   {
   delete []State->cpEnvelopeCapacityFactor;
   State->cpEnvelopeCapacityFactor = 0;
   }
}
//---------------------------------------------------------------------------
// connector
void TBrushBasic::Connect(spSurface<ptGray> *surfLeft, spSurface<ptGray> *surfRight, TGrapBasic *grapShape)
{
_surfLeft = surfLeft;
_surfRight = surfRight;
_grapShape = grapShape;
}
//---------------------------------------------------------------------------
// public  virutal
TBrushBasic* TBrushBasic::Clone(void)
{
TBrushBasic *clonedBrush = new TBrushBasic();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
void TBrushBasic::CopyParams(TBrushBasic *brush)
{
//structCommonParams *par = CommonParams;
Tspe_BrushCommonParams *par = CommonParams;
par->Type = brush->CommonParams->Type;
par->Size = brush->CommonParams->Size;
par->Capacity = brush->CommonParams->Capacity;
par->ColorVary = brush->CommonParams->ColorVary;
par->SizeVary = brush->CommonParams->SizeVary;
par->CapVary = brush->CommonParams->CapVary;
par->Step = brush->CommonParams->Step;
// filters
par->ChalkOn = brush->CommonParams->ChalkOn;
par->InvertOn = brush->CommonParams->InvertOn;
par->InvertValue = brush->CommonParams->InvertValue;
par->BlurOn = brush->CommonParams->BlurOn;
par->BlurValue = brush->CommonParams->BlurValue;
par->MedianOn = brush->CommonParams->MedianOn;
par->MedianValue = brush->CommonParams->MedianValue;
// bump
par->BumpOn = brush->CommonParams->BumpOn;
par->BumpSoft = brush->CommonParams->BumpSoft;
par->BumpScaleValue = brush->CommonParams->BumpScaleValue;
par->BumpThresholdValue = brush->CommonParams->BumpThresholdValue;
par->BumpRoughnessValue = brush->CommonParams->BumpRoughnessValue;
par->BumpXoffsetValue = brush->CommonParams->BumpXoffsetValue;
par->BumpYoffsetValue = brush->CommonParams->BumpYoffsetValue;
par->BumpInvertThreshold = brush->CommonParams->BumpInvertThreshold;
// envelope
par->EnvelopeSizeOn = brush->CommonParams->EnvelopeSizeOn;
par->EnvelopeCapacityOn = brush->CommonParams->EnvelopeCapacityOn;
par->EnvelopeSizeRepeat = brush->CommonParams->EnvelopeSizeRepeat;
par->EnvelopeCapacityRepeat  = brush->CommonParams->EnvelopeCapacityRepeat;
par->EnvelopeResolution = brush->CommonParams->EnvelopeResolution;
par->EnvelopeResolutionManual = brush->CommonParams->EnvelopeResolutionManual;
if (this->CommonParams->EnvelopeSizeOn)
   this->SetSizeEnvelope(brush->_sizeEnvelopeString.w_str());
if (this->CommonParams->EnvelopeCapacityOn)
   this->SetCapacityEnvelope(brush->_capacityEnvelopeString.w_str());
// jitter
par->JitterOn = brush->CommonParams->JitterOn;
par->JitterConnectedOn = brush->CommonParams->JitterConnectedOn;
par->JitterRangeValue = brush->CommonParams->JitterRangeValue;
par->JitterLoopValue = brush->CommonParams->JitterLoopValue;
// polygon
par->PolyFillOver = brush->CommonParams->PolyFillOver;
par->PolyExcludeFilters   = brush->CommonParams->PolyExcludeFilters;
par->PolyExcludeBump   = brush->CommonParams->PolyExcludeBump;
par->PolyColorIndex = brush->CommonParams->PolyColorIndex;
par->PolyOverrideCapacityOn = brush->CommonParams->PolyOverrideCapacityOn;
par->PolyCapacityValue = brush->CommonParams->PolyCapacityValue;
par->PolyBlurOn        = brush->CommonParams->PolyBlurOn;
par->PolyBlurValue = brush->CommonParams->PolyBlurValue;
// scratch
par->ScratchType = brush->CommonParams->ScratchType;
par->ScratchThicknessValue = brush->CommonParams->ScratchThicknessValue;
par->ScratchNumberValue = brush->CommonParams->ScratchNumberValue;
//
par->PrimaryColor   = brush->CommonParams->PrimaryColor;
par->SecondaryColor = brush->CommonParams->SecondaryColor;
par->SchemeIndex    = brush->CommonParams->SchemeIndex;
par->PolyColor      = brush->CommonParams->PolyColor;
// colors
TBrushColors *cpar = ColorParams;
// copy colors
cpar->SetPrimaryColor(ptRGB(brush->CommonParams->PrimaryColor));
cpar->SetSecondaryColor(ptRGB(brush->CommonParams->SecondaryColor));
cpar->SetPolyColor(ptRGB(brush->CommonParams->PolyColor));
cpar->SetSchemeIndex(brush->CommonParams->SchemeIndex);
// copy working colors
cpar->WorkPrimary   = brush->ColorParams->WorkPrimary;
cpar->WorkSecondary = brush->ColorParams->WorkSecondary;
// copy palette
for (int k = 0; k < 256; k++)
   cpar->PalEntry[k] = brush->ColorParams->PalEntry[k];
//cpar->SetPolyColor(brush->ColorParams->PolyColor);
cpar->ShareTexture(brush->ColorParams->Texture);
cpar->ShareBackgroundTile(brush->ColorParams->BackgroundTile);
cpar->SharePolyTexture(brush->ColorParams->PolyTexture);
// pal-grad
par->PalGradStepper =  brush->CommonParams->PalGradStepper;
par->PalGradAuto    =  brush->CommonParams->PalGradAuto;
//
// copy imagecontainer and grapshape -> already copied in constructor
//this->_SC = brush->_SC;
//this->_grapShape = brush->_grapShape;
}
//---------------------------------------------------------------------------
// public method
void TBrushBasic::SetDefaultCommonParams(void)
{
//structCommonParams *par = CommonParams;
Tspe_BrushCommonParams *par = CommonParams;
// set deafult values
par->Size = 10;
par->Capacity = 100;
par->ColorVary = 0;
par->SizeVary = 0;
par->CapVary = 0;
par->Step = 2;
// filters
par->ChalkOn = false;
par->InvertOn = false;
par->InvertValue = 16;
par->BlurOn = false;
par->BlurValue = 1;
par->MedianOn = false;
par->MedianValue = 1;
// bump
par->BumpOn = false;
par->BumpSoft = false;
par->BumpScaleValue = 1;
par->BumpThresholdValue = 0;
par->BumpRoughnessValue = 0;
par->BumpXoffsetValue = 1;
par->BumpYoffsetValue = -1;
par->BumpInvertThreshold = false;
// envelope
par->EnvelopeSizeOn = false;
par->EnvelopeCapacityOn = false;
par->EnvelopeSizeRepeat = false;
par->EnvelopeCapacityRepeat = false;
par->EnvelopeResolution = 10;
par->EnvelopeResolutionManual = false;
// jitter
par->JitterOn = false;
par->JitterConnectedOn = true;
par->JitterRangeValue = 1;
par->JitterLoopValue = 1;
// polygon
par->PolyFillOver = false;
par->PolyExcludeFilters   = true;
par->PolyExcludeBump   = false;
par->PolyColorIndex = 0;
par->PolyOverrideCapacityOn = false;
par->PolyCapacityValue = 100;
par->PolyBlurOn        = false;
par->PolyBlurValue = 1;
// scratch
par->ScratchType = 0;
par->ScratchThicknessValue = 1;
par->ScratchNumberValue = 10;
// pal-grad
par->PalGradStepper = 1.0;
par->PalGradAuto    = true;
// colors
par->PrimaryColor    = 0x00000000;
par->SecondaryColor  = 0x00ffffff;
par->PolyColor   	 = 0x0000ff00;
par->SchemeIndex     = spe_csPrimary;
// calculated params
CalculateAdditionalParams(par->Size, par->Capacity);
}
//---------------------------------------------------------------------------
void TBrushBasic::SetSizeEnvelope(wchar_t *str)
{
if (_sizeEnvelope)
   {
   delete _sizeEnvelope;
   _sizeEnvelope = 0;
   }
if (State->cpEnvelopeSizeFactor)
   {
   delete []State->cpEnvelopeSizeFactor;
   State->cpEnvelopeSizeFactor = 0;
   }
_sizeEnvelopeString = UnicodeString(str);
std::vector<int> out;
int count = ParseEnvelope(_sizeEnvelopeString, ",", out);
if (!count)
   return;
_sizeEnvelope = new int[count];
for (int i = 0; i < count; i++)
	_sizeEnvelope[i] = out[i];
State->cpEnvelopeSizeCount = count;
State->cpEnvelopeSizeFactorCount = count + (count - 1) * CommonParams->EnvelopeResolution;
State->cpEnvelopeSizeFactor = new float[State->cpEnvelopeSizeFactorCount];
int range, j = 0;
float factor;
for (int i = 1; i < count; i++)
	{
	State->cpEnvelopeSizeFactor[j] = _sizeEnvelope[i-1]/100.0;
	range = _sizeEnvelope[i] - _sizeEnvelope[i-1];
	if (range == 0)
	   factor = 0.0;
	else
	   factor = (float)range / (CommonParams->EnvelopeResolution + 1);
	for (int k = 0; k < CommonParams->EnvelopeResolution; k++)
		State->cpEnvelopeSizeFactor[++j] = _sizeEnvelope[i-1]/100.0 + factor * (k + 1)/100.0 ;
	j++;
	}
State->cpEnvelopeSizeFactor[j] = _sizeEnvelope[count-1]/100.0;
State->cpEnvelopeSizeFactorPos = 0;
State->cpEnvelopeSizeStepIncr = 1;
State->cpEnvelopeSizeStep = 0;
}
//---------------------------------------------------------------------------
void TBrushBasic::SetCapacityEnvelope(wchar_t *str)
{
if (_capacityEnvelope)
   {
   delete _capacityEnvelope;
   _capacityEnvelope = 0;
   }
if (State->cpEnvelopeCapacityFactor)
   {
   delete []State->cpEnvelopeCapacityFactor;
   State->cpEnvelopeCapacityFactor = 0;
   }
_capacityEnvelopeString = UnicodeString(str);
std::vector<int> out;
int count = ParseEnvelope(_capacityEnvelopeString, ",", out);
if (!count)
   return;
_capacityEnvelope = new int[count];
for (int i = 0; i < count; i++)
	_capacityEnvelope[i] = out[i];
State->cpEnvelopeCapacityCount = count;
State->cpEnvelopeCapacityFactorCount = count + (count - 1) * CommonParams->EnvelopeResolution;
State->cpEnvelopeCapacityFactor = new float[State->cpEnvelopeCapacityFactorCount];
int range, j = 0;
float factor;
for (int i = 1; i < count; i++)
	{
	State->cpEnvelopeCapacityFactor[j] = _capacityEnvelope[i-1]/100.0;
	range = _capacityEnvelope[i] - _capacityEnvelope[i-1];
	if (range == 0)
	   factor = 0.0;
	else
	   factor = (float)range / (CommonParams->EnvelopeResolution + 1);
	for (int k = 0; k < CommonParams->EnvelopeResolution; k++)
		State->cpEnvelopeCapacityFactor[++j] = _capacityEnvelope[i-1]/100.0 + factor * (k + 1)/100.0 ;
	j++;
	}
State->cpEnvelopeCapacityFactor[j] = _capacityEnvelope[count-1]/100.0;
State->cpEnvelopeCapacityFactorPos = 0;
State->cpEnvelopeCapacityStepIncr = 1;
State->cpEnvelopeCapacityStep = 0;
}
//---------------------------------------------------------------------------
void TBrushBasic::CalculateVariations(void)
{
if (CommonParams->SizeVary)
   State->cpSize  = CommonParams->Size - random(State->cpSizeVary);
if (CommonParams->CapVary)
   {
   State->cpCapacity = CommonParams->Capacity + State->cpCapVary - random(State->cpCapVary* 2);
   if (State->cpCapacity > 100)
	  State->cpCapacity = 100;
   State->cpNormCapacity = fast_ftol(((float)State->cpCapacity / 100.0) * spe_SP_NORM);
   }
State->cpHalfFloat     = State->cpSize /2.0;
State->cpHalfFloatSquared = State->cpHalfFloat * State->cpHalfFloat;
}
//---------------------------------------------------------------------------
void TBrushBasic::CalculateSizeEnvelope(void)
{
if (!_sizeEnvelope)
   return;
if (State->cpEnvelopeSizeFactorPos >= State->cpEnvelopeSizeFactorCount)
	State->cpEnvelopeSizeFactorPos = CommonParams->EnvelopeSizeRepeat ? 0 : State->cpEnvelopeSizeFactorCount-1;
int pos = State->cpEnvelopeSizeFactorPos;
State->cpSize =  CommonParams->Size * State->cpEnvelopeSizeFactor[pos];
State->cpHalfFloat     = State->cpSize /2.0;
State->cpHalfFloatSquared = State->cpHalfFloat * State->cpHalfFloat;
State->cpEnvelopeSizeStep += State->cpEnvelopeSizeStepIncr;
while (State->cpEnvelopeSizeStep >= 1.0)
   {
   State->cpEnvelopeSizeFactorPos++;
   State->cpEnvelopeSizeStep = State->cpEnvelopeSizeStep - 1.0;
   }
}
//---------------------------------------------------------------------------
void TBrushBasic::CalculateCapacityEnvelope(void)
{
if (!_capacityEnvelope)
   return;
if (State->cpEnvelopeCapacityFactorPos >= State->cpEnvelopeCapacityFactorCount)
	State->cpEnvelopeCapacityFactorPos = CommonParams->EnvelopeCapacityRepeat ? 0 : State->cpEnvelopeCapacityFactorCount-1;
int pos = State->cpEnvelopeCapacityFactorPos;
State->cpCapacity =  CommonParams->Capacity * State->cpEnvelopeCapacityFactor[pos];
State->cpNormCapacity  = fast_ftol(((float)State->cpCapacity / 100.0) * spe_SP_NORM);
State->cpEnvelopeCapacityStep += State->cpEnvelopeCapacityStepIncr;
while (State->cpEnvelopeCapacityStep >= 1.0)
   {
   State->cpEnvelopeCapacityFactorPos++;
   State->cpEnvelopeCapacityStep = State->cpEnvelopeCapacityStep - 1.0;
   }
}
//---------------------------------------------------------------------------
void TBrushBasic::CalculateAdditionalParams(int size, int capacity)
{
State->cpSize          = size;
State->cpHalfFloat     = size / 2.0;
State->cpCapacity      = CommonParams->Capacity;
State->cpNormCapacity  = fast_ftol(((float)capacity / 100.0) * spe_SP_NORM);
State->cpHalfFloatSquared = State->cpHalfFloat * State->cpHalfFloat;
State->cpCapVary =  fast_ftol(capacity * CommonParams->CapVary / 100.0);
State->cpSizeVary = fast_ftol(size * CommonParams->SizeVary / 100.0);
//State->cpFaderSizeMin = fast_ftol(size * CommonParams->FadeSizePercentage / 100.0);
//if (State->cpFaderSizeMin < 1)
//   State->cpFaderSizeMin = 1;
//State->cpFaderCapacityMin = fast_ftol(size * CommonParams->FadeCapacityPercentage / 100.0);
//State->cpFaderSizeIncrement = (CommonParams->FadeIncrementValue * (size -  State->cpFaderSizeMin)) / 100.0;
//if (CommonParams->FadeSizeIndex == 3 || CommonParams->FadeSizeIndex == 4 || CommonParams->FadeSizeIndex == 6)
//   State->cpFaderSize = size;
//else
//   State->cpFaderSize = State->cpFaderSizeMin;
//State->cpFaderCapacityIncrement = (CommonParams->FadeIncrementValue * (capacity - State->cpFaderCapacityMin)) / 100.0;
//if (CommonParams->FadeCapacityIndex == 3 || CommonParams->FadeCapacityIndex == 4 || CommonParams->FadeCapacityIndex == 6)
//   State->cpFaderCapacity = capacity;
//else
//   State->cpFaderCapacity = State->cpFaderCapacityMin;
// this parameter does not change as it determines bound rectangle only!!!
State->cpHalfSize  = (size + State->cpSizeVary)>>1;
if (State->cpHalfSize > State->cpHalfSizeMax)
    State->cpHalfSizeMax = State->cpHalfSize;
State->cpROItol = 0;
if (CommonParams->BlurOn)
	State->cpROItol = CommonParams->BlurValue * 2 + 1;
State->cpRealStepper = CommonParams->PalGradStepper;
State->cpROItol += CommonParams->JitterRangeValue; // test
// test
}
//---------------------------------------------------------------------------
void TBrushBasic::CalculateAdditionalParams(void)
{
CalculateAdditionalParams(CommonParams->Size, CommonParams->Capacity);
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
UnicodeString TBrushBasic::ClassName(void)
{
return "TBrushBasic";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushBasic::Begin(TGrapBasic *grapShape)
{
_grapShape = grapShape;	// grap shape can be changed
if (_grapShape)
   {
	_grapShape->ResetStepper(CommonParams->Step);
   }
if (_stampWork)
   {
   delete _stampWork;
   _stampWork = 0;
   }
if (_stampOriginal)
   {
   delete _stampOriginal;
   _stampOriginal = 0;
   }
CalculateAdditionalParams(); // mandatory!
// test
State->wpRealStepCount = 0.0;
State->wpStepCount = 0;
State->wpRealStepper = State->cpRealStepper; // initially - CommonParams->PalGradStepper (in ClculateAdditionalParams)
}
//---------------------------------------------------------------------------
void TBrushBasic::endInternal(bool freeHand)
{
//
//CalculateAdditionalParams();
if (!freeHand)
   {
   if (!CommonParams->EnvelopeResolutionManual && LineParams->Count > 0)
	  {
	  if (_sizeEnvelope && CommonParams->EnvelopeSizeOn)
		  State->cpEnvelopeSizeStepIncr =  (float) State->cpEnvelopeSizeFactorCount / LineParams->Count;
	  if (_capacityEnvelope && CommonParams->EnvelopeCapacityOn)
		  State->cpEnvelopeCapacityStepIncr =  (float)State->cpEnvelopeCapacityFactorCount / LineParams->Count;
	  }
   }
if (CommonParams->PalGradAuto  && LineParams->Count > 0)
   {
   State->cpRealStepper = State->cpStepperMax / LineParams->Count;
   if (ColorParams->SchemeIndex == spe_csPaletteStepFB || ColorParams->SchemeIndex ==  spe_csGradientStepFB)
	  State->cpRealStepper *= 2.0;
   }
else
   State->cpRealStepper = CommonParams->PalGradStepper;
}
//---------------------------------------------------------------------------
void TBrushBasic::End(int param)
{
// virtual
State->cpEnvelopeSizeFactorPos = 0;
State->cpEnvelopeCapacityFactorPos = 0;
State->cpEnvelopeSizeStep = 0;
State->cpEnvelopeSizeStepIncr = 1;
State->cpEnvelopeCapacityStep = 0;
State->cpEnvelopeCapacityStepIncr = 1;
State->cpHalfSizeMax = 0;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushBasic::DrawShape(TRect *boundRect)
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
	CopyStampToMask(_stampWork, _surfRight, &_brushTipRect);
	}
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushBasic::DrawTip(TRect *boundRect, int lp)
{
if (!_stampWork)
   return;
DrawVariations();
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
half_size += _brushTipTol;
ResizeStamper(half_size, _invertStamp);
_brushTipRect = GetTipBoundRectClear(LineParams->Point[lp].x, LineParams->Point[lp].y,
									 _beginHalfSize, _surfRight->Width, _surfRight->Height);
CopyStampToMask(_stampWork, _surfRight, &_brushTipRect);
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushBasic::DrawVariations(void)
{
if (CommonParams->CapVary || CommonParams->SizeVary)
   CalculateVariations();
if (CommonParams->EnvelopeSizeOn)
   CalculateSizeEnvelope();
if (CommonParams->EnvelopeCapacityOn)
   CalculateCapacityEnvelope();
}
//---------------------------------------------------------------------------
void TBrushBasic::PreparePoints(bool freeHand)
{
ResetLinePoints();
_grapShape->GetStepPoints(CommonParams->Step);
LineParams->Point = new TPoint[_grapShape->CountBmp];
for (int i = 0; i < _grapShape->CountBmp; i++)
	LineParams->Point[i] = _grapShape->PointBmp[i];
LineParams->Count = _grapShape->CountBmp;
_saveLineParamsCount = LineParams->Count;
endInternal(freeHand);
}
//---------------------------------------------------------------------------
void TBrushBasic::ResetLinePoints(void)
{
//_grapShape->Reset();
if (LineParams->Point)
   delete [] (LineParams->Point);
LineParams->Point = 0;
LineParams->Count = 0;
_saveLineParamsCount = 0;
}
//---------------------------------------------------------------------------
void TBrushBasic::AddLinePoints(int x1, int y1, int x2, int y2)
{
_grapShape->DrawCoords(x1, y1, x2, y2);
}
//---------------------------------------------------------------------------
void TBrushBasic::ApplyFilters(TRect *boundRect)
{
if (CommonParams->MedianOn)
   ApplyMedian(boundRect, CommonParams->MedianValue);
if (CommonParams->BlurOn)
   ApplyBlur(boundRect, CommonParams->BlurValue);
if (CommonParams->ScratchType > 0)
   ApplyScratching(boundRect);
}
//---------------------------------------------------------------------------
void TBrushBasic::ApplyScratching(TRect *boundRect)
{
int pt, x1, y1, x2, y2, points, radius;
//full_size = State->cpSize;
//half_size = fast_ftol(State->cpHalfFloat);
points = fast_ftol((boundRect->Width() *boundRect->Height() * CommonParams->ScratchNumberValue)/10000.0);
radius =  (CommonParams->ScratchThicknessValue)>>1;
//points = points>>1
//pxgp_SelectPxDC(_imageContainer->ImageMaskLeft->DcSurface);
//MGL_setPenSize(CommonParams->ScratchThicknessValue, CommonParams->ScratchThicknessValue);
//MGL_setColorCI(255);
_surfLeft->SetContext(255 , 255, CommonParams->ScratchThicknessValue);
for (pt = 0; pt <  points; pt++)
	{
	if (CommonParams->ScratchType == spe_SCRATCH_CIRCLE) // machine gun
	   {
	   x1 = boundRect->left + random(boundRect->Width());
	   y1 = boundRect->top  + random(boundRect->Height());
	   if (radius > 0)
		  _surfLeft->EllipseFilled(x1, y1, radius, radius);
	   else
		  _surfLeft->BrushCoord(x1, y1);
	   }
	else if (CommonParams->ScratchType == spe_SCRATCH_SQUARE) // square mesh
	   {
	   x1 = boundRect->left + random(boundRect->Width());
	   y1 = boundRect->top  + random(boundRect->Height());
	   if (radius > 0)
		  _surfLeft->RectangleFilled(x1-radius, y1-radius, x1+radius, y1+radius);
	   else
		  _surfLeft->BrushCoord(x1, y1);
       }
	else if (CommonParams->ScratchType == spe_SCRATCH_SHORT_LINE) // short lines mesh
       {
       x1 = boundRect->left + random(boundRect->Width());
       y1 = boundRect->top  + random(boundRect->Height());
       x2 = boundRect->left + random(boundRect->Width());
	   y2 = boundRect->top  + random(boundRect->Height());
	   _surfLeft->Line(x1, y1, x2, y2);
	   }
	else if (CommonParams->ScratchType == spe_SCRATCH_LONG_LINE) // long lines mesh
	   {
	   x1 = boundRect->left;
	   y1 = boundRect->top;
	   x2 = boundRect->left + boundRect->Width();
	   y2 = boundRect->top  + boundRect->Height();
	   x1 += random(boundRect->Width());
	   y1 += random(boundRect->Height());
	   x2 -= random(boundRect->Width());
	   y2 -= random(boundRect->Height());
	   _surfLeft->Line(x1, y1, x2, y2);
	   }
	}
// return context to righ mask
_surfRight->SetContext();
//MGL_setPenSize(1, 1);
//pxgp_SelectPxDC(_imageContainer->ImageMaskRight->DcSurface);
//MGL_setColorCI(255);
}
//---------------------------------------------------------------------------
void TBrushBasic::ApplyBlur(TRect *boundRect, int kernelValue)
{
// test - ok
//TODO: add ippFilter for brushes -  prepare on stamp
//if (boundRect->Width() > 0 && boundRect->Height() > 0 && boundRect->left >= 0 &&  boundRect->top >= 0 )
// works ok
//TODO: select algo in settings
//  spCommon::FBGrayRun(_surfRight, boundRect, kernelValue);
spCommon::BoxBlurGray(_surfRight, boundRect, kernelValue);
}
//---------------------------------------------------------------------------
void TBrushBasic::ApplyMedian(TRect *boundRect, int kernelValue)
{
spCommon::MedianGray(_surfRight, boundRect, kernelValue);
/*
if (_SCRight->Filter)
   _SCRight->Filter(boundRect, kernelValue, flMedian);
pxif_Median(_imageContainer->ImageMaskRight->SpImage, boundRect, kernelValue);
*/
}
//---------------------------------------------------------------------------
void TBrushBasic::FillPoly(void)
{
_surfRight->PolygonFilled(LineParams->Point, LineParams->Count, false);
}
//---------------------------------------------------------------------------
void TBrushBasic::FillPoly(TPoint *polyPoint, int polyCount)
{
_surfRight->PolygonFilled(polyPoint, polyCount, false);
}
//---------------------------------------------------------------------------
void TBrushBasic::FillRectangle(int x1, int y1, int x2, int y2)
{
_surfRight->RectangleFilled(x1, y1, x2, y2);
}
//---------------------------------------------------------------------------
void TBrushBasic::FillEllipse(int x1, int y1, int x2, int y2)
{
TRect b;
b.init(x1, y1, x2, y2);
_surfRight->EllipseFilled(b);
}
//---------------------------------------------------------------------------
void TBrushBasic::JitterLinePoints(void)
{
for (int i = 0; i < LineParams->Count; i++)
    {
    LineParams->Point[i].x = _grapShape->PointBmp[i].x  + CommonParams->JitterRangeValue - random(CommonParams->JitterRangeValue * 2);
    LineParams->Point[i].y = _grapShape->PointBmp[i].y + CommonParams->JitterRangeValue - random(CommonParams->JitterRangeValue * 2);
    }
}
//---------------------------------------------------------------------------
void TBrushBasic::ResizeStamper(int half_size, bool invert)
{
if (half_size == _beginHalfSize)
   {
   if (_resizedOnTheFly)
	  {
	  //pxmf_ResizeStamp(_stampOriginal->SpImage, _stampWork->SpImage, 0, invert);
	  _stampWork->SetRoi();
	  _stampWork->Copy(_stampOriginal);
	  _resizedOnTheFly = false;
	  }
   }
else
   {
   //pxmf_ResizeStamp(_stampOriginal->SpImage, _stampWork->SpImage, half_size, invert);
   int shift = _beginHalfSize - half_size;
   _stampWork->Zero();
   _stampWork->SetRoi(shift, shift, shift + (half_size<<1), shift + (half_size<<1));
   spCommon::DownsampleGray(_stampOriginal, _stampWork);
   _resizedOnTheFly= true;
   }
}
//---------------------------------------------------------------------------
void TBrushBasic::RestoreLinePoints(void)
{
LineParams->Count = _saveLineParamsCount;
}
//---------------------------------------------------------------------------
TRect TBrushBasic::GetTipBoundRectClear(int X, int Y, int half_size, int width, int height)
{
TRect boundRect;
boundRect.top      = Y - half_size;
boundRect.bottom   = Y + half_size;
boundRect.left     = X - half_size;
boundRect.right	   = X + half_size;
if (boundRect.top < 0)
	boundRect.top = 0;
if (boundRect.bottom < 0)
	boundRect.bottom = 0;
if (boundRect.left < 0)
	boundRect.left = 0;
if (boundRect.right < 0)
	boundRect.right = 0;
if (boundRect.top > height)
	boundRect.top = height;
if (boundRect.bottom > height)
	boundRect.bottom = height;
if (boundRect.left > width)
	boundRect.left = width;
if (boundRect.right > width)
	boundRect.right = width;
// check if need to work
//if (boundRect.Width() == 0 || boundRect.Height() == 0)
return boundRect;
}
//---------------------------------------------------------------------------
void TBrushBasic::CopyStampToMask(spImage<ptGray> *stamp, spImage<ptGray> *destMask, TRect *boundRect)
{
if (boundRect->Width() == 0 || boundRect->Height() == 0)
   return;
boundRect->NormalizeRect();
int i, j, stI, stJ, stamp_top = 0, stamp_left = 0, stamp_width = stamp->Width(), stamp_height = stamp->Height();
//Byte stamp_bgr, mask_bgr;
// left and right
if (boundRect->Width() < stamp_width)
   {
   if (boundRect->left > 0)
	  {
	  stamp_left = 0;
	  }
   else
	  stamp_left = stamp_width - boundRect->Width();
   }
// up and down
if (boundRect->Height() < stamp_height)
   {
   if (boundRect->top > 0)
	  stamp_top = 0; //boundRect->top;
   else
	  stamp_top = /*boundRect->top -*/ (stamp_height - boundRect->Height());
   }
try
  {
  for (i = boundRect->top, stI = stamp_top; i < boundRect->bottom; i++, stI++)
	  {
	  for (j = boundRect->left, stJ = stamp_left; j < boundRect->right; j++, stJ++)
		  {
		  if (stamp->Pixel(stI, stJ) > destMask->Pixel(i, j))
			  destMask->Pixel(i, j)  = stamp->Pixel(stI, stJ);
		  }
	  }
  }
catch (...)
  {
  // ouch!
  }
}
//---------------------------------------------------------------------------
int TBrushBasic::ParseEnvelope(UnicodeString input, UnicodeString delim, std::vector<int> &output)
{
int position, counter = 0;
int item;
output.clear();
input = input.Trim();
while ((position = input.Pos(delim)) > 0 )
   {
   try {
	   item = StrToInt(input.SubString(1, position -1));
	   }
   catch (...)
	   {
	   // bad data
	   return 0;
	   }
   output.push_back(item);
   input = input.SubString(position + 1, input.Length());
   counter++;
   }
if (!input.IsEmpty())   // ostalo retaja
   {
   try {
	   item = StrToInt(input);
	   }
   catch (...)
	   {
	   // bad data
	   return 0;
	   }
   output.push_back(item);
   counter++;
   }
return counter;
}
//---------------------------------------------------------------------------






