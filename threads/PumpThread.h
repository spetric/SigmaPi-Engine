//---------------------------------------------------------------------------
#ifndef PumpThreadH
#define PumpThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "spSurface.hpp"
//---------------------------------------------------------------------------
class TPumpThread : public TThread
{
protected:
	std::vector<spImage<ptGray>*>  *_pumpOriginal;
	std::vector<spImage<ptGray>*>* _pumpResized;
    std::vector<bool> *_pumpReady;
	int _brushsize, _index;
	void __fastcall ResizePumpImage(int k);
	void __fastcall Execute();
public:
	__fastcall TPumpThread(bool CreateSuspended, std::vector<spImage<ptGray>*>  *pumpOriginal,
												 std::vector<spImage<ptGray>*>  *pumpResized,
												 std::vector<bool> *pumpReady,
												 int brushsize, int index);
	bool ExceptionOccured;
	Exception *ExceptionReference;
};
//---------------------------------------------------------------------------
#endif
