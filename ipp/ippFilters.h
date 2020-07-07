//---------------------------------------------------------------------------
#ifndef ippFiltersH
#define ippFiltersH
#include <Types.hpp>
//#include "spPixelTypes.hpp"
#include "ippImage.hpp"
//---------------------------------------------------------------------------
class ippFilter
  {
  private:
	  IppStatus status;
	  IppiSize roiSize;
	  IppiSize maskSize;
	  Ipp8u *pBuffer;
	  int nChans;
  public:
	  IppiBorderType BorderType;
	  Ipp8u BorderValue;
	  ippFilter(int numBytes);
	  virtual ~ippFilter(void);
	  // median
	  /* ko fol s templejtom
	  static template <class T> int ippPrepareMedian( TRect *srcRect, int &kernelX, int &kernelY)
			{ippPrepareMedian(sizeof(T), TRect *srcRect, int &kernelX, int &kernelY)};
	  */
	  int ippPrepareMedian(TRect *srcRect, int &kernelX, int &kernelY);
	  int ippMedian(ippImage<ippRGB> *scrImg, ippImage<ippRGB> *dstImage);     // rgb
  };
#endif
