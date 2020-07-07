//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderBasic.h"
#include "ASMVars.h"
#include "spPixelMixer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderBasic::TRenderBasic(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
{
RenderParams = renderData->RenderParams;
//SelectionParams = renderData->SelectionParams;
ShadowParams     = renderData->ShadowParams;
GlobalColorParams = renderData->GlobalColorParams;
_SC = surfaceContainer;
GoRenderStroke = 0;
GoFilterIppRenderCoords = 0;
OnShapeStep = 0;
_spareImage = 0;
_autoRenderSuggest =  spe_RENDER_FAST;
RenderParams->DrawPolyInterior = false;
_filterIpp = 0;
CreateUndo = true;
CurrentBrush = _SC->Brush;
_polyInterImage = 0;
}
//---------------------------------------------------------------------------
// destructor
TRenderBasic::~TRenderBasic(void)
{
//delete PixData;
if (_spareImage)
   delete _spareImage;
if (_polyInterImage)
   delete _polyInterImage;
//TODO: if needed, move this to new class (renderIpp)
// because of swap filters (compound)
if (_filterIpp)
   delete _filterIpp;
}
//---------------------------------------------------------------------------
//TODO: return to brushbasic and grapbasic
UnicodeString TRenderBasic::ClassName(void)
{
return "TRenderBasic";
}
//---------------------------------------------------------------------------
// private
bool TRenderBasic::contourEncountered(const int &X, const int &Y)
{
TRect boundRect = GetTipBoundRectClear(X, Y, RenderParams->ContourClippingRadius);
// scan contour against given radius (window)
int i, j;
Byte ccb = (Byte)RenderParams->ContourClippingThreshold;  
// konturica
for (i = boundRect.top; i < boundRect.bottom; i++)
	{
	for (j = boundRect.left; j < boundRect.right; j++)
		{
		if (_SC->AssContour->Pixel(i, j).ch[0] > ccb)
		   return true;
		}
	}
return false;
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::Begin(int X, int Y)
{
CreateUndo = true;
//_SC->i_down = Y;
//_SC->j_down = X;
_SC->SetMixSurf(RenderParams->MixSurf);
_SC->SetArithSurf(RenderParams->ArithSurf);
//
_SC->MaskRight->SetContext(); // draw on mask
//pxgp_SelectPxDC(ImageContainer->ImageMaskRight->DcSurface); // draw on mask
_SC->MaskRight->PenColor = ptGray(255);
_SC->MaskRight->BrushColor = ptGray(255);
//pxgp_SetColorPxDC(255);
// hm, hm, hm
//PixData->X = X;
//PixData->Y = Y;
//PixData->SurfContainer = Surf; <- already done
_SC->update_alpha = true;
//_SC->SelClipping = SelectionParams->SelClipping;
_contourClipping = (RenderParams->ContourClipping && _SC->AssContour);
_SC->ContourEncountered = false;
//Brush->Begin(Surf->MaskLeft, Surf->MaskRight, grapShape);  <- NO, we can prepare it in idle time!!!!
//PAZI: !!!!!!!
//TODO:add option _overrideRenderMode, so if it's forced let him do it!!!!
switch (RenderParams->RenderingMode)
   {
   case spe_RENDER_AUTOMATIC:
		if (_SC->Shape)
		   {
		   //if (_SC->Shape->ClassName() != "TGrapHand" && (_SC->Brush->CommonParams->EnvelopeSizeOn || _SC->Brush->CommonParams->EnvelopeCapacityOn))
		   if (_SC->Brush->CommonParams->EnvelopeSizeOn || _SC->Brush->CommonParams->EnvelopeCapacityOn)
			  _autoRenderSuggest = spe_RENDER_SLOW;
		   }
		switch (_autoRenderSuggest)
			{
			case spe_RENDER_SLOW:
				 GoRenderStroke = &renderBrushTip;
				 break;
			case spe_RENDER_MEDIUM:
				 GoRenderStroke = &renderShape;
				 break;
			default:
				 GoRenderStroke = &renderShapeFast;
				 break;
			}
		break;
   case spe_RENDER_SLOW:
		GoRenderStroke = &renderBrushTip;
		break;
   case spe_RENDER_MEDIUM:
		GoRenderStroke = &renderShape;
        break;
   case spe_RENDER_FAST:
		GoRenderStroke = &renderShapeFast;
        break;
   }
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::Execute(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
GoRenderStroke(X1, Y1, X2, Y2);
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
TRect TRenderBasic::End(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
_boundRect = GetBoundRect(X1, Y1, X2, Y2);
if (ShadowParams->MainMode > 0 && RenderParams->DoShadow)
   {
   DropShadow(&_boundRect);
   }
// outer world
if (RenderParams->AssistantMaskUpdate && _SC->AssMask)
   {
   if (RenderParams->AssistantMaskLastShapeOnly)
	  _SC->AssMask->Zero();
   spCommon::UpdateMaskHigh(_SC->MaskRight, _SC->AssMask, &_boundRect);
   }
if (RenderParams->PostMaskingMode > 0)
   {
   if (RenderParams->PostMaskingMode == 1)
	  spCommon::HardMask(_SC->MaskLeft, &_boundRect);
   else
	  {
	  _SC->MaskRight->SetRoi(&_boundRect);
	  _SC->MaskLeft->SetRoi(&_boundRect);
	  _SC->MaskRight->ZeroRoi();
	  _SC->MaskLeft->ZeroRoi();
	  _SC->MaskRight->SetRoi();
      _SC->MaskLeft->SetRoi();
	  }
   }
//CreateUndo = true;
//TODO: delete _polyInterImage
return _boundRect;
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::FinishRender(void)
{
if (_spareImage)        // move it to end????
   delete _spareImage;
_spareImage = 0;
//
if (_polyInterImage)
   {
   _SC->Edit->SetRoi(&_polyBoundRect);
   _SC->Edit->CopyRoi(_polyInterImage);
   _SC->Edit->SetRoi();
   delete _polyInterImage;
   _polyInterImage = 0;
   }
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::PolyInterior(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
TRect boundRect = GetBoundRect(X1, Y1, X2, Y2);
_SC->Brush->FillPoly();
/*
switch (_SC->Shape->FillShape)
   {
   case fillPoly:
	   _SC->Brush->FillPoly();
	   break;
   case fillRectangle:
	   _SC->Brush->FillPoly();
	   //_SC->Brush->FillRectangle(X1, Y1, X2, Y2);
	   break;
   case fillEllipse:
	   _SC->Brush->FillEllipse(X1, Y1, X2, Y2);
	   break;
   case fillAlreadyDone:   //kad bude potreba - dodati u grapbasic setFillshape
	   break;
   default:
	   return;
   }
*/
TBrushBasic *polyBrush = _SC->Brush->Clone();
if (polyBrush->CommonParams->PolyExcludeBump)
   polyBrush->CommonParams->BumpOn = false;
if (polyBrush->CommonParams->PolyExcludeFilters)
   {
   polyBrush->CommonParams->InvertOn = false;
   polyBrush->CommonParams->ChalkOn  = false;
   }
if (polyBrush->CommonParams->PolyBlurOn)
   {
	polyBrush->State->cpROItol = polyBrush->CommonParams->PolyBlurValue;
    polyBrush->ApplyBlur(&boundRect, polyBrush->CommonParams->PolyBlurValue);
   }
//
TColorArithmetic saveColorArithmetic = _colorArithmetic;
if (polyBrush->CommonParams->PolyOverrideCapacityOn)
   polyBrush->CalculateAdditionalParams(polyBrush->CommonParams->Size, polyBrush->CommonParams->PolyCapacityValue);
else
   polyBrush->CalculateAdditionalParams(polyBrush->CommonParams->Size, polyBrush->CommonParams->Capacity);
if (RenderParams->DrawPolyInterior)
   {
	if (polyBrush->CommonParams->PolyColorIndex)
       {
       if (polyBrush->ColorParams->PolyTexture)
          _colorArithmetic = &spColorArithmetics::PolyTexture;
	   else
		  _colorArithmetic = &spColorArithmetics::PolyColor;
       }
    else
	   _colorArithmetic = &spColorArithmetics::PolyColor;
   }
TBrushBasic *curr_brush =  _SC->SetBrush(polyBrush);
DrawBoundRect(&boundRect);
_SC->SetBrush(curr_brush);
_colorArithmetic = saveColorArithmetic;
if (polyBrush->CommonParams->PolyFillOver)
   {
   //if (RenderParams->ClearMask)
	  {
	  _SC->ClearMask(mRight, &boundRect);
	  }
   }
else
   {
   if (ShadowParams->MainMode > 0 && RenderParams->DoShadow)
	  DropShadow(&_boundRect);
   //if (RenderParams->ClearMask)
   _SC->ClearMask(mBoth, &boundRect);
   if (_polyInterImage)
	  delete _polyInterImage;
   _polyInterImage = new spImage<ptRGB>(boundRect.Width(), boundRect.Height());
   _polyBoundRect = boundRect;
   _SC->Edit->SetRoi(&boundRect);
   _polyInterImage->CopyRoi(_SC->Edit);
   _SC->Edit->SetRoi();
   _SC->UpdateImage(imgEdit, &boundRect);
   /*
   if (RenderParams->AutoCommit)
	  _SC->UpdateImage(imgEdit, &boundRect);
   */
   }
delete polyBrush;
}
//---------------------------------------------------------------------------
// protected method
//---------------------------------------------------------------------------
void TRenderBasic::prepareColors(void)
{
_SC->Brush->ColorParams->SetPrimaryColor(ptRGB(_SC->Brush->CommonParams->PrimaryColor));
_SC->Brush->ColorParams->SetSecondaryColor(ptRGB(_SC->Brush->CommonParams->SecondaryColor));
_SC->Brush->ColorParams->SetPolyColor(ptRGB(_SC->Brush->CommonParams->PolyColor));
_SC->Brush->ColorParams->SetSchemeIndex(_SC->Brush->CommonParams->SchemeIndex);
// set initial stepper to dummy
strokeStepper = stepperDummy;
}
//---------------------------------------------------------------------------
bool TRenderBasic::prepareSpareImageCoords(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
_boundRect = GetBoundRect(X1, Y1, X2, Y2);
if (_boundRect.Width() == 0 || _boundRect.Height() == 0)
   return false;
_spareImage = new spFatImage<ptRGB,ippRGB>(_boundRect.Width(), _boundRect.Height());
_SC->j_start = _boundRect.left;
_SC->i_start = _boundRect.top;
//
_SC->FilterImage = _spareImage;
return true;
}
//---------------------------------------------------------------------------
bool TRenderBasic::prepareSpareImageRect(const TRect &roiRect)
{
_boundRect = roiRect;
if (_boundRect.Width() == 0 || _boundRect.Height() == 0)
   return false;
_spareImage = new spFatImage<ptRGB,ippRGB>(roiRect.Width(), roiRect.Height());
_SC->j_start = roiRect.left;
_SC->i_start = roiRect.top;
//
_SC->FilterImage = _spareImage;
return true;
}
//---------------------------------------------------------------------------
// private method
//---------------------------------------------------------------------------
void TRenderBasic::stepperDummy(void)
{
// NOP: prži vanka
}
//---------------------------------------------------------------------------
void TRenderBasic::stepperCircular(void)
{
_SC->Brush->State->wpRealStepCount += _SC->Brush->State->wpRealStepper;
if (_SC->Brush->State->wpRealStepCount > _SC->Brush->State->cpStepperMax)
   _SC->Brush->State->wpRealStepCount = std::fmod(_SC->Brush->State->wpRealStepCount, _SC->Brush->State->cpStepperMax);
_SC->Brush->State->wpStepCount = fast_ftol(_SC->Brush->State->wpRealStepCount);
}
//---------------------------------------------------------------------------
void TRenderBasic::stepperFB(void)
{
_SC->Brush->State->wpRealStepCount += _SC->Brush->State->wpRealStepper;
if (_SC->Brush->State->wpRealStepCount > _SC->Brush->State->cpStepperMax)
   {
   _SC->Brush->State->wpRealStepper = - fabs(_SC->Brush->State->wpRealStepper);
   _SC->Brush->State->wpRealStepCount = _SC->Brush->State->cpStepperMax;
   }
if (_SC->Brush->State->wpRealStepCount < 0.0)
   {
   _SC->Brush->State->wpRealStepper = fabs(_SC->Brush->State->wpRealStepper);
   _SC->Brush->State->wpRealStepCount = 0.0;
   }
_SC->Brush->State->wpStepCount = fast_ftol(_SC->Brush->State->wpRealStepCount);
}
//---------------------------------------------------------------------------
void TRenderBasic::stepperStop(void)
{
if (_SC->Brush->State->wpRealStepCount < _SC->Brush->State->cpStepperMax)
   {
   _SC->Brush->State->wpRealStepCount += _SC->Brush->State->wpRealStepper;
   if (_SC->Brush->State->wpRealStepCount > _SC->Brush->State->cpStepperMax)
	  _SC->Brush->State->wpRealStepCount = _SC->Brush->State->cpStepperMax;
   }
_SC->Brush->State->wpStepCount = fast_ftol(_SC->Brush->State->wpRealStepCount);
}
//---------------------------------------------------------------------------
void TRenderBasic::stepperImage(void)
{
int x = _SC->j_btc;
//int y = _SC->HeightM1 - _SC->i_btc;
int y = _SC->i_btc;
if (x > _SC->WidthM1 || x < 0 || y > _SC->HeightM1 || y < 0 )
   return;
ptRGB mouseColor = _SC->Edit->Pix[y][x];
_SC->Brush->ColorParams->WorkSecondary = mouseColor;
_SC->Brush->ColorParams->CustomColorPrim = mouseColor;
}
//---------------------------------------------------------------------------
void TRenderBasic::stepperClone(void)
{
int x = _SC->j_btc;
//int y = _SC->HeightM1 - _SC->i_btc;
int y = _SC->i_btc;
if (x > _SC->WidthM1 || x < 0 || y > _SC->HeightM1 || y < 0 )
   return;
_SC->Clone_j = x;
_SC->Clone_i = y;
}
//---------------------------------------------------------------------------
/*
void __fastcall TRenderBasic::setStepper(void)
{
if (Brush->ColorParams->SchemeIndex == spe_CTYPE_PALETTE_STEP_C)
   {
   _SC->stepCount++;
   if (_SC->stepCount > 255)
	   _SC->stepCount = 0;
   }
else if (Brush->ColorParams->SchemeIndex == spe_CTYPE_PALETTE_STEP_FB)
   {
   _SC->stepCount += _SC->stepper;
   if (_SC->stepCount > 255)
	  {
	  _SC->stepper = -1;
	  _SC->stepCount = 254;
	  }
   if (_SC->stepCount < 0)
	  {
	  _SC->stepper = 1;
	  _SC->stepCount = 1;
	  }
   }
else if (Brush->ColorParams->SchemeIndex == spe_CTYPE_IMAGE_STEP)
  {
  int x = _SC->j_btc;
  int y = _SC->ImageContainer->HeightM1 - _SC->i_btc;
  if (x > _SC->ImageContainer->WidthM1 || x < 0 || y > _SC->ImageContainer->HeightM1 || y < 0 )
	 return;
  TRGB mouseColorRGB = ImageContainer->ImageEdit->ieBitmap->Pixels[x][y];
  pxcf_Color2BGR(TRGB2TColor(mouseColorRGB), Brush->ColorParams->WorkSecondaryBGR);
  }
else if (Brush->ColorParams->SchemeIndex == spe_CTYPE_PALETTE_STEP_STOP)
   {
   if (_SC->stepCount < 255)
	  _SC->stepCount++;
   }
} */
//---------------------------------------------------------------------------
// protected method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::renderBrushTip(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
if (GoFilterIppRenderCoords)
   GoFilterIppRenderCoords(X1, Y1, X2, Y2);
int i, jitter_loop = 1;
if (_SC->Brush->CommonParams->JitterOn)
   jitter_loop = _SC->Brush->CommonParams->JitterLoopValue;
for (int jt = 0; jt < jitter_loop; jt++)
    {
	if (_SC->Brush->CommonParams->JitterOn)
	   _SC->Brush->JitterLinePoints();
	for (i = 0; i < _SC->Brush->LineParams->Count; i++)
		{
		// Check contour clipping
		if (_contourClipping)
		   {
		   if (_SC->ContourEncountered)	// nema majci
			  return;
		   else
			  _SC->ContourEncountered = contourEncountered(_SC->Brush->LineParams->Point[i].x, _SC->Brush->LineParams->Point[i].y);
		   }
		_boundRect = GetTipBoundRect(_SC->Brush->LineParams->Point[i].x, _SC->Brush->LineParams->Point[i].y);
		if (!_SC->Brush->CommonParams->JitterConnectedOn)
		   _SC->ClearMask(mBoth, &_boundRect);
		_SC->Brush->DrawTip(&_boundRect, i);
		_SC->Brush->ApplyFilters(&_boundRect);
		_SC->j_btc = _SC->Brush->LineParams->Point[i].x;    // for textures and image fixed
		_SC->i_btc = _SC->Brush->LineParams->Point[i].y;
		strokeStepper();
		if (OnShapeStep)
		   //OnShapeStep(_SC->j_btc, _SC->ImageContainer->Height - _SC->i_btc);
		   OnShapeStep(_SC->j_btc, _SC->i_btc);
		DrawBoundRect(&_boundRect);
        }
    }
}
//---------------------------------------------------------------------------
// protected method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::renderShape(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
if (GoFilterIppRenderCoords)
   GoFilterIppRenderCoords(X1, Y1, X2, Y2);
_boundRect = GetBoundRect(X1, Y1, X2, Y2);
int i, jitter_loop = 1;
if (_SC->Brush->CommonParams->JitterOn)
   jitter_loop = _SC->Brush->CommonParams->JitterLoopValue;
for (int jt = 0; jt < jitter_loop; jt++)
	{
	if (_SC->Brush->CommonParams->JitterOn)
	   _SC->Brush->JitterLinePoints();
	// Check contour clipping
	if (_contourClipping)
	   {
	   for (i = 0; i < _SC->Brush->LineParams->Count; i++)
		   {
		   if (_SC->ContourEncountered)	// go out
			  return;
		   else
			  {
			  _SC->ContourEncountered = contourEncountered(_SC->Brush->LineParams->Point[i].x, _SC->Brush->LineParams->Point[i].y);
			  if (_SC->ContourEncountered)
				 {
				 _SC->Brush->LineParams->Count = i;
				 break;
				 }
			  }
		   }
	   }
	_SC->Brush->DrawShape(&_boundRect);
	//_boundRect = pxdf_GetboundRect(PixData, X1, Y1, X2, Y2);
	_SC->Brush->ApplyFilters(&_boundRect);
	for (i = 0; i < _SC->Brush->LineParams->Count; i++)
		{
		_boundRect = GetTipBoundRect(_SC->Brush->LineParams->Point[i].x, _SC->Brush->LineParams->Point[i].y);
		if (!_SC->Brush->CommonParams->JitterConnectedOn)
		   _SC->ClearMask(mBoth, &_boundRect);
		_SC->j_btc = _SC->Brush->LineParams->Point[i].x;   // za texture fixed i image fixed
		_SC->i_btc = _SC->Brush->LineParams->Point[i].y;
		strokeStepper();
        if (OnShapeStep)
			//OnShapeStep(_SC->j_btc, _SC->ImageContainer->Height - _SC->i_btc);
			OnShapeStep(_SC->j_btc, _SC->i_btc);
        DrawBoundRect(&_boundRect);
        }
    }
}
//---------------------------------------------------------------------------
// protected method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::renderShapeFast(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
if (GoFilterIppRenderCoords)
   GoFilterIppRenderCoords(X1, Y1, X2, Y2);
_boundRect = GetBoundRect(X1, Y1, X2, Y2);
int i, jitter_loop = 1;
if (_SC->Brush->CommonParams->JitterOn)
   jitter_loop = _SC->Brush->CommonParams->JitterLoopValue;
if (!_SC->Brush->CommonParams->JitterConnectedOn)
		   _SC->ClearMask(mBoth, &_boundRect);
for (int jt = 0; jt < jitter_loop; jt++)
	{
	if (_SC->Brush->CommonParams->JitterOn)
	   _SC->Brush->JitterLinePoints();
	// Check contour clipping
	if (_contourClipping)
	   {
	   for (i = 0; i < _SC->Brush->LineParams->Count; i++)
		   {
		   if (_SC->ContourEncountered)	// nema majci
			  return;
		   else
			  {
			  _SC->ContourEncountered = contourEncountered(_SC->Brush->LineParams->Point[i].x, _SC->Brush->LineParams->Point[i].y);
			  if (_SC->ContourEncountered)
				 {
				 _SC->Brush->LineParams->Count = i;
				 break;
				 }
			  }
		   }
	   }
	_SC->Brush->DrawShape(&_boundRect);
    _SC->Brush->ApplyFilters(&_boundRect);
	DrawBoundRect(&_boundRect);
    }
}
//---------------------------------------------------------------------------
// protected method - virtual
//---------------------------------------------------------------------------
void TRenderBasic::renderDummy(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
int i, jitter_loop = 1;
if (_SC->Brush->CommonParams->JitterOn)
   jitter_loop = _SC->Brush->CommonParams->JitterLoopValue;
for (int jt = 0; jt < jitter_loop; jt++)
    {
	if (_SC->Brush->CommonParams->JitterOn)
	   _SC->Brush->JitterLinePoints();
	for (i = 0; i < _SC->Brush->LineParams->Count; i++)
		{
		// contour cilpping????
		_SC->j_btc = _SC->Brush->LineParams->Point[i].x;
		_SC->i_btc = _SC->Brush->LineParams->Point[i].y;
        //setStepper(); <- not necessary
        if (OnShapeStep)
		   //OnShapeStep(_SC->j_btc, _SC->ImageContainer->Height - _SC->i_btc);
		   OnShapeStep(_SC->j_btc, _SC->i_btc);
        }
    }
}
//---------------------------------------------------------------------------
void TRenderBasic::setDropShadowRect(TRect *boundRect)
{
// malo tolerancije na blur
if (ShadowParams->BlurOn)
   {
   boundRect->left   -=  (ShadowParams->BlurValue * 2 + 1);
   boundRect->top    -=  (ShadowParams->BlurValue * 2 + 1);
   boundRect->right  +=  (ShadowParams->BlurValue * 2 + 1);
   boundRect->bottom +=  (ShadowParams->BlurValue * 2 + 1);
   }
if (ShadowParams->Xoffset < 0)
   boundRect->left   +=  ShadowParams->Xoffset;
else
   boundRect->right  +=  ShadowParams->Xoffset;
if (ShadowParams->Yoffset < 0)
   boundRect->top    +=  ShadowParams->Yoffset;
else
   boundRect->bottom +=  ShadowParams->Yoffset;
if (boundRect->left < 0)
   boundRect->left = 0;
if (boundRect->top < 0)
   boundRect->top = 0;
if (boundRect->right > _SC->Width)
   boundRect->right = _SC->Width;
if (boundRect->bottom > _SC->Height)
   boundRect->bottom = _SC->Height;
}
//---------------------------------------------------------------------------
TRect TRenderBasic::GetBoundRect(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
TRect boundRect;
/* everyone passes me good coordinates, thanks...for now.
int tv;
if (X2 < X1)
   {
   tv = X1;
   X1 = X2;
   X2 = tv;
   }
if (Y2 < Y1)
   {
   tv = Y1;
   Y1 = Y2;
   Y2 = tv;
   }*/
int X_bmp1, Y_bmp1, X_bmp2, Y_bmp2;
Y_bmp1 = Y1; //Surf->HeightM1; // - Y1;
X_bmp1 = X1;
Y_bmp2 = Y2; //Surf->HeightM1; // - Y2;
X_bmp2 = X2;
boundRect.top      = std::min(Y_bmp1,Y_bmp2) - ((_SC->Brush->State->cpHalfSizeMax) + 2)  - _SC->Brush->State->cpROItol;
boundRect.bottom   = std::max(Y_bmp1,Y_bmp2) + ((_SC->Brush->State->cpHalfSizeMax) + 2)  + _SC->Brush->State->cpROItol;
boundRect.left     = std::min(X_bmp1,X_bmp2) - ((_SC->Brush->State->cpHalfSizeMax) + 2)  - _SC->Brush->State->cpROItol;
boundRect.right	   = std::max(X_bmp1,X_bmp2) + ((_SC->Brush->State->cpHalfSizeMax) + 2)  + _SC->Brush->State->cpROItol;
if (boundRect.top < 0)
    boundRect.top = 0;
if (boundRect.bottom < 0)
    boundRect.bottom = 0;
if (boundRect.left < 0)
    boundRect.left = 0;
if (boundRect.right < 0)
    boundRect.right = 0;
if (boundRect.top > _SC->Height)
	boundRect.top = _SC->Height;
if (boundRect.bottom > _SC->Height)
	boundRect.bottom = _SC->Height;
if (boundRect.left > _SC->Width)
	boundRect.left = _SC->Width;
if (boundRect.right > _SC->Width)
	boundRect.right = _SC->Width;
// do we need to do anything
//if (boundRect.Width() == 0 || boundRect.Height() == 0)
return boundRect;
}
//---------------------------------------------------------------------------
TRect TRenderBasic::GetBoundRectClear(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
TRect boundRect;
/* everyone passes me good coordinates, thanks...for now.
int tv;
if (X2 < X1)
   {
   tv = X1;
   X1 = X2;
   X2 = tv;
   }
if (Y2 < Y1)
   {
   tv = Y1;
   Y1 = Y2;
   Y2 = tv;
   }*/
int X_bmp1, Y_bmp1, X_bmp2, Y_bmp2;
Y_bmp1 = Y1; //Surf->HeightM1; // - Y1;
X_bmp1 = X1;
Y_bmp2 = Y2; //Surf->HeightM1; // - Y2;
X_bmp2 = X2;
boundRect.top      = std::min(Y_bmp1,Y_bmp2);
boundRect.bottom   = std::max(Y_bmp1,Y_bmp2);
boundRect.left     = std::min(X_bmp1,X_bmp2);
boundRect.right	   = std::max(X_bmp1,X_bmp2);
if (boundRect.top < 0)
    boundRect.top = 0;
if (boundRect.bottom < 0)
    boundRect.bottom = 0;
if (boundRect.left < 0)
    boundRect.left = 0;
if (boundRect.right < 0)
    boundRect.right = 0;
if (boundRect.top > _SC->Height)
	boundRect.top = _SC->Height;
if (boundRect.bottom > _SC->Height)
	boundRect.bottom = _SC->Height;
if (boundRect.left > _SC->Width)
	boundRect.left = _SC->Width;
if (boundRect.right > _SC->Width)
	boundRect.right = _SC->Width;
// anything to do
//if (boundRect.Width() == 0 || boundRect.Height() == 0)
return boundRect;
}
//---------------------------------------------------------------------------
TRect TRenderBasic::GetBoundRectComplete(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
TRect boundRect = GetBoundRect(X1, Y1, X2, Y2);
if (ShadowParams->MainMode > 0 && RenderParams->DoShadow)
   setDropShadowRect(&boundRect);
return boundRect;
}
//---------------------------------------------------------------------------
TRect TRenderBasic::GetTipBoundRect(const int &X, const int &Y)
{
TRect boundRect;
boundRect.top      = Y - ((_SC->Brush->State->cpHalfSize) + 2)  - _SC->Brush->State->cpROItol;
boundRect.bottom   = Y + ((_SC->Brush->State->cpHalfSize) + 2)  + _SC->Brush->State->cpROItol;
boundRect.left     = X - ((_SC->Brush->State->cpHalfSize) + 2)  - _SC->Brush->State->cpROItol;
boundRect.right	   = X + ((_SC->Brush->State->cpHalfSize) + 2)  + _SC->Brush->State->cpROItol;
if (boundRect.top < 0)
	boundRect.top = 0;
if (boundRect.bottom < 0)
	boundRect.bottom = 0;
if (boundRect.left < 0)
	boundRect.left = 0;
if (boundRect.right < 0)
	boundRect.right = 0;
if (boundRect.top > _SC->Height)
	boundRect.top = _SC->Height;
if (boundRect.bottom > _SC->Height)
	boundRect.bottom = _SC->Height;
if (boundRect.left > _SC->Width)
	boundRect.left = _SC->Width;
if (boundRect.right > _SC->Width)
	boundRect.right = _SC->Width;
// anything to do
//if (boundRect.Width() == 0 || boundRect.Height() == 0)
return boundRect;
}
//---------------------------------------------------------------------------
TRect TRenderBasic::GetTipBoundRectClear(const int &X, const int &Y, const int &half_size)
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
if (boundRect.top > _SC->Height)
	boundRect.top = _SC->Height;
if (boundRect.bottom > _SC->Height)
	boundRect.bottom = _SC->Height;
if (boundRect.left > _SC->Width)
	boundRect.left = _SC->Width;
if (boundRect.right > _SC->Width)
	boundRect.right = _SC->Width;
return boundRect;
}
//---------------------------------------------------------------------------
void TRenderBasic::DrawImage(TRect *boundRect)
{
if (boundRect)
   _boundRect = *boundRect;
else
   _boundRect.init(0, 0, _SC->Width, _SC->Height);
if (GoFilterIppRenderCoords)
   {
   GoFilterIppRenderRect(_boundRect);
   /*
   IppiSize roiSize = {_SC->Width, _SC->Height} ;
   Ipp8u* pSrc;
   Ipp8u* pDst;
   int srcStep;
   int dstStep;
   pSrc  = (unsigned char*)_SC->FilterImage->ImageIpp->Data();
   pDst  = (unsigned char*)_SC->ippOrig->Data();
   srcStep = _SC->FilterImage->ImageIpp->Stride();
   dstStep = _SC->ippOrig->Stride();

   ippiCopy_8u_C3R(pSrc, srcStep, pDst, dstStep, roiSize);
   */
   /*TODO:
   GoFilterRender(0, 0, 0, 0, false);
   if (renderEngine->ImageContainer->PaintView->Selected || masked || renderEngine->Brush->ColorParams->BackgroundTile)
	   pxdf_DrawImage(renderEngine->PixData,renderEngine->ColorArithmetic, renderEngine->PixelArithmetic);
   else
	  {
	   if (renderEngine->Brush->CommonParams->Capacity == 100) // full capacity
		  iplCopy(renderEngine->SpareImage->Image->SpImage->iplImage, renderEngine->ImageContainer->ImageOrig->SpImage->iplImage);
	   else
		  {         // a di je alfa kanal???
		  int alpha = (int)(renderEngine->Brush->CommonParams->Capacity * 255.0 / 100.0);
		  iplAlphaCompositeC(renderEngine->ImageContainer->ImageEdit->SpImage->iplImage, renderEngine->SpareImage->Image->SpImage->iplImage,
							 renderEngine->ImageContainer->ImageOrig->SpImage->iplImage,
							 IPL_COMPOSITE_PLUS, 255 - alpha, alpha, false, false);
		  }
	  }
   */
   }
DrawBoundRect(&_boundRect);
}
//---------------------------------------------------------------------------
void TRenderBasic::DrawBoundRect(TRect *boundRect)
{
// Temeljni dio crtanja...manipulacija pikselima unutar boundRect-a
int i, j;
bool selClipping =  _SC->Selection && RenderParams->SelectionClipping;
bool stencilClipping = _SC->AssStencil && _SC->StencilAlive;
for (i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (j = boundRect->left; j < boundRect->right; j++)
		{
		_SC->i = i;
		_SC->j = j;
		if (selClipping)  // opaaa, clippping selection
		   {
		   if (_SC->Selection->Pixel(i, j).ch[0] == 0x00)
			  continue;
		   }
		if (stencilClipping) //opaaa, we have stencil also
		   doStencil();
		if (_SC->MaskRight->Byte0[i][j] > 0x00)
		   {
		   if (_SC->Brush->ColorParams->BackgroundTile)
			  {
			  int tI = i % _SC->Brush->ColorParams->BackgroundTile->Height();
			  int tJ = j % _SC->Brush->ColorParams->BackgroundTile->Width();
			  Byte tile_byte = _SC->Brush->ColorParams->BackgroundTile->Pixel(tI, tJ).ch[0];
			  if(tile_byte < _SC->MaskRight->Byte0[i][j])
				 _SC->MaskRight->Byte0[i][j] = tile_byte;
			  }
		   if (_colorArithmetic)
			  _colorArithmetic(_SC);
		   }
		}
	}
}
//---------------------------------------------------------------------------
void TRenderBasic::DropShadow(TRect *boundRect)
{
// Throw shadod in a new way
setDropShadowRect(boundRect);
if (ShadowParams->BlurOn)
   spCommon::FBGrayRun(_SC->MaskLeft, boundRect, ShadowParams->BlurValue);
//
int undoSize   = _SC->Edit->Depth() * boundRect->Width()* boundRect->Height();
//unsigned int undoStride = _SC->Edit->Stride();
void *undoBuffer = GetMemory(undoSize);
_SC->Orig->SetRoi(boundRect);
_SC->Edit->SetRoi(boundRect);
_SC->Edit->CopyRoi2External(undoBuffer);
_SC->Edit->CopyRoi(_SC->Orig);
_SC->Orig->SetRoi();
_SC->Edit->SetRoi();
int i, j;
bool selClipping = _SC->Selection && RenderParams->SelectionClipping;
// REMARK: set CustomColorSec to shadow color(somewhere from the outside)
int shadowCap = fast_ftol((float)ShadowParams->Capacity /100.0 * spe_SP_NORM);
int si, sj;
// we don't want bad artifacts on image borders!?
TRect niceBound;
niceBound.top = boundRect->top;
niceBound.bottom = boundRect->bottom;
niceBound.left = boundRect->left;
niceBound.right = boundRect->right;
if (ShadowParams->Yoffset > 0)
   niceBound.top -= ShadowParams->Yoffset;
else
   niceBound.bottom -= ShadowParams->Yoffset;
if (ShadowParams->Xoffset > 0)
   niceBound.left -= ShadowParams->Xoffset;
else
   niceBound.right -= ShadowParams->Xoffset;
// first pass
ptRGB shadowColor = ptRGB(ShadowParams->Color);
for (i = niceBound.top; i < niceBound.bottom; i++)
	{
	si = i + ShadowParams->Yoffset;
	if (si < 0 || si > _SC->HeightM1)
	   continue;
	for (j = niceBound.left; j < niceBound.right; j++)
		{
		sj = j + ShadowParams->Xoffset; // test
		if (sj < 0 || sj > _SC->WidthM1)
		   continue;
		_SC->i = i < 0 ? 0 : i > _SC->HeightM1 ? _SC->HeightM1 : i;
		_SC->j = j < 0 ? 0 : j > _SC->WidthM1 ? _SC->WidthM1 : j;
		if (selClipping)  // opaaa, clipping selection
		   {
		   if (_SC->Selection->Pixel(_SC->i, _SC->j).ch[0] == 0x00)
			  continue;
		   }
		// stencil?
		/*TODO:
		if (Surf_SC->ImageContainer->StencilActive)
			pxdf_Stencil(PixData);
		*/
		spPixelMixer::ShadowColor(_SC, shadowColor, shadowCap, si, sj);
		}
	}
//restore maskLeft
_SC->MaskLeft->SetRoi(boundRect);
_SC->MaskRight->SetRoi(boundRect);
_SC->MaskLeft->CopyRoi(_SC->MaskRight);
_SC->MaskLeft->SetRoi();
_SC->MaskRight->SetRoi();
// second pass
for (i = boundRect->top; i < boundRect->bottom; i++)
	{
	for (j = boundRect->left; j < boundRect->right; j++)
		{
		_SC->i = i;
		_SC->j = j;
		if (selClipping)  // opaaa, clipping selection
		   {
		   if (_SC->Selection->Pixel(i, j).ch[0] == 0x00)
			  continue;
		   }
		if (ShadowParams->MainMode == 2)
			spPixelMixer::ShadowColor(_SC, shadowColor, shadowCap, i, j);
		spPixelMixer::BlendColor(_SC);
		}
	}
_SC->Edit->SetRoi(boundRect);
_SC->Edit->CopyExternal2Roi(undoBuffer);
_SC->Edit->SetRoi();
FreeMemory(undoBuffer);
}
//---------------------------------------------------------------------------
// protetluk
void TRenderBasic::doStencil(void)
{
if (_SC->i < _SC->Stencil_i || _SC->i >= _SC->Stencil_i + _SC->AssStencil->Height()
   || _SC->j < _SC->Stencil_j || _SC->j >= _SC->Stencil_j + _SC->AssStencil->Width())
   return;
int iMask = (int)_SC->MaskRight->Pix[_SC->i][_SC->j].ch[0];
int iStenc = (int)_SC->AssStencil->Pixel(_SC->i - _SC->Stencil_i, _SC->j - _SC->Stencil_j).ch[0];
_SC->MaskRight->Pix[_SC->i][_SC->j].ch[0] = spPixelArithmetics::NormColor(iMask - iStenc, 0x00, 0xff);
}
//---------------------------------------------------------------------------
void TRenderBasic::Connect(spSurfaceContainer *surfaceContainer)
{
_SC = surfaceContainer;
}
//---------------------------------------------------------------------------
void TRenderBasic::SetBoundRect(TRect boundRect)
{
_boundRect = boundRect;
}






