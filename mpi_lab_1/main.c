#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <math.h>

const double DIFFUSION_EQ_PARAM = 1.2;
const double TIME_GRID_VAL      = 0.5;
const double COORD_GRID_VAL     = 0.5;

double get_point_rectangle(const double **grid, ssize_t time_index, ssize_t coord_index);
void init_points(double **grid, ssize_t time_size, ssize_t coord_size);

int main(int argc, char *argv[])
{
    int error_value = 0;

    // Initialization

    error_value = MPI_Init(NULL, NULL);
    if (error_value != MPI_SUCCESS)
        errx(EX_OSERR, "error %d: MPI_Init()", error_value);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    errno = 0;

    if (argc < 2)
        errx(EX_USAGE, "error: invalid amount of arguments");

    char *endptr = NULL;
    double max_time = strtod(argv[1], &endptr);
    if (errno != 0)
        errx(EX_DATAERR, "1th argument: %s", strerror(errno));
    else if (max_time <= 0)
        errx(EX_DATAERR, "1th argument: should be positive");
  
    double max_coord = strtod(argv[2], &endptr);
    if (errno != 0)
        errx(EX_DATAERR, "2th argument: %s", strerror(errno));
    else if (max_coord <= 0)
        errx(EX_DATAERR, "2th argument: should be positive");

    ssize_t time_size  = max_time  / COORD_GRID_VAL;
    ssize_t coord_size = max_coord / TIME_GRID_VAL;

    double **grid = (double **) calloc(coord_size + 2, sizeof(double *));
    if (grid == NULL)
        errx(EX_OSERR, "error %s: calloc()", strerror(errno));

    for (int i = 0; i <= coord_size; ++i)
    {
        grid[i] = (double *) calloc(time_size + 2, sizeof(double));
        if (grid[i] == NULL)
            errx(EX_OSERR, "error %s: calloc()", strerror(errno));
    }

    init_points(grid, time_size, coord_size); 

    // Get comm_size and comm_rank

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_size()", error_value);

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_rank()", error_value);

    double start_program_time = MPI_Wtime();

    ssize_t coords_remainder = coord_size % comm_size;
    ssize_t coords_to_calc = coord_size / comm_size + 
        ((comm_rank < coords_remainder) && (coords_remainder != 0));

    ssize_t coord_start = 0;
    if (comm_rank < coords_remainder)
        coord_start = (coord_size / comm_size + 1) * comm_rank + 1;
    else
        coord_start = (coord_size / comm_size + 1) * coords_remainder + 
            (coord_size / comm_size) * (comm_rank - coords_remainder) + 1;

    ssize_t coord_end = coord_start + coords_to_calc - 1;
    
    ssize_t n_time_iters = 1; 
    if (comm_size > 1)
        n_time_iters = time_size;

    for (int iter = 0; iter < n_time_iters; ++iter)
    {
        ssize_t time_start = time_size / n_time_iters * iter + 1;
        ssize_t time_end   = time_start + time_size / n_time_iters - 1 + 
            (time_size % n_time_iters) * (iter == (comm_size - 1));

        if (comm_rank != 0)
        {
            error_value = MPI_Recv(&(grid[coord_start - 1][time_start]), time_end - time_start + 1, MPI_DOUBLE, comm_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Recv()", error_value);
        }

        for (int i = time_start; i <= time_end; ++i)
        {
            for (int j = coord_start; j <= coord_end; ++j)
                grid[j][i] = get_point_rectangle((const double **) grid, i, j);
        }

        if (comm_rank != (comm_size - 1))
        {   
            error_value = MPI_Send(&(grid[coord_end][time_start]), time_end - time_start + 1, MPI_DOUBLE, comm_rank + 1, 0, MPI_COMM_WORLD);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Send()", error_value);
        }
    }

    if (comm_rank == 0)
    {
        for (int coord = coord_end + 1; coord <= coord_size; ++coord)
        {
            error_value = MPI_Recv(&(grid[coord][1]), time_size, MPI_DOUBLE, MPI_ANY_SOURCE, coord, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Recv()", error_value);
        }
    }
    else 
    {
        for (int coord = coord_start; coord <= coord_end; ++coord)
        {
            error_value = MPI_Send(&(grid[coord][1]), time_size, MPI_DOUBLE, 0, coord, MPI_COMM_WORLD);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Send()", error_value);
        }
    }

    if (comm_rank == 0)
    {
        double calculation_time = MPI_Wtime() - start_program_time;
        printf("time: %lg\n", calculation_time);
    }
    
     if (comm_rank == 0)
     {
         for (int j = 0; j <= coord_size; ++j)
         {
             for (int i = 0; i <= time_size; ++i)
                 printf("%lg ", grid[j][i]);
             
             printf("\n");
         }
    }
   // 
    for (int i = 0; i < time_size; ++i)
        free(grid[i]);

    free(grid);

    MPI_Finalize();

    return EXIT_SUCCESS;
}

void init_points(double **grid, ssize_t time_size, ssize_t coord_size)
{
    for (int k = 0; k <= time_size; ++k)
        grid[0][k] = sin(TIME_GRID_VAL * k);
    
    for (int m = 0; m <= coord_size; ++m)
        grid[m][0] = sin(COORD_GRID_VAL * m);
}

double get_point_rectangle(const double **grid, ssize_t time_index, ssize_t coord_index)
{
    double func_half = sin(TIME_GRID_VAL * (time_index + 0.5) + COORD_GRID_VAL * (coord_index + 0.5));

    return (
            (grid[coord_index - 1][time_index - 1] + grid[coord_index][time_index - 1] - grid[coord_index - 1][time_index]) / TIME_GRID_VAL +
            (grid[coord_index - 1][time_index - 1] - grid[coord_index][time_index - 1] + grid[coord_index - 1][time_index]) / COORD_GRID_VAL * DIFFUSION_EQ_PARAM +
            2 * func_half
        ) / (1 / TIME_GRID_VAL + 1 / COORD_GRID_VAL) ;
}

