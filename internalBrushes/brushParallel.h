//---------------------------------------------------------------------------
#ifndef brushParallelH
#define brushParallelH
#include "brushBasic.h"
#include "grapLine.h"
//---------------------------------------------------------------------------
class TBrushParallel: public TBrushBasic
{
  private:
     TGrapLine *grapParaLine;
     TRect _boundRect;
	 int _radius;
     void prepareStamp(TPoint );
  public:
     struct structSpecificParams
	   {
       int Shape;
       int Thickness;
       int IntVary;
       int Spacing;
	   int Angle;
       int BlurRadius;
       };
   structSpecificParams *SpecificParams;
   TBrushParallel(void);
   virtual ~TBrushParallel(void);
   TBrushBasic* Clone(void);
   void setDefaultSpecificParams(void);
   UnicodeString ClassName(void);
   void Begin(TGrapBasic *grapShape);
};
//---------------------------------------------------------------------------
#endif

