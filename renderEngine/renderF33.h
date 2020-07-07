//---------------------------------------------------------------------------
#ifndef renderF33H
#define renderF33H
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderF33 : public TRenderBasic
{
  private:
  protected:
  public:
    Tspe_Filter33Params *Filter33Params;
	TRenderF33(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderF33(void);
	virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
};
//---------------------------------------------------------------------------
#endif
