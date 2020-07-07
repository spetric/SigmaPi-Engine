//---------------------------------------------------------------------------
#ifndef brushSolidH
#define brushSolidH
#include "brushBasic.h"
//---------------------------------------------------------------------------
class TBrushSolid: public TBrushBasic
{
  private:
  public:
     struct structSpecificParams
       {
	   int Shape;
	   int Thickness;
	   bool Filled;
	   float Angle;
	   bool RandomAngle;
       int BlurRadius;
       };
   structSpecificParams *SpecificParams;
   //
   TBrushSolid(void);
   virtual ~TBrushSolid(void);
   UnicodeString ClassName(void);
   void setDefaultSpecificParams(void);
   TBrushBasic* Clone(void);
   void Begin(TGrapBasic *grapShape);
};
//---------------------------------------------------------------------------
#endif
 