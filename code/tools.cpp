#include "tools.h"

#include <queue>
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
			&& n.x <= region.x + region.w 
			&& n.y <= region.y + region.h 
			&& (ncolor == oldColor)
			)
			{
				colors[n.y * stride + n.x] = newColor; 
				queue.push(n);
			}
		}
	}
}