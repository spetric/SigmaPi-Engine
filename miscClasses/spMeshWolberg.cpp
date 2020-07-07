//---------------------------------------------------------------------------
#pragma hdrstop
#include "spMeshWolberg.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
// constructor
TMeshWolberg::TMeshWolberg(spImage<ptRGB> *srcImage, spImage<ptGray> *srcAlpha, spImage<ptRGB> *dstImage, spImage<ptGray> *dstAlpha)
{
_srcImage = srcImage;
_dstImage = dstImage;
_srcAlpha = srcAlpha;
_dstAlpha = dstAlpha;
_strideRGB = (int)srcImage->Stride();
if (srcAlpha)
   _strideGray = (int)srcImage->Stride();
_interAlpha = 0;
_interImage = 0;
}
//---------------------------------------------------------------------------
// destructor
TMeshWolberg::~TMeshWolberg(void)
{
if (source_mesh->x_data)
   {
   free(source_mesh->x_data);
   free(source_mesh->y_data);
   }
if (dest_mesh->x_data)
   {
   free(dest_mesh->x_data);
   free(dest_mesh->x_data);
   }
}
//---------------------------------------------------------------------------
void TMeshWolberg::Execute(Tspe_WarpParams &params)
{
int i, j;
double *source_ptr_x, *source_ptr_y, *dest_ptr_x, *dest_ptr_y;
source_mesh = new wMesh();
dest_mesh   = new wMesh();
_interImage = new spImage<ptRGB>(_srcImage, true);
if (_srcAlpha)
   _interAlpha = new spImage<ptGray>(_srcAlpha, true);
else
   _interAlpha = 0;
source_mesh->width  = params.MeshCols;
source_mesh->height = params.MeshRows;
dest_mesh->width  = params.MeshCols;
dest_mesh->height = params.MeshRows;
source_mesh->x_data = (double *)malloc(sizeof(double) * source_mesh->width  * source_mesh->height );
source_mesh->y_data = (double *)malloc(sizeof(double) * source_mesh->width * source_mesh->height);
dest_mesh->x_data = (double *)malloc(sizeof(double) * dest_mesh->width * dest_mesh->height);
dest_mesh->y_data = (double *)malloc(sizeof(double) * dest_mesh->width * dest_mesh->height);
//
// move MeshSource koordinate
//
source_ptr_x = source_mesh->x_data;
source_ptr_y = source_mesh->y_data;
dest_ptr_x   = dest_mesh->x_data;
dest_ptr_y   = dest_mesh->y_data;
// init
int mSize = source_mesh->height * source_mesh->width;
for (i = 0; i < mSize; i++)
	{
	*source_ptr_x = (double)params.MeshSrc[i].x;
	*source_ptr_y = (double)params.MeshSrc[i].y;
	*dest_ptr_x = (double)params.MeshDst[i].x;
	*dest_ptr_y = (double)params.MeshDst[i].y;
	source_ptr_x++;
	source_ptr_y++;
	dest_ptr_x++;
	dest_ptr_y++;
	}
Wolberg24(!params.Antialias);
free(source_mesh->x_data);
free(source_mesh->y_data);
free(dest_mesh->x_data);
free(dest_mesh->y_data);
delete source_mesh;
delete dest_mesh;
delete _interImage;
if (_interAlpha)
   delete _interAlpha;
}
//---------------------------------------------------------------------------
// Modified mesh warping: specifically for 24-bit (+ alpha channel)
//---------------------------------------------------------------------------
void TMeshWolberg::Wolberg24(bool linear)
{
_in_position = 0;
int rows = _srcImage->Height();
int cols = _srcImage->Width();
int mesh_width, mesh_height; // dimensions of input and output meshes
int x, y;                    // destination coordinates
int u, v;                    // source coordiantes
int i, k;                    // loop variable
int line_size;               // size of largest image dimension
double *x1, *y1, *x2, *y2;   // pointers used to create splines
double *xrow, *yrow;         // pointers to row and column spline values
double *xcol, *ycol;         // temporary pointers to spline rows and cols
double *column_ptr;          // pointer to copy column data to image
double *index;               // independent variables for spline 0,1,2..
double *scale_factors;       // scaling factors used in resampling
double *interpolated;        // interpolated row or column
LPBYTE source_bgr, source_alpha;               // temporary image pointer to current source
LPBYTE dest_bgr, dest_alpha;                 // temporary image pointer to destination
wMesh *vert_splines;          // storage for vertical splines
wMesh *horz_splines;          // storage for horizontal splines

//
// allocate memory for scanline along the longist dimension
//
line_size = std::max(cols, rows);
index = (double *) malloc(line_size * sizeof(double));
xrow = (double *) malloc(line_size * sizeof(double));
yrow = (double *) malloc(line_size * sizeof(double));
scale_factors  = (double *) malloc((line_size + 1) * sizeof(double));
interpolated = (double *) malloc((line_size + 1) * sizeof(double));

//
// variable intializations
//
mesh_width = source_mesh->width;
mesh_height = source_mesh->height;
for (i = 0; i < line_size; i++)
	 index[i] = i;
//
// create vertical splines
//
vert_splines = new wMesh();
vert_splines->width  = mesh_width;
vert_splines->height = rows;
vert_splines->x_data = (double *)malloc(sizeof(double) * mesh_width * rows);
vert_splines->y_data = (double *)malloc(sizeof(double) * mesh_width * rows);
if ((vert_splines == NULL) || (vert_splines->x_data == NULL) || (vert_splines->y_data == NULL))
   return;
//
// create table of x-intercepts for source mesh's vertical splines
//
for (u = 0; u < mesh_width; u++)
    {
    // set up pointers for spline function
    xcol = (double *)(source_mesh->x_data + u);
	ycol = (double *)(source_mesh->y_data + u);
    column_ptr = (double *) vert_splines->x_data + u;
    // initialize xrow and yrow for spline function
    for (v = 0; v < mesh_height; v++)
		{
        xrow[v] = *xcol;
		yrow[v] = *ycol;
		xcol += mesh_width;
        ycol += mesh_width;
        }
	// generate source vertical spline
	if (linear)
		SplineLinear(yrow, xrow, index, interpolated, rows);
	else if (!SplineCatmullRom(yrow, xrow, mesh_height, index, interpolated, rows))
		SplineLinear(yrow, xrow, index, interpolated, rows);
	// store resampled row back into vert_slines
	for (y = 0; y < rows; y++)
		{
        *column_ptr = interpolated[y];
		column_ptr += mesh_width;
        }
    }
//
// create table of x-intercepts for destination mesh's vertical splines
//
for (u = 0; u < mesh_width; u++)
    {
	// set up pointers for spline function
	xcol = (double *) dest_mesh->x_data + u;
    ycol = (double *) dest_mesh->y_data + u;
    column_ptr = (double *) vert_splines->y_data + u;
    // initialize xrow and yrow
    for (v = 0; v < mesh_height; v++)
        {
		xrow[v] = *xcol;
        yrow[v] = *ycol;
        xcol += mesh_width;
        ycol += mesh_width;
        }
	// generate destination vertical spline
	if (linear)
	   SplineLinear(yrow, xrow, index, interpolated, rows);
	else if (!SplineCatmullRom(yrow, xrow, mesh_height, index, interpolated, rows))
	   SplineLinear(yrow, xrow, index, interpolated, rows);
	// store data back into vert_splines
    for (y = 0; y < rows; y++)
        {
		*column_ptr = interpolated[y];
        column_ptr += mesh_width;
		}
	}
// first pass: warp x using vert_splines
x1  = (double *) vert_splines->x_data;
x2  = (double *) vert_splines->y_data;
//
source_bgr = (unsigned char*)_srcImage->GetOriginalBuffer();
dest_bgr   = (unsigned char*)_interImage->GetOriginalBuffer();
source_bgr += (rows - 1) * _strideRGB;
dest_bgr   += (rows - 1) * _strideRGB;
if (_srcAlpha)
   {
   source_alpha = (unsigned char*)_srcAlpha->GetOriginalBuffer();
   dest_alpha   = (unsigned char*)_interAlpha->GetOriginalBuffer();
   source_alpha += (rows - 1) * _strideGray;
   dest_alpha   += (rows - 1) * _strideGray;
   }
else
   {
   source_alpha = 0;
   dest_alpha = 0;
   }
_in_position = (double *)malloc((cols + 1) * sizeof(double));
for (y = 0; y < rows; y++)
	{
	// fit spline to x-intercepts; resample over all cols
	if (linear)
	   SplineLinear(x1, x2, index, scale_factors, cols);
	else if (!SplineCatmullRom(x1, x2, mesh_width, index, scale_factors, cols))
	   SplineLinear(x1, x2, index, scale_factors, cols);
	scale_factors[cols]=cols;    // initialize last element
	// resample source row based on map
	for (k = 0; k < 3; k++)      // loop across all channels
		FantResample(LPBYTE(source_bgr + k), LPBYTE(dest_bgr + k), cols, 3, scale_factors);
    // advance all pointers to next row
	//source_bgr += _strideRGB;
	//dest_bgr   += _strideRGB;
	source_bgr -= _strideRGB;
	dest_bgr   -= _strideRGB;
	// is there alpha channel
	if (source_alpha)
	   {
	   FantResample(source_alpha, dest_alpha, cols, 1, scale_factors);
	   source_alpha -= _strideGray;
	   dest_alpha -= _strideGray;
	   }
	x1  += mesh_width;
	x2  += mesh_width;
	}
// deallocate vert_splines
free(vert_splines->x_data);
free(vert_splines->y_data);
delete vert_splines;
//
// create horizontal splines splines
//
horz_splines = new wMesh();
horz_splines->width = cols;
horz_splines->height = mesh_height;
horz_splines->x_data = (double *) malloc(sizeof(double) * cols * mesh_height);
horz_splines->y_data = (double *) malloc(sizeof(double) * cols * mesh_height);
if ((horz_splines == 0) || (horz_splines->x_data == 0) || (horz_splines->y_data == 0))
   return;
//
// create table of y-intercepts for intermediate mesh's hor splines
//
x1 = (double *) source_mesh->x_data;
y1 = (double *) source_mesh->y_data;
y2 = (double *) horz_splines->x_data;
for (v = 0; v < mesh_height; v++)
	{
	// generate horizontal spline
	if (linear)
	   SplineLinear(x1, y1, index, y2, cols);
	else if (!SplineCatmullRom(x1, y1, mesh_width, index, y2, cols))
	   SplineLinear(x1, y1, index, y2, cols);
	// advance pointers to next row
	x1 += mesh_width;
	y1 += mesh_width;
    y2 += cols;
    }
//
// create table of y-intercepts for dst mesh's horizontal splines
//
x1 = (double *) dest_mesh->x_data;
y1 = (double *) dest_mesh->y_data;
y2 = (double *) horz_splines->y_data;
for (v = 0; v < mesh_height; v++)
	{
	// generate horizontal splines
	if (linear)
	   SplineLinear(x1, y1, index, y2, cols);
	else if (!SplineCatmullRom(x1, y1, mesh_width, index, y2, cols))
	   SplineLinear(x1, y1, index, y2, cols);
	// advance pointers to next row
	x1 += mesh_width;
	y1 += mesh_width;
    y2 += cols;
    }
// second pass: warp y
// initialize image pointers
source_bgr = (unsigned char *)_interImage->GetOriginalBuffer();
dest_bgr   = (unsigned char *)_dstImage->GetOriginalBuffer();
source_bgr += (rows - 1) * _strideRGB;
dest_bgr   += (rows - 1) * _strideRGB;
if (_srcAlpha)
   {
   source_alpha = (unsigned char*)_interAlpha->GetOriginalBuffer();
   dest_alpha   = (unsigned char*)_dstAlpha->GetOriginalBuffer();
   }
else
   {
   source_alpha = 0;
   dest_alpha   = 0;
   }
free(_in_position);
_in_position = (double *)malloc((rows + 1) * sizeof(double));
for (x = 0; x < cols; x++)
	{
	// initialize pointers for spline function
	xcol = (double *) horz_splines->x_data + x;
	ycol = (double *) horz_splines->y_data + x;
	// initialize xrow and yrow
	for (v = 0; v < mesh_height; v++)
		{
		xrow[v] = *xcol;
		yrow[v] = *ycol;
		xcol += cols;
		ycol += cols;
		}
	// generate scale factors for image resampling
	if (linear)
	   SplineLinear(xrow, yrow, index, scale_factors, rows);
	else if (!SplineCatmullRom(xrow, yrow, mesh_height, index, scale_factors, rows))
	   SplineLinear(xrow, yrow, index, scale_factors, rows);
	scale_factors[rows]=rows; // initialize last element
	// resample source column based on scale_factors
	for (k = 0; k < 3; k++)      // lopp across all channels
		FantResample(LPBYTE(source_bgr + k), LPBYTE(dest_bgr + k), rows, -_strideRGB, scale_factors);
	// advance pointers to next column
	source_bgr += 3;
	dest_bgr   += 3;
	// is there alpha channel
	if (source_alpha)
	   {
	   FantResample(source_alpha, dest_alpha, rows, -_strideGray, scale_factors);
	   source_alpha++;
	   dest_alpha++;
	   }
	}
// deallocate horz_splines
free(horz_splines->x_data);
free(horz_splines->y_data);
delete horz_splines;
// free dynamically allocated memory
free(index);
free(xrow);
free(yrow);
free(scale_factors);
free(interpolated);
free(_in_position);
_in_position = 0;
}
//---------------------------------------------------------------------------
// Fant resample. Unmodified !!!!
//---------------------------------------------------------------------------
//
//
//  Desc: performs resample of image row or column using Fant's resampling
//        algorithm presented in IEEE Computer Graphics & Applications Jan 86
//
//  Params: source- pointer to source image
//          len- length of row or column being processed
//          pixel_offset- number of pixels to next pixel in row or col
//          out_position- output coordinates for each input pixel
//          dest- pointer to destination image
//
//---------------------------------------------------------------------------
void TMeshWolberg::FantResample(LPBYTE source, LPBYTE dest,
				   int len, int pixel_offset, double *out_position)
{
int i, j, k;         // loop variables
BYTE current_pixel;   // pixel being processed
BYTE next_pixel;      // one pixel beyond current_pixel
double intensity;    // linearly interpolated pixel value
double sizfac;       // size factor
double inseg;        // part of current pixel to contribute
double outseg;       // how much required to complete next pixel
double accum;        // accumulator
// normalize scaling factors to integer pixel boundaries
i = 0;
for (j = 0; j < len; j++)
	{
	while (out_position[i+1] < j)
		 i++;
	_in_position[j] = i + (double) (j - out_position[i]) /
						 (out_position[i + 1] - out_position[i]);
	}
_in_position[len] = len;      // initialize last element
// initialize variables
inseg  = 1.0;
accum = 0.0;
outseg = _in_position[1];
sizfac = outseg;
current_pixel = *source;
source += pixel_offset;
next_pixel = *source;
source += pixel_offset;
i = 1;
while (i < len)
      {
      intensity = inseg * current_pixel + (1.0 - inseg) * next_pixel;
      if (inseg < outseg) // there is not sufficient current pixel
         {              // available to compute output pixel
         accum += (intensity * inseg);      // add inseg contribution
         outseg -= inseg;             // inseg portion now satisfied
         inseg = 1.0;                 // reinitialize to only 1 pixel
         current_pixel = next_pixel;
         next_pixel = *source;
		 source += pixel_offset; // increment to next pixel
		 }
	  else                // there is sufficient input pixel left to
		 {               // compute a new output pixel
		 accum += (intensity * outseg);   // add outseg contribution
		 accum /= sizfac;                 // normalize pixel
		 *dest = (int) std::min(accum, 255.0);  // insure no overflow
		 dest += pixel_offset;      // increment to dest pointer
         accum = 0.0;                     // reset accumulator
         inseg -= outseg;                 // outseg portion has been used
		 outseg = _in_position[i+1] - _in_position[i];
         sizfac = outseg;                 // set outseg
		 i++;
         }
      }
}
//---------------------------------------------------------------------------
// Linear spline - not spline at all...just a bunch of lines
//---------------------------------------------------------------------------
void TMeshWolberg::SplineLinear(double *knot_x, double *knot_y,
				double *spline_x, double *spline_y, int spline_length)
{
int i, index;           // loop variable and array index
double left_x, right_x;  // pointer to left and right of current interval
double left_y, right_y;  // pointer to left and right value
double frac_x;           // fraction x is within current knot interval

// intialize variables
index = 0;
left_x = knot_x[index];
left_y = knot_y[index];
right_x = knot_x[index+1];
right_y = knot_y[index+1];
//
for (i = 0; i < spline_length; i++)
    {
    if (spline_x[i] > knot_x[index])
       {
       left_x = knot_x[index];
       left_y = knot_y[index];
       index++;
       right_x = knot_x[index];
       right_y = knot_y[index];
       }
    frac_x = (spline_x[i] - left_x) / (right_x - left_x);
	spline_y[i] = left_y + frac_x * (right_y - left_y);
    }
}
//---------------------------------------------------------------------------
// Catumull-Rom spline - now, this is a real spline
//---------------------------------------------------------------------------
bool TMeshWolberg::SplineCatmullRom(double *x1, double *y1, int len1,
	 								double *x2, double *y2, int len2)
{
int i, j, dir, j1, j2;
double x,  dx1, dx2;
double dx, dy, yd1, yd2, p1, p2, p3;
double a0y, a1y, a2y, a3y;
// find direction of monotonic x1 -  skip ends
if(x1[0] < x1[1])
   {	// increasing
	  double t1, t2, t3, t4;
   if ((x1[0] - x2[0]) > 0.5 || (x2[len2-1] - x1[len1-1]) > 0.5)
   //if ((x2[0] < x1[0]) || (x2[len2-1] > x1[len1-1]))
	  {
	  dir = 0;
	  t1 = x2[0];
	  t2 = x1[0];
	  t3 = x2[len2-1];
	  t4 = x1[len1-1];
	  }
   else
	  dir = 1;
   }
else
   {    // decreasing
   if ((x2[0] - x1[0]) > 0.5 || (x1[len1-1]- x1[len2-1]) > 0.5)
   //if ((x2[0] > x1[0]) || (x2[len2-1]<x1[len1-1]))
	  dir = 0;
   else
	  dir = -1;
   }
if (dir == 0)  // error
   return false;
// p1 is first endpoint of interval
// p2 is resampling position
// p3 is second endpoint of interval
// j  is input index for current interval
//
// force coefficient initialization
//
if (dir == 1)
   p3 = x2[0] - 1;
else
   p3 = x2[0] + 1;
for (i = 0; i < len2; i++)
    {
    // check if in new interval
    p2 = x2[i];
    if (((dir == 1) && (p2 > p3)) || ((dir == -1) && (p2 < p3)))
	   {
       // find the interval which contains p2
	   if (dir)
		  {
		  for (j = 0; (j < len1) && (p2 > x1[j]); j++)
			  {
			  if (p2 < x1[j])
				 j--;
              }
		  }
	   else
		  {
		  for (j = 0; (j < len1) && (p2 < x1[j]); j++)
			  {
			  if (p2 > x1[j])
				  j--;
			  }
		  }
       p1 = x1[j];     // update 1st endpt
       p3 = x1[j+1];   // update 2nd endpt
       // clamp indices for endpoint interpolation
       //
	   j1 = std::max(j - 1, 0);
       j2 = std::min(j + 2, len1 - 1);
       //
       // compute spline coefficients
       //
       dx  = 1.0 / (p3 - p1);
       dx1 = 1.0 / (p3 - x1[j1]);
       dx2 = 1.0 / (x1[j2] - p1);
       dy  = (y1[j+1] - y1[ j ]) * dx;
       yd1 = (y1[j+1] - y1[ j1]) * dx1;
       yd2 = (y1[j2 ] - y1[ j ]) * dx2;
       a0y =  y1[j];
       a1y =  yd1;
       a2y =  dx *  ( 3*dy - 2*yd1 - yd2);
       a3y =  dx*dx*(-2*dy +   yd1 + yd2);
       }
    // use Horner's rule to calculate cubic polynomial
    x = p2 - p1;
    y2[i] = ((a3y*x + a2y)*x + a1y)*x + a0y;
    }
return true;
}




