//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderDataKeeper.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
spRenderDataKeeper::spRenderDataKeeper(void)
{
int i;
// render params
RenderParams = new Tspe_RenderParams;
//RenderParams->RenderType = spe_rndDraw;
RenderParams->ClearImoc = true;
RenderParams->PostMaskingMode = 0;  // as is
RenderParams->AutoCommit = true;
RenderParams->CreateUndo = true;
RenderParams->DoShadow  = true;
RenderParams->RenderingMode = spe_RENDER_AUTOMATIC;
RenderParams->DrawTipOnMouseDown = false;
RenderParams->SelectionClipping = true;
RenderParams->ContourClipping = false;
RenderParams->ContourClippingThreshold = 160;
RenderParams->ContourClippingRadius = 2;
RenderParams->AssistantMaskUpdate = false;
RenderParams->AssistantMaskLastShapeOnly = false;
RenderParams->MultiDrawResetBoundRect = true;
RenderParams->MixSurf = 0;    // edit by default
RenderParams->ArithSurf = 0;  // edit by default
// selection params
/* removed in 0.6
SelectionParams = new spsSelectionParams;
SelectionParams->SelFeather = 0;
SelectionParams->SelRestore = true;
SelectionParams->SelClipping = true;
SelectionParams->SelWandTolerance = 15;
SelectionParams->SelWandMaxFilter = true;
SelectionParams->SelWandMode = 0;
SelectionParams->PasteIntoBlendMode = 0;
SelectionParams->PasteIntoResampleMode = 9;
*/
// assistant params
/*
AssistantParams = new spsAssistantParams;
AssistantParams->MainMode = 0;
// contour
AssistantParams->ContourMode = 0;
AssistantParams->ContourClipping = false;
AssistantParams->ContourClippingThreshold = 160;
AssistantParams->ContourClippingRadius = 2;
// mask
AssistantParams->MaskBlendOnly = false;
AssistantParams->MaskAutoUpdate = false;
AssistantParams->MaskLastShape = false;
// sidekick
AssistantParams->SidekickBlendMode = 0;
AssistantParams->SidekickBlendPercentage = 100;
*/
// stencil (todo???)
//
// action draw parameters
DrawParams = new Tspe_DrawParams;
DrawParams->MainMode = spe_ARITH_NORM;
DrawParams->SaveMode = spe_ARITH_NORM;
DrawParams->Shape = 0;
DrawParams->BitShift = 0;
DrawParams->InvertSource = false;
for (i = 0; i < 3; i++)
	{
	DrawParams->ScaleBottomRGB[i] = 0;
	DrawParams->ScaleTopRGB[i] = 255;
	}
DrawParams->UpdateAlpha = true;
DrawParams->StrokesNumber = 1;
DrawParams->EraseMatchAll = true;
DrawParams->EraseMatchColorTolerance = 0;
DrawParams->EraseMatchColor = 0x00000000;
DrawParams->EraseReplaceColor = 0x00ffffff;
//DrawParams->EraseCustom = true; // removed in 0.4
//DrawParams->Clone_Started = false; removed in 0.5
DrawParams->DrawOnAlpha = false;
//
// action color paremters
ColorParams = new Tspe_ColorParams;
ColorParams->MainMode = spe_COLOR_BRIGTHNESS;
ColorParams->ChanME[0] = true;
ColorParams->ChanME[1] = false;
ColorParams->ChanME[2] = true;
ColorParams->CPercentage = true;
ColorParams->ChanRGB[0] = 0;
ColorParams->ChanRGB[1] = 0;
ColorParams->ChanRGB[2] = 0;
ColorParams->ChanGamma[0] = 2.2;
ColorParams->ChanGamma[1] = 2.2;
ColorParams->ChanGamma[2] = 2.2;
ColorParams->CastOn[0] = true;
ColorParams->CastOn[1] = true;
ColorParams->CastOn[2] = true;
ColorParams->CastRange[0] = 64;
ColorParams->CastRange[1] = 191;
ColorParams->CastColor[0] = 0x00000000;
ColorParams->CastColor[1] = 0x00808080;
ColorParams->CastColor[2] = 0x00ffffff;
for (i = 0; i < 3; i++)
	{
	ColorParams->ScaleBottomRGB[i] = 64;
	ColorParams->ScaleTopRGB[i] = 191;
	}
ColorParams->ToneColor = 0x0000ffff;
ColorParams->Temperature = 1000;
ColorParams->ToneType = 0;
ColorParams->TonePreserveLuminance = true; //preserve luminance
//ColorParams->ColorCorrection = 0;	//TODO !!!!!!!
// action shift parameters
ShiftParams = new Tspe_ShiftParams;
ShiftParams->MainMode = spe_ARITH_XOR;
ShiftParams->X_shift = 1;
ShiftParams->Y_shift = 1;
ShiftParams->BumpScale = 1.0;
ShiftParams->InvertSource = false;
for (i = 0; i < 3; i++)
	{
	ShiftParams->ScaleBottomRGB[i] = 0;
	ShiftParams->ScaleTopRGB[i] = 255;
	}
ShiftParams->BitShift = 0;
ShiftParams->ZeroShiftRandom = 1;
// 3x3
Filter33Params = new Tspe_Filter33Params;
Filter33Params->MainMode = 0;
Filter33Params->Wrap = false;
for (i = 0; i < 3; i++)
	Filter33Params->Chan[i] = true;
Filter33Params->SyntaxOk = false;
// ipp
FilterIppParams = new Tspe_FilterIppParams;
FilterIppParams->MainMode = 0;
FilterIppParams->BorderType = 0; // replicate
FilterIppParams->RadiusX = 1;
FilterIppParams->RadiusY = 1;
FilterIppParams->Radius = 1;
FilterIppParams->KernelX = 3;
FilterIppParams->KernelY = 3;
FilterIppParams->AnchorX = 1;
FilterIppParams->AnchorY = 1;
FilterIppParams->NoiseLevel = 0.1f;
FilterIppParams->SigmaColor = 1400.0f;
FilterIppParams->SigmaSpace = 40.0f;
FilterIppParams->StDev      = 1.35f;
FilterIppParams->Fixed33 = true;
FilterIppParams->MorphMask = 0;
FilterIppParams->GeneralDiv = 1;
FilterIppParams->GeneralBias= 1;
//FilterIppParams->Iterations = 1;
for (i = 0; i < 3; i++)
	FilterIppParams->Chan[i] = true;
// color twist ipp
ColorTwistIppParams = new Tspe_ColorTwistIppParams;
ColorTwistIppParams->MainMode = 0;
ColorTwistIppParams->SyntaxOk = false;
//
// compound ipp
CompoundIppParams = new Tspe_CompoundIppParams;
CompoundIppParams->MainMode = 0;
CompoundIppParams->BorderType = 0; // replicate
CompoundIppParams->RadiusX = 1;
CompoundIppParams->RadiusY = 1;
CompoundIppParams->KernelX = 3;
CompoundIppParams->KernelY = 3;
CompoundIppParams->LoVal = 0;
CompoundIppParams->HiVal = 255;
CompoundIppParams->RangeType = 0;
CompoundIppParams->CDMode = 0;
CompoundIppParams->KFactor = 0.7;
for (i = 0; i < 3; i++)
	CompoundIppParams->Chan[i] = true;
//
//
SpreadParams = new Tspe_SpreadParams;
SpreadParams->MainMode = 0;
SpreadParams->BlendIn = 100;
SpreadParams->BlendOut = 100;
SpreadParams->SpreadArithMode = 0;
SpreadParams->SpreadColorPickMode = 0;
//
SpreadParams->SpreadMultiColorMode = 0;
SpreadParams->SpreadMultiColorRandomCount = 4;
SpreadParams->SpreadMultiColorRandomWalker = false;
//
SpreadParams->WarpColorPickMode = 0;
SpreadParams->WarpCount = 5;
for (i = 0; i < SpreadParams->WarpCount; i++)
    {
	SpreadParams->WarpOn[i] = false;
    SpreadParams->WarpOrder[i] = i;
    SpreadParams->WarpParam1[i] = 30;
    SpreadParams->WarpParam2[i] = 50;
    }
SpreadParams->WarpOn[0] = true;
SpreadParams->WarpParam2[2] = 10; // twirl
SpreadParams->WarpSameRange = false;
SpreadParams->WarpMaskSoft = false;
SpreadParams->WarpMaskHard  = false;
//
//SpreadParams->CloneImage = 0;
//SpreadParams->Shuffler = 0;
SpreadParams->ShufflerMode = 0;
SpreadParams->ShufflerDistance = 5;
SpreadParams->ShufflerRandomDistance = false;
SpreadParams->ShufflerGroupSize = 3;
SpreadParams->ShufflerLuminanceMin = 20;
SpreadParams->ShufflerLuminanceMax = 70;
SpreadParams->ShufflerSortGroup = 0;
SpreadParams->PickCustomPrim = false;
SpreadParams->PickCustomSec = false;
SpreadParams->CustomPrimPick.x = 0;
SpreadParams->CustomPrimPick.y = 0;
SpreadParams->CustomSecPick.x = 0;
SpreadParams->CustomSecPick.y = 0;
// calculated
//SpreadParams->cpSpreadPickRandom = false;
//
ShadowParams = new Tspe_ShadowParams;
ShadowParams->MainMode = 0;
ShadowParams->Xoffset = 4;
ShadowParams->Yoffset = 4;
ShadowParams->BlurOn  = false;
ShadowParams->BlurValue  = 1;
ShadowParams->Capacity = 100;
ShadowParams->Color = 0x00000000;
//
GlobalColorParams = new spsGlobalColorParams;
GlobalColorParams->ScaleTop = ptRGB(255, 255, 255);
GlobalColorParams->ScaleBottom = ptRGB(0, 0, 0);
GlobalColorParams->CloneImage = 0;
GlobalColorParams->Clone_Started = false;
GlobalColorParams->LUTCurveEntry = 0;
GlobalColorParams->NozzleImage = 0;
GlobalColorParams->NozzleImage = 0;
GlobalColorParams->ShortPalCount = 0;
GlobalColorParams->ExtraPalCount = 0;
//
NozzleParams = new Tspe_NozzleParams;
NozzleParams->MainMode = 0;
NozzleParams->Step = 10;
NozzleParams->Capacity = 100;
NozzleParams->CellWidth = 0;
NozzleParams->CellHeight = 0;
NozzleParams->StartCell = 0;
NozzleParams->Rows = 0;
NozzleParams->Cols = 0;
}
//---------------------------------------------------------------------------
spRenderDataKeeper::~spRenderDataKeeper(void)
{
delete RenderParams;
//delete SelectionParams;
//delete AssistantParams;
delete DrawParams;
delete ColorParams;
delete ShiftParams;
delete Filter33Params;
delete FilterIppParams;
delete ColorTwistIppParams;
delete CompoundIppParams;
delete SpreadParams;
delete ShadowParams;
delete NozzleParams;
// symmetryparams
//TODO: release export arrays
// delete clone/nozzle image?????
delete GlobalColorParams;
}


