#include "tools.h"

#include <queue>
#include <cmath>

void LineBresenham(Color *colors, int stride, int x1, int y1, int x2, int y2, const Color & color)
{
    // bresenham line
	int steep = abs( y2 - y1 ) > abs( x2 - x1 );
    int inc = -1;

    if (steep) {
		int t = x1;
		x1 = y1;
		y1 = t;
			
		t = x2;
		x2 = y2;
		y2 = t;
    }

    if (x1 > x2) {
		int t = x1;
		x1 = x2;
		x2 = t;
			
		t = y1;
		y1 = y2;
		y2 = t;
	}

    if (y1 < y2) {
		inc = 1;
    }

    int dx = abs(x2 - x1),
        dy = abs(y2 - y1),
        y = y1, x = x1,
        e = 0;

    for (; x <= x2; x++) {
		if (steep) 
			colors[x * stride + y] = color;
		else 
			colors[y * stride + x] = color;
		
		if (((e + dy) << 1) < dx) 
			e = e + dy;
		else {
			y += inc;
			e = e + dy - dx;
		}
    }
}

void FloodFill(Color * colors, int stride, const Rect & region, const Color & newColor, int x, int y )
{
	struct Cell
	{
		int x;
		int y;
	};
	Color oldColor = colors[y * stride + x]; 
	if(oldColor == newColor) return;

	colors[y * stride + x] = newColor;
	
	//x y in region space
	std::queue<Cell> queue;
	queue.push({x,y});
	while(!queue.empty())
	{

		Cell cell = queue.front();
		queue.pop();

		const int cx = cell.x;
		const int cy = cell.y;
		const int px = cx-1;
		const int py = cy-1;
		const int nx = cx+1;
		const int ny = cy+1;
		const Cell neighbors[4]= 
		{
					  {cx, py}, 
			{px, cy},           {nx, cy},
					  {cx, ny}, 
		};
		for(int i = 0; i < 4; i++)
		{
			Cell n =neighbors[i];
			Color & ncolor = colors[n.y * stride + n.x];
			if(n.x >= region.x 
			&& n.y >= region.y 
			&& n.x <= region.x + region.w-1 
			&& n.y <= region.y + region.h-1 
			&& (ncolor == oldColor)
			)
			{
				colors[n.y * stride + n.x] = newColor; 
				queue.push(n);
			}
		}
	}
}