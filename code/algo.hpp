#pragma once 

#include <queue>
#include <cmath>

template<typename Data>
void LineBresenham(Data *data, int stride, int x1, int y1, int x2, int y2, const Data & target)
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
			data[x * stride + y] = target;
		else 
			data[y * stride + x] = target;
		
		if (((e + dy) << 1) < dx) 
			e = e + dy;
		else {
			y += inc;
			e = e + dy - dx;
		}
    }
}

struct Bounds
{
	int x;
	int y;
	int w;
	int h;
};

template<typename Data>
void FloodFill(Data * data, int stride, const Bounds & region, const Data & target, int x, int y )
{
	struct Cell
	{
		int x;
		int y;
	};
	Data old = data[y * stride + x]; 
	if(old == target) return;

	data[y * stride + x] = target;
	
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
			Data & newcolor = data[n.y * stride + n.x];
			if(n.x >= region.x 
			&& n.y >= region.y 
			&& n.x <= region.x + region.w-1 
			&& n.y <= region.y + region.h-1 
			&& (newcolor == old)
			)
			{
				data[n.y * stride + n.x] = target; 
				queue.push(n);
			}
		}
	}
}