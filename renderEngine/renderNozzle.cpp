//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderNozzle.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
// constructor
TRenderNozzle::TRenderNozzle(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
					   : TRenderBasic(surfaceContainer, renderData)
{
NozzleParams  = renderData->NozzleParams;
NozzleParams->CurrentCell = NozzleParams->StartCell;
saveBrushStep = _SC->Brush->CommonParams->Step;
saveBrushSize = _SC->Brush->CommonParams->Size;
// TODO: add dummy brush, so we can modify it without wndangering other brushes
}
//---------------------------------------------------------------------------
// destructor
TRenderNozzle::~TRenderNozzle(void)
{
_SC->Brush->CommonParams->Step = saveBrushStep;
_SC->Brush->CommonParams->Size = saveBrushSize;
}
//---------------------------------------------------------------------------
UnicodeString TRenderNozzle::ClassName(void)
{
return "TRenderNozzle";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderNozzle::Begin(int X, int Y)
{
prepareColors();
TRect stampRect;
_autoRenderSuggest = spe_RENDER_FAST;
//
//_SC->realStepCount = 0.0;
//_SC->stepCount = 0;
//_SC->realStepper = NozzleParams->Step;
_SC->Brush->State->wpRealStepper = NozzleParams->Step;   // override pal stepper
_SC->CloneImage = 0;
spColorArithmetics::ArithData.topVal = GlobalColorParams->ScaleTop;
spColorArithmetics::ArithData.botVal = GlobalColorParams->ScaleBottom;
spColorArithmetics::ArithData.invertPixel = false;
spColorArithmetics::ArithData.bitShift = 0;
_colorArithmetic = &spColorArithmetics::CustomSingleColor;
spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
cellsCount = NozzleParams->Rows * NozzleParams->Cols;
nozzWidth  = fast_ftol(NozzleParams->CellWidth);
nozzHeight = fast_ftol(NozzleParams->CellHeight);
CreateUndo = true;
_SC->MaskRight->SetContext(); 	// crtanje na masku
_SC->MaskRight->PenColor   = ptGray(255);
_SC->MaskRight->BrushColor = ptGray(255);
_SC->update_alpha = true;
_SC->ContourEncountered = false;
GoRenderStroke = &renderBrushTip;
_SC->skip_empty_alpha = true;
//
_SC->Brush->CommonParams->Step = NozzleParams->Step;
_SC->Brush->CommonParams->Size = max(NozzleParams->CellWidth, NozzleParams->CellHeight);
_SC->Brush->Begin(_SC->Shape);  // reset grap
}
//---------------------------------------------------------------------------
// protected method
//---------------------------------------------------------------------------
void TRenderNozzle::renderBrushTip(const int &X1, const int &Y1, const int &X2, const int &Y2)
{
int i; //, jitter_loop = 1;
int nozzOffsetX, nozzOffsetY,  oX, oY;
for (i = 0; i < _SC->Brush->LineParams->Count; i++)
	{
	// No need for contour checking!!!!
	_boundRect = GetTipBoundRect(_SC->Brush->LineParams->Point[i].x, _SC->Brush->LineParams->Point[i].y);
	if (!_SC->Brush->CommonParams->JitterConnectedOn)
	   _SC->ClearMask(mBoth, &_boundRect);
	//_SC->j_btc = _SC->Brush->LineParams->Point[i].x;
	//_SC->i_btc = _SC->Brush->LineParams->Point[i].y;
	oX = _SC->Brush->LineParams->Point[i].x;
	oY = _SC->Brush->LineParams->Point[i].y;
	switch (NozzleParams->MainMode)
	  {
	  case spe_NOZZLE_RANDOM:       // random  cells
		   nozzOffset = random(cellsCount);
		   break;
	  case spe_NOZZLE_SEQUENCE:       // cell sequence
		   nozzOffset = NozzleParams->CurrentCell + 1;
		   if (nozzOffset >= cellsCount)
			   nozzOffset = 0;
		   break;
	  default:      // single cell
		   nozzOffset = NozzleParams->StartCell;
		   break;
	  }
	nozzOffsetY = nozzOffset / NozzleParams->Cols;
	nozzOffsetX = nozzOffset % NozzleParams->Cols;
	nozzX = fast_ftol(nozzOffsetX * NozzleParams->CellWidth);
	nozzY = fast_ftol(nozzOffsetY * NozzleParams->CellHeight);
	//oX -= (nozzWidth>>1);
	//oY -= (nozzHeight>>1);
	nozzleImageOut(oX, oY);
	}
}
//---------------------------------------------------------------------------
void TRenderNozzle::nozzleImageOut(const int &X, const int &Y)
{
GlobalColorParams->NozzleImage->SetRoi(nozzX, nozzY, nozzX + nozzWidth, nozzY + nozzHeight, true);
GlobalColorParams->NozzleAlpha->SetRoi(nozzX, nozzY, nozzX + nozzWidth, nozzY + nozzHeight, true);
_SC->Orig->SetRoi(X - (nozzWidth>>1), Y - (nozzHeight>>1), X + (nozzWidth>>1), Y + (nozzHeight>>1));
_boundRect.left = X - (nozzWidth>>1);
_boundRect.top  = Y - (nozzHeight>>1);
_boundRect.right  = X + (nozzWidth>>1);
_boundRect.bottom = Y + (nozzHeight>>1);
spCommon::PasteImage(GlobalColorParams->NozzleImage, GlobalColorParams->NozzleAlpha, _SC->Orig, _SC->AlphaOrig, spe_SP_NORM);
}
//---------------------------------------------------------------------------
// Finish render
//---------------------------------------------------------------------------
void TRenderNozzle::FinishRender(void)
{
TRenderBasic::FinishRender();
_SC->Orig->SetRoi();
}
//---------------------------------------------------------------------------
// on step events
//---------------------------------------------------------------------------
void TRenderNozzle::stepNozzle(const int &X, const int &Y)
{
//TODO: call host function
}




