#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <gmp.h>
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

    int new_rank = 0;
    MPI_Comm new_comm;
    
    int color = 1;
    if (comm_rank == 0)
        color = 2;

    error_value = MPI_Comm_split(MPI_COMM_WORLD, color, comm_rank, &new_comm);
    if (error_value != MPI_SUCCESS) 
        errx(error_value, "error %d: MPI_Comm_split()", error_value);
 
    if (comm_rank > 0)
    {
        error_value = MPI_Comm_rank(new_comm, &new_rank);
        if (error_value != MPI_SUCCESS)
            errx(error_value, "error %d: MPI_Comm_rank()", error_value);

        printf("My old rank is %d and new rank is %d\n", comm_rank, new_rank); 
    }  
    else
    {
        MPI_Finalize();
        return EXIT_SUCCESS;
    }

    // Calculate own sum

    if (comm_size > 1)
    {
        long int terms_per_proc = n_terms / (comm_size - 1) + (new_rank == 0) * n_terms % (comm_size - 1);
        long int end = n_terms - (comm_size - new_rank - 2) * (n_terms / (comm_size - 1));
        long int start = end - terms_per_proc + 1;

        double sum = 0;
        for (long int i = start; i <= end; ++i)
            sum += 1. / i;

        // Assemble result
        
        double total_sum = sum;

        if (comm_size > 2)
        {
            error_value = MPI_Reduce(&total_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, new_comm);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Reduce()", error_value);
        }
        
        if (new_rank == 0)
            printf("Total sum is %lg\n", total_sum);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
