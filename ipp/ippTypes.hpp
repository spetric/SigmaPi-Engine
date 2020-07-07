//Copyright (C) 2017 Siniša Petriæ
//
#ifndef IPP_PIXELTYPES_H_
#define IPP_PIXELTYPES_H_
#include "ipp.h"
//#include <iostream>
//using namespace std;
// 8 bit, 1 - channel templated pixel type
template <class T> class ippCh1
  {
  public:
  T ch[1];
  //------------------------------
  ippCh1()
	{
	ch[0] = T(0);
	}
  };
// 8 bit, 3 - channels templated pixel type
template <class T> class ippCh3
  {
  public:
  T  ch[3];
  //------------------------------
  ippCh3()
	{
	ch[0] = ch[1] = ch[2] = T(0);
	}
  //------------------------------
  explicit inline ippCh3(const T t[3])
	{
	ch[0] = t[0];
	ch[1] = t[1];
	ch[2] = t[2];
	}
};
typedef ippCh1<Ipp8u> ippGray;
typedef ippCh3<Ipp8u> ippRGB;
typedef ippCh1<Ipp32f> ippFloat;
typedef ippCh1<float> ippTest;
#endif

