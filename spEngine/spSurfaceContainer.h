//---------------------------------------------------------------------------
#ifndef spSurfaceContainerH
#define spSurfaceContainerH
#include <vector>
#include <Types.hpp>
#include "spSurface.hpp"
#include "brushBasic.h"
#include "spPixelArithmetics.hpp"
#include "ippTypes.hpp"
#include "ippImage.hpp"
#include "spFatImage.hpp"
//#include "spCommon.h"
enum TMaskSelection {mLeft, mRight, mBoth};
enum TImageSelection {imgOrig, imgEdit};
//---------------------------------------------------------------------------
class spSurfaceContainer
{
private:
   TGrapBasic *FGrapShape;
   TBrushBasic *FBrush;
   void **FScanOrig;
   void **FScanAlphaOrig;
   int FScanLineIndex;
protected:
   unsigned int FScanlineAlignement;
   int FWidth, FHeight, FWidthM1, FHeightM1;
   void *FSuperContainer;
   void setSuperContainer(void *val);
public:
   spSurfaceContainer(void);
   spSurfaceContainer(void *scanOrig, void *scanAlphaOrig, int width, int height, unsigned int scanlineAlignement,
   					  TBrushBasic *brush, TGrapBasic *shape);
   virtual ~spSurfaceContainer(void);
   //
   bool Open(int width, int height);
   bool AddScanLine(void *scanOrig, void *scanAlphaOrig);
   bool Close(void);
   //
   void SetImages(void *scanOrig, void *scanAlphaOrig, void *scanEdit, void *scanAlphaEdit);
   void SetMasks(void *scanLeft, void *scanRight);
   void SetSelection(void *scanSelection);
   void ClearMask(TMaskSelection mSelection, TRect *boundRect);
   void UpdateImage(TImageSelection imgSelection, TRect *boundRect);
   void SetMixSurf(int mode);
   void SetArithSurf(int mode);
   //
   TBrushBasic* SetBrush(TBrushBasic *brush);
   TGrapBasic*  SetShape(TGrapBasic *shape);
   // main surface images
   spSurface<ptRGB> *Orig, *Edit, *MixSurf, *ArithSurf;
   spSurface<ptGray> *MaskLeft, *MaskRight;
   spSurface<ptGray> *AlphaOrig, *AlphaEdit;
   // main ipp images
   ippImage<ippRGB> *ippOrig, *ippEdit;
   ippImage<ippGray> *ippMaskLeft, *ippMaskRight;
   ippImage<ippGray> *ippAlphaOrig, *ippAlphaEdit;
   // auxiliary images surface
   spImage<ptRGB>  *AssSidekick;
   spImage<ptGray> *AssContour, *AssMask, *AssStencil, *AssMarker;
   spImage<ptGray> *Selection;
   unsigned long SurfaceId;
   // color correction data
   double CumulBGR[3], DistanceBGR[3];
   // cumulative bound rect
   TRect BigBoundRect;
   //
   //not any more spCommon::spsPixel *PixData;
   //
   __property int Width = {read = FWidth};
   __property int Height = {read = FHeight};
   __property int WidthM1 = {read = FWidthM1};
   __property int HeightM1 = {read = FHeightM1};
   __property void *SuperContainer = {read = FSuperContainer, write = setSuperContainer};
   __property TGrapBasic *Shape = {read = FGrapShape};
   __property TBrushBasic *Brush = {read = FBrush};
   // ex PixData
	 int i;
	 int j;
	 //int i_down;
	 //int j_down;
	 //int i_move;
	 //int j_move;
	 int i_pick;
	 int j_pick;
	 int PixelVary;
	 int i_start;
	 int j_start;
	 bool update_alpha;
	 bool skip_empty_alpha;
	 Byte external_alpha;
	 int X_shift;
	 int Y_shift;
	 int i_btc;
	 int j_btc;
	 //int stepCount;
	 //float realStepCount;
	 //float realStepper;
	 //float BumpScale;
	 int Randomizator;
	 spImage<ptRGB> *CloneImage;
	 spImage<ptRGB> *CloneSelf;
	 int Clone_j;
	 int Clone_i;
	 bool StencilAlive;
	 int Stencil_i;
	 int Stencil_j;
	 //bool SelClipping;
	 //bool ContourClipping;
	 bool ContourEncountered;
	 // moved to  renderparams
	 //Byte ContourClippingThreshold;
	 //int  ContourClippingRadius;
	 //
	 ptRGB *PalEntry;
     int PalCount;
	 double *PalLuminance;
	 int *PalIndex;
	 Byte **LUTCurveEntry;
	 spFatImage <ptRGB, ippRGB> *FilterImage;  // intermediate image (TODO: list of threaded images) not property of container
	 //spImage<ptRGB> *NozzleImage;
	 //spImage<ptGray> *NozzleAlpha;
	 void *userData;
};
//---------------------------------------------------------------------------
#endif
