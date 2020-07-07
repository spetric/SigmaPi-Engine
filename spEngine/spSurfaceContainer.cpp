//---------------------------------------------------------------------------
#pragma hdrstop
#include "spSurfaceContainer.h"
// initialize spDraw static vars
void *spDraw::PenColor = 0;
unsigned int spDraw::PenSize = 1;
speDotType spDraw::PenType = dtCircle;
void *spDraw::BrushColor = 0;
void *spDraw::Surface = 0;
unsigned int spDraw::Height = 0;
unsigned int spDraw::Width = 0;
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor - empty container
//---------------------------------------------------------------------------
spSurfaceContainer::spSurfaceContainer(void)
{
Orig = 0;
Edit = 0;
MaskLeft  = 0;
MaskRight = 0;
AlphaOrig = 0;
AlphaEdit = 0;
Selection = 0;
AssContour = 0;
AssMask = 0;
AssStencil = 0;
AssMarker = 0;
AssSidekick = 0;
FWidth = 0;
FHeight = 0;
FWidthM1 = 0;
FHeightM1 = 0;
FScanlineAlignement = 0;
FGrapShape = 0;
FBrush = 0;
FScanOrig = 0;
FScanAlphaOrig = 0;
FScanLineIndex = 0;
FSuperContainer = 0;
CloneSelf = 0;
SurfaceId = 0;
StencilAlive = false;
}
//---------------------------------------------------------------------------
// constructor
//---------------------------------------------------------------------------
spSurfaceContainer::spSurfaceContainer(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignment,
									   TBrushBasic *brush, TGrapBasic *shape)
{
Orig = 0;
Edit = 0;
MaskLeft  = 0;
MaskRight = 0;
AlphaOrig = 0;
AlphaEdit = 0;
Selection = 0;
AssContour = 0;
AssMask = 0;
AssStencil = 0;
AssMarker = 0;
AssSidekick = 0;
FWidth = width;
FHeight = height;
FWidthM1 = width - 1;
FHeightM1 = height - 1;
FScanlineAlignement = scanlineAlignment;
FSuperContainer = 0;
CloneSelf = 0;
StencilAlive = false;
// ipp
ippOrig = 0;
ippEdit = 0;
ippAlphaOrig = 0;
ippAlphaEdit = 0;
ippMaskRight = 0;
ippMaskLeft = 0;
// color correction data
CumulBGR[0] = CumulBGR[1] = CumulBGR[2] = 0;
DistanceBGR[0] = DistanceBGR[1] = DistanceBGR[2] = 0;
//FGrapShape = 0;
//FBrush = 0;
Orig = new spSurface<ptRGB>(scanOrig, FWidth, FHeight, FScanlineAlignement);  // shared
Edit = new spSurface<ptRGB>(Orig, true);	// new non-shared image
// default mix surf and arith surf
MixSurf = Edit;
ArithSurf = Edit;
if (scanAlphaOrig)
   {
   AlphaOrig = new spSurface<ptGray>(scanAlphaOrig, FWidth, FHeight, FScanlineAlignement); // shared
   AlphaEdit = new spSurface<ptGray>(AlphaOrig, true); // new non-shared image
   }
MaskLeft  = new spSurface<ptGray>(FWidth, FHeight, FScanlineAlignement);  // new non-shared image
MaskLeft->Fill(ptGray(0));
MaskRight  = new spSurface<ptGray>(MaskLeft, true); // new non-shared image
MaskLeft->Fill(ptGray(0));
SetBrush(brush);
SetShape(shape);
if (spCommon::ippAlive)
   {
   ippOrig = new ippImage<ippRGB>((void*)Orig->GetOriginalBuffer(), width, height, scanlineAlignment);
   ippEdit = new ippImage<ippRGB>((void*)Edit->GetOriginalBuffer(), width, height, scanlineAlignment);
   if (scanAlphaOrig)
	  {
	  ippAlphaOrig = new ippImage<ippGray>((void*)AlphaOrig->GetOriginalBuffer(), width, height, scanlineAlignment);
	  ippAlphaEdit = new ippImage<ippGray>((void*)AlphaEdit->GetOriginalBuffer(), width, height, scanlineAlignment);
      }
   ippMaskRight = new ippImage<ippGray>((void*)MaskRight->GetOriginalBuffer(), width, height, scanlineAlignment);
   ippMaskLeft  = new ippImage<ippGray>((void*)MaskLeft->GetOriginalBuffer(), width, height, scanlineAlignment);
   }
}
//---------------------------------------------------------------------------
// destructor
spSurfaceContainer::~spSurfaceContainer(void)
{
//
if (Orig)
	delete Orig;
if (Edit)
	delete Edit;
if (MaskLeft)
   delete MaskLeft;
if (MaskRight)
   delete MaskRight;
if (AlphaOrig)
	delete AlphaOrig;
if (AlphaEdit)
	delete AlphaEdit;
if (Selection)
	delete Selection;
if (AssSidekick)
	delete AssSidekick;
if (AssMask)
	delete AssMask;
if (AssContour)
	delete AssContour;
if (AssStencil)
	delete AssStencil;
if (AssMarker)
   delete AssMarker;
if (ippOrig)
   delete ippOrig;
if (ippEdit)
   delete ippEdit;
if (ippMaskRight)
   delete ippMaskRight;
if (ippMaskLeft)
   delete ippMaskLeft;
CloneSelf = 0;
StencilAlive = false;
// not any more delete PixData;
}
//---------------------------------------------------------------------------
// public setters
//---------------------------------------------------------------------------
// non-conotiguous container - start
bool spSurfaceContainer::Open(int width, int height)
  {
  if (Orig)
	 return false;
  FWidth = width;
  FHeight = height;
  FWidthM1 = width - 1;
  FHeightM1 = height - 1;
  FScanlineAlignement = 0;
  if (FScanOrig)
	 delete []FScanOrig;
  if (FScanAlphaOrig)
	 delete []FScanAlphaOrig;
  FScanOrig = new void*[height];
  FScanAlphaOrig = new void*[height];
  FScanLineIndex = 0;
  return true;
}
//---------------------------------------------------------------------------
// non-conotiguous container - adding scanlines
bool spSurfaceContainer::AddScanLine(void *scanOrig, void *scanAlphaOrig)
  {
  if (Orig)
	 return false;
  FScanOrig[FScanLineIndex] = scanOrig;
  if (scanAlphaOrig)
	  FScanAlphaOrig[FScanLineIndex] = scanAlphaOrig;
  FScanLineIndex++;
  return true;
  }
//---------------------------------------------------------------------------
// non-conotiguous container - finish
bool spSurfaceContainer::Close(void)
  {
  if (Orig)
	 return false;
  if (FScanLineIndex == 0)
	 return false;
  Orig = new spSurface<ptRGB>(FScanOrig, FWidth, FHeight, true);  // shared fragmented
  Edit = new spSurface<ptRGB>(Orig);	// new non-shared image
  if (FScanAlphaOrig[0])
	 {
	 AlphaOrig = new spSurface<ptGray>(FScanAlphaOrig, FWidth, FHeight, true); // shared fragmented
	 AlphaEdit = new spSurface<ptGray>(AlphaOrig); // new non-shared image
	 }
  MaskLeft  = new spSurface<ptGray>(FWidth, FHeight);  // new non-shared image
  MaskLeft->Fill(ptGray(0));
  MaskRight  = new spSurface<ptGray>(MaskLeft); // new non-shared image
  MaskLeft->Fill(ptGray(0));
  // done, delete scanline array
  if (FScanOrig)
	 delete []FScanOrig;
  if (FScanAlphaOrig)
	 delete []FScanAlphaOrig;
  FScanLineIndex = 0;
  return true;
  }
//---------------------------------------------------------------------------
void spSurfaceContainer::SetImages(void *scanOrig, void *scanAlphaOrig, void *scanEdit, void *scanAlphaEdit)
  {
  Orig = new spSurface<ptRGB>(scanOrig, FWidth, FHeight, FScanlineAlignement);    //shared
  Edit = new spSurface<ptRGB>(scanEdit, FWidth, FHeight, FScanlineAlignement);    //shared
  if (scanAlphaOrig)
	  AlphaOrig = new spSurface<ptGray>(scanAlphaOrig, FWidth, FHeight, FScanlineAlignement); // shared
  if (scanAlphaEdit)
	  AlphaEdit = new spSurface<ptGray>(scanAlphaEdit, FWidth, FHeight, FScanlineAlignement); // shared
  }
//---------------------------------------------------------------------------
void spSurfaceContainer::SetMasks(void *scanLeft, void *scanRight)
  {
  MaskLeft  = new spSurface<ptGray>(scanLeft, FWidth, FHeight, FScanlineAlignement);  // shared
  MaskLeft->Fill(ptGray(0));
  MaskRight  = new spSurface<ptGray>(scanRight, FWidth, FHeight, FScanlineAlignement); // shared
  MaskRight->Fill(ptGray(0));
  }
//---------------------------------------------------------------------------
void spSurfaceContainer::SetSelection(void *scanSelection)
  {
  Selection = new spImage<ptGray>(scanSelection, FWidth, FHeight, FScanlineAlignement); // shared
  }
//---------------------------------------------------------------------------
TBrushBasic* spSurfaceContainer::SetBrush(TBrushBasic *brush)
  {
  TBrushBasic *curr_brush = FBrush;
  FBrush = brush;
  FBrush->Connect(MaskLeft, MaskRight, FGrapShape);
  return curr_brush;
  }
//---------------------------------------------------------------------------
TGrapBasic* spSurfaceContainer::SetShape(TGrapBasic *shape)
  {
  TGrapBasic *curr_shape = FGrapShape;
  FGrapShape = shape;
  return curr_shape;
  }
//---------------------------------------------------------------------------
void spSurfaceContainer::ClearMask(TMaskSelection mSelection, TRect *boundRect)
{
bool bound = false;
TRect lRoi = MaskLeft->GetRoi();
TRect rRoi = MaskRight->GetRoi();
if (boundRect)
   {
   if (boundRect->Width() == 0 || boundRect->Height() == 0)
	  return;
   bound = true;
   MaskLeft->SetRoi(boundRect);
   MaskRight->SetRoi(boundRect);
   }
switch (mSelection)
   {
   case mLeft:
	   bound ? MaskLeft->ZeroRoi(): MaskLeft->Zero();
	   break;
   case mRight:
	   bound ? MaskRight->ZeroRoi(): MaskRight->Zero();
	   break;
   case mBoth:
	   bound ? MaskLeft->ZeroRoi(): MaskLeft->Zero();
	   bound ? MaskRight->ZeroRoi(): MaskRight->Zero();
	   break;
   }
MaskLeft->SetRoi(&lRoi);
MaskRight->SetRoi(&rRoi);
}
//---------------------------------------------------------------------------
void spSurfaceContainer::SetMixSurf(int mode)
{
MixSurf = (mode == 0) ? Edit : Orig;
}
//---------------------------------------------------------------------------
void spSurfaceContainer::SetArithSurf(int mode)
{
ArithSurf = (mode == 0) ? Edit : Orig;
}
//---------------------------------------------------------------------------
void spSurfaceContainer::UpdateImage(TImageSelection imgSelection, TRect *boundRect)
{
bool bound = false;
TRect oRoi = Orig->GetRoi();
TRect eRoi = Edit->GetRoi();
if (boundRect)
   {
   if (boundRect->Width() == 0 || boundRect->Height() == 0)
	  return;
   bound = true;
   Orig->SetRoi(boundRect);
   Edit->SetRoi(boundRect);
   if (AlphaOrig)
	  {
	  AlphaOrig->SetRoi(boundRect);
	  AlphaEdit->SetRoi(boundRect);
	  }
   }
if (imgSelection == imgEdit)
   {
   bound ? Edit->CopyRoi(Orig) : Edit->Copy(Orig);
   if (AlphaOrig)
	  bound? AlphaEdit->CopyRoi(AlphaOrig) : AlphaEdit->Copy(AlphaOrig);
   }
else
   {
   bound ? Orig->CopyRoi(Edit) : Orig->Copy(Edit);
   if (AlphaOrig)
	  bound ? AlphaOrig->CopyRoi(AlphaEdit) : AlphaOrig->Copy(AlphaEdit);
   }
Orig->SetRoi(&oRoi);
Edit->SetRoi(&eRoi);
if (AlphaOrig)
   {
   AlphaOrig->SetRoi(&oRoi);
   AlphaEdit->SetRoi(&eRoi);
   }
}
//---------------------------------------------------------------------------
// private setters
//---------------------------------------------------------------------------
void spSurfaceContainer::setSuperContainer(void *val)
  {
  FSuperContainer = val;
  if (Orig)
	 Orig->SuperContainer = FSuperContainer;
  if (Edit)
	 Edit->SuperContainer = FSuperContainer;
  if (MaskLeft)
	 MaskLeft->SuperContainer = FSuperContainer;
  if (MaskRight)
	 MaskRight->SuperContainer = FSuperContainer;
  if (AlphaOrig)
	 AlphaOrig->SuperContainer = FSuperContainer;
  if (AlphaEdit)
	 AlphaEdit->SuperContainer = FSuperContainer;
  }
//---------------------------------------------------------------------------


