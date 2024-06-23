#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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
    long int message_len = strtol(argv[1], &endptr, 10);
    if (errno != 0)
        errx(EX_DATAERR, "invalid argument: %s", strerror(errno));
    else if (message_len <= 0)
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
    
    char *buffer = (char *) calloc(message_len, sizeof(char));
    if (buffer == NULL)
        errx(errno, "calloc() error: %s\n", strerror(errno));

    if (comm_rank == 1)
    {
        double start_time = 0;
        double end_time   = 0;

        start_time = MPI_Wtime();

#ifdef SEND 
        error_value = MPI_Send(buffer, message_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
#endif
#ifdef SSEND 
        error_value = MPI_Ssend(buffer, message_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
#endif
#ifdef RSEND 
        error_value = MPI_Rsend(buffer, message_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
#endif
#ifdef BSEND 
        error_value = MPI_Bsend(buffer, message_len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
#endif

        if (error_value != MPI_SUCCESS)
            errx(error_value, "error %d: MPI_Send()", error_value);
            
        end_time = MPI_Wtime();

        printf("Send time is %lg\n", end_time - start_time);
    }
    
    if (comm_rank == 0)
    {
        sleep(2);

        error_value = MPI_Recv(buffer, message_len, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (error_value != MPI_SUCCESS)
            errx(error_value, "error %d: MPI_Recv()", error_value);

    }

    MPI_Finalize();
    free(buffer);

    return EXIT_SUCCESS;
}
