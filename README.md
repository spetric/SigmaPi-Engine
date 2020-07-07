# SigmaPi Engine
## Drawing and Image Processing Engine   
SigmaPi Engine or shortly spEngine is library (DLL) mainly intended to be used with Embarcadero C++/Delphi on Windows platform.
This library is main drawing and image processing engine of Pixopedia XE painter and image editor:
https://www.sigmapi-design.com/pixopedia/

Besides native code, engine also uses a subset of Intel® IPP (Intel® Integrated Performance Primitives), packed in custom DLL. Using IPP is optional and if engine can not find ipp_custom.dll all APIs involving IPP processing routines will return false, indicating IPP is not installed. With little modification you can use a complete set of IPP DLLs instead of single custom DLL. Tools for creating cusom IPP DLL can be found in ipp_custom_def directory.  

## Supported image formats
Similar to LIPS engine (https://github.com/spetric/Lips), spEngine supports RGB24 bitmap/image container. Only width, height, scanline alignement and pointer to image buffer are passed to spEngine. Also, spEngine supports RGB24 images with separated alpha channel. Support for RGBA32 can be added as well, but currently it's not supported.  

