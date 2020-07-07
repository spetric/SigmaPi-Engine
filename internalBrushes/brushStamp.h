//---------------------------------------------------------------------------
#ifndef brushStampH
#define brushStampH
#include "brushBasic.h"
//---------------------------------------------------------------------------
class TBrushStamp: public TBrushBasic
{
  private:
	 typedef bool (__closure *TRotateStamp)(void);
	 TRect _boundRect;
	 spImage<ptGray> *FStampImage;
	 spImage<ptGray> *FStampResized;
	 double FAngleCumul;
	 TPoint FStrokePt;
     bool FStrokeStart;
     int  FLpStart, FLp;
	 bool FStampOwner;
	 bool rotateNone(void);
	 bool rotateJitter(void);
	 bool rotateCumulPlus(void);
	 bool rotateCumulMinus(void);
     bool rotateStroke(void);
	 TRotateStamp rotateStamp;
  public:
	 struct structSpecificParams
	   {
	   int  AngleMode;
       int  AngleValue;
	   unsigned long StampId;
	   };
   structSpecificParams *SpecificParams;
   TBrushStamp(void);
   virtual ~TBrushStamp(void);
   UnicodeString ClassName(void);
   TBrushBasic* Clone(void);
   bool SetStampImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
   bool DeleteStampImage(void);
   void Begin(TGrapBasic *grapShape);
   void DrawShape(TRect *boundRect);
   void DrawTip(TRect *boundRect, int lp);
};
//---------------------------------------------------------------------------
#endif
