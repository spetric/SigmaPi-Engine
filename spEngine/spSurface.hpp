//---------------------------------------------------------------------------
#ifndef spSurfaceH
#define spSurfaceH
#include <Types.hpp>
#include "spPixelTypes.hpp"
#include "spImage.hpp"
#include "spDraw.hpp"
//---------------------------------------------------------------------------
template <class T>
class spSurface : public spImage<T> //, public spDraw
{
using spImage<T>::_data;
using spImage<T>::_height;
using spImage<T>::_width;
public:
   //What the hack is this?? Regliquie reliquiarum.
   //typedef void __fastcall (__closure *TSurfaceFilter)(TRect*, int, int);
private:
   int FWidth, FHeight, FWidthM1, FHeightM1;
   unsigned int FPenSize;
   T FPenColor, FBrushColor;
   speDotType FPenType;
   //
   T &getPixel(int i, int j) {return _data[i][j];}
   void setPixel(int i, int j, T val) {_data[i][j] = val;}
   //
   Byte getByte0(int i, int j) {return _data[i][j].ch[0];}
   void setByte0(int i, int j, Byte val) {_data[i][j].ch[0] = val;}
   //
   void setPenColor(T val) {FPenColor = val; spDraw::PenColor = (void*)(&FPenColor);}
   void setBrushColor(T val) {FBrushColor = val; spDraw::BrushColor = (void*)(&FBrushColor);}
protected:
public:
   spSurface(int width, int height, bool contiguous = false) :
			spImage<T>(width, height, contiguous)
	  {
	  FHeight = _height;
	  FHeightM1 = FHeight - 1;
	  FWidth =   _width;
	  FWidthM1 = FWidth - 1;
	  FPenSize = 1;
	  FPenType = dtCircle;
	  SuperContainer = 0;
	  //Filter = 0;
	  }
   spSurface(int width, int height, unsigned int scanlineAlignement, bool contiguous = true) :
			spImage<T>(width, height, scanlineAlignement, contiguous)
	  {
	  FHeight = _height;
	  FHeightM1 = FHeight - 1;
	  FWidth =   _width;
	  FWidthM1 = FWidth - 1;
	  FPenSize = 1;
	  FPenType = dtCircle;
	  SuperContainer = 0;
	  //Filter = 0;
	  }
   spSurface(void *buffer, int width, int height, unsigned int scanlineAlignment) :
			spImage<T>(buffer, width, height, scanlineAlignment, true, orInvert)
	  {
	  FHeight = _height;
	  FHeightM1 = FHeight - 1;
	  FWidth =   _width;
	  FWidthM1 = FWidth - 1;
	  FPenSize = 1;
	  FPenType = dtCircle;
	  SuperContainer = 0;
	  //Filter = 0;
	  }
   spSurface(void **scanLine, int width, int height, bool shared) :
			spImage<T>(scanLine, width, height, shared)
	  {
	  FHeight = _height;
	  FHeightM1 = FHeight - 1;
	  FWidth =   _width;
	  FWidthM1 = FWidth - 1;
	  FPenSize = 1;
	  FPenType = dtCircle;
	  SuperContainer = 0;
	  //Filter = 0;
	  }
   spSurface(const spSurface<T> *surface, bool contiguous = false) :
			spImage<T>(surface, contiguous)
	  {
	  FHeight = _height;
	  FHeightM1 = FHeight - 1;
	  FWidth =   _width;
	  FWidthM1 = FWidth - 1;
	  FPenSize = 1;
	  FPenType = dtCircle;
	  SuperContainer = 0;
	  //Filter = 0;
	  }
   virtual ~spSurface(void)
	  {

	  }
   //--------------------------------------------------------------
   // must be called before any drawing with predefined color
   //--------------------------------------------------------------
   //template <class T>
   void SetContext(T penColor , T brushColor, unsigned int penSize = 1, speDotType penType = dtCircle)
	  {
	  FPenColor = penColor;
	  FBrushColor = brushColor;
	  FPenSize  = penSize;
	  FPenType  = penType;
	  spDraw::PenColor = (void*)(&FPenColor);
	  spDraw::PenSize = FPenSize;
	  spDraw::PenType = FPenType;
	  spDraw::BrushColor = (void*)(&FBrushColor);
	  spDraw::Surface = (void*)this;
	  spDraw::Width = FWidth;
	  spDraw::Height = FHeight;
	  }
   //--------------------------------------------------------------
   // alternative -> use it carefully (no pen and brush collors) - restores previous context
   //--------------------------------------------------------------
   void SetContext(void)
	  {
	  spDraw::PenColor = (void*)(&FPenColor);
	  spDraw::PenSize = FPenSize;
	  spDraw::PenType = FPenType;
	  spDraw::BrushColor = (void*)(&FBrushColor);
	  spDraw::Surface = (void*)this;
	  spDraw::Width = FWidth;
	  spDraw::Height = FHeight;
	  }
   //--------------------------------------------------------------
   // static functions: draw wrappers with predefined colors
   //--------------------------------------------------------------
   static void Line(int x0, int y0, int x1, int y1)
	  {
	  spDraw::Line(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), x0, y0, x1, y1, spDraw::PenSize, spDraw::PenType);
	  }
   //--------------------------------------------------------------
   static void SpaLine(int x0, int y0, int x1, int y1)
	  {
	  spDraw::SpaLine(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), x0, y0, x1, y1, spDraw::PenSize, spDraw::PenType);
	  }
   //--------------------------------------------------------------
   static void WuLine(int x0, int y0, int x1, int y1)
	  {
	  spDraw::WuLine(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), *static_cast<T*>(spDraw::BrushColor), x0, y0, x1, y1);
	  }
   //--------------------------------------------------------------
   static void ScanLine(int y, int x1, int x2)
	  {
	  spDraw::ScanLine(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), y, x1, x2);
	  }
   //--------------------------------------------------------------
   static void Ellipse(int xc, int yc, int rx, int ry)
	  {
	  spDraw::Ellipse(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), xc, yc, rx, ry, spDraw::PenSize);
	  }
   //--------------------------------------------------------------
   static void Ellipse (TRect rect)
	  {
	  spDraw::Ellipse (static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), rect, spDraw::PenSize);
	  }
   //--------------------------------------------------------------
   static void EllipseFilled(int xc, int yc, int rx, int ry, bool border = false)
	   {
	   spDraw::EllipseFilled(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::BrushColor), xc, yc, rx, ry);
	   if (border) Ellipse(xc, yc, rx, ry);
	   }
   //--------------------------------------------------------------
   static void EllipseFilled(TRect rect, bool border = false)
	   {
	   spDraw::EllipseFilled(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::BrushColor), rect);
	   if (border) Ellipse(rect);
	   }
   //--------------------------------------------------------------
   static void Rectangle(int x1, int y1, int x2, int y2)
	   {
	   spDraw::Rectangle(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), x1, y1, x2, y2, spDraw::PenSize);
	   }
   //--------------------------------------------------------------
   static void Rectangle(TRect rect)
	   {
	   spDraw::Rectangle(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), rect, spDraw::PenSize);
	   }
   //--------------------------------------------------------------
   static void RectangleFilled(int x1, int y1, int x2, int y2, bool border = false)
	   {
	   spDraw::RectangleFilled(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::BrushColor), x1, y1, x2, y2);
	   if (border) Rectangle(x1, y1, x2, y2);
	   }
   //--------------------------------------------------------------
   static void RectangleFilled(TRect rect, bool border = false)
	   {
	   spDraw::RectangleFilled(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::BrushColor), rect);
	   if (border) Rectangle(rect);
	   }
   //--------------------------------------------------------------
   static void Triangle(int x1, int y1, int x2, int y2, int x3, int y3)
	   {
	   spDraw::Triangle(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), x1, y1, x2, y2, x3, y3, spDraw::PenSize);
	   }
   //--------------------------------------------------------------
   static void TriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, bool border = false)
	   {
	   spDraw::TriangleFilled(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::BrushColor), x1, y1, x2, y2, x3, y3);
	   if (border) Triangle(x1, y1, x2, y2, x3, y3);
	   }
   //--------------------------------------------------------------
   static void TriangleGouraud(spDraw::spsColorTriangle<T> inpT)
	   {
	   spDraw::TriangleGouraud(static_cast <spImage<T>*>(spDraw::Surface), inpT);
	   }
   //--------------------------------------------------------------
   static void Polyline(TPoint *polyPts, int polyCount)
	   {
	   spDraw::Polyline(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), polyPts, polyCount, spDraw::PenSize);
	   }
   //--------------------------------------------------------------
   static void Polygon(TPoint *polyPts, int polyCount)
	   {
	   spDraw::Polygon(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::PenColor), polyPts, polyCount, spDraw::PenSize);
	   }
   //--------------------------------------------------------------
   static void PolygonFilled(TPoint *polyPts, int polyCount, bool border = false)
	   {
	   spDraw::PolygonFilled(static_cast <spImage<T>*>(spDraw::Surface), *static_cast<T*>(spDraw::BrushColor), polyPts, polyCount);
	   if (border) Polygon(polyPts, polyCount);
	   }
   //--------------------------------------------------------------
   static void PenCoord(int x, int y)
	   {
	   if (x < 0 || y < 0 || x > spDraw::Width-1 || y > spDraw::Height -1)
		  return;
	   static_cast <spImage<T>*>(spDraw::Surface)->Pixel(y, x) =  *static_cast<T*>(spDraw::PenColor);
	   }
   //--------------------------------------------------------------
   static void BrushCoord(int x, int y)
	   {
	   if (x < 0 || y < 0 || x > spDraw::Width-1 || y > spDraw::Height -1)
		  return;
	   static_cast <spImage<T>*>(spDraw::Surface)->Pixel(y, x) =  *static_cast<T*>(spDraw::BrushColor);
	   }
   //--------------------------------------------------------------
   void *SuperContainer;
   // filter (brush)
   //TSurfaceFilter Filter;
   //
   __property int Width = {read = FWidth};
   __property int Height = {read = FHeight};
   __property int WidthM1 = {read = FWidthM1};
   __property int HeightM1 = {read = FHeightM1};
   __property T PenColor = {read = FPenColor, write = setPenColor};
   __property T BrushColor = {read = FBrushColor, write = setBrushColor};
   __property T Pix[int i][int j] = {read = getPixel, write = setPixel};
   __property Byte Byte0[int i][int j] = {read = getByte0, write = setByte0};
   // {read = getPixelRGB, write = setPixelRGB};
};
//---------------------------------------------------------------------------
#endif
