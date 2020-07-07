//---------------------------------------------------------------------------
#ifndef renderColorsH
#define renderColorsH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderColors : public TRenderBasic
{
  private:
	 double palLuminance[256];
	 int    palIndex[256];
  protected:
  public:
	Tspe_ColorParams *ColorParams;
	TRenderColors(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderColors(void);
	virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
};
//---------------------------------------------------------------------------
#endif

