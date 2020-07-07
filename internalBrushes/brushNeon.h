//---------------------------------------------------------------------------
#ifndef brushNeonH
#define brushNeonH
#include "brushBasic.h"
//---------------------------------------------------------------------------
class TBrushNeon: public TBrushBasic
{
  private:
  public:
     struct structSpecificParams
       {
       int FalloffIndex;
       int LinearValue;
       int TrigonometricValue;
       };
   structSpecificParams *SpecificParams;
   TBrushNeon(void);
   virtual ~TBrushNeon(void);
   UnicodeString ClassName(void);
   void setDefaultSpecificParams(void);
   TBrushBasic* Clone(void);
   void Begin(TGrapBasic *grapShape);
   double BrushArith(double x_var, double half_fact, double cor_fact, double add_const);
};
//---------------------------------------------------------------------------
#endif
 