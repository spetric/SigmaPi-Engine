//---------------------------------------------------------------------------
#ifndef renderConvolveH
#define renderConvolveH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderConvolve : public TRenderBasic
{
  private:
	bool renderIppFilterCoords(const int &X1, const int &Y1, const int &X2, const int &Y2);
	bool renderIppFilterRect(const TRect &roiRects);
  protected:
  public:
	Tspe_ConvolutionIppParams *ConvolveParams;
	TRenderConvolve(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderConvolve(void);
    virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
};
//---------------------------------------------------------------------------
#endif
