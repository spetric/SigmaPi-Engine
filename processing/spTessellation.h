//---------------------------------------------------------------------------
#ifndef spTessellationH
#define spTessellationH
#include "spImage.hpp"
#include "spGeometry.hpp"
#include "spPixelTypes.hpp"
#include "spGlobals.h"
//---------------------------------------------------------------------------
class spTessellation
   {
   private:
	   static int iWidth;
	   static int iHeight;
	   static std::vector<TPoint> points;
	   static std::vector<spGeometry::TLine> lines;
	   static std::vector<Triangle> triples;
	   static float minSitesDistance;
	   static spImage<ptRGB> *interImage;
	   static void createSites(std::vector<TPoint> &inPts, int ptsNum);
	   static void tesselateLevel(int displayType, spImage<ptRGB> *inpImage, spImage<ptGray> *maskImage);
	   static void drawTessellation(int displayType, spImage<ptRGB> *inpImage, spImage<ptGray> *maskImage);
   public:
	   static void DrawSolidEdges(spImage<ptRGB> *inpImage, spImage<ptRGB> *outImage, Tspe_Tessellation &params);
	   static void DrawMaskedEdges(spImage<ptGray> *maskImage, Tspe_Tessellation &params);
	   static void Tesselate(spImage<ptRGB> *inpImage, spImage<ptRGB> *outImage, spImage<ptGray> *maskImage, Tspe_Tessellation &params);
	   static void Redraw(spImage<ptRGB> *inpImage, spImage<ptRGB> *outImage, spImage<ptGray> *maskImage, Tspe_Tessellation &params);
	   static void Clear(void);
   };
#endif
