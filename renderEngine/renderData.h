//---------------------------------------------------------------------------
#ifndef renderDataKeeperH
#define renderDataKeeperH
#include "spPixelTypes.hpp"
#include "spImage.hpp"
struct spsRenderParams
   {
   bool ClearMask;
   bool HardMask;
   bool UpdateImage;
   bool AddUndo;
   int *RenderingMode;
   bool DoShadow;
   bool DrawPolyInterior;
   };
struct spsSelectionParams
   {
   // selection
   int SelFeather;
   bool SelRestore;
   bool SelClipping;
   bool SelWandMaxFilter;
   int SelWandTolerance;
   int SelWandMode;
   int PasteIntoBlendMode;
   int PasteIntoResampleMode;
   };
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
struct spsDrawParams
   {
   int MainMode;
   int SaveMode;
   int Shape;
   int VectObjPenWidth;
   ptRGB VectObjPenColor;
   ptRGB ScaleTop;
   ptRGB ScaleBottom;
   bool InvertSource;
   int BitShift;
   bool UpdateAlpha;     //TODO: provjeri treba li ovdje uopæe ili samo u pixData
   int  StrokesNumber;      // za undo buffer
   bool EraseMatchAll;
   ptRGB EraseMatchColor;
   int EraseMatchColorTolerance;
   bool EraseCustom;
   ptRGB EraseCustomColor;
   // clone image
   spImage<ptRGB> *CloneImage;
   int Clone_XSS;
   int Clone_YSS;
   int Clone_XS;
   int Clone_YS;
   int Clone_X;
   int Clone_Y;
   bool Clone_Started;
   };
struct spsColorParams
   {
   int  MainMode;
   bool ChanME[3];
   bool CPercentage;
   int  ChanRGB[3];
   float ChanGamma[3];
   bool CastOn[3];
   int CastRange[2];
   ptRGB CastColor[3];
   int ScaleRange[3][2];
   double Temperature;
   bool ToneSelectedColor;
   ptRGB ToneColor;
   bool TonePreserveLuminance;
   //TColorCurve *ColorCurve;
   int CurveIndex;
   //TColorCorrection *ColorCorrection;
   };
struct spsShiftParams
   {
   int  MainMode;
   int X_shift;
   int Y_shift;
   float BumpScale;
   int ScaleRange[3][2];
   int BitShift;
   bool InvertSource;
   int ZeroShiftRandom;
   };
struct spsFilter33Params
   {
   int MainMode;
   bool Wrap;
   bool SyntaxOk;
   bool Chan[3];
   int Mode;
   int Logic[9];
   int LogicSign[9]; 
   int Kernel[9];
   int Bias;
   int Div;
   };
struct spsColorTwistParams
   {
   int MainMode;
   bool SyntaxOk;
   int Scale;
   int Bias;
   double Matrix[16];
   };
struct spsSpreadParams
   {
   int MainMode;
   int SpreadMainMode;
   int SpreadColorPickMode;
   int SpreadBlender;
   int SpreadMultiColorMode;
   int WarpColorPickMode;
   int WarpCount;
   bool WarpOn[5];
   int WarpOrder[5];
   int WarpParam1[5];
   double WarpParam2[5];
   int WarpBlender;
   bool WarpSameRange;
   bool WarpMaskSoft;
   bool WarpMaskHard;
   spImage<ptRGB> *CloneImage;
   //TShuffler *Shuffler;
   int ShufflerMode;
   int ShufflerDistance;
   bool ShufflerRandomDistance;
   int ShufflerGroupSize;
   int ShufflerLuminanceMin;
   int ShufflerLuminanceMax;
   int ShufflerSortGroup;
   int Clone_XS;
   int Clone_YS;
   // calculated params
   bool cpSpreadPickRandom;
   };
struct spsShadowParams
   {
   int MainMode;
   int Xoffset;
   int Yoffset;
   bool BlurOn;
   int BlurValue;
   ptRGB Color;
   int Capacity;
   };
struct spsRenderData
   {
   public:
	 spsRenderParams *RenderParams;
	 spsSelectionParams *SelectionParams;
	 spsAssistantParams *AssistantParams;
	 spsShadowParams *ShadowParams;
	 spsDrawParams *DrawParams;
   spsRenderData(void)
	 {
	 RenderParams = new spsRenderParams;
	 SelectionParams = new spsSelectionParams;
	 AssistantParams = new spsAssistantParams;
	 ShadowParams = new spsShadowParams;
	 DrawParams = new spsDrawParams;
	 }
   virtual ~spsRenderData()
	 {
	 delete RenderParams;
	 delete SelectionParams;
	 delete AssistantParams;
	 delete ShadowParams;
	 delete DrawParams;
	 }
   };   
#endif
//---------------------------------------------------------------------------






