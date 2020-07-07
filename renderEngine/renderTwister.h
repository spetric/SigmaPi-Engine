//---------------------------------------------------------------------------
#ifndef renderTwisterH
#define renderTwisterH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderTwister : public TRenderBasic
{
  private:
	bool renderColorTwistCoords(const int &X1, const int &Y1, const int &X2, const int &Y2);
	bool renderColorTwistRect(const TRect &roiRect);
  protected:
	Ipp32f _Twister[3][4];
  public:
	Tspe_ColorTwistIppParams *ColorTwistIppParams;
	TRenderTwister(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderTwister(void);
	virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
};
#endif
