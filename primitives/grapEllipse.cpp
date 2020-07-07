//---------------------------------------------------------------------------
#pragma hdrstop
#include "grapEllipse.h"
#include "spGeometry.hpp"
#include "ASMVars.h"
#include <math.h>
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TGrapEllipse::TGrapEllipse(void)
           : TGrapLine()
{
FClosed = true;
//FFillShape = fillEllipse;
structPtChunk *cItem;
for (int i= 0; i < 4; i++)  // four quadrants
	{
	cItem = new structPtChunk;
	cItem->Count = 0;
	chunkList[i].push_back(cItem);
	chunkPosition[i] = 0;
	}
isPoly = false;
}
//---------------------------------------------------------------------------
// destructor
TGrapEllipse::~TGrapEllipse(void)
{
for (int i = 0; i < 4; i++) // four quadrants
	clearChunks(chunkList[i], false);
}
//---------------------------------------------------------------------------
UnicodeString TGrapEllipse::ClassName(void)
{
return "TGrapEllipse";
}
//---------------------------------------------------------------------------
TGrapBasic* TGrapEllipse::Clone(void)
{
TGrapEllipse *shape = new TGrapEllipse();
return shape;
}
//---------------------------------------------------------------------------
void TGrapEllipse::DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed)
{
// when ellipse is a set of polypoints (multidraw)
isPoly = true;
TGrapLine::DrawPoly(boundRect, polyPts, polyCount);
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
bool TGrapEllipse::DrawBound(TRect *boundRect)
{
if (boundRect->Width() == 0 || boundRect->Height() == 0)
   return false;
int rx = (boundRect->Width()>>1);
int ry = (boundRect->Height()>>1);
int xc = rx + boundRect->Left;
int yc = ry + boundRect->Top;
DrawCoords(xc, yc, rx, ry);
return true;
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TGrapEllipse::DrawCoords(int xc, int yc, int rx, int ry)
{
Reset();
int x = 0, y = ry;
int p =(ry*ry)-(rx*rx*ry)+((rx*rx)/4);
while ((2*x*ry*ry)<(2*y*rx*rx))
   {
   savePoint(chunkList[0], chunkPosition[0], xc+x, yc-y); // 1. quadrant
   savePoint(chunkList[1], chunkPosition[1], xc-x, yc-y); // 2. quadant
   savePoint(chunkList[2], chunkPosition[2], xc-x, yc+y); // 3. quadrant
   savePoint(chunkList[3], chunkPosition[3], xc+x, yc+y); // 4. qaudrant
   if ( p < 0)
	  {
	  x = x + 1;
	  p = p +(2*ry*ry*x)+(ry*ry);
	  }
   else
	  {
	  x = x + 1;
	  y = y - 1;
	  p = p + (2*ry*ry*x+ry*ry)-(2*rx*rx*y);
	  }
   }
   p = ((float)x+0.5)*((float)x+0.5)*ry*ry+(y-1)*(y-1)*rx*rx-rx*rx*ry*ry;
while( y >= 0)
   {
   savePoint(chunkList[0], chunkPosition[0], xc+x, yc-y); // 1. quadrant
   savePoint(chunkList[1], chunkPosition[1], xc-x, yc-y); // 2. quadant
   savePoint(chunkList[2], chunkPosition[2], xc-x, yc+y); // 3. quadrant
   savePoint(chunkList[3], chunkPosition[3], xc+x, yc+y); // 4. qaudrant
   if (p > 0)
      {
      y = y-1;
      p = p-(2*rx*rx*y)+(rx*rx);
      }
   else
      {
      y = y-1;
      x = x+1;
      p = p+(2*ry*ry*x)-(2*rx*rx*y)-(rx*rx);
      }
  }
}
//---------------------------------------------------------------------------
// public method
//---------------------------------------------------------------------------
void TGrapEllipse::Rotate(TRect *boundRect, TPoint *origin, double angle)
{
structPtChunk *cItem;
double radians = angle *  M_PI/180.0;
int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
for (int i = 0; i < 4; i++)
	{
	for (unsigned int j = 0; j < chunkList[i].size(); j++)
		{
		cItem = chunkList[i][j];
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
	}
boundRect->left = minX;
boundRect->top  = minY;
boundRect->right = maxX;
boundRect->bottom = maxY;
}
//---------------------------------------------------------------------------
// public method - implementation
//---------------------------------------------------------------------------
void TGrapEllipse::GetStepPoints(int lineStep)
{
if (isPoly)
   {
   TGrapBasic::GetStepPoints(lineStep);
   return;
   }
int i, j ,k;
int ptCount = 0;
int chunkSize = 0;
//int test =  (chunkList[0]).size();
for (i = 0; i < 4; i++)
	chunkSize += getOverallChunkSize(chunkList[i]);
if (PointBmp)
   delete[] PointBmp;
PointBmp = new TPoint[chunkSize];
// collect chunks in one array
//std::vector<structPtChunk*> *cList;
structPtChunk *cItem;
for (i = 0; i < 4; i++)
	{
	if (i == 0 || i == 2)
	   {
		for (j = chunkList[i].size() - 1; j >= 0; j--)
			{
			cItem = chunkList[i][j];
			for (k = cItem->Count - 1; k >= 0; k--)
			   {
			   //if (++step < lineStep)
			   if (++_Step < lineStep)
				  continue;
			   PointBmp[ptCount].x   = cItem->Point[k].x;
			   PointBmp[ptCount++].y = cItem->Point[k].y;
			   //step = 0;
			   _Step = 0;
			   }
			}
		}
	else
		{
		for (j = 0; j < chunkList[i].size(); j++)
			{
			cItem = chunkList[i][j];
			for (k = 0; k < cItem->Count; k++)
               {
               //if (++step < lineStep)
			   if (++_Step < lineStep)
                  continue;
			   PointBmp[ptCount].x   = cItem->Point[k].x;
               PointBmp[ptCount++].y = cItem->Point[k].y;
               //step = 0;
               _Step = 0;
               }
            }
        }
    }
CountBmp = ptCount;
}
//---------------------------------------------------------------------------
void TGrapEllipse::GetRawPoints(void)
{
int i, j ,k;
int ptCount = 0;
int chunkSize = 0;
//int test =  (chunkList[0]).size();
for (i = 0; i < 4; i++)
	chunkSize += getOverallChunkSize(chunkList[i]);
if (PointRaw)
   delete[] PointRaw;
PointRaw = new TPoint[chunkSize];
// collect chunks in one array
//std::vector<structPtChunk*> *cList;
structPtChunk *cItem;
for (i = 0; i < 4; i++)
	{
	if (i == 0 || i == 2)
	   {
		for (j = chunkList[i].size() - 1; j >= 0; j--)
			{
			cItem = chunkList[i][j];
			for (k = cItem->Count - 1; k >= 0; k--)
			   {
			   PointRaw[ptCount].x   = cItem->Point[k].x;
			   PointRaw[ptCount++].y = cItem->Point[k].y;
			   }
			}
		}
	else
		{
		for (j = 0; j < chunkList[i].size(); j++)
			{
			cItem = chunkList[i][j];
			for (k = 0; k < cItem->Count; k++)
			   {
			   PointRaw[ptCount].x   = cItem->Point[k].x;
			   PointRaw[ptCount++].y = cItem->Point[k].y;
               }
            }
        }
    }
CountRaw = ptCount;
}
//---------------------------------------------------------------------------
void TGrapEllipse::Reset(void)
{
for (int i = 0; i < 4; i++)
	clearChunks(chunkList[i]);
}
//---------------------------------------------------------------------------
bool TGrapEllipse::MouseUp(TShiftState Shift, int X, int Y)
{
if (!TGrapBasic::MouseUp(Shift, X, Y))
   return false;
TRect boundRect;
boundRect.left    = _mouseData.X_down;
boundRect.top     = _mouseData.Y_down;
boundRect.right   = _mouseData.X_up;
boundRect.bottom  = _mouseData.Y_up;
boundRect.NormalizeRect();
DrawBound(&boundRect);
return true;
}


