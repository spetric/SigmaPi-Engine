//---------------------------------------------------------------------------
#ifndef brushStarH
#define brushStarH
#include "brushBasic.h"
//---------------------------------------------------------------------------
class TBrushStar: public TBrushBasic
{
  private:
     int pointsNum;
  public:
     struct structSpecificParams
       {
       int Thickness;
       int IntVary;
       int Density;
       };
   structSpecificParams *SpecificParams;
   TBrushStar(void);
   virtual ~TBrushStar(void);
   void setDefaultSpecificParams(void);
   UnicodeString ClassName(void);
   TBrushBasic* Clone(void);
   void Begin(TGrapBasic *grapShape);
   void DrawShape(TRect *boundRect);
   void DrawTip(TRect *boundRect, int lp);
};
//---------------------------------------------------------------------------
#endif

