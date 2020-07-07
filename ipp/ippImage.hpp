//Copyright (C) 2017 Siniša Petriæ
#ifndef IPP_IMAGE_H
#define IPP_IMAGE_H
#include <Types.hpp>
#include "ippTypes.hpp"
//using namespace std;
//-- wrapper for a 2D images for Intel Processing Primitives Library (IPP)
//---------------------------
template <class T> class ippImage
  {
  public:
	typedef T Tpixel;
  //speEdgeOptions PixelOverFlow;
  //------------------------------
  // constructor - allocate image width height
  //------------------------------
  ippImage (int width, int height)
	{
	_shared = false;
	_width = width;
	_height = height;
	_scanlineAlignment = 64;
	int allStride;
	if (sizeof(T) == 3)
		_ippBuffer = ippiMalloc_8u_C3(width, height, &allStride);
	else
		_ippBuffer = ippiMalloc_8u_C1(width, height, &allStride);
	_stride = allStride;
	_ippData = reinterpret_cast<T*>(_ippBuffer);
	}
  //------------------------------
  // constructor - raw image void
  //------------------------------
  ippImage(void *buffer, int width, int height, unsigned int scanlineAlignment, bool shared = true)
	{
	constructFromBuffer(buffer, width, height, scanlineAlignment, shared);
	//_orientation = orientation;
	}
  //------------------------------
  // constructor - raw image  byte
  //------------------------------
  ippImage(Byte *buffer, int width, int height, unsigned int scanlineAlignment, bool shared = true)
	{
	constructFromBuffer(reinterpret_cast <void*>(buffer), width, height, scanlineAlignment, shared);
	//_orientation = orientation;
	}
  //------------------------------
  // destructor
  //------------------------------
  virtual ~ippImage()
	{
	_freeMemory();
	}
  //------------------------------
  // accessors/getters methods
  //------------------------------
  // Retrieve the width of the image
  int Width()  const { return _width; }
  // Retrieve the height of the image
  int Height() const { return _height; }
  // Retrieve image alignement
  unsigned int Alignment() const { return _scanlineAlignment; }
  // Retrieve image stride
  int Stride() const { return _stride; }
  // Return the depth in byte of the pixel (unsigned char will return 1)
  int Depth() const  { return sizeof(T);}
  // image memory access
  T * Data() const {return _ippData;}
  // random pixel access
  inline T & Pixel(int y, int x)
	 {
	 //T *mate;
	 //mate = (T*)((unsigned char*)_ippData + (x * sizeof(T) + y * _stride));
	 return *((T*)((unsigned char*)_ippData + (x * sizeof(T) + y * _stride)));
	 }
  //
  protected :
	// Internal data :
	T * _ippData;
	Ipp8u *_ippBuffer;
	//T *_contiguous_memory;
	int _width;
	int _height;
	unsigned int _align;
	unsigned int _scanlineAlignment;
	int _stride;
	bool _shared;
	//TRect _roi;
  //------------------------------
  void _freeMemory(void)
	{
	if (!_ippData)
	   return;
	if (!_shared)
	   {
	   ippiFree(_ippBuffer);
	   }
	_ippBuffer = NULL;
	_width = _height = 0;
	}
  //---------------------------------
  void constructFromBuffer(void *buffer, int width, int height, unsigned int scanlineAlignment, bool shared = true)
	{
	_width = width;
	_height = height;
	_shared = shared;
	_align = 0;
	_stride = _width * sizeof(T);
	_scanlineAlignment = scanlineAlignment;
	if (scanlineAlignment > 0)
	   _align = _stride % scanlineAlignment;         // alignemnt
	if (_align > 0) _align =  scanlineAlignment - _align;
	_stride = _stride + _align; 					    // stride
	if (!_shared)
	   {
	   //_allocateMemory(true, sizeof(T));	// always contiguous
	   //_data ->
	   }
	else
	   _ippData = static_cast<T*>(buffer);
	}
  };
//---------------------------
// specialized class for ippFloat
//---------------------------
template <> class ippImage<ippFloat>
  {
  public:
	typedef ippFloat Tpixel;
  //speEdgeOptions PixelOverFlow;
  //------------------------------
  // constructor - allocate image width height
  //------------------------------
  ippImage (int width, int height)
	{
	_shared = false;
	_width = width;
	_height = height;
	_scanlineAlignment = 64;
	int allStride;
	_ippBuffer = ippiMalloc_32f_C1(width, height, &allStride);
	_stride = allStride / sizeof(ippFloat);
	_ippData = reinterpret_cast<ippFloat*>(_ippBuffer);
	}
  //------------------------------
  // destructor
  //------------------------------
  virtual ~ippImage()
	{
	_freeMemory();
	}
  //------------------------------
  // accessors/getters methods
  //------------------------------
  // Retrieve the width of the image
  int Width()  const { return _width; }
  // Retrieve the height of the image
  int Height() const { return _height; }
  // Retrieve image alignement
  unsigned int Alignment() const { return _scanlineAlignment; }
  // Retrieve image stride
  int Stride() const { return _stride; }
  // Return the depth in byte of the pixel (unsigned char will return 1)
  int Depth() const  { return sizeof(ippFloat);}
  // image memory access
  ippFloat * Data() const {return _ippData;}
  // random pixel access
  inline ippFloat & Pixel(int y, int x)
	 {
	 return *(_ippData + (x  + y * _stride));
	 }
  //
  protected :
	// Internal data :
	ippFloat* _ippData;
	Ipp32f *_ippBuffer;
	//T *_contiguous_memory;
	int _width;
	int _height;
	unsigned int _align;
	unsigned int _scanlineAlignment;
	int _stride;   // number of floats
	bool _shared;
	//TRect _roi;
  //------------------------------
  void _freeMemory(void)
	{
	if (!_ippData)
	   return;
	if (!_shared)
	   {
	   ippiFree(_ippBuffer);
	   }
	_ippBuffer = NULL;
	_width = _height = 0;
	}
  };
#endif // SP_IMAGE_H
