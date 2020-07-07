//---------------------------------------------------------------------------
#ifndef renderShiftH
#define renderShiftH
#include "renderBasic.h"
//---------------------------------------------------------------------------
class TRenderShift : public TRenderBasic
  {
  private:
  protected:
  public:
	Tspe_ShiftParams *ShiftParams;
	TRenderShift(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderShift(void);
	virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
  };
//---------------------------------------------------------------------------
#endif

