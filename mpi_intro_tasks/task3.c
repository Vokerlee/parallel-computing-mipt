#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

#define FIRST_RANK_VAL 0
#define MESSAGE 0L

int main(int argc, char *argv[])
{
    int error_value = 0;

    // Initialization

    error_value = MPI_Init(&argc, &argv);
    if (error_value != MPI_SUCCESS)
        errx(EX_OSERR, "error %d: MPI_Init()", error_value);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    errno = 0;

    // Get comm_size and comm_rank

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_size()", error_value);

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_rank()", error_value);

    // Required procedure

    if (comm_size > 1 && comm_rank == FIRST_RANK_VAL)
    {
        long message = MESSAGE;

        printf("My rank is %d, initial message = %ld\n", comm_rank, message);

        error_value = MPI_Send(&message, 1, MPI_LONG, FIRST_RANK_VAL + 1, 0, MPI_COMM_WORLD);
        if (error_value != MPI_SUCCESS)
            errx(error_value, "error %d: MPI_Send()", error_value);

        MPI_Status status;
        error_value = MPI_Recv(&message, 1, MPI_LONG, MPI_ANY_SOURCE,
                               MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        message += 1;

        printf("My rank is %d, end message = %ld\n", comm_rank, message);
    }
    else if (comm_size == 1)
    {
        printf("My rank is %d, message = %ld\n", comm_rank, MESSAGE);
    }
    else
    {
        long message = -1;

        MPI_Status status;
        error_value = MPI_Recv(&message, 1, MPI_LONG, MPI_ANY_SOURCE,
                               MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        message += 1;

        printf("My rank is %d, message = %ld\n", comm_rank, message);

        int destination = comm_rank + 1;
        if (comm_rank >= (comm_size - 1))
            destination = FIRST_RANK_VAL;

        error_value = MPI_Send(&message, 1, MPI_LONG, destination, 0, MPI_COMM_WORLD);
        if (error_value != MPI_SUCCESS)
            errx(error_value, "error %d: MPI_Send()", error_value);
    }

    MPI_Finalize();

    return 0;
}
