//Copyright (C) 2017 Siniša Petriæ
#ifndef SP_FATIMAGE_H
#define SP_FATIMAGE_H
#include <Types.hpp>
#include "spImage.hpp"
#include "ippTypes.hpp"
//---------------------------------------------------------------------------
// Fat image is container of various image types sharing the same buffer
// For now: spImage and ippImage
//---------------------------------------------------------------------------
using namespace std;
template <class T, class Z>
class spFatImage
  {
  public:
  spImage <T> *ImageSp;
  // ne valja -> treba biti ippImage
  // Ipp8u *IppBuffer;
  ippImage<Z> *ImageIpp;
  //------------------------------
  // constructor - allocate image width height - alway contiguous
  //------------------------------
  // allocate ipp memory and share data with spImage
  spFatImage (int width, int height, speOrientation orientation = orInvert)
	{
	initVars();
	_height = height;
	_width = width;
	_orientation = orientation;
	_channels = sizeof(T);
	/*
	if (_channels == 3)
	   IppBuffer = ippiMalloc_8u_C3(width, height, &_stride);
	else
	   IppBuffer = ippiMalloc_8u_C1(width, height, &_stride);
	_original_buffer = (void*)IppBuffer;
	*/
	ImageIpp = new ippImage<Z>(width, height);
	_original_buffer = (void*)ImageIpp->Data();
	ImageSp = new spImage<T>(ImageIpp->Data(), _width, _height, _scanlineAlignment, true, orientation);
	//SetRoi(); <- o yes, we'll need that
	}
  //------------------------------
  // destructor
  //------------------------------
  virtual ~spFatImage()
	{
	delete ImageSp;
	delete ImageIpp;
	}
protected :
  void *_original_buffer;
  int _width;
  int _height;
  unsigned int _align;
  unsigned int _scanlineAlignment;
  int _stride;
  int _channels;
  bool _shared;
  TRect _roi;
  speOrientation _orientation;
  // Internal methods
  //------------------------------
  void initVars(void)
	{
	_align = 0;
	_scanlineAlignment = 64; //new ipp image is always aligned on 64-byte boundary
	_stride = 0;
	_original_buffer = 0;
	_shared = false;
	_orientation = orAsIs;
	}
};
#endif


