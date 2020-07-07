//---------------------------------------------------------------------------
#ifndef brushCmlH
#define brushCmlH
#include "brushBasic.h"
#include "grapLine.h"
#include "automataCml.h"
//---------------------------------------------------------------------------
class TBrushCml: public TBrushBasic
{
  private:
	 TGrapLine *grapCmlLine;
	 int starLineRadius;
	 int pointsNum;
	 TAutomataCml *cml;
  public:
	 struct structSpecificParams
	   {
	   int MinIntensity;
	   int MaxIntensity;
	   int SeedIntensity;
	   int LatticeType;
	   int CreationOps[9];
	   int DestructionOps[9];
	   float EpsilonGrow;
	   float EpsilonDecay;
	   int Iterations;
	   int CreateProbability;
	   int DecayProbability;
	   int GrowProbability;
	   bool Reborn;
	   bool MatrixMes;
	   bool DelayCreation;
	   bool RandomSplat;
	   bool SequentialScan;
	   int  GrowFunc;
	   int  DecayFunc;
	   float EpsilonWeight;
	   int CreationMutation;
	   int DestructionMutation;
	   };
   structSpecificParams *SpecificParams;
   TBrushCml(void);
   virtual ~TBrushCml(void);
   void setDefaultSpecificParams(void);
   UnicodeString ClassName(void);
   TBrushBasic* Clone(void);
   void Begin(TGrapBasic *grapShape);
   void DrawShape(TRect *boundRect);
   void DrawTip(TRect *boundRect, int lp);
   void End(int param);
};
//---------------------------------------------------------------------------

#endif
