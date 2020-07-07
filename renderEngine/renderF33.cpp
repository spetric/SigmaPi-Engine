//---------------------------------------------------------------------------
#pragma hdrstop
#include "renderF33.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TRenderF33::TRenderF33(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData)
						:TRenderBasic(surfaceContainer, renderData)

{
Filter33Params = renderData->Filter33Params;
}
//---------------------------------------------------------------------------
// destructor
TRenderF33::~TRenderF33(void)
{
}
//---------------------------------------------------------------------------
UnicodeString TRenderF33::ClassName(void)
{
return "TRenderF33";
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TRenderF33::Begin(int X, int Y)
{
int k;
prepareColors();
_autoRenderSuggest =  spe_RENDER_FAST;
//
if (!Filter33Params->SyntaxOk)
   {
   _colorArithmetic = 0;
   spColorArithmetics::FilterFunc = 0;
   }
else
   {
   _SC->CloneImage = 0;
   spColorArithmetics::ArithData.topVal = GlobalColorParams->ScaleTop;
   spColorArithmetics::ArithData.botVal = GlobalColorParams->ScaleBottom;
   _SC->Brush->ColorParams->WorkPrimary = _SC->Brush->ColorParams->PrimaryColor;
   _SC->Brush->ColorParams->WorkSecondary = _SC->Brush->ColorParams->SecondaryColor;
   _colorArithmetic = &spColorArithmetics::Filter33;
   // prepare data
   spColorArithmetics::Filter33Data.wrap = Filter33Params->Wrap;
   for (k = 0; k < 3; k++)
	   spColorArithmetics::Filter33Data.chanOn[k] = Filter33Params->Chan[k];
   spColorArithmetics::Filter33Data.bias = (float)Filter33Params->Bias;
   spColorArithmetics::Filter33Data.div  = (float)Filter33Params->Div;
   for (k = 0; k < 9; k++)
	   {
	   spColorArithmetics::Filter33Data.kernel[k] = Filter33Params->Kernel[k];
	   spColorArithmetics::Filter33Data.logic[k] = Filter33Params->Logic[k];
	   spColorArithmetics::Filter33Data.sign[k] = Filter33Params->LogicSign[k];
	   }
   //
   switch (Filter33Params->Mode)
	   {
	   case 0: // linear
			spColorArithmetics::FilterFunc = &spPixelArithmetics::Filter33_Linear;
			break;
	   case 1: // closest
			spColorArithmetics::FilterFunc = &spPixelArithmetics::Filter33_Closest;
			break;
	   case 2: // difference
			spColorArithmetics::FilterFunc = &spPixelArithmetics::Filter33_Diff;
			break;
	   case 3: // minmax
			spColorArithmetics::FilterFunc = &spPixelArithmetics::Filter33_MinMax;
			break;
	   case 4: // logic
			spColorArithmetics::FilterFunc = &spPixelArithmetics::Filter33_Logic;
			break;
       }
	}
TRenderBasic::Begin(X, Y);
_SC->skip_empty_alpha = true;
}
