#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <math.h>

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
    ssize_t n_iters = strtol(argv[1], &endptr, 10);
    if (errno != 0)
        errx(EX_DATAERR, "1th argument: %s", strerror(errno));
    else if (n_iters <= 0)
        errx(EX_DATAERR, "1th argument: should be positive");
    
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

    double some_value = 0;
    if (comm_rank == 0)
    {
        for (int i = 0; i < n_iters; ++i)
        {
            error_value = MPI_Recv(&some_value, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);        
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Recv()", error_value);
        }
    }
    else 
    {
        for (int i = 0; i < n_iters; ++i)
        {
            error_value = MPI_Send(&some_value, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Send()", error_value);
        }
    }
 
    if (comm_rank == 0)
    {
        double consumed_time = MPI_Wtime() - start_program_time;
        printf("time per iter: %lg\n", consumed_time / n_iters);
    }
    
    MPI_Finalize();

    return EXIT_SUCCESS;
}

