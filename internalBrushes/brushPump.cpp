//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushPump.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushPump::TBrushPump(void):
			 TBrushBasic()
{
//TODO: CommonParams->Type =  spe_brshPump;
SpecificParams = new structSpecificParams;
SpecificParams->Index = 0;
SpecificParams->Count = 0;
SpecificParams->ResetIndex = false;
SpecificParams->Mode = 0;
_thrPump = 0;
FPumpOwner = true;
FPumpStarted = false;
FIndex = 0;
FSelIndex = 0;
}
//---------------------------------------------------------------------------
// destructor
TBrushPump::~TBrushPump(void)
{
if (_thrPump)
   {
   delete _thrPump;
   _thrPump = 0;
   }
DeletePump();
delete SpecificParams;
}
//---------------------------------------------------------------------------
UnicodeString TBrushPump::ClassName(void)
{
return "TBrushPump";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
TBrushBasic* TBrushPump::Clone(void)
{
TBrushPump *clonedBrush = new TBrushPump();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->FPumpOriginal = this->FPumpOriginal;
clonedBrush->FPumpResized  = this->FPumpResized;
clonedBrush->FPumpReady	   = this->FPumpReady;
clonedBrush->FPumpSelected = this->FPumpSelected;
clonedBrush->FPumpOwner = false;
clonedBrush->SpecificParams->Mode = par->Mode;
clonedBrush->SpecificParams->Index = par->Index;
clonedBrush->SpecificParams->ResetIndex = par->ResetIndex;
clonedBrush->SpecificParams->Count    = par->Count;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
bool TBrushPump::AddPumpImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
spImage<ptGray> *img = new spImage<ptGray>(buffer, width, height, scanlineAlignement, false, orInvert);
FPumpOriginal.push_back(img);
FPumpResized.push_back(0);
FPumpReady.push_back(false);
SpecificParams->Count++;
//TODO: OutSpecific->PumpId = (unsigned long)buffer;
return (bool)(img);
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
bool TBrushPump::SelectPumpImage(int idx)
{
if (FPumpOriginal.empty() || idx >= FPumpOriginal.size() || idx < 0)
   return false;
FPumpSelected.push_back(idx);
FIndex = idx;
return true;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
bool TBrushPump::DeselectPump(void)
{
FPumpSelected.clear();
return true;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
bool TBrushPump::DeletePump(void)
{
if (!FPumpOriginal.empty() && FPumpOwner)
	{
	for (size_t i = 0; i < FPumpOriginal.size(); i++)
		{
		delete FPumpOriginal[i];
		FPumpOriginal[i] = 0;
		if (FPumpResized[i])
		   {
			delete FPumpResized[i];
			FPumpResized[i] = 0;
		   }
		}
	FPumpOriginal.clear();
	FPumpResized.clear();
	FPumpReady.clear();
    FPumpSelected.clear();
    SpecificParams->Count = 0;
	return true;
	}
return false;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushPump::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
if (FPumpOriginal.empty())
   return;
if (!FPumpStarted)
   {
   FIndex = SpecificParams->Index;
   FPumpStarted = true;
   }
else if (SpecificParams->ResetIndex)
   FIndex = SpecificParams->Index;
if (FIndex < 0 || FIndex >= (int)FPumpOriginal.size())
   FIndex = 0;
// there must be at least one selected
if (FPumpSelected.empty())
	FPumpSelected.push_back(FIndex);
FSelIndex = getSelIndex(FIndex);
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
_beginHalfSize = half_size;
// stamp image -> stamp original (initial resize)
FBrushSize = (half_size<<1);
_stampOriginal = new spSurface<ptGray>(FBrushSize, FBrushSize);
if (resizePump())
   _stampOriginal->Copy(FPumpResized[FIndex]);
_stampWork = new spSurface<ptGray>(_stampOriginal);
_resizedOnTheFly = false;
}
//---------------------------------------------------------------------------
// public implentation
void TBrushPump::DrawShape(TRect *boundRect)
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
	changeStamp();
	if (half_size == _beginHalfSize)
	   _stampWork->Copy(_stampOriginal);
	else
	   ResizeStamper(half_size, _invertStamp);
	}
//TBrushBasic::DrawShape(boundRect);
}
//---------------------------------------------------------------------------
// public implentation
void TBrushPump::DrawTip(TRect *boundRect, int lp)
{
if (!_stampWork)
   return;
TBrushBasic::DrawTip(boundRect, lp);
changeStamp();
}
//---------------------------------------------------------------------------
// private
void TBrushPump::changeStamp(void)
{
switch (SpecificParams->Mode)
	{
	case 0: // sequential
		FIndex++;   // up!
		if (FIndex >= (int)FPumpOriginal.size())
		   FIndex = 0;
		break;
	case 1: // random
		FIndex = random((int)FPumpOriginal.size());
		break;
	case 2: // selected sequential
		FSelIndex++; // up
		if (FSelIndex >= (int)FPumpSelected.size())
		   FSelIndex = 0;
		FIndex = FPumpSelected[FSelIndex];
		break;
	case 3: // selected random
		FSelIndex = random((int)FPumpSelected.size());
		FIndex = FPumpSelected[FSelIndex];
		break;
	default:
		break;
	}
// copy to stamp original
if (checkPump())
   _stampOriginal->Copy(FPumpResized[FIndex]);
}
//---------------------------------------------------------------------------
// private
bool TBrushPump::resizePump(void)
{
for (size_t i = 0; i < FPumpReady.size(); i++)
	FPumpReady[i] = false;
// start thread
if (_thrPump)
   {
   delete _thrPump;
   _thrPump = 0;
   }
_thrPump = new TPumpThread(false, &FPumpOriginal, &FPumpResized, &FPumpReady, FBrushSize, FIndex);
return checkPump();
}
//---------------------------------------------------------------------------
bool TBrushPump::checkPump(void)
{
for (int i = 0; i < 100; i++) // retry 100 - should be enough
   {
   if (FPumpReady[FIndex])
	  return true;
   Sleep(10);
   }
return false;
}
//---------------------------------------------------------------------------
int TBrushPump::getSelIndex(int idx)
{
for (size_t i = 0; i < FPumpSelected.size(); i++)
   {
   if (FPumpSelected[i] ==  idx)
	  return (int)i;
   }
return 0;
}
//---------------------------------------------------------------------------

