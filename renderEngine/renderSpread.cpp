//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderSpread.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
// constructor
TRenderSpread::TRenderSpread(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
					   : TRenderDraw(surfaceContainer, renderData)
{
SpreadParams  = renderData->SpreadParams;
pushImage = 0;
shuffler = 0;
remapper = 0;
}
//---------------------------------------------------------------------------
// destructor
TRenderSpread::~TRenderSpread(void)
{
_SC->CloneImage = GlobalColorParams->CloneImage;
if (pushImage)
   delete pushImage;
if (shuffler)
   delete shuffler;
if (remapper)
   delete remapper;
}
//---------------------------------------------------------------------------
UnicodeString TRenderSpread::ClassName(void)
{
return "TRenderSpread";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderSpread::Begin(int X, int Y)
{
//int i, k;
prepareColors();
TRect stampRect;
_autoRenderSuggest = spe_RENDER_FAST;
//
_SC->Brush->State->cpStepperMax = 255.0;
_SC->PalEntry = _SC->Brush->ColorParams->PalEntry;
_SC->PalCount = 256;
//_SC->realStepCount = 0.0;
//_SC->stepCount = 0;
//_SC->realStepper = _SC->Brush->CommonParams->PalGradStepper;
_SC->CloneImage = 0;
spColorArithmetics::ArithData.topVal = GlobalColorParams->ScaleTop;
spColorArithmetics::ArithData.botVal = GlobalColorParams->ScaleBottom;
spColorArithmetics::ArithData.invertPixel = false;
spColorArithmetics::ArithData.bitShift = 0;
_SC->Brush->ColorParams->WorkPrimary = _SC->Brush->ColorParams->PrimaryColor;
_SC->Brush->ColorParams->WorkSecondary = _SC->Brush->ColorParams->SecondaryColor;
_blenderIn = fast_ftol(((float)SpreadParams->BlendIn / 100.0f) * spe_SP_NORM);
_blenderOut = fast_ftol(((float)SpreadParams->BlendOut / 100.0f) * spe_SP_NORM);
cpSpreadPickRandom = false;
_pickMousePrimColor = false;
_pickMouseSecColor = false;
switch (SpreadParams->MainMode)
	{
	case spe_SPREAD_SPREAD:
		 {
		 if (SpreadParams->PickCustomPrim)
			_SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[SpreadParams->CustomPrimPick.y][SpreadParams->CustomPrimPick.x]; //ptRGB(_SC->Brush->CommonParams->CustomColorPrim);
		 else if (SpreadParams->SpreadColorPickMode == 0 || SpreadParams->SpreadColorPickMode == 1)
			{
			_SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[Y][X];
			}
		 else
			{
			strokeStepper = stepperImage;
			_SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[Y][X];
			_autoRenderSuggest = spe_RENDER_SLOW;
			}
		 _pickMousePrimColor = (SpreadParams->SpreadColorPickMode == 1);
		 _colorArithmetic = &spColorArithmetics::CustomSingleColor;
		 switch (SpreadParams->SpreadArithMode)
			   {
			   case spe_ARITH_NORM:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
					break;
			   case spe_ARITH_XOR:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_XOR;
					break;
			   case spe_ARITH_AND:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_AND;
					break;
			   case spe_ARITH_OR:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_OR;
					break;
			   case spe_ARITH_ADD:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_ADD;
					break;
			   case spe_ARITH_SUB:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_SUB;
					break;
			   case spe_ARITH_MULT:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MULT;
					break;
			   case spe_ARITH_MULT_nA:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MULT_nA;
					break;
			   case spe_ARITH_MULT_nB:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MULT_nB;
					break;
			   case spe_ARITH_HIGHER:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Higher;
					break;
			   case spe_ARITH_LOWER:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Lower;
					break;
			   case spe_ARITH_MEAN:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Mean;
					break;
			   case spe_ARITH_MINMAX:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_MinMax;
					break;
			   case spe_ARITH_RANDOM:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Random;
					break;
			   case spe_ARITH_EQUALIZE:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Equalize;
					break;
			   case spe_ARITH_BURNER:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Burner;
					break;
			   case spe_ARITH_INTENSITY_B:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_IntensityB;
					break;
			   case spe_ARITH_INTENSITY_A:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_IntensityA;
					break;
			   case spe_ARITH_A_MOD_B:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_A_MOD_B;
					break;
			   case spe_ARITH_AxB:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_AxB;
					break;
			   case spe_ARITH_BURN_LINEAR:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_BurnLinear;
					break;
			   case spe_ARITH_SQRT_AB:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_SqrtAB;
					break;
			   case spe_ARITH_ABS_SUB:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_AbsSub;
					break;
			   case spe_ARITH_EXCLUSION:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_Exclusion;
					break;
			   default:
					spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
					break;
			   }
		   }
		   break;
	case spe_SPREAD_SMUDGE:
		   //TODO: eventually option with stpper - delay
		   // clone sub-self fixed
		   stampRect = GetTipBoundRect(X, Y);
		   if (pushImage)
			  delete pushImage;
		   pushImage = new spImage<ptRGB>(stampRect.Width(), stampRect.Height());
		   _SC->CloneImage = pushImage;
		   _SC->Edit->SetRoi(&stampRect);
		   _SC->CloneImage->CopyRoi(_SC->Edit);
		   _SC->Clone_j = (stampRect.Width()>>1);
		   _SC->Clone_i = (stampRect.Height()>>1);
		   _autoRenderSuggest =  spe_RENDER_SLOW;
		   spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		   _colorArithmetic = &spColorArithmetics::CloneImageFixed;
		   OnShapeStep = stepSmudge;
		   break;
	case spe_SPREAD_SPREAD_MC:
		 {
		 if (SpreadParams->PickCustomPrim)
			_SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[SpreadParams->CustomPrimPick.y][SpreadParams->CustomPrimPick.x]; //ptRGB(_SC->Brush->CommonParams->CustomColorPrim);
		 else if (SpreadParams->SpreadColorPickMode == 0 || SpreadParams->SpreadColorPickMode == 1)
			{
			_SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[Y][X];
			}
		 else
			{
			strokeStepper = stepperImage;
			_SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[Y][X];
			_autoRenderSuggest = spe_RENDER_SLOW;
			}
		 _pickMousePrimColor = (SpreadParams->SpreadColorPickMode == 1);
		 spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		 _SC->Brush->ColorParams->CustomColorSec = _SC->Brush->ColorParams->PrimaryColor;
		 switch (SpreadParams->SpreadMultiColorMode)
			{
			case 0:
				_colorArithmetic = &spColorArithmetics::CustomAlphaMixed;
				break;
			case 1:
				_colorArithmetic = &spColorArithmetics::CustomSolidMixed;
				break;
			case 2:
				_autoRenderSuggest = spe_RENDER_MEDIUM;
				_colorArithmetic = &spColorArithmetics::CustomGradient;
				strokeStepper = stepperStop;
				break;
			case 3:
				if (SpreadParams->PickCustomSec)
				  _SC->Brush->ColorParams->CustomColorSec = _SC->Edit->Pix[SpreadParams->CustomSecPick.y][SpreadParams->CustomSecPick.x];
				else
				  _pickMouseSecColor = true;
				_autoRenderSuggest = spe_RENDER_MEDIUM;
				_colorArithmetic = &spColorArithmetics::CustomGradient;
				strokeStepper = stepperStop;
                break;
			case 4:
				_colorArithmetic = &spColorArithmetics::CustomAlphaMixed;
				_SC->Brush->ColorParams->CustomColorSec = GlobalColorParams->ShortPalEntry[random(GlobalColorParams->ShortPalCount)];
				break;
			case 5:
				_colorArithmetic = &spColorArithmetics::CustomSolidMixed;
				_SC->Brush->ColorParams->CustomColorSec = GlobalColorParams->ShortPalEntry[random(GlobalColorParams->ShortPalCount)];
				break;
			case 6:
				_colorArithmetic = &spColorArithmetics::AlphaShortPal;
				cpSpreadPickRandom =true;
				break;
			case 7:
				_colorArithmetic = &spColorArithmetics::SolidShortPal;
				cpSpreadPickRandom =true;
				break;
			default:
				_colorArithmetic = &spColorArithmetics::CustomSingleColor;
				break;
			}
		 if (cpSpreadPickRandom)
			{
			_SC->PalEntry = GlobalColorParams->ExtraPalEntry;
			_SC->PalCount = SpreadParams->SpreadMultiColorRandomCount;
			int rX = 0,  rY = 0;
			GlobalColorParams->ExtraPalEntry[0] = _SC->Brush->ColorParams->CustomColorPrim;
			for (int i = 1; i < _SC->PalCount; i++)
				{
				//rX = _SC->Brush->CommonParams->Size - random(2 * _SC->Brush->CommonParams->Size);
				//rY = _SC->Brush->CommonParams->Size - random(2 * _SC->Brush->CommonParams->Size);
				if (SpreadParams->SpreadMultiColorRandomWalker)
				   {
				   rX += random(2);
				   rY += random(2);
				   }
				else
				   {
				   rX = _SC->Brush->CommonParams->Size - random(2 * _SC->Brush->CommonParams->Size);
				   rY = _SC->Brush->CommonParams->Size - random(2 * _SC->Brush->CommonParams->Size);
				   }
				GlobalColorParams->ExtraPalEntry[i] = spCommon::GetWrapPixel(_SC->Edit, X + rX, Y + rY, false);
				}
			}
		 }
		 break;
	case spe_SPREAD_SHUFFLER:
		 if (!shuffler)
			 shuffler = new TShuffler();
		 if (SpreadParams->PickCustomPrim)
			{
			_SC->i_pick = SpreadParams->CustomPrimPick.y;
			_SC->j_pick = SpreadParams->CustomPrimPick.x;
			}
		 else
			{
			 switch (SpreadParams->SpreadColorPickMode)
				{
				case 0:
					//_SC->i_pick = &(_SC->i_down);
					//_SC->j_pick = &(_SC->j_down);
					_SC->i_pick = Y;
					_SC->j_pick = X;
					//_SC->i_pick = &(_SC->Shape->MouseData.Y_down);
					//_SC->j_pick = &(_SC->Shape->MouseData.X_down);
					break;
				case 1:
					//_SC->i_pick = &(_SC->i_move);
					//_SC->j_pick = &(_SC->j_move);
					_SC->i_pick = _SC->Shape->MouseData.Y_move_to;
					_SC->j_pick = _SC->Shape->MouseData.X_move_to;
					break;
				default:
					strokeStepper = stepperImage;
					_autoRenderSuggest = spe_RENDER_SLOW;
					_SC->i_pick = _SC->i_btc;
					_SC->j_pick = _SC->j_btc;
					break;
				}
			}
		 //_colorArithmetic = &spColorArithmetics::CustomSingleColor;
		 spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		 _colorArithmetic = &spColorArithmetics::Shuffle;
		 switch (SpreadParams->ShufflerMode)
			{
			case 0:
				shuffler->PrepareManhattanSimple(SpreadParams->ShufflerDistance, SpreadParams->ShufflerRandomDistance);
				shuffler->Execute = shuffler->ExecuteManhattanSimple;
				break;
			case 1:
				shuffler->PrepareGroup(SpreadParams->ShufflerDistance, SpreadParams->ShufflerGroupSize, SpreadParams->ShufflerRandomDistance);
				shuffler->Execute = shuffler->ExecuteGroupBest;
				break;
			case 2:
				shuffler->PrepareGroup(SpreadParams->ShufflerDistance, SpreadParams->ShufflerGroupSize, SpreadParams->ShufflerRandomDistance);
				shuffler->Execute = shuffler->ExecuteGroupWorst;
				break;
			case 3:
				shuffler->PrepareGroupRange(SpreadParams->ShufflerDistance, SpreadParams->ShufflerGroupSize, SpreadParams->ShufflerRandomDistance
														  ,SpreadParams->ShufflerLuminanceMin, SpreadParams->ShufflerLuminanceMax, SpreadParams->ShufflerSortGroup);
				shuffler->Execute = shuffler->ExecuteLuminaRange;
				break;
			case 4:
				shuffler->PrepareGroupAverage(SpreadParams->ShufflerDistance, SpreadParams->ShufflerGroupSize, SpreadParams->ShufflerRandomDistance);
				shuffler->Execute = shuffler->ExecuteGroupAverage;
				break;
			}
		 _SC->userData =  (void*)(shuffler);
		 break;
	case spe_SPREAD_WARPER:
		{
		_blenderIn = ((_blenderIn * 255) >> spe_SP_SHIFT);
		_blenderOut = ((_blenderOut *255) >> spe_SP_SHIFT);
		_warpTipSize = 2;
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		_colorArithmetic = &spColorArithmetics::CustomSingleColor;
		if (SpreadParams->PickCustomPrim)
		   _SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[SpreadParams->CustomPrimPick.y][SpreadParams->CustomPrimPick.x];
		else
		   _SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[Y][X];
		OnShapeStep = stepWarp;
		 _pickMousePrimColor = (SpreadParams->WarpColorPickMode == 1);
		switch (SpreadParams->WarpColorPickMode)
		  {
		  case 2:
			   _autoRenderSuggest = spe_RENDER_SLOW;
			   strokeStepper = stepperImage;
			   break;
		  case 3:
			   _autoRenderSuggest = spe_RENDER_SLOW;
			   //Brush->ColorParams->SetSchemeIndex(px_CTYPE_MOUSE_DOWN);
			   break;
		  case 4:
			   _autoRenderSuggest = spe_RENDER_SLOW;
			   _colorArithmetic = &spColorArithmetics::SineColor;
			   //Brush->ColorParams->SetSchemeIndex(px_CTYPE_ABSQUE);
			   break;
		  case 5:
			   _autoRenderSuggest = spe_RENDER_SLOW;
			   prepareBrushScheme(_SC->Brush->ColorParams->SchemeIndex, X, Y);
			   //Brush->ColorParams->SetSchemeIndex(px_CTYPE_MOUSE_DOWN);
			   break;
		  default:
			   _autoRenderSuggest = spe_RENDER_SLOW;
			   //_colorArithmetic = &spColorArithmetics::SineColor;
			   //Brush->ColorParams->SetSchemeIndex(px_CTYPE_MOUSE_DOWN);
			   break;
		  }
		_SC->Brush->CalculateAdditionalParams(); // neka ga - obavezno!
		int roiTolAdd = 0, half_tip, brushTolTipSize = _SC->Brush->State->cpSize + _SC->Brush->State->cpROItol;
		for (int i = 0; i < SpreadParams->WarpCount; i++)
			{
			if (SpreadParams->WarpOn[i] && SpreadParams->WarpParam1[i] > _warpTipSize)
			   _warpTipSize = SpreadParams->WarpParam1[i];
			}
		_warpTipSize += 3;  // malo rešpira
		if (_warpTipSize >= brushTolTipSize)
			roiTolAdd = _warpTipSize - brushTolTipSize;
		half_tip = (_warpTipSize>>1);
		//TODO: not like this...call a nice method
		if (_SC->Brush->State->cpROItol < roiTolAdd)
		   _SC->Brush->State->cpROItol += roiTolAdd;
		//TODO: is it the same half_tip before you kill it???
		if (remapper)
		   delete remapper;
		remapper = new ippRemap(half_tip);
		remapper->PatchIn(_SC->ippOrig, _SC->ippEdit, X, Y, _blenderIn);
		}
		break;
	default:
		spColorArithmetics::ArithFunc = &spPixelArithmetics::Pixel_NORM;
		break;
	}
TRenderBasic::Begin(X, Y);
/* not necessary any more
if (Brush->State->cpROItol < roiTolAdd)
	Brush->State->cpROItol += roiTolAdd; */
//_SC->skip_empty_alpha = false;
_SC->skip_empty_alpha = true;
}
//---------------------------------------------------------------------------
// Finish render (finish move) - read new mouse color if SpreadParams->SpreadColorPickMode == 1
//---------------------------------------------------------------------------
void TRenderSpread::FinishRender(void)
{
TRenderBasic::FinishRender();
//if ((SpreadParams->MainMode == spe_SPREAD_SPREAD  || SpreadParams->MainMode == spe_SPREAD_SPREAD_MC)
//	&& SpreadParams->SpreadColorPickMode == 1)
if (_pickMousePrimColor)
   _SC->Brush->ColorParams->CustomColorPrim = _SC->Edit->Pix[_SC->Shape->MouseData.Y_move_to][_SC->Shape->MouseData.X_move_to];
else if (_pickMouseSecColor)
   {
   _SC->Brush->ColorParams->CustomColorSec = _SC->Edit->Pix[_SC->Shape->MouseData.Y_move_to][_SC->Shape->MouseData.X_move_to];
  _pickMouseSecColor = false;
  }
}
//---------------------------------------------------------------------------
// stroke stepper
//---------------------------------------------------------------------------
/*
void TRenderSpread::smudgeStepper(void)
{
if (pushImage)
   {
   int x = _SC->j_btc;
   int y = _SC->i_btc;
   if (x > _SC->WidthM1 || x < 0 || y > _SC->HeightM1 || y < 0 )
	  return;
   TRect stampRect = GetTipBoundRect(x, y);
   _SC->Edit->SetRoi(&stampRect);
   _SC->CloneImage->CopyRoi(_SC->Edit);
   }
}
*/
//---------------------------------------------------------------------------
// on step events
//---------------------------------------------------------------------------
void TRenderSpread::stepSmudge(const int &X, const int &Y)
{
if (X > _SC->WidthM1 || X < 0 || Y > _SC->HeightM1 || Y < 0 )
   return;
TRect stampRect = GetTipBoundRect(X, Y);
if (_blenderIn < spe_SP_NORM)
   {
   _SC->Edit->SetRoi(&stampRect);
   spCommon::BlendImages(_SC->Edit, pushImage, _blenderIn, pushImage);
   }
if (_blenderOut < spe_SP_NORM)
   {
   _SC->Orig->SetRoi(&stampRect);
   spCommon::BlendImages(_SC->Orig, pushImage, _blenderOut, pushImage);
   }
}
//---------------------------------------------------------------------------
void TRenderSpread::stepWarp(const int &X, const int &Y)
{
if (X > _SC->WidthM1 || X < 0 || Y > _SC->HeightM1 || Y < 0 )
   return;
int half_tip = (_warpTipSize>>1);
_tipRect = GetBoundRectClear(X - half_tip, Y - half_tip, X + half_tip, Y+ half_tip);
//if (_blenderIn < 255)
//   remapper->BlendSrcDst(_SC->ippEdit, _SC->ippOrig, X, Y, (Ipp8u)(255 - _blenderIn));
remapper->PatchIn(_SC->ippOrig, _SC->ippEdit, X, Y, _blenderIn);
remapper->Prepare(SpreadParams);
remapper->Apply();
if (SpreadParams->WarpMaskHard)
   remapper->PatchOutHard(_SC->ippOrig, X, Y, _SC->ippMaskRight);
else if (SpreadParams->WarpMaskSoft)
   remapper->PatchOutSoft(_SC->ippOrig, X, Y, _SC->ippMaskRight);
else
   remapper->PatchOut(_SC->ippOrig, X, Y, (Ipp8u)_blenderOut);
}




