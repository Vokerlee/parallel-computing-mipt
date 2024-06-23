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

#ifndef ISIZE
    #define ISIZE 5000
#endif

#ifndef JSIZE
    #define JSIZE 5000
#endif

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
    double **b = (double **) calloc(ISIZE, sizeof(double *));
    if (a == NULL || b == NULL)
        errx(EX_OSERR, "error: calloc()");

    for (size_t i = 0; i < ISIZE; ++i)
    {
        a[i] = (double *) calloc(JSIZE, sizeof(double));
        b[i] = (double *) calloc(JSIZE, sizeof(double));
        if (a[i] == NULL || b[i] == NULL)
            errx(EX_OSERR, "error: calloc()");
    }

    for (size_t i = 0; i < ISIZE; i++)
    {
        for (size_t j = 0; j < JSIZE; j++)
        {
            a[i][j] = 10 * i + j;
            b[i][j] = 0;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    size_t chunk_start = JSIZE / comm_size * comm_rank;
    size_t chunk_end   = (comm_rank == (comm_size - 1)) ? JSIZE :
                          chunk_start + JSIZE / comm_size;

    for (size_t i = 0; i < ISIZE; ++i)
    {
        for (size_t j = chunk_start; j < chunk_end; ++j)
            a[i][j] = sin(0.1 * a[i][j]);
    }

    for (size_t i = 0; i < ISIZE - 1; ++i)
    {
        for (size_t j = chunk_start; j < chunk_end; ++j)
            b[i][j] = a[i + 1][j] * 1.5;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (comm_rank == 0)
    {
        for (size_t rank = 1; rank < comm_size; ++rank)
        {
            size_t j_start = JSIZE / comm_size * rank;
            size_t j_end   = (rank == (comm_size - 1)) ? JSIZE :
                              chunk_start + JSIZE / comm_size;

            for (size_t i = 0; i < ISIZE; i++)
                MPI_Recv(&b[i][j_start], j_end - j_start + 1, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    else
    {
        for (size_t i = 0; i < ISIZE; i++)
            MPI_Send(&b[i][chunk_start], chunk_end - chunk_start + 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

#ifdef DEBUG_ON

    if (comm_rank == 0)
    {
        for (size_t i = 0; i < ISIZE; i++)
        {
            for (size_t j = 0; j < JSIZE; j++)
                printf("%f ", b[i][j]);

            printf("\n");
        }
    }

#endif // DEBUG_ON

    if (comm_rank == 0)
        printf("%lf\n", end_time - start_time);

    for (size_t i = 0; i < JSIZE; ++i)
    {
        free(a[i]);
        free(b[i]);
    }

    free(b);
    free(a);

    MPI_Finalize();
}
