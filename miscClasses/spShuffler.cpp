//---------------------------------------------------------------------------
#pragma hdrstop
#include "spShuffler.h"
#include "spGlobals.h"
#include <math.h>
#include <algorithm>
#include "ASMVars.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TShuffler::TShuffler(void)
{
_manh_distance = 5;
_manh_rand_dist = false;
_group_size = 0;
_pixelGroup = 0;
_i_center = -1;
_j_center = -1;
_sort_group = 0;
}
//---------------------------------------------------------------------------
// destructor
TShuffler::~TShuffler(void)
{
//
deleteGroup();
}
//---------------------------------------------------------------------------
// privatli
//---------------------------------------------------------------------------
void TShuffler::deleteGroup(void)
{
if (_pixelGroup)
   {
   delete []_pixelGroup;
   _pixelGroup = 0;
   }
}
//---------------------------------------------------------------------------
void TShuffler::calculateManhattan(int i_c, int j_c)
{
int fraction;
fraction = random(_manh_distance + 1);
_manh_i = fraction;
_manh_j = _manh_distance - fraction;
if (_manh_rand_dist)
   {
   _manh_i = random(_manh_i) + 1;
   _manh_j = random(_manh_j) + 1;
   }
if (random(2))
   _manh_i *= -1;
if (random(2))
   _manh_j *= -1;
_manh_i += i_c;
_manh_j += j_c;
}
//---------------------------------------------------------------------------
void TShuffler::calculateGroup(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j)
{
int k, m;
for (m = 0; m < _group_size; m++)
	{
	calculateManhattan(i_c, j_c);
	_pixelGroup[m].luminance = 0.0;
	if (!srcImage->SetSafeIndex(&_manh_i, &_manh_j))
	   {
	   _manh_i = i;
	   _manh_j = j;
       }
	_pixelGroup[m].pixel =  srcImage->Pixel(_manh_i, _manh_j);
	for (k = 0; k < 3; k++)
		_pixelGroup[m].luminance  += (float)_pixelGroup[m].pixel.ch[k] * spe_F_LUMINANCE[k];
	}
   _i_center = i_c;
   _j_center = j_c;
}
//---------------------------------------------------------------------------
void TShuffler::PrepareManhattanSimple(int distance, bool rdist)
{
_manh_distance = distance;
_manh_rand_dist = rdist;
}
//---------------------------------------------------------------------------
void TShuffler::PrepareGroup(int distance, int groupsize, bool rdist)
{
_i_center = -1;
_j_center = -1;
_manh_distance = distance;
_manh_rand_dist = rdist;
if (_pixelGroup)
   deleteGroup();
_group_size = groupsize;
_pixelGroup = new groupStruct [_group_size];
_sort_group = 0;
}
//---------------------------------------------------------------------------
void TShuffler::PrepareGroupRange(int distance, int groupsize, bool rdist, int value_min, int value_max, int sort_group)
{
PrepareGroup(distance, groupsize, rdist);
_lumina_min = value_min;
_lumina_max = value_max;
_sort_group = sort_group;
}
//---------------------------------------------------------------------------
void TShuffler::PrepareGroupAverage(int distance, int groupsize, bool rdist)
{
_i_center = -1;
_j_center = -1;
_manh_distance = distance;
_manh_rand_dist = rdist;
if (_pixelGroup)
   deleteGroup();
_group_size = groupsize;
_sort_group = 0;
}
//---------------------------------------------------------------------------
void TShuffler::ExecuteManhattanSimple(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j)
{
int k;
calculateManhattan(i_c, j_c);
if (!srcImage->SetSafeIndex(&_manh_i, &_manh_j))
   {
   _manh_i = i;
   _manh_j = j;
   }
_pixel = srcImage->Pixel(_manh_i, _manh_j);
}
//---------------------------------------------------------------------------
void TShuffler::ExecuteGroupBest(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j)
{
int k, m;
if (_i_center != i_c || _j_center != j_c)
   calculateGroup(srcImage, i_c, j_c, i, j);
float lumina_min = 99999, lumina_orig = 0, lumina_diff;
for (k = 0; k < 3; k++)
	lumina_orig += (float)srcImage->Pixel(i, j).ch[k] * spe_F_LUMINANCE[k];
int index = 0;
for (m  = 0; m < _group_size; m++)
   {
   lumina_diff = fabs(_pixelGroup[m].luminance - lumina_orig);
   if (lumina_diff < lumina_min )
	  {
	  lumina_min = lumina_diff;
	  index = m;
	  }
   }
_pixel = _pixelGroup[index].pixel;
}
//---------------------------------------------------------------------------
void TShuffler::ExecuteGroupWorst(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j)
{
int k, m;
if (_i_center != i_c || _j_center != j_c)
   calculateGroup(srcImage, i_c, j_c, i, j);
float lumina_max = 0, lumina_orig = 0, lumina_diff;
for (k = 0; k < 3; k++)
	lumina_orig += (float)srcImage->Pixel(i, j).ch[k] * spe_F_LUMINANCE[k];
int index = 0;
for (m  = 0; m < _group_size; m++)
   {
   lumina_diff = fabs(_pixelGroup[m].luminance - lumina_orig);
   if (lumina_diff > lumina_max )
	  {
	  lumina_max = lumina_diff;
	  index = m;
	  }
   }
_pixel = _pixelGroup[index].pixel;
}
//---------------------------------------------------------------------------
void TShuffler::ExecuteLuminaRange(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j)
{
int k, m;
if (_i_center != i_c || _j_center != j_c)
   {
   calculateGroup(srcImage, i_c, j_c, i, j);
   if (_sort_group == 1)
	  std::sort(&_pixelGroup[0], &_pixelGroup[_group_size], &aLuminaAsc);
   else if (_sort_group == 2)
	  std::sort(&_pixelGroup[0], &_pixelGroup[_group_size], &aLuminaDsc);
   }
float lumina_max = 0, lumina_orig = 0, lumina_diff;
for (k = 0; k < 3; k++)
	lumina_orig += (float)srcImage->Pixel(i, j).ch[k] * spe_F_LUMINANCE[k];
int index = -1;
for (m  = 0; m < _group_size; m++)
   {
   lumina_diff = fabs(_pixelGroup[m].luminance - lumina_orig);
   if (lumina_diff > _lumina_min && lumina_diff < _lumina_max )
	  {
	  index = m;
	  _pixel = _pixelGroup[index].pixel;
	  break;
	  }
   }
if (index < 0)
   _pixel = srcImage->Pixel(i_c, j_c);
}
//---------------------------------------------------------------------------
void TShuffler::ExecuteGroupAverage(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j)
{
int k, m;
if (_i_center != i_c || _j_center != j_c)
   {
   float average[3];
   average[0] = average[1] = average[2] = 0.0;
   for (m = 0; m < _group_size; m++)
	   {
		calculateManhattan(i_c, j_c);
		if (!srcImage->SetSafeIndex(&_manh_i, &_manh_j))
		   {
		   _manh_i = i;
		   _manh_j = j;
		   }
		for (k = 0; k < 3; k++)
		   average[k] += (float)srcImage->Pixel(_manh_i, _manh_j).ch[k];
	   }
   _i_center = i_c;
   _j_center = j_c;
   for (k = 0; k < 3; k++)
	   {
	   average[k] /= _group_size;
	   _pixel.ch[k] = fast_ftol(average[k]);
	   }
   }
}
//---------------------------------------------------------------------------


