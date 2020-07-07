//---------------------------------------------------------------------------
#ifndef spShufflerH
#define spShufflerH
#include <Types.hpp>
#include "spPixelTypes.hpp"
#include "spImage.hpp"
typedef void (__closure *TShufflerExecute)(spImage<ptRGB> *, int , int , int, int);
//---------------------------------------------------------------------------
class TShuffler
{
struct groupStruct
	{
	ptRGB pixel;
	float luminance;
	//bool operator<(const groupStruct &rhs) const { luminance < rhs.luminance; };
	};
private:
	int _manh_i, _manh_j, _manh_distance;
	int _group_size;
	int _i_center, _j_center, _sort_group;
	float _lumina_min, _lumina_max;
	bool _manh_rand_dist;
	groupStruct *_pixelGroup;
	ptRGB _pixel;
	void deleteGroup(void);
	void calculateManhattan(int i_c, int j_c);
	void calculateGroup(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j);
	bool aLuminaAsc(groupStruct &lhs, groupStruct &rhs) { return lhs.luminance < rhs.luminance;}
	bool aLuminaDsc(groupStruct &lhs, groupStruct &rhs) { return lhs.luminance > rhs.luminance;}
protected:
public:
	TShuffler(void);
	virtual ~TShuffler(void);
	void PrepareManhattanSimple(int distance, bool rdist);
	void PrepareGroup(int distance, int groupsize, bool rdist);
	void PrepareGroupRange(int distance, int groupsize, bool rdist, int value_min, int value_max, int sort_group);
	void PrepareGroupAverage(int distance, int groupsize, bool rdist);
	void ExecuteManhattanSimple(spImage<ptRGB> *srcImage, int i_c, int j_c, int i, int j);
	void ExecuteGroupBest(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j);
	void ExecuteGroupWorst(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j);
	void ExecuteLuminaRange(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j);
	void ExecuteGroupAverage(spImage<ptRGB> *srcImage, int i_c, int j_c,  int i, int j);
	TShufflerExecute Execute;
	__property ptRGB Pixel = {read = _pixel};
};
//---------------------------------------------------------------------------
#endif
