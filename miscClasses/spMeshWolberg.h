//---------------------------------------------------------------------------
#ifndef spMeshWolbergH
#define spMeshWolbergH
#include "spGlobals.h"
#include <vector>
#include <Classes.hpp>
#include <Types.hpp>
#include "spPixelTypes.hpp"
#include "spImage.hpp"
//---------------------------------------------------------------------------
class TMeshWolberg
{
public:
	struct wMesh
	  {
	  int width;
	  int height;
	  double *x_data;
	  double *y_data;
	  };
private:
	spImage <ptRGB> *_srcImage, *_dstImage, *_interImage;
	spImage <ptGray> *_srcAlpha, *_dstAlpha, *_interAlpha;
	int _strideRGB, _strideGray;
	double *_in_position; // normalized scaling data
	//
	void Wolberg24(bool linear);
	bool SplineCatmullRom(double *x1, double *y1, int len1,
						  double *x2, double *y2, int len2);
	void SplineLinear(double *knot_x, double *knot_y,
					  double *spline_x, double *spline_y, int spline_length);
	void FantResample(LPBYTE source, LPBYTE dest,
					  int len, int pixel_offset, double *out_position);
protected:
public:
	wMesh *source_mesh, *dest_mesh;
	TMeshWolberg(spImage<ptRGB> *srcImage, spImage<ptGray> *srcAlpha, spImage<ptRGB> *dstImage, spImage<ptGray> *dstAlpha);
	virtual ~TMeshWolberg(void);
	void Execute(Tspe_WarpParams &params);
};
//---------------------------------------------------------------------------

#endif
