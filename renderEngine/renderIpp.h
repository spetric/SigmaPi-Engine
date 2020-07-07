//---------------------------------------------------------------------------
#ifndef renderIppH
#define renderIppH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderFilterIpp : public TRenderBasic
{
  private:
	bool renderIppFilterCoords(const int &X1, const int &Y1, const int &X2, const int &Y2);
	bool renderIppFilterRect(const TRect &roiRects);
  protected:
  public:
	Tspe_FilterIppParams *FilterIppParams;
	TRenderFilterIpp(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderFilterIpp(void);
    virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
};
//---------------------------------------------------------------------------
#endif
