//---------------------------------------------------------------------------
#ifndef brushMeshH
#define brushMeshH
#include "brushBasic.h"
//---------------------------------------------------------------------------
class TBrushMesh: public TBrushBasic
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
   TBrushMesh(void);
   virtual ~TBrushMesh(void);
   void setDefaultSpecificParams(void);
   TBrushBasic* Clone(void);
   UnicodeString ClassName(void);
   void Begin(TGrapBasic *grapShape);
   void DrawShape(TRect *boundRect);
   void DrawTip(TRect *boundRect, int lp);
};
//---------------------------------------------------------------------------
#endif

