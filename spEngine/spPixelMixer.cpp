//---------------------------------------------------------------------------
#pragma hdrstop
#include "spPixelMixer.h"
#include "spSurfaceContainer.h"
#include "spPixelArithmetics.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// Pixel manipulation methods
//---------------------------------------------------------------------------
void spPixelMixer::SingleColor(Tsc *sc, bool force_solid)
{
if (sc->MaskLeft->Byte0[sc->i][sc->j]	>= sc->MaskRight->Byte0[sc->i][sc->j])
	return; // pixel skipped
int k, mask_perc, new_bgr, old_bgr, varyqty = 0, cor_len = 0;
varyqty = spCommon::CalcVariation(sc->Brush->CommonParams->ColorVary);
bool randori = spCommon::CoinToss(sc->Brush->CommonParams->ChalkOn);
mask_perc = (int)sc->MaskRight->Byte0[sc->i][sc->j] + 1;
if (sc->Brush->CommonParams->InvertOn)
   {
   if (mask_perc > sc->Brush->CommonParams->InvertValue)
	  mask_perc = 256 - mask_perc;
   //mask_perc = 256 - mask_perc + PixData->Brush->CommonParams->InvertValue;
   }
if (sc->Brush->CommonParams->BumpOn)
   {
   BumpCalc(sc, mask_perc);
   cor_len          = sc->Brush->State->cpBumpLength;
   mask_perc        = sc->Brush->State->cpBumpMaskPerc;
   }
mask_perc = (mask_perc * sc->Brush->State->cpNormCapacity) >> 8;
//Byte *wp =  (Byte*)(&PixData->Brush->ColorParams->WorkPrimary);
//Byte *ep =  (Byte*)(&Surf->Edit->Pixel(PixData->i, PixData->j));
for (k = 0; k < 3; k++)
	{
	if (randori)
	   {
	   if (!force_solid)
		  {
		  new_bgr = ((int)sc->Brush->ColorParams->WorkPrimary.ch[k] + cor_len + varyqty) * mask_perc;
		  //old_bgr = ((int)(sc->Edit->Pix[sc->i][sc->j].ch[k]) * (spe_SP_NORM - mask_perc));
		  old_bgr = ((int)(sc->MixSurf->Pix[sc->i][sc->j].ch[k]) * (spe_SP_NORM - mask_perc));
		  //new_bgr = ((int)wp[k] + cor_len + varyqty) * mask_perc;
		  //old_bgr = ((int)ep[k] * (spe_SP_NORM - mask_perc));
		  }
	   else
		  {
		  new_bgr = ((int)sc->Brush->ColorParams->WorkPrimary.ch[k] + cor_len + varyqty)<<spe_SP_SHIFT;  //* spe_SP_NORM;
		  old_bgr = 0x00;
		  }
	   new_bgr += old_bgr;
	   new_bgr = new_bgr>>spe_SP_SHIFT;
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
	   }
	else
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = sc->MixSurf->Pix[sc->i][sc->j].ch[k]; //sc->Edit->Pix[sc->i][sc->j].ch[k];
	}
sc->MaskLeft->Byte0[sc->i][sc->j] = sc->MaskRight->Byte0[sc->i][sc->j];
//Surf->MaskLeft->Pixel(PixData->i,PixData->j) = Surf->MaskRight->Pixel(PixData->i,PixData->j);
}
//---------------------------------------------------------------------------
void spPixelMixer::SingleColorAlpha(Tsc *sc, bool force_solid)
{
if (sc->MaskLeft->Pix[sc->i][sc->j]	>= sc->MaskRight->Pix[sc->i][sc->j])
   return; // pixel skipped
if (sc->skip_empty_alpha && sc->AlphaEdit->Byte0[sc->i][sc->j] == 0x00)
   return;
int k, mask_byte;
float alpha_perc, mask_perc, new_bgr, old_bgr, cor_len = 0.0, varyqty = 0.0;
varyqty = spCommon::CalcVariation(sc->Brush->CommonParams->ColorVary);
bool randori = spCommon::CoinToss(sc->Brush->CommonParams->ChalkOn);
mask_byte = (int)sc->MaskRight->Byte0[sc->i][sc->j];
if (sc->Brush->CommonParams->InvertOn)
   {
   if (mask_byte > sc->Brush->CommonParams->InvertValue)
	  mask_byte = 256 - mask_byte;
   }
//if (sc->Brush->CommonParams->InvertOn)
//   mask_byte = (255 - int(sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j]) + sc->Brush->CommonParams->InvertValue);
//else
//   mask_byte = sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j];
//if (sc->Brush->CommonParams->InvertOn)
//   mask_byte = 255 - (int)*sc->mright_bgr + sc->Brush->CommonParams->InvertValue;
//else
//   mask_byte = *sc->mright_bgr;
if (sc->Brush->CommonParams->BumpOn)
   {
   BumpCalc(sc, mask_perc);    // nije inicijalizirano??
   cor_len          = sc->Brush->State->cpBumpLength;
   mask_perc        = sc->Brush->State->cpBumpMaskPerc - 1;
   }
mask_perc =  (float)(mask_byte) / 255.0;
//float alpha_edit = (float)(sc->AlphaEdit->Byte0[sc->i][sc->j]) / 255.0; //  norm [0,1]
alpha_perc = sc->Brush->State->cpCapacity /100.0;
for (k = 0; k < 3; k++)
	{
	if (randori)
	   {
	   if (!force_solid)
		  {
		  new_bgr = (float)(sc->Brush->ColorParams->WorkPrimary.ch[k] + cor_len + varyqty);
		  old_bgr = (float)(sc->MixSurf->Pix[sc->i][sc->j].ch[k]);
		  // merge
		  if (sc->AlphaEdit->Byte0[sc->i][sc->j] > 0x00) // nešto je veæ nacrtano
			 new_bgr = new_bgr * mask_perc * alpha_perc + old_bgr * (1.0 - mask_perc*alpha_perc);
			 //new_bgr = new_bgr * mask_perc * alpha_perc;
		  }
	   else
		  {
		  new_bgr = ((int)sc->Brush->ColorParams->WorkPrimary.ch[k] + cor_len + varyqty);
		  old_bgr = 0x00;
		  }
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = spPixelArithmetics::NormColor(fast_ftol(new_bgr), 0x00, 0xff);
	   }
	else
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = sc->MixSurf->Pix[sc->i][sc->j].ch[k];
	}
sc->MaskLeft->Pix[sc->i][sc->j] = sc->MaskRight->Pix[sc->i][sc->j];
//if (*sc->mright_bgr > *sc->orig_alpha)
if (fast_ftol((float)sc->MaskRight->Byte0[sc->i][sc->j] * alpha_perc) > sc->AlphaOrig->Byte0[sc->i][sc->j] )
   sc->AlphaOrig->Pix[sc->i][sc->j] = fast_ftol(mask_byte * alpha_perc);
}
//---------------------------------------------------------------------------
void spPixelMixer::DoubleColor(Tsc *sc, bool force_solid)
{
if (sc->MaskLeft->Pix[sc->i][sc->j] >= sc->MaskRight->Pix[sc->i][sc->j])
   return; // pixel skipped
int k, mask_perc, new_bgr, old_bgr, varyqty, cor_len = 0, cor_perc;
varyqty = spCommon::CalcVariation(sc->Brush->CommonParams->ColorVary);
bool randori = spCommon::CoinToss(sc->Brush->CommonParams->ChalkOn);
mask_perc = (int)sc->MaskRight->Byte0[sc->i][sc->j] + 1;
if (sc->Brush->CommonParams->InvertOn)
   {
   if (mask_perc > sc->Brush->CommonParams->InvertValue)
	  mask_perc = 256 - mask_perc;
   //mask_perc = 256 - mask_perc + sc->Brush->CommonParams->InvertValue;
   }
//if (sc->Brush->CommonParams->InvertOn)
//   mask_perc = (256 - int(sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j]) + sc->Brush->CommonParams->InvertValue);
//else
//   mask_perc = int(sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j]) + 1;
cor_perc = mask_perc;
if (sc->Brush->CommonParams->BumpOn)
   {
   BumpCalc(sc, mask_perc);
   cor_len          = sc->Brush->State->cpBumpLength;
   cor_perc         = sc->Brush->State->cpBumpMaskPerc;
   }
//test
mask_perc = (mask_perc * sc->Brush->State->cpNormCapacity) >> 8;
cor_perc  = (cor_perc * sc->Brush->State->cpNormCapacity) >> 8;
for (k = 0; k < 3; k++)
	{
	if (randori)
	   {
	   new_bgr = (((int)sc->Brush->ColorParams->WorkPrimary.ch[k] + cor_len + varyqty) * mask_perc)>>spe_SP_SHIFT;
	   new_bgr +=(((int)sc->Brush->ColorParams->WorkSecondary.ch[k] + cor_len + varyqty)*(spe_SP_NORM - mask_perc))>>spe_SP_SHIFT;
	   if (!force_solid)
		  {
		  new_bgr = (new_bgr*cor_perc)>>spe_SP_SHIFT;
		  old_bgr = (int(sc->MixSurf->Pix[sc->i][sc->j].ch[k])*(spe_SP_NORM-cor_perc))>>spe_SP_SHIFT;
		  }
	   else
		  old_bgr = 0x00;
	   new_bgr += old_bgr;
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
	   }
	else
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = sc->MixSurf->Pix[sc->i][sc->j].ch[k];
	}
sc->MaskLeft->Pix[sc->i][sc->j] = sc->MaskRight->Pix[sc->i][sc->j];
}
//---------------------------------------------------------------------------
void spPixelMixer::DoubleColorAlpha(Tsc *sc, bool force_solid)
{
if (sc->MaskLeft->Pix[sc->i][sc->j]	>= sc->MaskRight->Pix[sc->i][sc->j])
   return; // pixel skipped
int k, mask_byte;
float alpha_perc, mask_perc, new_bgr, old_bgr, cor_len = 0.0, varyqty = 0.0;
varyqty = spCommon::CalcVariation(sc->Brush->CommonParams->ColorVary);
bool randori = spCommon::CoinToss(sc->Brush->CommonParams->ChalkOn);
mask_byte = (int)sc->MaskRight->Byte0[sc->i][sc->j];
if (sc->Brush->CommonParams->InvertOn)
   {
   if (mask_byte > sc->Brush->CommonParams->InvertValue)
	  mask_byte = 256 - mask_byte;
   }
//if (sc->Brush->CommonParams->InvertOn)
//   mask_byte = (255 - int(sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j]) + sc->Brush->CommonParams->InvertValue);
//else
//   mask_byte = sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j];
if (sc->Brush->CommonParams->BumpOn)
   {
   BumpCalc(sc, mask_perc);   // nije inicijalizirano
   cor_len          = sc->Brush->State->cpBumpLength;
   mask_perc        = sc->Brush->State->cpBumpMaskPerc - 1;
   }
mask_perc =  (float)(mask_byte) / 255.0;
//alpha_perc = (float)(*sc->edit_alpha) / 255.0; //  norm [0,1]
alpha_perc = sc->Brush->State->cpCapacity /100.0;
for (k = 0; k < 3; k++)
    {
    if (randori)
       {
	   new_bgr =  ((float)sc->Brush->ColorParams->WorkPrimary.ch[k] + cor_len  + varyqty) * mask_perc;
       new_bgr += ((float)sc->Brush->ColorParams->WorkSecondary.ch[k] + cor_len + varyqty)* (1.0 - mask_perc);
	   old_bgr = (float)(sc->MixSurf->Pix[sc->i][sc->j].ch[k]);
	   // merge
       if (force_solid)
		  mask_byte = 1;
	   else  if (sc->AlphaEdit->Byte0[sc->i][sc->j] > 0x00) // nešto je veæ nacrtano
		  new_bgr = new_bgr * mask_perc * alpha_perc + old_bgr * (1.0 - mask_perc*alpha_perc);
		  //new_bgr = new_bgr * mask_perc * alpha_perc;
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = spPixelArithmetics::NormColor(fast_ftol(new_bgr), 0x00, 0xff);
	   }
	else
	   sc->Orig->Pix[sc->i][sc->j].ch[k] = sc->MixSurf->Pix[sc->i][sc->j].ch[k];
	}
sc->MaskLeft->Pix[sc->i][sc->j] = sc->MaskRight->Pix[sc->i][sc->j];
if (fast_ftol(sc->MaskRight->Byte0[sc->i][sc->j] * alpha_perc)
   > sc->AlphaOrig->Byte0[sc->i][sc->j])
   sc->AlphaOrig->Byte0[sc->i][sc->j] = fast_ftol(mask_byte * alpha_perc);
}
//---------------------------------------------------------------------------
void spPixelMixer::SingleColorExternalAlpha(Tsc *sc)
{
if (sc->MaskLeft->Pix[sc->i][sc->j]	>= sc->MaskRight->Pix[sc->i][sc->j])
   return; // pixel skipped
//BYTE external_alpha =  sc->Brush->ColorParams->WorkPrimary.ch[3]; // nema više èetvrtog kanala
if (sc->skip_empty_alpha && sc->external_alpha == 0x00)
   return;
int k, mask_byte;
float alpha_perc, mask_perc, new_bgr, old_bgr, cor_len = 0.0, varyqty = 0.0;
varyqty = spCommon::CalcVariation(sc->Brush->CommonParams->ColorVary);
bool randori = spCommon::CoinToss(sc->Brush->CommonParams->ChalkOn);
mask_byte = (int)sc->MaskRight->Byte0[sc->i][sc->j];
if (sc->Brush->CommonParams->InvertOn)
   {
   if (mask_byte > sc->Brush->CommonParams->InvertValue)
	  mask_byte = 256 - mask_byte;
   }
//if (sc->Brush->CommonParams->InvertOn)
//   mask_byte = (255 - int(sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j]) + sc->Brush->CommonParams->InvertValue);
//else
//   mask_byte = sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j];
if (sc->Brush->CommonParams->BumpOn)
   {
   BumpCalc(sc, mask_perc);	// nije inicijalizirano
   cor_len          = sc->Brush->State->cpBumpLength;
   mask_perc        = sc->Brush->State->cpBumpMaskPerc - 1;
   }
mask_perc =  (float)(mask_byte) / 255.0;
alpha_perc = (float)(sc->external_alpha) / 255.0; //  norm [0,1]
for (k = 0; k < 3; k++)
	{
	if (randori)
	   {
	   new_bgr = (float)sc->Brush->ColorParams->WorkPrimary.ch[k] + cor_len + varyqty;
	   if (sc->external_alpha > 0x00)
		  sc->Orig->Pix[sc->i][sc->j].ch[k] = spPixelArithmetics::NormColor(fast_ftol(new_bgr), 0x00, 0xff);
	   }
/*    else
	   *to_maper = *from_maper; */
	}
sc->MaskLeft->Pix[sc->i][sc->j] = sc->MaskRight->Pix[sc->i][sc->j];
//if (*sc->mright_bgr > *sc->orig_alpha)
   sc->AlphaOrig->Byte0[sc->i][sc->j] = sc->external_alpha; //mask_byte;
}
//---------------------------------------------------------------------------
void spPixelMixer::EraseAlpha(Tsc *sc)
{
if (sc->MaskLeft->Pix[sc->i][sc->j]	>= sc->MaskRight->Pix[sc->i][sc->j])
   return; // pixel skipped
if (sc->Edit->Byte0[sc->i][sc->j] == 0x00) // pobrisano
   return;
int k, new_alpha, mask_byte;
mask_byte = (int)sc->MaskRight->Byte0[sc->i][sc->j];
if (sc->Brush->CommonParams->InvertOn)
   {
   if (mask_byte > sc->Brush->CommonParams->InvertValue)
	  mask_byte = 256 - mask_byte;
   }
//if (sc->Brush->CommonParams->InvertOn)
//   mask_byte = (255 - int(sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j]) + sc->Brush->CommonParams->InvertValue);
//else
//   mask_byte = sc->ImageContainer->ImageMaskRight->SpSurface->DirectByte[sc->i][sc->j];
// erase po masci
new_alpha = (int)sc->AlphaEdit->Byte0[sc->i][sc->j] -  mask_byte;
if (new_alpha < 0)
	new_alpha = 0;
//
sc->MaskLeft->Pix[sc->i][sc->j] = sc->MaskRight->Pix[sc->i][sc->j];
sc->AlphaOrig->Byte0[sc->i][sc->j] = (BYTE)new_alpha;
}
//---------------------------------------------------------------------------
void spPixelMixer::Marker(Tsc *sc)
{
if (sc->MaskLeft->Byte0[sc->i][sc->j]	>= sc->MaskRight->Byte0[sc->i][sc->j])
	return; // pixel skipped
if (sc->AssMarker)
   {
   sc->AssMarker->Pixel(sc->i, sc->j).ch[0] = sc->MaskRight->Byte0[sc->i][sc->j];
   /*
   int mask_perc, new_bgr, varyqty = 0;
   varyqty = spCommon::CalcVariation(sc->Brush->CommonParams->ColorVary);
   bool randori = spCommon::CoinToss(sc->Brush->CommonParams->ChalkOn);
   mask_perc = (int)sc->MaskRight->Byte0[sc->i][sc->j] + 1;
   if (sc->Brush->CommonParams->InvertOn)
	  {
	  if (mask_perc > sc->Brush->CommonParams->InvertValue)
		 mask_perc = 256 - mask_perc;
	  }
   mask_perc = (mask_perc * sc->Brush->State->cpNormCapacity) >> 8;
   if (randori)
	  {
	  new_bgr = (255 + varyqty) * mask_perc;
	  new_bgr = new_bgr>>spe_SP_SHIFT;
	  Byte pval = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
	  sc->AssMarker->Pixel(sc->i, sc->j).ch[0] = pval;
	  sc->AssMarker->Pixel(sc->i, sc->j).ch[1] = pval;
	  sc->AssMarker->Pixel(sc->i, sc->j).ch[2] = pval;
	  }
   */
   }
sc->MaskLeft->Byte0[sc->i][sc->j] = sc->MaskRight->Byte0[sc->i][sc->j];
}
//---------------------------------------------------------------------------
ptRGB & spPixelMixer::GetTexturePixelAligned(Tsc *sc)
{
int texture_i = sc->i % sc->Brush->ColorParams->Texture->Height();
int texture_j = sc->j % sc->Brush->ColorParams->Texture->Width();
if (texture_i < 0)
   texture_i += sc->Brush->ColorParams->Texture->Height();
if (texture_j < 0)
   texture_j += sc->Brush->ColorParams->Texture->Width();
return sc->Brush->ColorParams->Texture->Pixel(texture_i, texture_j);
/*
LPBYTE texture_bgr = LPBYTE(sc->Brush->ColorParams->TextureMap +
					(long)((texture_i * sc->Brush->ColorParams->TextureScanline24)
					 + texture_j * 3));
for (int k = 0; k < 3; k ++)
	{
	sc-> Brush->ColorParams->WorkPrimaryBGR[k] = *texture_bgr;
	texture_bgr++;
	}
*/
}
//---------------------------------------------------------------------------
ptRGB & spPixelMixer::GetTexturePixelFixed(Tsc *sc)
{
int texture_i, texture_j;
texture_i = (sc->i - sc->i_btc) + ((sc->Brush->ColorParams->Texture->Height())>>1);
texture_i = texture_i % sc->Brush->ColorParams->Texture->Height();
texture_j = (sc->j - sc->j_btc) + ((sc->Brush->ColorParams->Texture->Width())>>1);
texture_j = texture_j % sc->Brush->ColorParams->Texture->Width();
if (texture_i < 0)
   texture_i += sc->Brush->ColorParams->Texture->Height();
if (texture_j < 0)
   texture_j += sc->Brush->ColorParams->Texture->Width();
return sc->Brush->ColorParams->Texture->Pixel(texture_i, texture_j);
/*
LPBYTE texture_bgr = LPBYTE(sc->Brush->ColorParams->TextureMap +
					(long)((texture_i * sc->Brush->ColorParams->TextureScanline24)
					 + texture_j * 3));
for (int k = 0; k < 3; k ++)
	{
	sc->Brush->ColorParams->WorkPrimaryBGR[k] = *texture_bgr;
	texture_bgr++;
	}
*/
}
//---------------------------------------------------------------------------
ptRGB & spPixelMixer::GetCloneImagePixelAligned(Tsc *sc)
{
int clone_i, clone_j;
clone_i =   sc->i - sc->Clone_i;
clone_i = clone_i % sc->CloneImage->Height();
clone_j =   sc->j - sc->Clone_j;
clone_j = clone_j % sc->CloneImage->Width();
if (clone_i < 0)
   clone_i += sc->CloneImage->Height();
if (clone_j < 0)
   clone_j += sc->CloneImage->Width();
return sc->CloneImage->Pixel(clone_i, clone_j);
/*
LPBYTE img_bgr = LPBYTE(sc->CloneImage->image_map +
					(long)((clone_i * sc->CloneImage->Scanline)
					 + clone_j * 3));
for (int k = 0; k < 3; k ++)
	{
	sc->Brush->ColorParams->WorkPrimaryBGR[k] = *img_bgr;
	img_bgr++;
	}
*/
}
//---------------------------------------------------------------------------
ptRGB & spPixelMixer::GetCloneImagePixelFixed(Tsc *sc)
{
int clone_i, clone_j;
clone_i =   (sc->i - sc->i_btc) + sc->Clone_i;
clone_i = clone_i % sc->CloneImage->Height();
clone_j =   (sc->j - sc->j_btc) + sc->Clone_j;
clone_j = clone_j % sc->CloneImage->Width();
if (clone_i < 0)
   clone_i += sc->CloneImage->Height();
if (clone_j < 0)
   clone_j += sc->CloneImage->Width();
return sc->CloneImage->Pixel(clone_i, clone_j);
/*
LPBYTE img_bgr = LPBYTE(sc->CloneImage->image_map +
					(long)((clone_i * sc->CloneImage->Scanline)
					 + clone_j * 3));
for (int k = 0; k < 3; k ++)
	{
	sc->Brush->ColorParams->WorkPrimaryBGR[k] = *img_bgr;
	img_bgr++;
	}
*/
}
//---------------------------------------------------------------------------
ptRGB & spPixelMixer::GetPolyTexturePixelAligned(Tsc *sc)
{
int texture_i = sc->i % sc->Brush->ColorParams->PolyTexture->Height();
int texture_j = sc->j % sc->Brush->ColorParams->PolyTexture->Width();
if (texture_i < 0)
   texture_i += sc->Brush->ColorParams->PolyTexture->Height();
if (texture_j < 0)
   texture_j += sc->Brush->ColorParams->PolyTexture->Width();
return sc->Brush->ColorParams->PolyTexture->Pixel(texture_i, texture_j);
/*
LPBYTE texture_bgr = LPBYTE(sc->Brush->ColorParams->PolyTextureMap +
					(long)((texture_i * sc->Brush->ColorParams->PolyTextureScanline24)
					 + texture_j * 3));
for (int k = 0; k < 3; k ++)
	{
	sc->Brush->ColorParams->WorkPrimaryBGR[k] = *texture_bgr;
	texture_bgr++;
	}
*/
}
//---------------------------------------------------------------------------
void spPixelMixer::BumpCalc(Tsc *sc, int mask_perc)
{
sc->Brush->State->cpBumpMaskPerc = mask_perc;
if (sc->Brush->CommonParams->BumpScaleValue == 0)
   {
   sc->Brush->State->cpBumpLength = 0;
   return;
   }
Byte mask_maper;
Byte mask_test;
float from_len = 0;
mask_maper = spCommon::GetWrapPixel(sc->MaskRight,
								sc->j - sc->Brush->CommonParams->BumpXoffsetValue,
								sc->i - sc->Brush->CommonParams->BumpYoffsetValue).ch[0];
// korekcija, ako postiji tile
if (sc->Brush->ColorParams->BackgroundTile)
   {
   int tile_i = (sc->i - sc->Brush->CommonParams->BumpYoffsetValue) % sc->Brush->ColorParams->BackgroundTile->Height();
   int tile_j = (sc->j - sc->Brush->CommonParams->BumpXoffsetValue) % sc->Brush->ColorParams->BackgroundTile->Width();
   if (tile_i < 0)
	   tile_i = sc->Brush->ColorParams->BackgroundTile->Height() + tile_i;
   if (tile_j < 0)
	   tile_j = sc->Brush->ColorParams->BackgroundTile->Width() + tile_j;
   mask_maper = sc->Brush->ColorParams->BackgroundTile->Pixel(tile_i, tile_j).ch[0];
   /*
   LPBYTE tile_bgr = LPBYTE(sc->Brush->ColorParams->BackgroundTileMap + (long)((tile_i * sc->Brush->ColorParams->BackgroundTileScanline8) + tile_j));
   if (*tile_bgr < *mask_maper)
	   *mask_maper = *tile_bgr;
   */
   }
//sc->Brush->State->cpBumpMaskPerc = mask_perc;
// goni ga dalje
if (sc->Brush->CommonParams->InvertOn) // a di je invert value???
	{
	from_len  = (float)mask_maper - (float)(sc->MaskRight->Byte0[sc->i][sc->j]);
	mask_test =  0xff - mask_maper;
	}
else
	{
	from_len  = (float)(sc->MaskRight->Byte0[sc->i][sc->j]) - (float)mask_maper;
	mask_test =  mask_maper;
	}
if ((sc->Brush->CommonParams->BumpXoffsetValue == 0) && (sc->Brush->CommonParams->BumpYoffsetValue == 0))
   {
   from_len = (float)(sc->MaskRight->Byte0[sc->i][sc->j]) - 128.0;
   from_len /= 4.0;     // pi puta alocho
   }
from_len  *= sc->Brush->CommonParams->BumpScaleValue;
// spièi sve u strukturu i begaj
if (sc->Brush->CommonParams->BumpRoughnessValue > 0)
	from_len += sc->Brush->CommonParams->BumpRoughnessValue - random(sc->Brush->CommonParams->BumpRoughnessValue * 2);
if (from_len > 255)
	from_len = 255;
if (from_len < -255)
	from_len = -255;
sc->Brush->State->cpBumpLength  = fast_ftol(from_len);
// optimiziraj malo ovu prièu
if (sc->Brush->CommonParams->BumpInvertThreshold)
   {
   if (mask_test < (BYTE)(sc->Brush->CommonParams->BumpThresholdValue))
	  sc->Brush->State->cpBumpMaskPerc   = sc->Brush->CommonParams->BumpSoft ? mask_perc : 256;
   else
	  sc->Brush->State->cpBumpLength     = 0;
   }
else
   {
   if (mask_test > (BYTE)(sc->Brush->CommonParams->BumpThresholdValue))
	  sc->Brush->State->cpBumpMaskPerc   = sc->Brush->CommonParams->BumpSoft ? mask_perc : 256;
   else
      sc->Brush->State->cpBumpLength     = 0;
   }
// de ga probaj prominit, nevaja !!!!
}
//---------------------------------------------------------------------------
// special method - drop shadow  (no alpha)
//---------------------------------------------------------------------------
void spPixelMixer::ShadowColor(Tsc *sc, ptRGB &shadowColor, int &shadowCap, int &si, int &sj)
{
int k, mask_shadow, mask_perc, new_bgr, old_bgr;
mask_shadow = (int)sc->MaskLeft->Byte0[sc->i][sc->j]; // +  1;
mask_shadow = (mask_shadow * shadowCap) >> 8;
for (k = 0; k < 3; k++)
	{
	new_bgr = ((int)shadowColor.ch[k]) * mask_shadow;
	old_bgr = ((int)(sc->Edit->Pix[si][sj].ch[k])* (spe_SP_NORM - mask_shadow));
	new_bgr += old_bgr;
	new_bgr = new_bgr>>spe_SP_SHIFT;
	sc->Edit->Pix[si][sj].ch[k] = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
	}
}
//---------------------------------------------------------------------------
// special method - drop shadow  (no alpha)
//---------------------------------------------------------------------------
void spPixelMixer::BlendColor(Tsc *sc)
{
int k, mask_perc, new_bgr, old_bgr;
mask_perc = (int)sc->MaskLeft->Byte0[sc->i][sc->j]; //  + 1;
mask_perc = (mask_perc * spe_SP_NORM) >> 8;
for (k = 0; k < 3; k++)
	{
	new_bgr = ((int)sc->Orig->Pix[sc->i][sc->j].ch[k]) * mask_perc;
	old_bgr = ((int)(sc->Edit->Pix[sc->i][sc->j].ch[k])* (spe_SP_NORM - mask_perc));
	new_bgr += old_bgr;
	new_bgr = new_bgr>>spe_SP_SHIFT;
	sc->Orig->Pix[sc->i][sc->j].ch[k] = spPixelArithmetics::NormColor(new_bgr, 0x00, 0xff);
	}
}













