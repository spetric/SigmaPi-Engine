//---------------------------------------------------------------------------
#ifndef brushBasicH
#define brushBasicH
#include "grapBasic.h"
#include "spSurface.hpp"
#include "spDraw.hpp"
#include "spCommon.h"
#include "spGlobals.h"
//---------------------------------------------------------------------------
class TBrushColors
{
private:
   ptRGB FPrimaryColor;
   ptRGB FSecondaryColor;
   ptRGB FPolyColor;
   bool FTextureOwner;
   bool FBackgroundTileOwner;
   bool FPolyTextureOwner;
   spImage<ptRGB> *FTexture;
   spImage<ptGray> *FBackgroundTile;
   spImage<ptRGB> *FPolyTexture;
   //todo: clone image data
   TspeColorScheme FSchemeIndex;
   ptRGB getPrimaryColor(void) {return FPrimaryColor; }
   ptRGB getSecondaryColor(void) {return FSecondaryColor; }
   ptRGB getPolyColor(void) { return FPolyColor; }
protected:
public:
   TBrushColors(void)
     {
	 FTextureOwner = true;
	 FBackgroundTileOwner = true;
	 FPolyTextureOwner = true;
	 FTexture = 0;
	 FBackgroundTile = 0;
	 FPolyTexture = 0;
	 PalEntry = new ptRGB[256];
	 SetDefaultColors();
	 }
   virtual ~TBrushColors(void)
	 {
	 delete []PalEntry;
	 if (FTexture && FTextureOwner)
		delete FTexture;
	 if (FBackgroundTile && FBackgroundTileOwner)
		delete FBackgroundTile;
	 if (FPolyTexture && FPolyTextureOwner)
		delete FPolyTexture;
	 }
   // properties - getters
   __property ptRGB PrimaryColor = {read = getPrimaryColor};
   __property ptRGB SecondaryColor = {read = getSecondaryColor};
   __property ptRGB PolyColor = {read = getPolyColor};
   __property spImage<ptRGB> *Texture = {read = FTexture};
   __property spImage<ptGray> *BackgroundTile = {read = FBackgroundTile};
   __property spImage<ptRGB> *PolyTexture = {read = FPolyTexture};
   __property TspeColorScheme SchemeIndex = {read = FSchemeIndex};
   //
   // setters
   void SetPrimaryColor(ptRGB val)   { FPrimaryColor = val; }
   void SetSecondaryColor(ptRGB val) { FSecondaryColor = val; }
   void SetPolyColor(ptRGB val) { FPolyColor = val; }
   // setters
   void SetPrimaryColor(Byte red, Byte green, Byte blue)
	  {
	  FPrimaryColor.ch[0] = blue;
	  FPrimaryColor.ch[1] = green;
	  FPrimaryColor.ch[2] = red;
	  }
   void SetSecondaryColor(Byte red, Byte green, Byte blue)
	  {
	  FSecondaryColor.ch[0] = blue;
	  FSecondaryColor.ch[1] = green;
	  FSecondaryColor.ch[2] = red;
	  }
   void SetPolyColor(BYTE red, BYTE green, BYTE blue)
	  {
	  FPolyColor.ch[0] = blue;
	  FPolyColor.ch[1] = green;
	  FPolyColor.ch[2] = red;
	  }
   void SetSchemeIndex(TspeColorScheme index) { FSchemeIndex = index; }
   void SetPalette3C(unsigned int botC, unsigned int midC, unsigned int topC)
	  {
      spCommon::Palette3C(PalEntry, botC, midC, topC);
	  };
   void SetPaletteShortPalC(unsigned int *shortPal, int shortPalCount)
	  {
	  spCommon::PaletteShortPalC(PalEntry, shortPal, shortPalCount);
	  };
   void SetDefaultColors(void)
      {
	  SetPrimaryColor(0xff, 0xff, 0xff);
	  SetSecondaryColor(0x00, 0x00, 0x00);
	  SetPolyColor(0x00, 0xff, 0x00);
	  if (FTexture && FTextureOwner)
		  delete FTexture;
	  if (FBackgroundTile && FBackgroundTileOwner)
		  delete FBackgroundTile;
	  if (FPolyTexture && FPolyTextureOwner)
		  delete FPolyTexture;
	  FTexture = 0;
	  FBackgroundTile = 0;
	  FPolyTexture = 0;
	  // some shitty palette
	  unsigned int botC = 0x00ff0000;
	  unsigned int midC = 0x0000ff00;
	  unsigned int topC = 0x000000ff;
	  spCommon::Palette3C(PalEntry, botC, midC, topC);
	  }
   void ClearTexture(void)
	  {
	  if (FTexture && FTextureOwner)
		 {
		 delete FTexture;
		 FTexture = 0;
		 }
	  }
   bool SetTexture(void *buffer, int width, int height, unsigned int scanlineAlignement)
	  {
	  FTextureOwner = true;
	  ClearTexture();
	  FTexture = new spImage<ptRGB>(buffer, width, height, scanlineAlignement, false, orInvert);
	  return (bool)FTexture;
	  }
   void ClearBackgroundTile(void)
	  {
	  if (FBackgroundTile && FBackgroundTileOwner)
		 {
		 delete FBackgroundTile;
		 FBackgroundTile = 0;
		 }
	  }
   bool SetBackgroundTile(void *buffer, int width, int height, unsigned int scanlineAlignement)
	  {
	  FBackgroundTileOwner = true;
	  ClearBackgroundTile();
	  FBackgroundTile = new spImage<ptGray>(buffer, width, height, scanlineAlignement, false, orInvert);
	  return (bool)FBackgroundTile;
	  }
   void ClearPolyTexture(void)
	  {
	  if (FPolyTexture && FPolyTextureOwner)
		 {
		 delete FPolyTexture;
		 FPolyTexture = 0;
		 }
	  }
   bool SetPolyTexture(void *buffer, int width, int height, unsigned int scanlineAlignement)
	  {
	  FPolyTextureOwner = true;
	  ClearPolyTexture();
	  FPolyTexture = new spImage<ptRGB>(buffer, width, height, scanlineAlignement, false, orInvert);
	  return (bool)FPolyTexture;
	  }
   void ShareTexture(spImage<ptRGB> *img) // share texture
	  {
	  FTextureOwner = false;
	  FTexture = img;
	  }
   void ShareBackgroundTile(spImage<ptGray> *img) // share bck tile
	  {
	  FBackgroundTileOwner = false;
	  FBackgroundTile = img;
	  }
   void SharePolyTexture(spImage <ptRGB> *img) // share bck tile
	  {
	  FPolyTextureOwner = false;
	  FPolyTexture = img;
	  }
   // public variables
   ptRGB WorkPrimary, WorkSecondary, CustomColorPrim, CustomColorSec;
   ptRGB *PalEntry;
};
//---------------------------------------------------------------------------
class TBrushBasic
{
private:
	int _saveLineParamsCount;
	void *_filterFunction;
protected:
	spSurface<ptGray> *_surfLeft, *_surfRight;
	TGrapBasic *_grapShape;
	int _beginHalfSize;
	bool _resizedOnTheFly;
	int _brushTipTol;
	TRect _brushTipRect;
	bool _invertStamp;
	spSurface<ptGray> *_stampWork, *_stampOriginal;
	int *_sizeEnvelope, *_capacityEnvelope;
    UnicodeString _sizeEnvelopeString, _capacityEnvelopeString;
	void endInternal(bool freeHand);
   // static
   static TRect GetTipBoundRectClear(int X, int Y, int half_size, int width, int height);
   static void CopyStampToMask(spImage<ptGray> *stamp, spImage<ptGray> *destMask, TRect *boundRect);
   static int ParseEnvelope(UnicodeString input, UnicodeString delim, std::vector<int> &output);
public:
   enum TFilterType {flBlur, flMedian, flClose};
   TBrushBasic(void);
   virtual ~TBrushBasic(void);
   virtual void Connect(spSurface<ptGray> *surfLeft, spSurface<ptGray> *surfRight, TGrapBasic *grapShape);
   virtual UnicodeString ClassName(void);
   //
   /*
   struct structCommonParams
	   {
       // brush index (inside list)
       int ListIndex;
	   // brush tip
       int Size;
       int Capacity;
       int ColorVary;
       int SizeVary;
       int CapVary;
       int Step;
       // filters
       bool ChalkOn;
	   bool InvertOn;
       int  InvertValue;
       bool  CloseOn;
	   int CloseValue;
       bool BlurOn;
       int BlurValue;
       bool MedianOn;
       int MedianValue;
       // bump
	   bool BumpOn;
	   bool BumpSoft;
       float BumpScaleValue;
       int  BumpThresholdValue;
       int  BumpRoughnessValue;
       int BumpXoffsetValue;
       int BumpYoffsetValue;
       bool BumpInvertThreshold;
	   // envelopes
	   bool EnvelopeSizeOn;
	   bool EnvelopeCapacityOn;
	   bool EnvelopeSizeRepeat;
	   bool EnvelopeCapacityRepeat;
	   int  EnvelopeResolution;
	   bool EnvelopeResolutionManual;
	   // jitter
       bool JitterOn;
       bool JitterConnectedOn;
       int JitterRangeValue;
       int JitterLoopValue;
	   // polygon
       bool PolyFillOver;
       bool PolyExcludeFilters;
       bool PolyExcludeBump;
       int  PolyColorIndex;
       bool PolyOverrideCapacityOn;
       int  PolyCapacityValue;
       bool PolyShrinkOn;
       int  PolyShrinkValue;
       bool PolyBlurOn;
       int  PolyBlurValue;
	   // scratches
       int ScratchType;
       int ScratchThicknessValue;
	   int ScratchNumberValue;
	   // pal-grad stepper
	   float PalGradStepper;
	   bool  PalGradAuto;
	   };
   */
   struct structStateParams
	   {
	   // aditional params (work params) - calculated
	   int    cpSize;
	   int    cpHalfSize;
	   float  cpHalfFloat;
	   float  cpHalfFloatSquared;
	   int    cpCapacity;
	   int    cpNormCapacity;
	   int    cpBumpMaskPerc;
	   float  cpBumpLength;
	   int    cpSizeVary;
	   int    cpCapVary;
	   int    cpROItol;
	   // envelope
	   int    cpEnvelopeSizeCount;
	   int    cpEnvelopeSizeFactorCount;
	   int    cpEnvelopeSizeFactorPos;
	   float  *cpEnvelopeSizeFactor;
	   float  cpEnvelopeSizeStep;
	   float  cpEnvelopeSizeStepIncr;
	   int    cpEnvelopeCapacityCount;
	   int    cpEnvelopeCapacityFactorPos;
	   int    cpEnvelopeCapacityFactorCount;
	   float  *cpEnvelopeCapacityFactor;
	   float  cpEnvelopeCapacityStep;
	   float  cpEnvelopeCapacityStepIncr;
	   // steppers
       float  cpStepperMax;
	   float  cpRealStepper;
	   //
	   float  wpRealStepper;
	   float  wpRealStepCount;
	   int    wpStepCount;
	   // for on the flay size change
       int cpHalfSizeMax;
	   };
   struct structLineParams
       {
       TPoint *Point;
       int Count;
	   };
   // properties
   //structCommonParams *CommonParams;
   Tspe_BrushCommonParams *CommonParams;
   structStateParams *State;
   structLineParams *LineParams;
   // za sada neka stoji ovako---ionako ga samo ja koristim
   TBrushColors *ColorParams;
   // public methods
   void CopyParams(TBrushBasic *brush);
   void SetDefaultCommonParams(void);
   void SetSizeEnvelope(wchar_t *str);
   void SetCapacityEnvelope(wchar_t *str);
   void CalculateVariations(void);
   void CalculateSizeEnvelope(void);
   void CalculateCapacityEnvelope(void);
   void CalculateAdditionalParams(int size, int capacity);
   void CalculateAdditionalParams(void);
   virtual TBrushBasic* Clone(void);
   virtual void Begin(TGrapBasic *grapShape);
   virtual void End(int param);
   virtual void DrawShape(TRect *boundRect);
   virtual void DrawTip(TRect *boundRect, int lp);
   void DrawVariations(void);
   void ResizeStamper(int half_size, bool invert);
   void FillPoly(void);
   void FillPoly(TPoint *polyPoint, int polyCount);
   void FillRectangle(int x1, int y1, int x2, int y2);
   void FillEllipse(int x1, int y1, int x2, int y2);
   void ApplyScratching(TRect *boundRect);
   void ApplyFilters(TRect *boundRect);
   void ApplyBlur(TRect *boundRect, int kernelValue);
   void ApplyMedian(TRect *boundRect, int kernelValue);
   void ApplyShrink(TRect *boundRect, int kernelValue);
   void JitterLinePoints(void);
   void ResetLinePoints(void);
   void AddLinePoints(int x1, int y1, int x2, int y2);
   void PreparePoints(bool freeHand);
   void RestoreLinePoints(void);
};
//---------------------------------------------------------------------------
#endif
