//---------------------------------------------------------------------------
#ifndef renderDrawH
#define renderDrawH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderDraw : public TRenderBasic
{
  private:
  protected:
	void prepareBrushScheme(int schemeIndex, int X, int Y);
  public:
	Tspe_DrawParams *DrawParams;
	TRenderDraw(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderDraw(void);
	virtual UnicodeString ClassName(void);
    void  Begin(int X, int Y);
    //void __fastcall PolyInterior(TFillShape shapeType, int X1, int Y1, int X2, int Y2);
};
//---------------------------------------------------------------------------

#endif


