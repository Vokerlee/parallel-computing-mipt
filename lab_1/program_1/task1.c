#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <math.h>

// All variables naming is taken from original task,
// os if you don't like them - I understand you.

#define ISIZE 20
#define JSIZE 20

int main(int argc, char **argv)
{
    int error_value = 0;

    error_value = MPI_Init(&argc, &argv);
    if (error_value != MPI_SUCCESS)
        errx(EX_OSERR, "error %d: MPI_Init()", error_value);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    errno = 0;

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_size()", error_value);

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_rank()", error_value);

    double **a = (double **) calloc(ISIZE, sizeof(double *));
    if (a == NULL)
        errx(error_value, "error: calloc()");

    for (size_t i = 0; i < ISIZE; ++i)
    {
        a[i] = (double *) calloc(JSIZE, sizeof(double));
        if (a[i] == NULL)
            errx(error_value, "error: calloc()");
    }

    // Master process initializes the array
    for (size_t i = 0; i < ISIZE; i++)
    {
        for (size_t j = 0; j < JSIZE; j++)
            a[i][j] = 10 * i + j;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    size_t chunk_start = (JSIZE - 1) / comm_size * comm_rank;
    size_t chunk_end   = (comm_rank == (comm_size - 1)) ? (JSIZE - 1) :
                          chunk_start + (JSIZE - 1) / comm_size;

    for (size_t i = 1; i <= ISIZE; ++i)
    {
        if (comm_rank == 0)
        {
            MPI_Recv(&a[i - 1][chunk_end], 1, MPI_DOUBLE, comm_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if (comm_rank == comm_size - 1)
        {
            MPI_Send(&a[i - 1][chunk_start], 1, MPI_DOUBLE, comm_rank - 1, 0, MPI_COMM_WORLD);
        }
        else
        {
            MPI_Send(&a[i - 1][chunk_start], 1, MPI_DOUBLE, comm_rank - 1, 0, MPI_COMM_WORLD);
            MPI_Recv(&a[i - 1][chunk_end], 1, MPI_DOUBLE, comm_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        if (i < ISIZE)
        {
            for (size_t j = chunk_start; j < chunk_end; ++j)
             a[i][j] = sin(2 * a[i - 1][j + 1]);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    // ОСТАЛАСЬ ТОЛЬКО ЗАПИСЬ В КОНЦЕ

    if (comm_rank == 0)
    {
        for (size_t rank = 1; rank < comm_size; ++rank)
        {
            size_t j_start = (JSIZE - 1) / comm_size * rank;
            size_t j_end   = (rank == (comm_size - 1)) ? (JSIZE - 1) :
                              chunk_start + (JSIZE - 1) / comm_size;

            for (size_t i = 1; i < ISIZE; i++)
                MPI_Recv(&a[i][j_start], j_end - j_start + 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else
    {
        for (size_t i = 1; i < ISIZE; i++)
            MPI_Send(&a[i][chunk_start], chunk_end - chunk_start + 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

#ifdef DEBUG_ON

    if (comm_rank == 0)
    {
        for (size_t i = 0; i < ISIZE; i++)
        {
            for (size_t j = 0; j < JSIZE; j++)
                printf("%f ", a[i][j]);

            printf("\n");
        }
    }

#endif // DEBUG_ON

    if (comm_rank == 0)
        printf("%lf\n", end_time - start_time);

    for (size_t i = 0; i < JSIZE; ++i)
        free(a[i]);

    free(a);

    MPI_Finalize();
}
