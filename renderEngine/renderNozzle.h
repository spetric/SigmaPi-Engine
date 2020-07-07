//---------------------------------------------------------------------------
#ifndef renderNozzleH
#define renderNozzleH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderNozzle : public TRenderBasic
{
  private:
	int saveBrushStep, saveBrushSize;
	int nozzOffset, cellsCount, nozzWidth, nozzHeight, nozzX, nozzY;
	void nozzleImageOut(const int &X, const int &Y);
  protected:
	void stepNozzle(const int &X, const int &Y);
	void renderBrushTip(const int &X1, const int &Y1, const int &X2, const int &Y2);
  public:
	Tspe_NozzleParams *NozzleParams;
	TRenderNozzle(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderNozzle(void);
	virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
	void  FinishRender(void);
};
#endif
