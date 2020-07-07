//---------------------------------------------------------------------------
#ifndef ippCommonH
#define ippCommonH
#include <Types.hpp>
#include "spGlobals.h"
#include "ippImage.hpp"
typedef bool (*TIppCompoType)(int, int ,int);
//---------------------------------------------------------------------------
// za compound filtere
//TIppCompoType compTyper;
class ippCommon
{
	private:
		static bool compInner(int intensity, int lo, int hi);
		static bool compOuter(int intensity, int lo, int hi);
		static bool compOutHi(int intensity, int lo, int hi);
		static bool compOutLo(int intensity, int lo, int hi);
	public:
		static void CompCloseFar(ippImage <ippRGB> *work1, ippImage<ippRGB> *work2, ippImage<ippRGB> *dstImage, Tspe_CompoundIppParams *ipf);
		static void FactorAddition(ippImage<ippRGB> *work1, ippImage<ippRGB> *work2, ippImage<ippRGB> *dstImage, float left_factor, float right_factor);
		static void Blend(ippImage<ippRGB> *srcImage, TRect *srcRect, ippImage<ippRGB> *dstImage, TRect *dstRect, Ipp8u srcAlpha, IppiAlphaType alphaType, Byte parivrta);
		static void Blend(ippImage<ippRGB> *srcImage1, ippImage<ippRGB> *srcImage2, TRect *srcRect, ippImage<ippRGB> *dstImage, TRect *dstRect, Ipp8u srcAlpha, IppiAlphaType alphaType, Byte parivrta);
		static void CopyOne2Three(ippImage<ippGray> *srcImage, ippImage<ippRGB> *dstImage, TRect *srcRect, TRect *dstRect, Byte parivrta);
		static void Copy(ippImage<ippRGB> *srcImage, ippImage<ippRGB> *dstImage, TRect *srcRect, TRect *dstRect, Byte parivrta);
		static void Copy(ippImage<ippGray> *srcImage, ippImage<ippGray> *dstImage, TRect *srcRect, TRect *dstRect, Byte parivrta);
		// distance calculation
		inline static int MinDistL1(ippRGB &refer_pix, ippRGB &left_pix, ippRGB &right_pix, int &left_val, int &right_val);
		inline static int MinDistEuclid(ippRGB &refer_pix, ippRGB &left_pix, ippRGB &right_pix, int &left_val, int &right_val);
};
#endif
