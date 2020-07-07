# SigmaPi Engine
## Drawing and Image Processing Engine   
SigmaPi Engine or shortly spEngine is library (DLL) mainly intended to be used with Embarcadero C++/Delphi on Windows platform.
This library is main drawing and image processing engine of Pixopedia XE painter and image editor:
https://www.sigmapi-design.com/pixopedia/

Besides native code, engine also uses a subset of Intel® IPP (Intel® Integrated Performance Primitives), packed in custom DLL. Using IPP is optional and if engine can not find ipp_custom.dll all APIs involving IPP processing routines will return false, indicating IPP is not installed. With little modification you can use a complete set of IPP DLLs instead of single custom DLL. Tools for creating cusom IPP DLL can be found in ipp_custom_def directory.  

## Supported image formats
Similar to LIPS engine (https://github.com/spetric/Lips), spEngine supports RGB24 bitmap/image container. Only width, height, scanline alignement and pointer to image buffer are passed to spEngine. Also, spEngine supports RGB24 images with separated alpha channel. Support for RGBA32 can be added as well, but currently it's not supported.  

## Drawing and Image processing capabilites
### Graphics primitives
Free hand drawing, line, polyline, polygon, spline, closed spline, rectangle, ellipse, square, circle. All closed shapes can be rotated.
Strokes can be filtered, jittered, bumped (simulating 3D using height field). Stroke envolopes and stroke scratchings are supported.
### Brushes
Neon, solid, hair, star, mesh, parallel, spray, stamp (PNG brushes), pump, CML. Brush can be changed on th fly. Brush intensity and size variations are supported.
### Arithmetics
Drawing can be performd using various arithmetic and boolean operations. Drawing also can be perform using grayscale masks. Contour clipping is also supported.
### Color schemes
Various color schemes are supported: primary color, secondary color, mixed primary/secondary, color palette, color texture, background tiles, clone brushing.
### Image warping
spEngine supports a big collection of image warping algorithms.
### Image filtering-processing
Various native 3x3 filters, IPP filters (blur, median, bilateral, morphological filters, Gaussian, Laplacian, etc...). Voronoi diagram and Daulanay triangulation is supported. IPP color twisting and convolution (cross-correlation), histogram, remaping, afine, bilinear and perspective transformations, etc... Note: all filtes can be applied on complete image, on some ROI, through the grayscale mask and under the brush stroke (including free hand strokes).




