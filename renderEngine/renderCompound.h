//---------------------------------------------------------------------------
#ifndef renderCompoundH
#define renderCompoundH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderCompound : public TRenderBasic
{
  private:
	bool renderIppFilterCoords(const int &X1, const int &Y1, const int &X2, const int &Y2);
	bool renderIppFilterRect(const TRect &roiRects);
  protected:
  public:
	Tspe_CompoundIppParams *CompoundIppParams;
	TRenderCompound(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderCompound(void);
    virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
};
//---------------------------------------------------------------------------
#endif
