//---------------------------------------------------------------------------
#pragma hdrstop
#include "grapBasic.h"
#include "spGeometry.hpp"
//#include "pxArithmeticFunctions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TGrapBasic::TGrapBasic(void)
{
structPtChunk *cItem = new structPtChunk;
cItem->Count = 0;
chunkList.push_back(cItem);
chunkPosition = 0;
PointBmp = 0;
CountBmp = 0;
PointRaw = 0;
CountRaw = 0;
_Step = 0;
FFilled = false;
FBorder = true;
FClosed = false;
//FFillShape = fillNone;
_mouseData.MouseButtonDown = false;
}
//---------------------------------------------------------------------------
// destructor
TGrapBasic::~TGrapBasic(void)
{
clearChunks(chunkList, false);	// complete clear
if (PointBmp)
   delete[] PointBmp;
if (PointRaw)
   delete[] PointRaw;
//
}
//---------------------------------------------------------------------------
UnicodeString TGrapBasic::ClassName(void)
{
return "TGrapBasic";
}
//---------------------------------------------------------------------------
TGrapBasic* TGrapBasic::Clone(void)
{
return 0;
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed)
{
//
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::DrawPolyVect(std::vector<TPoint> &polyList)
{
//
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
bool TGrapBasic::DrawBound(TRect *boundRect)
{
//
return false;
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::DrawCoords(int X1, int Y1, int X2, int Y2)
{
//
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::Rotate(TRect *boundRect, TPoint *origin, double angle)
{
structPtChunk *cItem;
double radians = angle *  M_PI/180.0;
int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
for (size_t i = 0; i < chunkList.size(); i++)
	{
	cItem = chunkList[i];
	//rotate points
	spGeometry::RotatePoints(cItem->Point, cItem->Count, origin, radians);
	for (int k = 0; k < cItem->Count; k++)
		{
		if (cItem->Point[k].x < minX)
		   minX = cItem->Point[k].x;
		if (cItem->Point[k].y < minY)
		   minY = cItem->Point[k].y;
		if (cItem->Point[k].x > maxX)
		   maxX = cItem->Point[k].x;
		if (cItem->Point[k].y > maxY)
		   maxY = cItem->Point[k].y;
		}
	}
boundRect->left = minX;
boundRect->top  = minY;
boundRect->right = maxX;
boundRect->bottom = maxY;
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::GetStepPoints(int lineStep)
{
int ptCount = 0;
int chunkSize = getOverallChunkSize(chunkList);
if (PointBmp)
   delete[] PointBmp;
PointBmp = new TPoint[chunkSize];
// collect chunks in one array
structPtChunk *cItem;
for (size_t i = 0; i < chunkList.size(); i++)
	{
	cItem = chunkList[i];
	for (int j = 0; j < cItem->Count; j++)
	   {
	   if (++_Step < lineStep)
		  continue;
	   PointBmp[ptCount].x   = cItem->Point[j].x;
	   PointBmp[ptCount++].y = cItem->Point[j].y;
	   _Step = 0;
	   }
	}
CountBmp = ptCount;
}
//---------------------------------------------------------------------------
// public method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::GetRawPoints(void)
{
int ptCount = 0;
int chunkSize = getOverallChunkSize(chunkList);
if (PointRaw)
   delete[] PointRaw;
PointRaw = new TPoint[chunkSize];
// collect chunks in one array
structPtChunk *cItem;
for (size_t i = 0; i < chunkList.size(); i++)
	{
	cItem = chunkList[i];
	for (int j = 0; j < cItem->Count; j++)
		PointRaw[ptCount++] = cItem->Point[j];
	}
CountRaw = ptCount;
}
//---------------------------------------------------------------------------
void TGrapBasic::SetSimplifyRule(bool simplify, int dpTolerance, int maxPts)
{
// virutal
}
//---------------------------------------------------------------------------
void TGrapBasic::GetSimplifiedPoints(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts)
{
// virtual
}
//---------------------------------------------------------------------------
void TGrapBasic::GetSplinePoints(TPoint *polyPts, int &polyCount, bool closed)
{
// virtual
}
//---------------------------------------------------------------------------
// protected method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::ResetStepper(int initStep)
{
_Step = initStep;
}
//---------------------------------------------------------------------------
// protected method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::Reset(void)
{
clearChunks(chunkList);
}
//---------------------------------------------------------------------------
// protected method
//---------------------------------------------------------------------------
int TGrapBasic::getOverallChunkSize(std::vector<structPtChunk*> &cList)
{
int oSize = 0;
// calculate memory
structPtChunk *cItem;
for (size_t i = 0; i < cList.size(); i++)
	{
	cItem = cList[i];
	oSize += cItem->Count;
	}
return oSize;
}
//---------------------------------------------------------------------------
// protected method - virtual
//---------------------------------------------------------------------------
void TGrapBasic::savePoint(std::vector<structPtChunk*> &cList, size_t &cPosition, int X, int Y)
{
// test
//int si = cList.size();
if (cList[cPosition]->Count == GP_CHUNK_MAX) // we need a new chunk
   {
   structPtChunk *cItem = new structPtChunk;
   cItem->Count = 0;
   chunkList.push_back(cItem);
   cPosition++;
   }
int itemCount = cList[cPosition]->Count;
cList[cPosition]->Point[itemCount].x   = X;
cList[cPosition]->Point[itemCount].y = Y;
cList[cPosition]->Count++;
}
//---------------------------------------------------------------------------
// protected method
//---------------------------------------------------------------------------
void TGrapBasic::savePoint(int X, int Y)
{
savePoint(chunkList, chunkPosition, X, Y);
}
//---------------------------------------------------------------------------
// protected method
//---------------------------------------------------------------------------
void TGrapBasic::clearChunks(std::vector<structPtChunk*> &cList, bool resetOnly)
{
structPtChunk *cItem = cList[0];
cItem->Count = 0;
for (size_t i = 1; i < cList.size(); i++)
	{
	cItem = cList[i];
	delete cItem;
	cItem = 0;
	}
cList.clear();
if (resetOnly)
   cList.push_back(cItem);
else
   {
   cItem = cList[0];
   delete cItem;
   cItem = 0;
   }
}
//---------------------------------------------------------------------------
// public - base class
//---------------------------------------------------------------------------
bool TGrapBasic::MouseDown(TShiftState Shift, int X, int Y)
{
// save mouse data
_mouseData.X_down = X;
_mouseData.Y_down = Y;
_mouseData.X_up = X;
_mouseData.Y_up = Y;
_mouseData.X_move_to = _mouseData.X_down;
_mouseData.Y_move_to = _mouseData.Y_down;
_mouseData.X_min = _mouseData.X_down;
_mouseData.Y_min = _mouseData.Y_down;
_mouseData.X_max = _mouseData.X_down;
_mouseData.Y_max = _mouseData.Y_down;
_mouseData.MouseButtonDown = true;
_mouseData.Shift = Shift;
Reset();
return _mouseData.MouseButtonDown;
}
//---------------------------------------------------------------------------
bool TGrapBasic::MouseMove(TShiftState Shift, int X, int Y)
{
if (!_mouseData.MouseButtonDown)
   return false;
_mouseData.Shift = Shift;
_mouseData.X_move_from = _mouseData.X_move_to;
_mouseData.Y_move_from = _mouseData.Y_move_to;
_mouseData.X_move_to   = X;
_mouseData.Y_move_to   = Y;
if (X < _mouseData.X_min)
   _mouseData.X_min = X;
if (Y < _mouseData.Y_min)
   _mouseData.Y_min = Y;
if (X > _mouseData.X_max)
   _mouseData.X_max = X;
if (Y > _mouseData.Y_max)
   _mouseData.Y_max = Y;
if (_mouseData.X_move_from == _mouseData.X_move_to && _mouseData.Y_move_from == _mouseData.Y_move_to)    //???
   return false;   // do not update viewer
return true;
}
//---------------------------------------------------------------------------
bool TGrapBasic::MouseUp(TShiftState Shift, int X, int Y)
{
if (!_mouseData.MouseButtonDown)
   return false;
_mouseData.Shift = Shift;
_mouseData.X_up = X;
_mouseData.Y_up = Y;
_mouseData.MouseButtonDown = false;
return true;
}
//---------------------------------------------------------------------------

