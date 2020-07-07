#ifndef WU_LINE_H
#define WU_LINE_H
#include <Types.hpp>
#include <vector>
struct TVertex
   {
   int x;
   int y;
   float bgr[3];
   };
// Draw an anti-aliased line between two points using the Xiaolin Wu's line algorithm
void drawWuLine(std::vector<TVertex> &va, float x1, float y1, float x2, float y2, const Byte *fColor, const Byte *bColor, const int chans);
#endif
