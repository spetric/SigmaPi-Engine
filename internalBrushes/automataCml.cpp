//---------------------------------------------------------------------------
#pragma hdrstop
#include "automataCml.h"
#include "ASMVars.h"
#include <math.h>
#include <algorithm>
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
#pragma link "float2int"
// constructor
TAutomataCml::TAutomataCml(void)
{
std::srand(time(NULL));
_surfaceRight = 0;
_surfaceLeft = 0;
_fpInput = 0;
_fpOutput = 0;
// test
_fpIntermediate = 0;
Mask = 0;
_controlMask = 0;
_mutator[0] = nopOps;
_mutator[1] = rotateOps;
_mutator[2] = shuffleOps;
_boundRect = 0;
_ijArray = 0;
resetLogicContainer();
}
//---------------------------------------------------------------------------
// destructor
TAutomataCml::~TAutomataCml(void)
{
//
if (_fpInput)
   {
   delete _fpInput;
   delete _fpOutput;
   delete Mask;
   delete _controlMask;
   }
if (_fpIntermediate)
   delete _fpIntermediate;
if (_ijArray)
   delete []_ijArray;
//if (_dataKeeper)
//   _dataKeeper->RenderParams->UpdateImage = true;
}
//---------------------------------------------------------------------------
void TAutomataCml::addLogicContainer(structCmlLogic *logic, int index, bool v1)
{
//logic[index].Container[logic[index].Count++] = v1;
logic[index].Container[logic[index].Count++] = v1 ? 0x01 : 0x00 ;
}
//---------------------------------------------------------------------------
void TAutomataCml::resetLogicContainer(void)
{
for (int i = 0; i < 5; i++)
	{
	_createLogic[i].Count = 0;
	_createLogic[i].Container[0] = 0x01;
	//_createLogic[i].Can = 0x80;
	_destructLogic[i].Count = 0;
	_destructLogic[i].Container[0] = 0x01;
	//_destructLogic[i].Can = 0x80;
	}
//_createLogic[0].Container[0] = _createLogic[1].Container[0] =  true;
//_destructLogic[0].Container[0] = _destructLogic[1].Container[0] = true;
//_createLogic[2].Container[0] = _createLogic[3].Container[0] =  false;
//_destructLogic[2].Container[0] = _destructLogic[3].Container[0] = false;
}
//---------------------------------------------------------------------------
bool TAutomataCml::calculateLogicContainer(structCmlLogic *logic)
{
int i;
// OR
for (i = 1; i < logic[0].Count; i++)
	logic[0].Container[0] = logic[0].Container[0] | logic[0].Container[i];
logic[0].Result = (bool)logic[0].Container[0];
// AND
for (i = 1; i < logic[1].Count; i++)
	logic[1].Container[0] = logic[1].Container[0] & logic[1].Container[i];
logic[1].Result = (bool)logic[1].Container[0];
// XOR
for (i = 1; i < logic[2].Count; i++)
	logic[2].Container[0] = logic[2].Container[0] + logic[2].Container[i];
logic[2].Result = (bool)(logic[2].Container[0] & 0x01);
// NAND
for (i = 1; i < logic[3].Count; i++)
	logic[3].Container[0] = logic[3].Container[0] + logic[3].Container[i];
logic[3].Container[0] = (logic[3].Container[0] >> 1);
logic[3].Result = !(bool)logic[3].Container[0];
// TRUE - always true
logic[4].Result = true;
// Result
return logic[0].Result && logic[1].Result && logic[2].Result && logic[3].Result && logic[4].Result;
}
//---------------------------------------------------------------------------
void TAutomataCml::clearMask(void)
{
if (_boundRect)
   {
   if (_boundRect->Width() == 0 || _boundRect->Height() == 0)
	  return;
   Mask->SetRoi(_boundRect);
   Mask->ZeroRoi();
   }
else
   Mask->Zero();
}
//---------------------------------------------------------------------------
void TAutomataCml::swapImages(void)
{
_fpTrans = _fpInput;
_fpInput = _fpOutput;
_fpOutput = _fpTrans;
}
//---------------------------------------------------------------------------
void TAutomataCml::drawContainerMask(void)
{
int i, j;
BYTE mByte, outByte;
for (i = _boundRect->top; i < _boundRect->bottom; i++)
	{
	for (j = _boundRect->left; j < _boundRect->right; j++)
		{
		mByte = Mask->Pixel(i, j).ch[0];
		if (!mByte)
		   continue;
		outByte = (BYTE)fast_ftol(_fpOutput->Pixel(i, j).ch[0] * 255.0);
		_surfaceRight->Pixel(i, j).ch[0] = outByte;
		}
	}
}
//---------------------------------------------------------------------------
void TAutomataCml::nopOps(int *ops)
{
// Nada
}
//---------------------------------------------------------------------------
void TAutomataCml::rotateOps(int *ops)
{
int last = ops[7];
for (int i = 7; i > 0; i--)
	ops[i] = ops[i-1];
ops[0] = last;
}
//---------------------------------------------------------------------------
void TAutomataCml::shuffleOps(int *ops)
{
std::random_shuffle( &ops[0], &ops[8]);
}
//---------------------------------------------------------------------------
void TAutomataCml::modifyWithNeighbour(float epsilon)
{
_output_value = _input_value + epsilon * _neighbour_cumul;
}
//---------------------------------------------------------------------------
void TAutomataCml::modifyWithNeighbourInverse(float epsilon)
{
_output_value = _input_value + epsilon * (8.0 - _neighbour_cumul);
}
//---------------------------------------------------------------------------
void TAutomataCml::modifyWithInputLinear(float epsilon)
{
_output_value = _input_value + epsilon * _input_value;
}
//---------------------------------------------------------------------------
void TAutomataCml::modifyWithInputSquared(float epsilon)
{
_output_value = _input_value + epsilon * (_input_value * _input_value * 10.0);
}
//---------------------------------------------------------------------------
void TAutomataCml::exploreNeighbour(int i, int j)
{
float neighbour_value;
bool neighbour;
int si, sj, k;
_neighbour_cumul = 0.0;
for (k = 0; k < 8; k++)
	{
	si = i + N33I[k];
	sj = j + N33J[k];
	_fpInput->SetSafeIndex(&si, &sj);
	neighbour_value = _fpInput->Pixel(si, sj).ch[0];
	_neighbour_cumul += neighbour_value;
	neighbour =  (bool)(neighbour_value > OPS_tolerance) && !(_controlMask->Pixel(si, sj).ch[0] & 0x01);
	addLogicContainer(_createLogic, _creationOps[k], neighbour);
	addLogicContainer(_destructLogic, _destructionOps[k], neighbour);
	}
}
//---------------------------------------------------------------------------
void TAutomataCml::cumulateNeighbour(int i, int j)
{
int si, sj, k;
_neighbour_cumul = 0.0;
for (k = 0; k < 8; k++)
	{
	si = i + N33I[k];
	sj = j + N33J[k];
	_fpInput->SetSafeIndex(&si, &sj);
	_neighbour_cumul += _fpInput->Pixel(si, sj).ch[0];
	}
}
//---------------------------------------------------------------------------
void TAutomataCml::cumulateNeighbourNz(int i, int j)
{
int si, sj, k;
int neighbour_nz_count = 0;
float neighbour_value;
_neighbour_cumul = 0.0;
for (k = 0; k < 8; k++)
	{
	si = i + N33I[k];
	sj = j + N33J[k];
	_fpInput->SetSafeIndex(&si, &sj);
	neighbour_value = _fpInput->Pixel(si, sj).ch[0];
	_neighbour_cumul += neighbour_value;
	neighbour_nz_count += (bool)(neighbour_value > OPS_tolerance) ? 1 : 0;
	}
_neighbour_factor = neighbour_nz_count > 0 ? 1.0 / neighbour_nz_count : 0;
}
//---------------------------------------------------------------------------
bool TAutomataCml::getProbability(int prob)
{
return prob == 100 ? true : prob == 0 ? false : random(101) < prob;
}
//---------------------------------------------------------------------------
bool TAutomataCml::doDestruction(int i, int j)
{
bool lRet =  (_controlMask->Pixel(i, j).ch[0] & 0x02);
if (lRet)
   {
   if (calculateLogicContainer(_destructLogic) && getProbability(_decayProbability))
	  {
	  _modifyValueDecay(_epsilonDecay);
	  _surfaceLeft->Pixel(i, j).ch[0] = 0x00;
	  if (_output_value < _minIntensity)
		 {
		 _output_value = _minIntensity;
		 if (_reBorn)
			_controlMask->Pixel(i, j).ch[0] = 0x00;
		 }
	  }
   }
/*
else
   {
   doGrow();
   if (_output_value == _maxIntensity)
	  {
	  _controlMask->SpSurface->DirectByte[i][j] = 0x01;
	  for (int k = 0; k < 3; k++)
		  _imageContainer->ImageOrig->SpSurface->DirectBGR[i][j].bgr[k] = _imageContainer->ImageEdit->SpSurface->DirectBGR[i][j].bgr[k];
	  // alpha
	  if (_imageContainer->ImageOrig->SpImage->alpha_map)
		  _imageContainer->ImageOrig->SpSurface->DirectAlpha[i][j] = _imageContainer->ImageEdit->SpSurface->DirectAlpha[i][j];
	  lRet = true;
	  }
   }
*/
   return lRet;
}
//---------------------------------------------------------------------------
void TAutomataCml::doGrow(int i, int j)
{
// grow
if (getProbability(_growProbability))
   {
   _modifyValueGrow(_epsilonGrow);
   if (_output_value >= _maxIntensity)
	  {
	  _output_value = _maxIntensity;
	   _controlMask->Pixel(i, j).ch[0] |=  0x02;
	  //for (int k = 0; k < 3; k++)
	  //	  _imageContainer->ImageOrig->SpSurface->DirectBGR[i][j].bgr[k] = _imageContainer->ImageEdit->SpSurface->DirectBGR[i][j].bgr[k];
	  // alpha
	  //if (_imageContainer->ImageOrig->SpImage->alpha_map)
	  //	  _imageContainer->ImageOrig->SpSurface->DirectAlpha[i][j] = _imageContainer->ImageEdit->SpSurface->DirectAlpha[i][j];
	  }
   }
else
   _output_value = _input_value;
 }
 //---------------------------------------------------------------------------
bool TAutomataCml::doCreation(void)
{
// creation
bool result = calculateLogicContainer(_createLogic);
result = result && getProbability(_createProbability);
_output_value = result ?  _seedIntensity : _input_value;
return result;
}
//---------------------------------------------------------------------------
void TAutomataCml::clearHiddenCanvas(void)
{
if (_fpInput)
	_fpInput->Zero();
if (_fpOutput)
	_fpOutput->Zero();
if (_controlMask)
	_controlMask->Zero();
}
//---------------------------------------------------------------------------
void TAutomataCml::processUncoupledSeq(spImage<ptFloat> *outputImage, bool outputMask)
{
int i, j;
BYTE mByte, outByte;
int si, sj, k;
bool destructed, input_state;
for (i = _boundRect->top; i < _boundRect->bottom; i++)
	{
	for (j = _boundRect->left; j < _boundRect->right; j++)
		{
		if (i >= _boundRect->bottom)
		   continue;
		if (j >= _boundRect->right)
		   continue;
		_input_value = _fpInput->Pixel(i, j).ch[0];
		mByte = Mask->Pixel(i, j).ch[0];
		if (!mByte)
		   {
		   outputImage->Pixel(i, j).ch[0] = _input_value;
		   continue;
		   }
		//input_state = (_controlMask->SpSurface->DirectByte[i][j] & 0x07);
		//input_state = (_controlMask->SpSurface->DirectByte[i][j] & 0x03);
		input_state = (bool)(_input_value > OPS_tolerance);
		resetLogicContainer();
		exploreNeighbour(i, j);      		 // sets neighbour_cumul
		_output_value = _input_value;		 // problem when everything is killed and no more creation????
		if (input_state)
		   {
		   destructed = false;
		   // destruction
		   if (_decayProbability > 0)
			  destructed = doDestruction(i, j);
		   if (!destructed) // grow
			  doGrow(i, j);
		   }
		else
		   {
		   if (doCreation())
			  {
			  _controlMask->Pixel(i, j) = _delayCreation ? 0x01 : 0x00;
			  }
		   }
		outputImage->Pixel(i, j).ch[0] = _output_value;
		if (outputMask)
		   {
		   outByte = (BYTE)fast_ftol(_output_value * 255.0);
		   _surfaceRight->Pixel(i, j).ch[0] = outByte;
		   }
		}
	}
}
//---------------------------------------------------------------------------
void TAutomataCml::processUncoupledRand(spImage<ptFloat> *outputImage, bool outputMask)
{
int i, j;
BYTE mByte, outByte;
int si, sj, k, rco, wh = _boundRect->Height() * _boundRect->Width();
bool destructed, input_state;
	for (rco = 0; rco < wh; rco++)
		{
		i = _ijArray[rco].y;
		j = _ijArray[rco].x;
		if (i >= _boundRect->bottom)
		   continue;
		if (j >= _boundRect->right)
		   continue;
		_input_value = _fpInput->Pixel(i, j).ch[0];
		mByte = Mask->Pixel(i, j).ch[0];
		if (!mByte)
		   {
		   outputImage->Pixel(i, j).ch[0] = _input_value;
		   continue;
		   }
		//input_state = (_controlMask->SpSurface->DirectByte[i][j] & 0x07);
		//input_state = (_controlMask->SpSurface->DirectByte[i][j] & 0x03);
		input_state = (bool)(_input_value > OPS_tolerance);
		resetLogicContainer();
		exploreNeighbour(i, j);      		 // sets neighbour_cumul
		_output_value = _input_value;		 // problem kad je sve ubijeno i nema kreacije ????
		if (input_state)
		   {
		   destructed = false;
		   // destruction
		   if (_decayProbability > 0)
			  destructed = doDestruction(i, j);
		   if (!destructed) // grow
			  doGrow(i, j);
		   }
		else
		   {
		   if (doCreation())
			  {
			  _controlMask->Pixel(i, j).ch[0] = _delayCreation ? 0x01 : 0x00;
			  }
		   }
		outputImage->Pixel(i, j).ch[0] = _output_value;
		if (outputMask)
		   {
		   outByte = (BYTE)fast_ftol(_output_value * 255.0);
		   _surfaceRight->Pixel(i, j).ch[0] = outByte;
		   }
		}

}
//---------------------------------------------------------------------------
void TAutomataCml::processCoupled(void)
{
int i, j;
BYTE mByte, outByte;
int si, sj, k;
bool result, input_state;
// first step
cmlProcessUncoupled(_fpIntermediate, false);
// second step
for (i = _boundRect->top; i < _boundRect->bottom; i++)
	{
	for (j = _boundRect->left; j < _boundRect->right; j++)
		{
		_input_value = _fpIntermediate->Pixel(i, j).ch[0];
		mByte = Mask->Pixel(i, j).ch[0];
		if (!mByte)
		   {
		   _fpOutput->Pixel(i, j).ch[0] = _input_value;
		   continue;
		   }
		//cumulateNeighbour(i, j);
		_cmlCumulateNeighbour(i, j);
		if (_neighbour_factor < OPS_tolerance)  // nzn!
			_output_value = _input_value;
		else
		   {
		   if ((_controlMask->Pixel(i, j).ch[0] & 0x02) && _semiCoupled)
			  _output_value = _input_value;
		   else if (_controlMask->Pixel(i, j).ch[0] & 0x01)
			  _output_value = _input_value;
		   else
			  _output_value = (1.0 - _epsilonWeight) *_input_value + _epsilonWeight * _neighbour_cumul * _neighbour_factor;     // couple output
		   }
		_fpOutput->Pixel(i, j).ch[0] = _output_value;
		if (_output_value < _minIntensity)
		   _output_value = 0;
		//_fpOutput->SpSurface->DirectFloat[i][j] = _output_value;
		outByte = (BYTE)fast_ftol(_output_value * 255.0);
		_surfaceRight->Pixel(i, j).ch[0] = outByte;
		//test
		/*
		_input_value =  _output_value * 255;
		cumulateNeighbourMask(i, j);
		_output_value = (1.0 - _epsilonWeight) *_input_value + _epsilonWeight * _neighbour_cumul * _neighbour_factor;     // couple output
		outByte = (BYTE)fast_ftol(_output_value);
		_imageContainer->ImageMaskRight->SpSurface->DirectByte[i][j] = outByte;
		*/
		}
	}
}
//---------------------------------------------------------------------------
void TAutomataCml::postIteration(void)
{
if (_delayCreation)
   {
	for (int i = _boundRect->top; i < _boundRect->bottom; i++)
		{
		for (int j = _boundRect->left; j < _boundRect->right; j++)
			{
			_controlMask->Pixel(i, j).ch[0] &=  0x02;
			//_controlMask->SpSurface->DirectByte[i][j] &=  0x07; // preserve fixed  //0x03;
			// test
			}
		}
   }
if (_randomSplat)
  {
  int jumpY = random(_boundRect->Height()) + _boundRect->top;
  int jumpX = random(_boundRect->Width()) + _boundRect->left;
  _fpInput->Pixel(jumpY, jumpX).ch[0] = _seedIntensity;
  }
if (!_matrixMes)
   {
   _mutateCreation(_creationOps);
   _mutateDestruction(_destructionOps);
   }
}
//---------------------------------------------------------------------------
void TAutomataCml::preIteration(void)
{
if (_sequentialScan)
   {
   cmlProcessUncoupled = processUncoupledSeq;
   return;
   }
if (_ijArray)
   delete []_ijArray;
int rco = 0, wh = _boundRect->Height() * _boundRect->Width();
_ijArray = new TPoint[wh];
int i, j;
for (i = 0; i < _boundRect->Height(); i++)
	{
	for (j = 0; j < _boundRect->Width(); j++)
		{
		_ijArray[rco].y = _boundRect->top  + i;
		_ijArray[rco].x = _boundRect->left + j;
		rco++;
		}
	}
std::random_shuffle( &_ijArray[0], &_ijArray[wh]);
cmlProcessUncoupled = processUncoupledRand;
}
//---------------------------------------------------------------------------
void TAutomataCml::Prepare(spSurface<ptGray> *surfaceLeft, spSurface<ptGray> *surfaceRight, structSpecificParams *params)
{
for (int i = 0; i < 9; i++)
	{
	if (i == 4)
	   continue;
	_creationOps[OPS_pos[i]] = params->CreationOps[i];
	_destructionOps[OPS_pos[i]] = params->DestructionOps[i];
	}
_latticeType  = params->LatticeType;
_epsilonWeight = params->EpsilonWeight;
_minIntensity = params->MinIntensity/255.0;
_maxIntensity = params->MaxIntensity/255.0;
_seedIntensity = params->SeedIntensity/255.0;
_epsilonGrow = params->EpsilonGrow;
_epsilonDecay = -params->EpsilonDecay;
_iterations = params->Iterations;
_growProbability = params->GrowProbability;
_createProbability = params->CreateProbability;
_decayProbability = params->DecayProbability;
_creationMutation = params->CreationMutation;
_destructionMutation = params->DestructionMutation;
_mutateCreation 	  = _mutator[_creationMutation];
_mutateDestruction    = _mutator[_destructionMutation];
_reBorn = params->Reborn;
_matrixMes = params->MatrixMes;
_delayCreation = params->DelayCreation;
_randomSplat = params->RandomSplat;
_sequentialScan = params->SequentialScan;
_semiCoupled = (_latticeType == 3 || _latticeType == 4);
if (_latticeType == 2 || _latticeType == 4)
   _cmlCumulateNeighbour = cumulateNeighbourNz;
else
   {
   _cmlCumulateNeighbour = cumulateNeighbour;
   _neighbour_factor = 0.125;
   }
switch (params->GrowFunc)
   {
   case 1:
		_modifyValueGrow = modifyWithInputSquared;
		break;
   case 2:
		_modifyValueGrow = modifyWithNeighbour;
		break;
   case 3:
		_modifyValueGrow = modifyWithNeighbourInverse;
		break;
   default:
		_modifyValueGrow = modifyWithInputLinear;
		break;
   }
switch (params->DecayFunc)
   {
   case 1:
		_modifyValueDecay = modifyWithInputSquared;
		break;
   case 2:
		_modifyValueDecay = modifyWithNeighbour;
		break;
   case 3:
		_modifyValueDecay = modifyWithNeighbourInverse;
		break;
   default:
		_modifyValueDecay = modifyWithInputLinear;
		break;
   }
if (surfaceRight != _surfaceRight)
   {
   _surfaceLeft = surfaceLeft;
   _surfaceRight = surfaceRight;
   if (_fpInput)
	  {
	  delete _fpInput;
	  delete _fpOutput;
	  delete Mask;
	  delete _controlMask;
	  }
   _fpInput = 0;
   _fpOutput = 0;
   Mask = 0;
   _controlMask = 0;
   if (_fpIntermediate)
	  delete _fpIntermediate;
   _fpIntermediate = 0;
   }
if (!_fpInput)
   {
   _fpInput = new spImage<ptFloat>(_surfaceRight->Width, _surfaceRight->Height);
   _fpOutput = new spImage<ptFloat>(_surfaceRight->Width, _surfaceRight->Height);
   Mask = new spImage<ptGray>(_surfaceRight->Width, _surfaceRight->Height);
   _controlMask = new spImage<ptGray>(_surfaceRight->Width, _surfaceRight->Height);
   clearHiddenCanvas();
   }
if (_latticeType > 0 && !_fpIntermediate)
   {
   _fpIntermediate = new spImage<ptFloat>(_surfaceRight->Width, _surfaceRight->Height);
   }
//
//_dataKeeper->RenderParams->UpdateImage = _updateImage;
//_dataKeeper->RenderParams->AddUndo = _updateImage;
//_dataKeeper->RenderParams->ClearMask = _updateImage;
}
//---------------------------------------------------------------------------
void TAutomataCml::ExecuteShape(TRect *boundRect, TPoint *Point, int Count)
{
_boundRect = boundRect;
preIteration();
int lp, si, sj;
float fByte;
for (lp = 0; lp < Count; lp++)
	{
	si = Point[lp].y;
	sj = Point[lp].x;
	_fpInput->SetSafeIndex(&si, &sj);
	fByte = (float)_surfaceRight->Pixel(si, sj).ch[0]/255.0;
	if (fByte >= _minIntensity)
	   _fpInput->Pixel(si, sj).ch[0] = fByte;
	else
	   _fpInput->Pixel(si, sj).ch[0] = _seedIntensity;
	}
for (int iterator = 0; iterator < _iterations; iterator++)
	{
	if (_latticeType)
	   processCoupled();
	else
	   cmlProcessUncoupled(_fpOutput, true);
	swapImages();
	if (_matrixMes)
	   {
	   _mutateCreation(_creationOps);
	   _mutateDestruction(_destructionOps);
	   }
	}
if (_iterations % 2 == 0)
   swapImages();
postIteration();
clearMask();
}
//---------------------------------------------------------------------------
void TAutomataCml::ExecuteTip(TRect *boundRect, TPoint *Point)
{
_boundRect = boundRect;
preIteration();
int si, sj;
float fByte;
si = Point->y;
sj = Point->x;
_fpInput->SetSafeIndex(&si, &sj); // ????
fByte = (float)_surfaceRight->Pixel(si, sj).ch[0]/255.0;
if (fByte >= _minIntensity)
   _fpInput->Pixel(si, sj).ch[0] = fByte;
else
   _fpInput->Pixel(si, sj).ch[0] = _seedIntensity;
for (int iterator = 0; iterator < _iterations; iterator++)
	{
	if (_latticeType)
	   processCoupled();
	else
	   cmlProcessUncoupled(_fpOutput, true);
	swapImages();
	if (_matrixMes)
	   {
	   _mutateCreation(_creationOps);
	   _mutateDestruction(_destructionOps);
	   }
	}
if (_iterations % 2 == 0)
   swapImages();
postIteration();
clearMask();
}
//---------------------------------------------------------------------------
void TAutomataCml::Update(int param)
{
clearHiddenCanvas();
}
//---------------------------------------------------------------------------
