//---------------------------------------------------------------------------
#ifndef renderMultiDrawH
#define renderMultiDrawH
#include "renderDraw.h"
//---------------------------------------------------------------------------
class TRenderMultiDraw : public TRenderDraw
{
  protected:
	TPoint _origin;
	TBrushBasic *_saveBrush;
	TGrapBasic *_saveShape;
  public:
	TRenderMultiDraw(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderMultiDraw(void);
	virtual UnicodeString ClassName(void);
	//
    TList *MultiStroke;
	//
	void  Begin(int X, int Y);
	TRect End(const int &X1, const int &Y1, const int &X2, const int &Y2);
};
#endif
