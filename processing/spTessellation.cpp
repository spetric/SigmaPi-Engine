//---------------------------------------------------------------------------
#pragma hdrstop
#include "spTessellation.h"
#include "spDraw.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
std::vector<TPoint> spTessellation::points;
std::vector<spGeometry::TLine> spTessellation::lines;
std::vector<Triangle> spTessellation::triples;
int spTessellation::iWidth = 0;
int spTessellation::iHeight = 0;
float spTessellation::minSitesDistance = 2.2;
spImage<ptRGB>* spTessellation::interImage = NULL;
//---------------------------------------------------------------------------
void spTessellation::createSites(std::vector<TPoint> &inPts, int ptsNum)
{
bool equal = false;
TPoint pt;
// put pixels in the image corners.
   pt.x = 0;
   pt.y = 0;
   inPts.push_back(pt);
   pt.x = iWidth-1;
   pt.y = 0;
   inPts.push_back(pt);
   pt.x = iWidth-1;
   pt.y = iHeight-1;
   inPts.push_back(pt);
   pt.x = 0;
   pt.y = iHeight-1;
   inPts.push_back(pt);
for (int i = 0; i < ptsNum; i++)
	{
	pt.x = random(iWidth  - 6) + 4;
	pt.y = random(iHeight - 6) + 4;
	equal = false;
	// do not add duplicated sites
	for (size_t j = 0; j < inPts.size(); j++)
		{
		if (inPts[j].y == pt.y && inPts[j].x == pt.x)
		   {
		   equal = true;
		   break;
		   }
		}
	if (equal)
	   continue;
	inPts.push_back(pt);
	}
}
//---------------------------------------------------------------------------
void spTessellation::tesselateLevel(int displayType, spImage<ptRGB> *inpImage, spImage<ptGray> *maskImage)
{
lines.clear();
triples.clear();
//
spGeometry::VoronoiGenerate(points, lines, triples, iWidth, iHeight, minSitesDistance);
//
drawTessellation(displayType, inpImage, maskImage);
}
//---------------------------------------------------------------------------
void spTessellation::drawTessellation(int displayType, spImage<ptRGB> *inpImage, spImage<ptGray> *maskImage)
{
ptGray pixMask = ptGray(255);
//
if (displayType > 0)
   {
   int m;
   spDraw::spsColorTriangle<ptRGB> cTri;
   for (size_t i = 0; i < triples.size(); i++)
	   {
	   for (int k = 0; k < 3; k++)
		   {
		   cTri.x[k] = triples[i].s[k].coord.x;
		   cTri.y[k] = triples[i].s[k].coord.y;
		   cTri.val[k] = inpImage->Pixel(cTri.y[k], cTri.x[k]);
		   }
	   if (displayType == 2)
		  spDraw::TriangleGouraud(interImage, cTri);
	   else
		  spDraw::TriangleFilled(interImage, cTri, -1);
	   }
   }
else
   {
	maskImage->Fill(ptGray(0));
	for (size_t i = 0; i < lines.size(); i++)
		{
		spGeometry::TLine line = lines[i];
		spDraw::Line(maskImage, pixMask, line.x1, line.y1, line.x2, line.y2, 1);
		}
	// fill
	int x, y;
	for (size_t i = 0; i < points.size(); i++)
		{
		x = points[i].x;
		y = points[i].y;
		try {
			spDraw::FloodFillConvex(maskImage, interImage, x, y, inpImage->Pixel(y, x), true);
			}
		catch (...)
			{
			throw "Flood fill error!";
			// ups!!!
			}
		}
	}

}
//---------------------------------------------------------------------------
void spTessellation::DrawSolidEdges(spImage<ptRGB> *inpImage, spImage<ptRGB> *outImage, Tspe_Tessellation &params)
{
ptRGB pixEdgeShadow = ptRGB(128, 128, 128);
ptRGB pixEdge = ptRGB(params.EdgeColor);
ptRGB pixVertex;
bool noVertex;
//
if (params.DisplayType > 0)
  {
  spDraw::spsColorTriangle<ptRGB> cTri;
  for (size_t i = 0; i < triples.size(); i++)
	  {
	  for (int k = 0; k < 3; k++)
		  {
		  cTri.x[k] = triples[i].s[k].coord.x;
		  cTri.y[k] = triples[i].s[k].coord.y;
		  cTri.val[k] = inpImage->Pixel(cTri.y[k], cTri.x[k]);
		  }
	  if (params.EdgeType == 2)
		 spDraw::Triangle(outImage, cTri, params.EdgeWidth, -1);
	  else
		 {
		 cTri.val[0] = pixEdge;
		 spDraw::Triangle(outImage, cTri, params.EdgeWidth, 0);
		 }
	  }
  }
else
  {
  int x, y;
  for (size_t i = 0; i < lines.size(); i++)
	   {
	   spGeometry::TLine line = lines[i];
	   if (abs(line.x1 - line.x2) == 0 &&  abs(line.y1 - line.y2) == 0)
		  continue;
	  if (params.EdgeType == 2)
		  {
		  x = line.x1 + ((abs(line.x2 - line.x1))>>1);
		  y = line.y1 + ((abs(line.y2 - line.y1))>>1);
		  if (y < 0 || y > iHeight-1 || x < 0 || x > iWidth-1)
			 continue;
		  pixVertex = inpImage->Pixel(y, x);
		  spDraw::Line(outImage, pixVertex, line.x1, line.y1, line.x2, line.y2, params.EdgeWidth);
		  }
	   else
		  {
		  if (params.EdgeWidth == 1)
			 {
			  x = line.x1 + ((abs(line.x2 - line.x1))>>1);
			  y = line.y1 + ((abs(line.y2 - line.y1))>>1);
			  if (y < 0 || y > iHeight-1 || x < 0 || x > iWidth-1)
				 pixVertex = pixEdgeShadow;
			  else
				 pixVertex = inpImage->Pixel(y, x);
			  spDraw::WuLine(outImage, pixEdge, pixVertex, line.x1, line.y1, line.x2, line.y2);
			 }
		  else
			  spDraw::Line(outImage, pixEdge, line.x1, line.y1, line.x2, line.y2, params.EdgeWidth);
		  }
	   }
  }
}
//---------------------------------------------------------------------------
void spTessellation::DrawMaskedEdges(spImage<ptGray> *maskImage, Tspe_Tessellation &params)
{
ptGray pixEdge = ptGray(255);
//
if (params.DisplayType > 0)
  {
  for (size_t i = 0; i < triples.size(); i++)
	  {
	  spDraw::Triangle(maskImage, pixEdge, triples[i].s[0].coord.x, triples[i].s[0].coord.y
										 , triples[i].s[1].coord.x, triples[i].s[1].coord.y
										 , triples[i].s[2].coord.x, triples[i].s[2].coord.y
										 , params.EdgeWidth);
	  }
  }
else
  {
  int x, y;
  for (size_t i = 0; i < lines.size(); i++)
	  {
	  spGeometry::TLine line = lines[i];
	  if (abs(line.x1 - line.x2) == 0 &&  abs(line.y1 - line.y2) == 0)
		 continue;
	  spDraw::Line(maskImage, pixEdge, line.x1, line.y1, line.x2, line.y2, params.EdgeWidth);
	  }
   }
}
//---------------------------------------------------------------------------
void spTessellation::Tesselate(spImage<ptRGB> *inpImage, spImage<ptRGB> *outImage, spImage<ptGray> *maskImage, Tspe_Tessellation &params)
{
iWidth  = inpImage->Width();
iHeight = inpImage->Height();
if (interImage)
   delete interImage;
interImage = new spImage<ptRGB>(outImage);
minSitesDistance = params.MinSitesDistance;
points.clear();
lines.clear();
triples.clear();
std::vector<TPoint> levelPts;
int ptsNum, ptsCandidates = 0, ptsAccepted;
int iterations = params.Iterations;
int ptsIterAdd = (params.SitesMax - params.SitesMin) / iterations;
if (ptsIterAdd <= 0)
   iterations = 1;
if (iterations == 1)	// only one iteration
   ptsNum = params.SitesMax;
else
   ptsNum = params.SitesMin;
//
ptRGB pixIn, pixOut;
// lowest level
createSites(points, ptsNum);
ptsAccepted = ptsNum;
ptsCandidates = ptsNum;
int x, y;
float lumOut, lumIn, error = (float)params.LevelError;
bool equal;
for (int i = 0; i < iterations; i++)
	{
	tesselateLevel(params.DisplayType, inpImage, maskImage);
	ptsNum += ptsIterAdd;
	if (i < iterations-1)
	   {
	   ptsCandidates += ptsNum;
	   createSites(levelPts, ptsNum);
	   for (size_t k = 0; k < levelPts.size(); k++)
		   {
		   x = levelPts[k].x;
		   y = levelPts[k].y;
		   pixOut = outImage->Pixel(y, x);
		   pixIn  = inpImage->Pixel(y, x);
		   lumOut = pixOut.ch[0]*spe_F_LUMINANCE[0] + pixOut.ch[1]*spe_F_LUMINANCE[1] + pixOut.ch[2]*spe_F_LUMINANCE[2];
		   lumIn  = pixIn.ch[0]*spe_F_LUMINANCE[0] + pixIn.ch[1]*spe_F_LUMINANCE[1] + pixIn.ch[2]*spe_F_LUMINANCE[2];
		   if (fabs(lumOut - lumIn) > error )
			  {
			  equal = false;
			  // do not add duplicated sites
			  for (size_t j = 0; j < points.size(); j++)
				  {
				  if (points[j].y == levelPts[k].y && points[j].x == levelPts[k].x)
					 {
					 equal = true;
					 break;
					 }
				  }
			  if (equal)
				 continue;
			  points.push_back(levelPts[k]);
			  ptsAccepted ++;
			  }
		   }
	   }
	// draw onto orig
	outImage->Copy(interImage);
	}
params.CandidateSites    = ptsCandidates;
params.AcceptedSites	 = ptsAccepted;
params.DiscardedSites	 = ptsCandidates - ptsAccepted;
}
//---------------------------------------------------------------------------
void spTessellation::Redraw(spImage<ptRGB> *inpImage, spImage<ptRGB> *outImage, spImage<ptGray> *maskImage, Tspe_Tessellation &params)
{
if (iWidth == 0 || iHeight == 0)
   return;
if (points.empty() || lines.empty() || triples.empty())
   return;
interImage->Copy(inpImage);
drawTessellation(params.DisplayType, inpImage, maskImage);
outImage->Copy(interImage);
}
//---------------------------------------------------------------------------
void spTessellation::Clear(void)
{
if (interImage)
   delete interImage;
interImage = NULL;
}



