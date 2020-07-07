//---------------------------------------------------------------------------
#ifndef automataCmlH
#define automataCmlH
//#include "spImage.hpp"
#include "spSurface.hpp"
#include "spPixelTypes.hpp"
//---------------------------------------------------------------------------
const int OPS_pos[9] = {5, 6, 7, 3, 0, 4, 0, 1, 2};
const int N33I[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int N33J[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
const float  OPS_tolerance =  0.00001;
class TAutomataCml
{
//typedef bool __fastcall (__closure *TCmlLogic)(bool, bool);
typedef void (__closure *TCmlModifyValue)(float);
typedef void (__closure *TCmlMutation)(int *ops);
typedef void (__closure *TCmlIJ)(int i, int j);
typedef void (__closure *TCmlProcess)(spImage<ptFloat> *outImage, bool outMask);
private:
	//TImageContainer *_imageContainer;
	spImage<ptFloat> *_fpInput, *_fpOutput, *_fpTrans, *_fpIntermediate;
	spImage<ptGray> *_controlMask;
	spSurface<ptGray> *_surfaceLeft, *_surfaceRight;
	TRect *_boundRect;
	TCmlMutation _mutateCreation, _mutateDestruction, _mutator[3];
	TCmlModifyValue _modifyValueGrow, _modifyValueDecay;
	TCmlIJ _cmlCumulateNeighbour;
	TCmlProcess cmlProcessUncoupled;
	int _creationOps[8], _destructionOps[8];
	int _iterations, _growProbability, _createProbability, _decayProbability, _latticeType;
	float _minIntensity, _maxIntensity, _seedIntensity, _epsilonGrow, _epsilonDecay, _epsilonWeight, _input_value, _output_value, _neighbour_cumul, _neighbour_factor;
	bool _reBorn, _semiCoupled, _matrixMes, _delayCreation, _randomSplat, _sequentialScan;
	int  _creationMutation, _destructionMutation;
	TPoint *_ijArray;
	//TDataKeeper *_dataKeeper;
	struct structCmlLogic
		{
		BYTE Container[8];
		bool Result;
		int Count;
		};
	structCmlLogic _createLogic[5], _destructLogic[5];
	void addLogicContainer(structCmlLogic *logic, int index, bool v1);
	void resetLogicContainer(void);
	bool calculateLogicContainer(structCmlLogic *logic);
	void swapImages(void);
	void drawContainerMask(void);
	void processUncoupledSeq(spImage<ptFloat> *outputImage, bool outputMask);
	void processUncoupledRand(spImage<ptFloat> *outputImage, bool outputMask);
	void processCoupled(void);
	void clearMask(void);
	void nopOps(int *ops);
	void rotateOps(int *ops);
	void shuffleOps(int *ops);
	void exploreNeighbour(int i, int j);
	void cumulateNeighbour(int i, int j);
	void cumulateNeighbourNz(int i, int j);
	bool getProbability(int prob);
	void modifyWithNeighbour(float epsilon);
	void modifyWithNeighbourInverse(float epsilon);
	void modifyWithInputLinear(float epsilon);
	void modifyWithInputSquared(float epsilon);
	bool doDestruction(int i, int j);
	void doGrow(int i, int j);
	bool doCreation(void);
	void clearHiddenCanvas(void);
	void postIteration(void);
	void preIteration(void);
	//
protected:
public:
	 struct structSpecificParams
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
	spImage<ptGray> *Mask;
	TAutomataCml(void);
	virtual ~TAutomataCml(void);
	void Prepare(spSurface<ptGray> *surfaceLeft, spSurface<ptGray> *surfaceRight, structSpecificParams *params);
	void ExecuteShape(TRect *boundRect, TPoint *Point, int Count);
	void ExecuteTip(TRect *boundRect, TPoint *Point);
	void Update(int param);
};
//---------------------------------------------------------------------------
#endif
