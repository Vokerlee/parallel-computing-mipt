#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

#define FIRST_RANK_VAL 0
#define ELEMENTARY_TERM 1. / 5.

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
    long int n_terms = strtol(argv[1], &endptr, 10);
    if (errno != 0)
        errx(EX_DATAERR, "invalid argument: %s", strerror(errno));
    else if (n_terms <= 0)
        errx(EX_DATAERR, "invalid argument: should be positive");

    // Get comm_size and comm_rank

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_size()", error_value);

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_rank()", error_value);

    // Calculate own sum

    long int terms_per_proc = n_terms / comm_size + (comm_rank == 0) * n_terms % comm_size;
    long int end = n_terms - (comm_size - comm_rank - 1) * (n_terms / comm_size);
    long int start = end - terms_per_proc + 1;

    double sum = 0;
    for (long int i = start; i <= end; ++i)
        sum += 1. / i;

    // Assemble result

    if (comm_rank == FIRST_RANK_VAL)
    {
        double temp_sum = 0;
        for (int i = FIRST_RANK_VAL + 1; i < comm_size; ++i)
        {
            MPI_Status status;
            error_value = MPI_Recv(&temp_sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
                                   MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Recv()", error_value);
        
            sum += temp_sum;
        }

        printf("My rank is %d, total sum is %lg\n", comm_rank, sum);
    }
    else
    {
        error_value = MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        if (error_value != MPI_SUCCESS)
            errx(error_value, "error: MPI_Send()");
    }

    MPI_Finalize();

    return 0;
}
