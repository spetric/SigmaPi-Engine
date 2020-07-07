//---------------------------------------------------------------------------
#ifndef brushPumpH
#define brushPumpH
#include "brushBasic.h"
#include "PumpThread.h"
//---------------------------------------------------------------------------
class TBrushPump: public TBrushBasic
{
  private:
	 TRect _boundRect;
	 std::vector<spImage<ptGray>*>  FPumpOriginal;
	 std::vector<spImage<ptGray>*>  FPumpResized;
	 std::vector<bool> FPumpReady;
     std::vector<int> FPumpSelected;
	 TPumpThread *_thrPump;
     int  FBrushSize, FIndex, FSelIndex;
	 bool FPumpOwner, FPumpStarted;
	 void changeStamp(void);
	 bool resizePump(void);
	 bool checkPump(void);
     int getSelIndex(int idx);
  public:
	 struct structSpecificParams
	   {
	   int Mode;   			// Pump mode - how to change brush (sequential, random, selected sequential, selected random)
	   int Index;    		// start-currnet brush number - can be changed by host
       bool ResetIndex;     // reset index
	   int Count;           // pump images count
	   };
   structSpecificParams *SpecificParams;
   TBrushPump(void);
   virtual ~TBrushPump(void);
   UnicodeString ClassName(void);
   TBrushBasic* Clone(void);
   bool AddPumpImage(void *buffer, int width, int height, unsigned int scanlineAlignement, bool shared);
   bool SelectPumpImage(int idx);
   bool DeselectPump(void);
   bool DeletePump(void);
   void Begin(TGrapBasic *grapShape);
   void DrawShape(TRect *boundRect);
   void DrawTip(TRect *boundRect, int lp);
};
//---------------------------------------------------------------------------
#endif
