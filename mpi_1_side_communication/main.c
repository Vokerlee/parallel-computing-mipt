#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

#define FIRST_RANK_VAL 0

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

    MPI_Offset offset = sizeof(int) * (comm_size - comm_rank - 1);
    MPI_File file;
    MPI_Status status;

    int num_to_write = comm_rank + '0';

    MPI_File_open(MPI_COMM_WORLD, "result.txt", MPI_MODE_CREATE|MPI_MODE_WRONLY, MPI_INFO_NULL, &file);
    MPI_File_seek(file, offset, MPI_SEEK_SET);
    MPI_File_write(file, &num_to_write, 1, MPI_INT, &status);
    MPI_File_close(&file);

    // Assemble result

    MPI_Win win_header, win_data;
    double *shared_data = NULL;
    int n_written_data = 0;

    MPI_Win_allocate_shared(sizeof(double) * comm_size, sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &shared_data, &win_data);
    MPI_Win_create(&n_written_data, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win_header);
    MPI_Win_fence(0, win_data);
    
    int offset_put;

    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win_header);
    const int n_numbers_to_send = 1;
    MPI_Get_accumulate(&n_numbers_to_send, 1, MPI_INT, &offset_put, 1, MPI_INT, 0, 0, 1, MPI_INT, MPI_SUM, win_header);
    MPI_Win_unlock(0, win_header);

    MPI_Put(&sum, 1, MPI_DOUBLE, 0, offset_put, 1, MPI_DOUBLE, win_data);

    MPI_Win_fence(0, win_data);
  
    if (comm_rank == 0)
    {
        printf("ndat after communication: %d\n", n_written_data);
        for(int i = 0; i < n_written_data; ++i)
        {
            sum += shared_data[i];
            printf("received data[%d] = %f\n", i, shared_data[i]);
        }

        printf("My rank is %d, total sum is %lg\n", comm_rank, sum);
    }

    MPI_Win_free(&win_header);
    MPI_Win_free(&win_data);

    MPI_Finalize();

    return 0;
}
