//---------------------------------------------------------------------------
#ifndef SpeGlobalsH
#define SpeGlobalsH
#include <Classes.hpp>
typedef void (__stdcall *TCbBrushMask)(void *, TRect *);
// action/rendering types
enum TspeActionType {spe_actDraw = 0, spe_actColor, spe_actShift, spe_actFilter33, spe_actSpread, spe_actFilterIpp, spe_actColorTwistIpp,
					 spe_actCompoundIpp, spe_actNozzle, spe_actMultiDraw};
// shapes
enum TspeShapeType {spe_shpNone = 0, spe_shpFreeHand, spe_shpLine, spe_shpEllipse, spe_shpRectangle,
					spe_shpPoly, spe_shpSplineCR};
// brush types
enum TspeBrushType {spe_brshNeon = 0, spe_brshSolid, spe_brshSpray, spe_brshStar, spe_brshHair,
					spe_brshMesh, spe_brshParallel, spe_brshStamp, spe_brshCml, spe_brshPump, spe_brshUndefined};
// cloning styles
enum TspeCloneType {spe_cltExternal = 0, spe_cltDirect, spe_cltBuffered};
// color schemes
enum TspeColorScheme  {spe_csPrimary = 0, spe_csSecondary, spe_csMixed, spe_csSolidMixed, spe_csPaletteAlpha, spe_csPaletteSolid,
					  spe_csPaletteStepC, spe_csPaletteStepFB, spe_csPaletteStepStop, spe_csPaletteRandom,
					  spe_csTextureAligned, spe_csTextureFixed, spe_csCloneAligned, spe_csCloneRepos, spe_csCloneFixed,
					  spe_csMouseDown, spe_csImageStep,  spe_csAbsque, spe_csSidekick,
					  spe_csGradienStepC, spe_csGradientStepFB, spe_csGradientStepStop,
					  spe_csShortPalAlpha, spe_csShortPalSolid,
					  spe_csShortPalStepC, spe_csShortPalStepFB, spe_csShortPalStepStop, spe_csShortPalRandom,
					  spe_csCloneStep, spe_csMarker };
// pixel arithmetics - actions: drawing, pixels shifting, spread
enum TspeActDrawType {spe_ARITH_NORM = 0, spe_ARITH_SCALED, spe_ARITH_ERASE, spe_ARITH_XOR, spe_ARITH_AND,
					  spe_ARITH_OR, spe_ARITH_ADD, spe_ARITH_SUB, spe_ARITH_MULT, spe_ARITH_MULT_nA, spe_ARITH_MULT_nB,
					  spe_ARITH_HIGHER, spe_ARITH_LOWER, spe_ARITH_MEAN, spe_ARITH_MINMAX, spe_ARITH_RANDOM,
					  spe_ARITH_EQUALIZE, spe_ARITH_BURNER, spe_ARITH_INTENSITY_B, spe_ARITH_INTENSITY_A, spe_ARITH_A_MOD_B,
					  spe_ARITH_AxB, spe_ARITH_BURN_LINEAR, spe_ARITH_SQRT_AB, spe_ARITH_ABS_SUB, spe_ARITH_EXCLUSION,
					  spe_ARITH_BUMP, spe_ARITH_BUMP_COLOR};
// color action
enum TspeActColorType {spe_COLOR_BRIGTHNESS = 0, spe_COLOR_STRETCH, spe_COLOR_HSV, spe_COLOR_LMH, spe_COLOR_GAMMA,
					   spe_COLOR_EXCHNAGE, spe_COLOR_MAX, spe_COLOR_MIN, spe_COLOR_MIN_MAX, spe_COLOR_CAST,
					   spe_COLOR_SCALE, spe_COLOR_TEMPERATURE, spe_COLOR_TONING, spe_COLOR_CURVES, spe_COLOR_CORRECTION};
// spread action
enum TspeActSpreadType {spe_SPREAD_SPREAD = 0, spe_SPREAD_SMUDGE, spe_SPREAD_SPREAD_MC, spe_SPREAD_SHUFFLER,
						spe_SPREAD_WARPER };
// nozzle action
enum TspeActNozzleType {spe_NOZZLE_RANDOM = 0, spe_NOZZLE_SEQUENCE, spe_NOZZLE_SELECTED};
// warp action
enum TspeActWarpType {spe_WARP_SMEAR = 0, spe_WARP_MOVE, spe_WARP_BLOB, spe_WARP_BLUB, spe_WARP_RIPPLES, spe_WARP_LONG_RIPPLE,
					  spe_WARP_BALLOON, spe_WARP_DOUBLE_BALLOON, spe_WARP_SPLASH, spe_WARP_INSIDE_TWIRL, spe_WARP_OUTSIDE_TWIRL,
					  spe_WARP_FULL_TWIRL, spe_WARP_DISORDER, spe_WARP_MOTION, spe_WARP_WAVES_HORIZONTAL, spe_WARP_WAVES_VERTICAL,
					  spe_WARP_EXPLODE, spe_WARP_IMPLODE, spe_WARP_QUADRANGLE, spe_WARP_MESH };
// ipp filters action
enum TippFilterType {ipp_BLUR = 0, ipp_MEDIAN, ipp_BILATERAL, ipp_GAUSSIAN, ipp_MAX, ipp_MIN, ipp_MEDIAN_CROSS_XX,
					 ipp_MEDIAN_COLOR_XX, ipp_HIPASS_XX, ipp_LAPLACE_XX, ipp_SHARPEN_XX, ipp_WIENER,
					 ipp_DILATE, ipp_ERODE, ipp_CLOSE, ipp_OPEN, ipp_MORPH_TOPHAT, ipp_MORPH_GRADIENT,
					 ipp_PREWITT_VERT, ipp_PREWITT_HOR, ipp_SOBEL_VERT, ipp_SOBEL_HOR, ipp_GAUSS_XX, ipp_GENERAL_FILTER };
// ipp compound action
enum TippCompoundType {ipp_CMP_MAX_SUB_MIN = 0, ipp_CMP_MINMAX_CF, ipp_CMP_MEDIAN_MINMAX_CF, ipp_CMP_BLUR_MINMAX_CF,
					   ipp_CMP_BLUR_MEDIAN_CF, ipp_CMP_ERODE_DILATE_CF, ipp_CMP_MEDIAN_ERODE_DILATE_CF, ipp_CMP_UNSHARP_MASK};
// ipp quad transform
enum TippQuadTransType {ipp_QT_PERSPECTIVE = 0, ipp_QT_BILINEAR, ipp_QT_AFFINE};
// ipp arithmetic operations
enum TippArithBool {ipp_ADD = 0, ipp_SUB, ipp_MULT, ipp_DIV};
// ipp moment type
enum TippMomentType {ipp_MOMENT_SPATIAL = 0, ipp_MOMENT_CENTRAL, ipp_MOMENT_SPATIAL_NORM, ipp_MOMENT_CENTRAL_NORM };
// ipp norm type
enum TippNormType {ipp_NORM_INF = 0, ipp_NORM_L1, ipp_NORM_L2 };
// ipp color conversion
enum TippCoCoType {ipp_RGB2HSV = 0, ipp_HSV2RGB, ipp_RGB2XYZ, ipp_XYZ2RGB, ipp_RGB2YUV, ipp_YUV2RGB, ipp_RGB2YCbCr, ipp_YCbCr2RGB,
				   ipp_RGB2Lab, ipp_Lab2RGB, ipp_RGB2HLS, ipp_HLS2RGB };
// arith - bool const value ops
enum TspeArithBool {spe_AB_Add = 0, spe_AB_Sub, spe_AB_Mult, spe_AB_Div, spe_AB_And, spe_AB_Or, spe_AB_Xor};
// apply on image modes - Pixopedia: four cats
enum TspeApplyType {spe_appImage = 0, spe_appSelection, spe_appContour, spe_appMask };
// image types
enum TspeImageType {spe_it_24RGB = 0, spe_it_8Gray, spe_it_8Float};
// internal mask type
enum TspeInternalMaskType {spe_RightMask = 0, spe_LeftMask};
// stroke events
const BYTE spe_SE_DOWN  = 0x01;
const BYTE spe_SE_MOVE  = 0x02;
const BYTE spe_SE_UP    = 0x04;
const BYTE spe_SE_PICK_COLOR = 0x08;
const BYTE spe_SE_RESET_CC = 0x10;
const BYTE spe_SE_FLUSH	= 0x20;
// color space scaling
const int spe_SP_NORM           = 4194304;     // 2 ^ 22
const int spe_SP_SHIFT          = 22;
// default scanline alignement
const int spe_SP_ALIGNMENT      = 4;
// luminance b, g, r
const float spe_F_LUMINANCE[3] = {0.11, 0.59, 0.30};
const double spe_D_LUMINANCE[3] = {0.11, 0.59, 0.30};
//
const TspeActDrawType spe_DRAW_ARITH[26]    =  {spe_ARITH_NORM, spe_ARITH_SCALED, spe_ARITH_ERASE, spe_ARITH_XOR, spe_ARITH_AND, spe_ARITH_OR,
							   spe_ARITH_ADD, spe_ARITH_SUB, spe_ARITH_MULT, spe_ARITH_MULT_nA, spe_ARITH_MULT_nB,
							   spe_ARITH_HIGHER, spe_ARITH_LOWER, spe_ARITH_MEAN, spe_ARITH_MINMAX, spe_ARITH_RANDOM,
							   spe_ARITH_EQUALIZE, spe_ARITH_BURNER, spe_ARITH_INTENSITY_B, spe_ARITH_INTENSITY_A, spe_ARITH_A_MOD_B,
							   spe_ARITH_AxB, spe_ARITH_BURN_LINEAR, spe_ARITH_SQRT_AB, spe_ARITH_ABS_SUB, spe_ARITH_EXCLUSION
							   };
const TspeActDrawType  spe_SHIFT_ARITH[26]   =  {spe_ARITH_NORM, spe_ARITH_XOR, spe_ARITH_AND, spe_ARITH_OR, spe_ARITH_ADD, spe_ARITH_SUB,
							   spe_ARITH_MULT, spe_ARITH_MULT_nA, spe_ARITH_MULT_nB, spe_ARITH_HIGHER, spe_ARITH_LOWER,
							   spe_ARITH_MEAN, spe_ARITH_MINMAX, spe_ARITH_RANDOM, spe_ARITH_EQUALIZE, spe_ARITH_BURNER,
							   spe_ARITH_INTENSITY_B, spe_ARITH_INTENSITY_A, spe_ARITH_A_MOD_B, spe_ARITH_AxB, spe_ARITH_BURN_LINEAR,
							   spe_ARITH_SQRT_AB, spe_ARITH_ABS_SUB, spe_ARITH_EXCLUSION, spe_ARITH_BUMP, spe_ARITH_BUMP_COLOR
							   };
const TspeActDrawType  spe_SPREAD_ARITH[24]   =  {spe_ARITH_NORM, spe_ARITH_XOR, spe_ARITH_AND, spe_ARITH_OR,
							   spe_ARITH_ADD, spe_ARITH_SUB, spe_ARITH_MULT, spe_ARITH_MULT_nA, spe_ARITH_MULT_nB,
							   spe_ARITH_HIGHER, spe_ARITH_LOWER, spe_ARITH_MEAN, spe_ARITH_MINMAX, spe_ARITH_RANDOM,
							   spe_ARITH_EQUALIZE, spe_ARITH_BURNER, spe_ARITH_INTENSITY_B, spe_ARITH_INTENSITY_A, spe_ARITH_A_MOD_B,
							   spe_ARITH_AxB, spe_ARITH_BURN_LINEAR, spe_ARITH_SQRT_AB, spe_ARITH_ABS_SUB, spe_ARITH_EXCLUSION
							   };
//
const int  spe_K33I[9]		     =  {-1, -1, -1, 0, 0, 0, 1, 1, 1};
const int  spe_K33J[9]			 =  {-1, 0, 1, -1, 0, 1, -1, 0, 1};
// rendering modes
const int  spe_RENDER_AUTOMATIC     = 0;
const int  spe_RENDER_SLOW          = 1;
const int  spe_RENDER_MEDIUM        = 2;
const int  spe_RENDER_FAST          = 3;
const int  spe_RENDER_DUMMY         = 4;
// 0.5 - scrath types
const int spe_SCRATCH_NONE = 0;
const int spe_SCRATCH_CIRCLE = 1;
const int spe_SCRATCH_SQUARE = 2;
const int spe_SCRATCH_SHORT_LINE = 3;
const int spe_SCRATCH_LONG_LINE = 4;
// 0.7 structure
struct Tspe_RenderParams
   {
   //TspeRenderType RenderType;
   bool ClearImoc;
   int  PostMaskingMode;
   bool AutoCommit;
   //bool AddUndo;   // change to createUndo -> creates undo block
   bool CreateUndo; // create undo buffer
   int RenderingMode;
   bool DoShadow;
   bool DrawPolyInterior;
   bool DrawTipOnMouseDown;
   bool SelectionClipping;    // selection clipping
   bool ContourClipping;	  // contour clipping
   unsigned int ContourClippingThreshold;
   unsigned int ContourClippingRadius;
   bool AssistantMaskUpdate;
   bool AssistantMaskLastShapeOnly;
   bool MultiDrawResetBoundRect;
   int  MixSurf;
   int  ArithSurf;
   };
struct Tspe_DrawParams
   {
   int MainMode;
   int SaveMode;
   int Shape;    			// remove it - it's part of host application
   bool InvertSource;
   int BitShift;
   int ScaleTopRGB[3];
   int ScaleBottomRGB[3];
   bool UpdateAlpha;
   int  StrokesNumber;      // for undo buffer
   bool EraseMatchAll;
   int EraseMatchColorTolerance;
   unsigned int	EraseMatchColor;
   unsigned int EraseReplaceColor;
   //bool EraseCustom;
   // 0.4
   bool DrawOnAlpha;
   };
struct Tspe_ColorParams
   {
   int  MainMode;
   bool ChanME[3];
   bool CPercentage;
   int  ChanRGB[3];
   float ChanGamma[3];
   bool CastOn[3];
   int CastRange[2];
   unsigned int CastColor[3];
   int ScaleTopRGB[3];
   int ScaleBottomRGB[3];
   double Temperature;
   int ToneType; // 0 - selected color, 1 - palette, 2 - short pal
   unsigned int ToneColor;
   bool TonePreserveLuminance;
   //TColorCorrection *ColorCorrection;
   };
struct Tspe_Filter33Params
   {
   int MainMode;
   bool Wrap;
   bool SyntaxOk;
   bool Chan[3];
   int Mode;
   int Logic[9];
   int LogicSign[9];
   int Kernel[9];
   int Bias;
   int Div;
   };
struct Tspe_FilterIppParams
   {
   int MainMode;
   int BorderType;
   bool Chan[3];
   //int Iterations;
   int RadiusX;   // for blur, median, etc
   int RadiusY;   //
   int Radius;    // gaussian
   int KernelX;   // kernel width
   int KernelY;   // kernel height
   int AnchorX;   // non-centered filters (Wienner)
   int AnchorY;   // non-centered filters (Wienner)
   float NoiseLevel;
   float SigmaColor;
   float SigmaSpace;
   float StDev;
   bool Fixed33;	// otherwise 55
   int *MorphMask;
   // general filter - arbitraty convolutions (user defined kernel)
   int GeneralRows;
   int GeneralColumns;
   signed short GeneralKernel[900];    // if you need more then 30x30 kernel adjust the size!!!
   int GeneralDiv;
   int GeneralBias;
   };
struct Tspe_CompoundIppParams
   {
   int MainMode;
   int BorderType;
   bool Chan[3];
   int RadiusX;
   int RadiusY;
   int KernelX;
   int KernelY;
   int LoVal;
   int HiVal;
   int RangeType;
   float KFactor;
   int CDMode;
   //TODO: user defined compound filter
   };
struct Tspe_ColorTwistIppParams
   {
   int MainMode;
   bool SyntaxOk;
   int Scale;     // div
   int Bias;
   float Matrix[12];
   };
struct Tspe_ShadowParams
   {
   int MainMode;
   int Xoffset;
   int Yoffset;
   bool BlurOn;
   int BlurValue;
   int Capacity;
   unsigned int Color;
   };
struct Tspe_ShiftParams
   {
   int  MainMode;
   int X_shift;
   int Y_shift;
   float BumpScale;
   int BitShift;
   int ScaleTopRGB[3];
   int ScaleBottomRGB[3];
   bool InvertSource;
   int ZeroShiftRandom;
   };
struct Tspe_SpreadParams
   {
   int MainMode;
   //  input/output blending
   int BlendIn;
   int BlendOut;
   // simple spread
   int SpreadArithMode;
   int SpreadColorPickMode;  // 0 - mouse down, 1 - mosue move, 2 - step
   // mulitcolor mode
   int SpreadMultiColorMode; // [0, 7]
   bool SpreadMultiColorRandomWalker;
   int SpreadMultiColorRandomCount;
   // shuffler
   int ShufflerMode;       	 // [0, 4]
   int ShufflerDistance;
   bool ShufflerRandomDistance;
   int ShufflerGroupSize;
   int ShufflerLuminanceMin;
   int ShufflerLuminanceMax;
   int ShufflerSortGroup;
   // spread warping params
   int WarpColorPickMode;
   int WarpCount;
   bool WarpOn[5];
   int WarpOrder[5];
   int WarpParam1[5];
   double WarpParam2[5];
   bool WarpSameRange;
   bool WarpMaskSoft;
   bool WarpMaskHard;
   bool PickCustomPrim;
   bool PickCustomSec;
   TPoint CustomPrimPick;
   TPoint CustomSecPick;
   // clone pos
   //int Clone_XS;
   //int Clone_YS;
   // calculated params
   //bool cpSpreadPickRandom;
   };
struct Tspe_NozzleParams
   {
   int MainMode; 	  // 0 - selected cell, 1 - cell in sequence, 2 - random cell (best)
   int Step;   		  // spacing  - overrides brush step
   int Capacity;      // capacity - overrides brush capacity
   float CellWidth;	  // single cell width (all cells have the same width)
   float CellHeight;  // single cell height (all cells have the same height)
   int StartCell;  	  // selected cell number
   int Rows;
   int Cols;
   int CurrentCell;	  // current cell number
   //int CellsCount;	  // number of cells
   };
struct Tspe_WarpParams
   {
   int MainMode;
   bool Antialias;
   bool Chan[3];
   int Capacity;
   int StrengthMode;
   int Strength;
   float BqFactor;
   int SmearMode; 		// spread function
   int QuadTransform;
   int TilesX;         //TODO: remove tiles and put them in host program
   int TilesY;
   int MeshConMode;
   int SelectionBound;
   int MeshRows;
   int MeshCols;
   TPoint *MeshSrc;
   TPoint *MeshDst;
   };
struct Tspe_QuadIppParams
   {
   int MainMode;	// transformation type
   TRect RoiRect;
   TPoint Quad[4];
   bool Forward;
   int Interpolation;
   bool SmoothEdges;
   };
struct Tspe_BrushCommonParams
   {
   // brush index (inside list)
   // int ListIndex;  // internal use
   // brush type
   TspeBrushType Type;
   // brush tip
   int Size;
   int Capacity;
   int ColorVary;
   int SizeVary;
   int CapVary;
   int Step;
   // filters
   bool ChalkOn;
   bool InvertOn;
   int  InvertValue;
   bool BlurOn;
   int BlurValue;
   bool MedianOn;    // external - n/a
   int MedianValue;  // external - n/a
   // bump
   bool BumpOn;
   bool BumpSoft;
   float BumpScaleValue;
   int  BumpThresholdValue;
   int  BumpRoughnessValue;
   int BumpXoffsetValue;
   int BumpYoffsetValue;
   bool BumpInvertThreshold;
   // envelopes -> 0.4
   bool EnvelopeSizeOn;
   bool EnvelopeCapacityOn;
   bool EnvelopeSizeRepeat;
   bool EnvelopeCapacityRepeat;
   int  EnvelopeResolution;
   bool EnvelopeResolutionManual;
   // jitter
   bool JitterOn;
   bool JitterConnectedOn;
   int JitterRangeValue;
   int JitterLoopValue;
   // polygon
   bool PolyFillOver;
   bool PolyExcludeFilters;
   bool PolyExcludeBump;
   int  PolyColorIndex;
   bool PolyOverrideCapacityOn;
   int  PolyCapacityValue;
   bool PolyBlurOn;
   int  PolyBlurValue;
   // scratches
   int ScratchType;
   int ScratchThicknessValue;
   int ScratchNumberValue;
   // pal-grad stepper
   float PalGradStepper;
   bool  PalGradAuto;
   // colors
   unsigned int PrimaryColor;
   unsigned int SecondaryColor;
   unsigned int PolyColor;
   unsigned int CustomColorPrim;
   unsigned int CustomColorSec;
   TspeColorScheme SchemeIndex;
   };
// specific params
struct Tspe_BrushNeonSpecific
   {
   int FalloffIndex;
   int LinearValue;
   int TrigonometricValue;
   };
struct Tspe_BrushSolidSpecific
   {
   int Shape;
   int Thickness;
   bool Filled;      // 0.6
   float Angle;		 // 0.6
   bool RandomAngle; // 0.6
   int BlurRadius;
   };
struct Tspe_BrushSpraySpecific
   {
   int Thickness;
   int IntVary;
   int Density;
   float Gravity;
   };
struct Tspe_BrushStarSpecific
   {
   int Thickness;
   int IntVary;
   int Density;
   };
struct Tspe_BrushMeshSpecific
   {
   int Thickness;
   int IntVary;
   int Density;
   };
struct Tspe_BrushHairSpecific
   {
   int Thickness;
   int IntVary;
   int Density;
   int BlurRadius;
   };
struct Tspe_BrushParallelSpecific
   {
   int Shape;
   int Thickness;
   int IntVary;
   int Spacing;
   int Angle;
   int BlurRadius;
   };
struct Tspe_BrushStampSpecific
   {
   int AngleMode;
   int AngleValue;
   unsigned long StampId;
   };
struct Tspe_BrushCmlSpecific
   {
   int MinIntensity;
   int MaxIntensity;
   int SeedIntensity;
   int LatticeType;
   int CreationOps[9];
   int DestructionOps[9];
   float EpsilonGrow;
   float EpsilonDecay;
   int Iterations;
   int CreateProbability;
   int DecayProbability;
   int GrowProbability;
   bool Reborn;
   bool MatrixMes;
   bool DelayCreation;
   bool RandomSplat;
   bool SequentialScan;
   int  GrowFunc;
   int  DecayFunc;
   float EpsilonWeight;
   int CreationMutation;
   int DestructionMutation;
   };
struct Tspe_BrushPumpSpecific
   {
   int Mode;       // sequential, random, selected sequential, selected random
   int Index;
   bool ResetIndex;
   int Count;
   };
struct Tspe_MouseData
   {
   int X_down;
   int Y_down;
   int X_up;
   int Y_up;
   int X_move_from;
   int Y_move_from;
   int X_move_to;
   int Y_move_to;
   int X_min;
   int Y_min;
   int X_max;
   int Y_max;
   int X_wrap_from;
   int Y_wrap_from;
   int X_wrap_to;
   int Y_wrap_to;
   bool MouseButtonDown;
   TShiftState Shift;
   };
struct Tspe_Geometry
   {
   bool SimplifyPolyline;
   int  SimplifyAlgorithm;    // Douglass-Poucker for now
   int  SimplifyDPTolerance;
   int  SimplifyMaxPts;		  // 0 by default
   int  PolygonModification;
   double RotationAngle;
   TPoint RotationOrigin;
   bool Filled;
   bool Border;
   bool PolyClosed;			// polygon
   };
struct Tspe_UndoBlock
   {
   void *Buffer;
   int Size;
   int Stride;
   TRect Rect;
   bool CompleteImage;		// puts true if undo block containes complete image, otherwise false
   // alpha channel
   void *AlphaBuffer;
   int AlphaSize;
   int AlphaStride;
   };
struct Tspe_Tessellation
   {
   // input
   int DisplayType;			// 0 - voronoi, 1 - delaunay solid, 2 - delaunay with gouraud shading
   int Iterations;
   int SitesMin;
   int SitesMax;
   float MinSitesDistance; 	// min distance between sites -> default 2.2
   int LevelError;			// default 16
   int EdgeType;			// 0 - no edges, 1 - custom color edges, 2 - vertex color, 3 - brush edges (mask)
   int EdgeBlurRadius;		// 1 by default
   unsigned int EdgeColor;
   int EdgeWidth;
   // output
   int CandidateSites;
   int AcceptedSites;
   int DiscardedSites;
   };
#endif
