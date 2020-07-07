//---------------------------------------------------------------------------
#ifndef grapBasicH
#define grapBasicH
#include <Types.hpp>
#include <vector>
#include <Classes.hpp>
static const int GP_CHUNK_MAX = 65536; //64k
//const GP_CHUNK_MAX = 131072; //128k
//enum TFillShape {fillNone, fillPoly, fillRectangle, fillEllipse, fillAlreadyDone};
//---------------------------------------------------------------------------
class TGrapBasic
{
public:
	struct  mouseDataStruct
	   {
	   int X_down;
	   int Y_down;
	   int X_up;
	   int Y_up;
	   int X_move_from;
	   int Y_move_from;
	   int X_move_to;
	   int Y_move_to;
	   int X_min;
	   int Y_min;
	   int X_max;
	   int Y_max;
	   int X_wrap_from;
	   int Y_wrap_from;
	   int X_wrap_to;
	   int Y_wrap_to;
	   bool MouseButtonDown;
	   TShiftState Shift;
	   };
protected:
	struct structPtChunk
	   {
	   TPoint Point[GP_CHUNK_MAX];
	   int Count;
	   };
	bool FFilled, FBorder, FClosed;
	//TFillShape FFillShape;
private:
	std::vector<structPtChunk*> chunkList;
	size_t chunkPosition;
protected:
	int   _Step;
	mouseDataStruct _mouseData;
	//TImageContainer *_imageContainer;
	void  savePoint(int X, int Y);
	virtual void clearChunks(std::vector<structPtChunk*> &cList, bool resetOnly = true);
	virtual int  getOverallChunkSize(std::vector<structPtChunk*> &cList);
	virtual void savePoint(std::vector<structPtChunk*> &cList, size_t &cPosition, int X, int Y);
	//
public:
	TGrapBasic(void);
	virtual ~TGrapBasic(void);
	virtual UnicodeString ClassName(void);
    virtual TGrapBasic* Clone(void);
	//
	TPoint *PointBmp;
	int    CountBmp;
	TPoint *PointRaw;
	int    CountRaw;

	virtual void DrawPoly(TRect *boundRect, TPoint *polyPts, int polyCount, bool closed = false);
	virtual void DrawPolyVect(std::vector<TPoint> &polyList);
	virtual bool DrawBound(TRect *boundRect);
	virtual void DrawCoords(int X1, int Y1, int X2, int Y2);
	virtual void Rotate(TRect *boundRect, TPoint *origin, double angle);
	void  ResetStepper(int initStep);
	virtual void Reset(void);
	virtual void GetStepPoints(int lineStep);
	virtual void GetRawPoints(void);
	virtual void SetSimplifyRule(bool simplify, int dpTolerance, int maxPts);
	virtual void GetSimplifiedPoints(TPoint *polyPts, int &polyCount, int dpTolerance, int maxPts);
	virtual void GetSplinePoints(TPoint *polyPts, int &polyCount, bool closed = false);
	//
	virtual bool MouseDown(TShiftState Shift, int X, int Y);
	virtual bool MouseMove(TShiftState Shift, int X, int Y);
	virtual bool MouseUp(TShiftState Shift, int X, int Y);
	__property mouseDataStruct MouseData = {read =_mouseData};
	__property bool Filled = {read = FFilled, write = FFilled};
	__property bool Border = {read = FBorder, write = FBorder};
	__property bool Closed = {read = FClosed, write = FClosed};
	//__property TFillShape FillShape = {read = FFillShape};
};
#endif
