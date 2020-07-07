//---------------------------------------------------------------------------
#include <System.hpp>
#pragma hdrstop
#include "PumpThread.h"
#include "spCommon.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TPumpThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TPumpThread::TPumpThread(bool CreateSuspended, std::vector<spImage<ptGray>*>  *pumpOriginal,
												 std::vector<spImage<ptGray>*>* pumpResized,
												 std::vector<bool> *pumpReady,
												 int brushsize, int index)
	: TThread(CreateSuspended)
{
_pumpOriginal = pumpOriginal;
_pumpResized = pumpResized;
_pumpReady = pumpReady;
_brushsize = brushsize;
_index = index;
}
//---------------------------------------------------------------------------
void __fastcall TPumpThread::Execute()
{
	NameThreadForDebugging(System::String(L"PumpThread"));
	//---- Place thread code here ----
try {
	if (Terminated)
	   return;
	// resize Pump at index
	ResizePumpImage(_index);
	for (size_t i = 0; i < _pumpOriginal->size(); i++)
		{
		if (i == _index)
		   continue;
		ResizePumpImage(i);
		}
	Terminate();
	}
catch (Exception &Ex) {
	ExceptionOccured = true;
	ExceptionReference = &Ex;
	throw Ex;
	}
}
//---------------------------------------------------------------------------
void __fastcall TPumpThread::ResizePumpImage(int k)
{
if (_pumpResized->at(k))
   {
   if (_pumpResized->at(k)->Width() == _brushsize)
	  {
	  _pumpReady->at(k) = true;
	  return;
	  }
   delete _pumpResized->at(k);
   _pumpResized->at(k) = 0;
   }
_pumpResized->at(k) = new spImage<ptGray>(_brushsize, _brushsize, true, orInvert);
if (_pumpOriginal->at(k)->Width() != _pumpOriginal->at(k)->Height())
   spCommon::ResizeStampMar(_pumpOriginal->at(k), _pumpResized->at(k), _brushsize);
else
   spCommon::ResizeBilinearGray(_pumpOriginal->at(k), _pumpResized->at(k));
_pumpReady->at(k) = true;
}
//---------------------------------------------------------------------------
