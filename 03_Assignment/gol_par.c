#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

struct pthread_args
{
	unsigned long 	p_id;
	unsigned long 	num_threads;
	unsigned long 	ratio;
	unsigned long 	time_steps;
	unsigned long 	dim_x;
	unsigned long 	dim_y;
	unsigned char *grid_in;
	unsigned char *grid_out;
	pthread_barrier_t *barrier;
};

void evolve(unsigned char *grid_in, unsigned char *grid_out, unsigned int dim_x, unsigned int dim_y, unsigned int x, unsigned int y)
{
	/* local variables for grid and local counter of alive neighbor cells */
	unsigned char (*c_grid_in)[dim_x] = (unsigned char(*)[dim_x])grid_in;
	unsigned char (*c_grid_out)[dim_x] = (unsigned char(*)[dim_x])grid_out;
    unsigned int alive_neigh_count = 0;
	
	/* visit all the neighbors and determine if they are alive or not */
	/* similar to a 2d stencil used for, e.g. Gauss-Seidel solver */
	if (c_grid_in[(y + dim_y - 1) % dim_y][(x + dim_x - 1) % dim_x])
	{
		alive_neigh_count++;
	}
	if (c_grid_in[(y + dim_y - 1) % dim_y][(x + dim_x) % dim_x])
	{
		alive_neigh_count++;
	}
	if (c_grid_in[(y + dim_y - 1) % dim_y][(x + dim_x + 1) % dim_x])
	{
		alive_neigh_count++;
	}


	if (c_grid_in[(y + dim_y) % dim_y][(x + dim_x - 1) % dim_x])
	{
		alive_neigh_count++;
	}
	if (c_grid_in[(y + dim_y) % dim_y][(x + dim_x + 1) % dim_x])
	{
		alive_neigh_count++;
	}

	if (c_grid_in[(y + dim_y + 1) % dim_y][(x + dim_x - 1) % dim_x])
	{
		alive_neigh_count++;
	}
	if (c_grid_in[(y + dim_y + 1) % dim_y][(x + dim_x) % dim_x])
	{
		alive_neigh_count++;
	}
	if (c_grid_in[(y + dim_y + 1) % dim_y][(x + dim_x + 1) % dim_x])
	{
		alive_neigh_count++;
	}
	

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

void *runner(void *ptr)
{
	struct pthread_args *arg = ptr;

	unsigned long id = arg->p_id;
	unsigned long no_rows = arg->dim_y;
	unsigned long no_columns = arg->dim_x;
	unsigned long no_threads = arg->num_threads;
	unsigned long time_steps = arg->time_steps;
	unsigned long ratio = arg->ratio;

	size_t size = sizeof(unsigned char) * no_columns * no_rows;
	
	unsigned long y_max = ratio * no_threads;

	for (unsigned long t = 0; t < time_steps; t++)
	{
		for(unsigned long y = id; y < y_max; y += no_threads)
		{
			for(unsigned long x = 0; x < no_columns; x++)
			{
				evolve(arg->grid_in, arg->grid_out, no_columns, no_rows, x, y);
			}
		}

		pthread_barrier_wait(arg->barrier);
		swap(&arg->grid_in, &arg->grid_out);
	}

	if(time_steps % 2 != 0)
	{
		memcpy(arg->grid_out, arg->grid_in, size);
	}

	return 0;
}

unsigned int gol(unsigned char *grid, unsigned int dim_x, unsigned int dim_y, unsigned int time_steps, unsigned int num_threads)
{
	unsigned char *grid_in, *grid_out, *grid_tmp;
	size_t size = sizeof(unsigned char) * dim_x * dim_y;

	const unsigned long thread_ratio = dim_y / num_threads;
	const unsigned long thread_remainder = dim_y % num_threads;

	pthread_t *thread;
	struct pthread_args *thread_arg;
	pthread_barrier_t barrier;

	pthread_barrier_init(&barrier, 0, num_threads);

	grid_tmp = malloc(size);
	if(grid_tmp == NULL)
		exit(EXIT_FAILURE);

	memset(grid_tmp, 0, size);

	grid_in = grid;
	grid_out = grid_tmp;

	// Distribute the number of threads with respect to the number of rows

	if(num_threads > dim_y)
	{
		num_threads = dim_y;
		thread 		= malloc(num_threads*sizeof(*thread));
		thread_arg 	= malloc(num_threads*sizeof(*thread_arg));

		for(unsigned long i = 0; i < num_threads; i++)
		{
			thread_arg[i].ratio  = 1;
		}
	}
	else 
	{
		thread 		= malloc(num_threads*sizeof(*thread));
		thread_arg 	= malloc(num_threads*sizeof(*thread_arg));

		for(unsigned long i = 0; i < num_threads ; i++)
		{
			thread_arg[i].ratio = thread_ratio;
		}

		// Assign unallocated rows to first threads if necessary
		if(thread_remainder != 0)
		{
			for(unsigned long i = 0; i < thread_remainder ; i++)
			{
				thread_arg[i].ratio += 1;
			}
		}
	}

	// Create num_threads threads
	for(unsigned long i = 0; i < num_threads; i++)
	{
		thread_arg[i].p_id = i;
		thread_arg[i].dim_x = dim_x;
		thread_arg[i].dim_y = dim_y;
		thread_arg[i].num_threads = num_threads;
		thread_arg[i].time_steps = time_steps;
		thread_arg[i].grid_in = grid_in;
		thread_arg[i].grid_out = grid_out;
		thread_arg[i].barrier = &barrier;
		pthread_create(thread+i, 0, &runner, thread_arg+i);
	}

	// Join threads
	for(unsigned long i = 0; i < num_threads; i++)
	{
		pthread_join(thread[i], 0);
	}

	// Free dynamically allocated memory
	free(thread);
	free(thread_arg);
	free(grid_tmp);

	return cells_alive(grid, dim_x, dim_y);
}