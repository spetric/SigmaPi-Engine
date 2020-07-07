//---------------------------------------------------------------------------
#ifndef brushSprayH
#define brushSprayH
#include "brushBasic.h"
//---------------------------------------------------------------------------
class TBrushSpray: public TBrushBasic
{
  private:
       int pointsNum; 
  public:
     struct structSpecificParams
       {
       int Thickness;
       int IntVary;
       int Density;
       float Gravity;
       };
   structSpecificParams *SpecificParams;
   TBrushSpray(void);
   virtual ~TBrushSpray(void);
   void setDefaultSpecificParams(void);
   UnicodeString ClassName(void);
   TBrushBasic* Clone(void);
   void Begin(TGrapBasic *grapShape);
   void DrawShape(TRect *boundRect);
   void DrawTip(TRect *boundRect, int lp);
};
//---------------------------------------------------------------------------
#endif
