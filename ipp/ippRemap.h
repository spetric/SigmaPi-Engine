//---------------------------------------------------------------------------
#ifndef ippRemapH
#define ippRemapH
#include <Types.hpp>
#include "ippImage.hpp"
#include "spGlobals.h"
//---------------------------------------------------------------------------
class ippRemap
   {
   private:
		Ipp8u* pSrc;
		Ipp8u* pDst;
		int srcStep;
		int dstStep;
		IppiSize roiSize;
		TRect patchRect, srcRect;
		int _radius;
		// ipp images
		ippImage<ippFloat> *ippFX, *ippFY;
		ippImage <ippRGB>*ippIN, *ippOUT;
		//ippImage <ippRGB>*ippMASK;
		void setSrcRect(ippImage<ippRGB> *srcImage, const int &X, const int &Y);
		void preparePatch(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect);
		void prepareIn(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect);
		void prepareOut(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *roiRect);
		void remapBlob(int wRange, int wStrength, int bSign);
		void remapTwirl(int wRange, double wAngle);
		void remapExplode(int wRange, int wStrength, int bSign);
   public:
		ippRemap(int radius);
		virtual ~ippRemap(void);
		void Prepare(Tspe_SpreadParams *spreadParams);
		void BlendSrcDst(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, const int &X, const int &Y, Ipp8u blendIn);
		//void PatchIn(ippImage<ippRGB> *srcImage, const int &X, const int &Y);
		void PatchIn(ippImage<ippRGB> *srcImage1, ippImage<ippRGB> *srcImage2, const int &X, const int &Y, Ipp8u blendIn);
		void PatchOut(ippImage<ippRGB> *dstImage, const int &X, const int &Y, Ipp8u blendOut);
		void PatchOutHard(ippImage<ippRGB> *dstImage, const int &X, const int &Y, ippImage<ippGray> *maskImage);
		void PatchOutSoft(ippImage<ippRGB> *dstImage, const int &X, const int &Y, ippImage<ippGray> *maskImage);
		int Apply(void);
   };
#endif
