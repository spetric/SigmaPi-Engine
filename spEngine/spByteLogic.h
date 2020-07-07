//Copyright (C) 2016 Siniša Petriæ
#ifndef SP_BYTE_LOGICH
#define SP_BYTE_LOGICH
typedef Byte (*TByteLogic)(const Byte &, const Byte &);
//---------------------------------------------------------------------------
// Byte logic functions
//---------------------------------------------------------------------------
static Byte Byte_AND(const Byte &left, const Byte &right) {return left & right;}
//---------------------------------------------------------------------------
static Byte Byte_OR(const Byte &left, const Byte &right) {return left | right;}
//---------------------------------------------------------------------------
static Byte Byte_XOR(const Byte &left, const Byte &right) {return left ^ right;}
//---------------------------------------------------------------------------
static Byte Byte_NAND(const Byte &left, const Byte &right) {return (left & right) ^ 0xff;}
//---------------------------------------------------------------------------
static Byte Byte_NOR(const Byte &left, const Byte &right) {return (left | right) ^ 0xff;}
//---------------------------------------------------------------------------
static Byte Byte_NXOR(const Byte &left, const Byte &right) {return (left ^ right) ^ 0xff;}
//---------------------------------------------------------------------------
static const TByteLogic bLogic[6] = {Byte_AND, Byte_OR, Byte_XOR, Byte_NAND, Byte_NOR, Byte_NXOR};
#endif

