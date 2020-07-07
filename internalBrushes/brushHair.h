//---------------------------------------------------------------------------
#ifndef brushHairH
#define brushHairH
#include "brushBasic.h"
//---------------------------------------------------------------------------
class TBrushHair: public TBrushBasic
{
  private:
     TRect _boundRect;
  public:
     struct structSpecificParams
       {
       int Thickness;
       int IntVary;
	   int Density;
       int BlurRadius;
       };
   structSpecificParams *SpecificParams;
   TBrushHair(void);
   virtual ~TBrushHair(void);
   void  setDefaultSpecificParams(void);
   UnicodeString ClassName(void);
   TBrushBasic* Clone(void);
   void Begin(TGrapBasic *grapShape);
};
//---------------------------------------------------------------------------
#endif

