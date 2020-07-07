//---------------------------------------------------------------------------
#ifndef renderDataKeeperH
#define renderDataKeeperH
//---------------------------------------------------------------------------
#include <Types.hpp>
#include "spPixelTypes.hpp"
#include "spImage.hpp"
#include "spGlobals.h"
//---------------------------------------------------------------------------
class spRenderDataKeeper
  {
  public:
	/*
	struct spsAssistantParams
	   {
	   // global
	   int MainMode;
	   // contour
	   int ContourMode;
	   bool ContourClipping;
	   int  ContourClippingThreshold;
	   int  ContourClippingRadius;
	   // mask
	   bool MaskBlendOnly;
	   bool MaskAutoUpdate;
	   bool MaskLastShape;
	   // sidekick
	   int SidekickBlendMode;
	   int SidekickBlendPercentage;
	   // stencil
	   };
    */
	struct spsGlobalColorParams
	   {
	   ptRGB ScaleTop;
	   ptRGB ScaleBottom;
	   spImage<ptRGB> *CloneImage; 	   // clone image
	   //ptRGB BumpVal;
	   // variables related to clone brush
	   int CloneStartX;
	   int CloneStartY;
	   int Clone_XS;
	   int Clone_YS;
	   int Clone_X;
	   int Clone_Y;
	   bool Clone_Started;
	   //
	   spImage<ptRGB> *NozzleImage;
	   spImage<ptGray> *NozzleAlpha;
	   // color curve LUT entry
	   Byte **LUTCurveEntry;
	   ptRGB ShortPalEntry[16];
	   int ShortPalCount;
	   ptRGB ExtraPalEntry[16];
       int ExtraPalCount;
	   };
  /*
   struct spsVectorialObjectParams
	   {
	   int VectObjPenWidth;
	   ptRGB VectObjPenColor;
	   };
   */
   // pts to structures
   Tspe_RenderParams *RenderParams;
   //spsSelectionParams *SelectionParams;
   //spsAssistantParams *AssistantParams;
   Tspe_DrawParams *DrawParams;
   Tspe_ColorParams *ColorParams;
   Tspe_Filter33Params *Filter33Params;
   Tspe_FilterIppParams *FilterIppParams;
   Tspe_ShiftParams *ShiftParams;
   Tspe_ColorTwistIppParams *ColorTwistIppParams;
   Tspe_CompoundIppParams *CompoundIppParams;
   Tspe_SpreadParams *SpreadParams;
   Tspe_NozzleParams *NozzleParams;
   //
   Tspe_ShadowParams *ShadowParams;
   spsGlobalColorParams *GlobalColorParams;
   // constru-destru
   spRenderDataKeeper(void);
   virtual ~spRenderDataKeeper(void);
   };
#endif
