//---------------------------------------------------------------------------
#pragma hdrstop
#include "brushStamp.h"
#include "spCommon.h"
#include "spGeometry.hpp"
#include "ASMVars.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TBrushStamp::TBrushStamp(void):
			 TBrushBasic()
{
CommonParams->Type =  spe_brshStamp;
SpecificParams = new structSpecificParams;
SpecificParams->StampId = 0;
SpecificParams->AngleMode = 0;
SpecificParams->AngleValue = 20;
FStampImage = 0;
FStampResized = 0;
FAngleCumul = 0.0;
FStampOwner = true;
_invertStamp = true;
}
//---------------------------------------------------------------------------
// destructor
TBrushStamp::~TBrushStamp(void)
{
DeleteStampImage();
delete SpecificParams;
}
//---------------------------------------------------------------------------
UnicodeString TBrushStamp::ClassName(void)
{
return "TBrushStamp";
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
TBrushBasic* TBrushStamp::Clone(void)
{
TBrushStamp *clonedBrush = new TBrushStamp();
clonedBrush->Connect(_surfLeft, _surfRight, _grapShape);
clonedBrush->CopyParams(this);
// copy specific params
structSpecificParams *par = SpecificParams;
clonedBrush->SpecificParams->AngleMode = par->AngleMode;
clonedBrush->SpecificParams->AngleValue = par->AngleValue;
clonedBrush->FStampImage = this->FStampImage;
clonedBrush->FStampOwner = false;
return clonedBrush;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
bool TBrushStamp::SetStampImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared)
{
if (DeleteStampImage())
   {
   FStampImage = new spImage<ptGray>(buffer, width, height, scanlineAlignement, shared, orInvert);
   SpecificParams->StampId = (unsigned long)FStampImage;
   return (bool)(FStampImage);
   }
return false;
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
bool TBrushStamp::DeleteStampImage(void)
{
if (FStampImage && FStampOwner)
   {
   delete FStampImage;
   FStampImage = 0;
   SpecificParams->StampId = 0;
   }
if (FStampResized)
   {
   delete FStampResized;
   FStampResized = 0;
   }
return (bool)(!FStampImage);
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TBrushStamp::Begin(TGrapBasic *grapShape)
{
TBrushBasic::Begin(grapShape);
if (!FStampImage)
   return;
FAngleCumul = 0.0; // reset cumulative angle
switch (SpecificParams->AngleMode)
   {
   case 1:
		rotateStamp = rotateJitter;
		break;
   case 2:
		rotateStamp = rotateCumulPlus;
		break;
   case 3:
		rotateStamp = rotateCumulMinus;
		break;
   case 4:
		rotateStamp = rotateStroke;
		break;
   default:
		rotateStamp = rotateNone;
		break;
   }
int half_size = fast_ftol(State->cpHalfFloat);
if (half_size <= 0)
	half_size = 1;
_beginHalfSize = half_size;
if (FStampResized)
   {
   delete FStampResized;
   FStampResized = 0;
   }
FStrokeStart = true;
// stamp image -> stamp original (initial resize)
int full_size = half_size<<1;
_stampOriginal = new spSurface<ptGray>(full_size, full_size);
if (FStampImage->Width() != FStampImage->Height())
   spCommon::ResizeStampMar(FStampImage, _stampOriginal, full_size);
else
   spCommon::ResizeBilinearGray(FStampImage, _stampOriginal);
FStampResized = new spImage<ptGray>(_stampOriginal);
_stampWork = new spSurface<ptGray>(_stampOriginal);
_resizedOnTheFly = false;
}
//---------------------------------------------------------------------------
// public implentation
void TBrushStamp::DrawShape(TRect *boundRect)
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
	FLp = lp;
	if (rotateStamp())
	   {
	   _brushTipRect = GetTipBoundRectClear(LineParams->Point[lp].x, LineParams->Point[lp].y,
										_beginHalfSize, _surfRight->Width, _surfRight->Height);
	   CopyStampToMask(_stampWork, _surfRight, &_brushTipRect);
	   }
	}
}
//---------------------------------------------------------------------------
// public implentation
void TBrushStamp::DrawTip(TRect *boundRect, int lp)
{
if (!_stampWork)
   return;
FLp = lp;
if (rotateStamp())
   TBrushBasic::DrawTip(boundRect, lp);
}
//---------------------------------------------------------------------------
bool TBrushStamp::rotateNone(void)
{
return true;
}
//---------------------------------------------------------------------------
bool TBrushStamp::rotateJitter(void)
{
if (SpecificParams->AngleValue == 0)
   return true;
if (FStrokeStart)
   {
   FStrokeStart = false;
   return true;
   }
int jta = random(SpecificParams->AngleValue * 2) - SpecificParams->AngleValue;
if (jta == 0)
   return true;
double radians = (double)jta *  M_PI/180.0;
spCommon::RotateBilinearGray(FStampResized, _stampOriginal, radians);
_stampWork->Copy(_stampOriginal);
return true;
}
//---------------------------------------------------------------------------
bool TBrushStamp::rotateCumulPlus(void)
{
if (SpecificParams->AngleValue == 0)
   return true;
if (FStrokeStart)
   {
   FStrokeStart = false;
   return true;
   }
FAngleCumul += (double)SpecificParams->AngleValue * M_PI/180.0;
spCommon::RotateBilinearGray(FStampResized, _stampOriginal, FAngleCumul);
_stampWork->Copy(_stampOriginal);
return true;
}
//---------------------------------------------------------------------------
bool TBrushStamp::rotateCumulMinus(void)
{
if (SpecificParams->AngleValue == 0)
   return true;
if (FStrokeStart)
   {
   FStrokeStart = false;
   return true;
   }
FAngleCumul -= (double)SpecificParams->AngleValue * M_PI/180.0;
spCommon::RotateBilinearGray(FStampResized, _stampOriginal, FAngleCumul);
_stampWork->Copy(_stampOriginal);
return true;
}
//---------------------------------------------------------------------------
bool TBrushStamp::rotateStroke(void)
{
// calculate stroke angle
if (FStrokeStart)
   {
   FStrokePt.x = LineParams->Point[0].x;
   FStrokePt.y = LineParams->Point[0].y;
   FLpStart = 0;
   FAngleCumul = 0;
   FStrokeStart = false;
   if (_grapShape->ClassName() != "TGrapHand")
	  return false; // do not draw first one
   return true; // no rotation - wait for next point
   }
double radius;
spGeometry::GetLineData(&radius, &FAngleCumul, FStrokePt.x, FStrokePt.y, LineParams->Point[FLp].x, LineParams->Point[FLp].y);
if (radius > 10.0)   // rule of thumb - at least 10 pixels
   {
   // do rotation
   if (fabs(FAngleCumul) > 0.005)
	  {
	   // rotate and draw first one
	   if (_grapShape->ClassName() != "TGrapHand" && FLp == 1)
		  {
		  if (_grapShape->ClassName() == "TGrapEllipse") // reduce angle by 20%
			  spCommon::RotateBilinearGray(FStampResized, _stampOriginal, FAngleCumul - FAngleCumul *0.2);
		  else
			  spCommon::RotateBilinearGray(FStampResized, _stampOriginal, FAngleCumul);
		  _stampWork->Copy(_stampOriginal);
		  _brushTipRect = GetTipBoundRectClear(LineParams->Point[0].x, LineParams->Point[0].y,
											_beginHalfSize, _surfRight->Width, _surfRight->Height);
		  CopyStampToMask(_stampWork, _surfRight, &_brushTipRect);
		  }
	  spCommon::RotateBilinearGray(FStampResized, _stampOriginal, FAngleCumul);
	  _stampWork->Copy(_stampOriginal);
	  FLpStart++;
	  if (FLpStart >= LineParams->Count)
		 FLpStart = LineParams->Count - 1;
	  FStrokePt.x = LineParams->Point[FLpStart].x;
	  FStrokePt.y = LineParams->Point[FLpStart].y;
	  }
   }
return true;
}
//---------------------------------------------------------------------------

