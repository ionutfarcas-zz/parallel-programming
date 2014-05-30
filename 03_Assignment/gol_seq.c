#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void evolve(unsigned char *grid_in, unsigned char *grid_out, unsigned int dim_x, unsigned int dim_y, unsigned int x, unsigned int y)
{
	/* local variables for grid and local counter of alive neighbor cells */
	unsigned char (*c_grid_in)[dim_x] = (unsigned char(*)[dim_x])grid_in;
	unsigned char (*c_grid_out)[dim_x] = (unsigned char(*)[dim_x])grid_out;
    unsigned int alive_neigh_count = 0;

	unsigned int x_temp = 0, y_temp= 0 ;
	
	/* visit all the neighbors and determine if they are alive or not */
	/* similar to a 2d stencil used for, e.g. Gauss-Seidel solver */
	
	for (y_temp = y - 1; y_temp <= y + 1; y_temp++)
	{
		for (x_temp = x - 1; x_temp <= x + 1; x_temp++)
		{
			if (c_grid_in[(y_temp + dim_y) % dim_y][(x_temp + dim_x) % dim_x])
			{
				alive_neigh_count++;
			}
		}
	}

	/* if cell is alive, we are only interested in the neighbors, not the cell itself */
	if (c_grid_in[y][x]) 
		alive_neigh_count--;
	
	// Determine new cell state based on the number of living neighbors
	if(c_grid_in[y][x] == 1 && (alive_neigh_count < 2 || alive_neigh_count > 3))
	{
		c_grid_out[y][x] = 0;  	// Cell dies
	}
	else if(c_grid_in[y][x] == 0 && alive_neigh_count == 3)
	{
		c_grid_out[y][x] = 1;  	// Cell becomes alive
	}
	else
	{
		c_grid_out[y][x] = c_grid_in[y][x]; // Else keep the current state
	}
}

void swap(unsigned char **a, unsigned char **b)
{
	unsigned char *tmp = *a;
	*a = *b;
	*b = tmp;
}

unsigned int cells_alive(unsigned char *grid, unsigned int dim_x, unsigned int dim_y)
{
	unsigned char (*c_grid)[dim_x] = (unsigned char (*)[dim_x])grid;

	unsigned int cells = 0;

	for (int y = 0; y < dim_y; ++y)
	{
		for (int x = 0; x < dim_x; ++x)
		{
			cells += c_grid[y][x];
		}
	}

	return cells;
}

unsigned int gol(unsigned char *grid, unsigned int dim_x, unsigned int dim_y, unsigned int time_steps, unsigned int num_threads)
{
	unsigned char *grid_in, *grid_out, *grid_tmp;
	size_t size = sizeof(unsigned char) * dim_x * dim_y;

	grid_tmp = malloc(size);
	if(grid_tmp == NULL)
		exit(EXIT_FAILURE);

	memset(grid_tmp, 0, size);

	grid_in = grid;
	grid_out = grid_tmp;

	for (int t = 0; t < time_steps; ++t)
	{
		for (int y = 0; y < dim_y; ++y)
		{
			for (int x = 0; x < dim_x; ++x)
			{
				evolve(grid_in, grid_out, dim_x, dim_y, x, y);
			}
		}
		swap(&grid_in, &grid_out);
	}

	if(grid != grid_in)
		memcpy(grid, grid_in, size);

	free(grid_tmp);

	return cells_alive(grid, dim_x, dim_y);
}
