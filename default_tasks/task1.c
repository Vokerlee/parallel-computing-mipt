#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int error_value = MPI_Init(&argc, &argv);
    if (error_value != MPI_SUCCESS)
        errx(EX_OSERR, "error %d: MPI_Init()", error_value);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

    error_value = 0;
    errno       = 0;

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error: MPI_Comm_size()");

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error: MPI_Comm_rank()");

    printf("Hello World! My rank is %d out of %d processes\n", comm_rank, comm_size);

    MPI_Finalize();

    return 0;
}
