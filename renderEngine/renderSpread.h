//---------------------------------------------------------------------------
#ifndef renderSpreadH
#define renderSpreadH
#include "renderDraw.h"
#include "spShuffler.h"
#include "ippRemap.h"
//---------------------------------------------------------------------------
class TRenderSpread : public TRenderDraw
{
  private:
	int _blenderIn;
	int _blenderOut;
	bool cpSpreadPickRandom;
	int  _warpTipSize;
	TRect _tipRect;
    bool _pickMousePrimColor, _pickMouseSecColor;
  protected:
	spImage <ptRGB>* pushImage;
    spImage <ptRGB>* pickImage;
	TShuffler *shuffler;
	ippRemap *remapper;
	void stepSmudge(const int &X, const int &Y);
	void stepWarp(const int &X, const int &Y);
	//void smudgeStepper(void); // we don't need it anymore
  public:
	Tspe_SpreadParams *SpreadParams;
	TRenderSpread(spSurfaceContainer *surfaceContainer, spRenderDataKeeper *renderData);
	virtual ~TRenderSpread(void);
	virtual UnicodeString ClassName(void);
	void  Begin(int X, int Y);
	void  FinishRender(void);
	//void __fastcall PolyInterior(TFillShape shapeType, int X1, int Y1, int X2, int Y2);
};

#endif
