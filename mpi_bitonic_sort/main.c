#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <math.h>

void test_sort(int array[], long int array_size, int n_processes, int rank);

void compute_top   (int partner, int dir, int *array, long int array_size, int rank, int k);
void compute_bottom(int partner, int dir, int *array, long int array_size, int rank, int k);
int cmp_asc (const void *a, const void *b);
int cmp_desc(const void *a, const void *b);

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
    long int array_size = strtol(argv[1], &endptr, 10);
    if (errno != 0)
        errx(EX_DATAERR, "invalid argument: %s", strerror(errno));
    else if (array_size <= 0)
        errx(EX_DATAERR, "invalid argument: should be positive");

    array_size = 1 << array_size;

    // Get comm_size and comm_rank

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_size()", error_value);

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_rank()", error_value);

    // Array initialization for sorting 

    long int array_size_per_rank = array_size / comm_size;

    int *total_array = NULL;
    int *local_array = NULL;
    
    if (comm_rank == 0)
    {
        total_array = (int *) calloc(array_size, sizeof(int));
        if (total_array == NULL)
            errx(EX_OSERR, "error %s: calloc()", strerror(errno));
        
        for (long int i = 0; i < array_size; ++i)
        {
            total_array[i] = rand();
        }
    }

    // Sorting start
    
    MPI_Barrier(MPI_COMM_WORLD);

    local_array = (int *) malloc(array_size_per_rank * sizeof(int));
    if (local_array == NULL)
        errx(EX_OSERR, "error %s: calloc()", strerror(errno));

    double sort_start = MPI_Wtime();

    MPI_Scatter(total_array, array_size_per_rank, MPI_INT, local_array, array_size_per_rank, MPI_INT, 0, MPI_COMM_WORLD);

    if (comm_rank % 2 == 0) 
        qsort(local_array, array_size_per_rank, sizeof(int), cmp_asc);
    else 
        qsort(local_array, array_size_per_rank, sizeof(int), cmp_desc);

    // for (long int i = 0; i < array_size_per_rank; ++i)
    // {
    //     printf("%d ", local_array[i]);
    // }
    // printf("\n");
    //
    MPI_Barrier(MPI_COMM_WORLD);

    for (int k = 1; k < comm_size; k <<= 1)
    {
        int dir = ((k * 2 & comm_rank) == 0);

        for (int j = k; j >= 1; j >>= 1)
        {
            int partner = comm_rank ^ j;  
            if (comm_rank < partner)
                compute_bottom(partner, dir, local_array, array_size_per_rank, comm_rank, j);
            else 
                compute_top(partner, dir, local_array, array_size_per_rank, comm_rank, j);
            
            MPI_Barrier(MPI_COMM_WORLD);
        }

        if (dir) 
            qsort(local_array, array_size_per_rank, sizeof(int), cmp_asc);
        else 
            qsort(local_array, array_size_per_rank, sizeof(int), cmp_desc);
    }

    double sort_time = MPI_Wtime() - sort_start;
    
    if (comm_rank == 0)
        printf("total sort time: %lg\n", sort_time);

    // Test if everything is correctly sorted

    test_sort(local_array, array_size_per_rank, comm_size, comm_rank);

    // Sorting end & deinitialization
    
    free(total_array);
    free(local_array);

    MPI_Finalize();

    return EXIT_SUCCESS;
}

int cmp_asc(const void *a, const void *b)
{
   return *(int *)a - *(int *)b;
}

int cmp_desc(const void *a, const void *b)
{
   return *(int *)b - *(int *)a;
}

inline void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void compute_bottom(int partner, int dir, int *array, long int array_size, int rank, int k)
{
    MPI_Status status;
    int *swap_temp = (int *) malloc(array_size / 2 * sizeof(int));

    int error_state = MPI_Send(array + array_size / 2, array_size / 2, MPI_INT, partner, 1, MPI_COMM_WORLD);
    if (error_state != MPI_SUCCESS) 
    {
        fprintf(stderr, "Error idle. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, error_state);
    }

    MPI_Recv(swap_temp, array_size / 2, MPI_INT, partner, 1, MPI_COMM_WORLD, &status);
    
    if (dir)
    {
        for(long int i = 0; i < array_size / 2; ++i)
            if(array[i] > swap_temp[i]) 
                swap(&array[i], &swap_temp[i]);
    }
    else
    {
        for(long int i = 0; i < array_size / 2; i++)
            if(array[i] < swap_temp[i]) 
                swap(&array[i], &swap_temp[i]);
    }
    //this guy sends the results before receiving
    error_state = MPI_Send(swap_temp, array_size / 2, MPI_INT, partner, 2, MPI_COMM_WORLD);
    if (error_state != MPI_SUCCESS) 
    {
        fprintf(stderr, "Error idle. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, error_state);
    }
    //receiving the computed top part from the partner.
    MPI_Recv(array + array_size / 2, array_size / 2, MPI_INT, partner, 2, MPI_COMM_WORLD, &status);
}


/** function called by the bigger partner to swap the top part
    of both tables 
**/
void compute_top(int partner, int dir, int *array, long int array_size, int rank, int k)
{
    MPI_Status status;
    int *swap_temp = (int *) malloc(array_size / 2 * sizeof(int));
 
    //this guy will receive before sending. If they both attemp to send first it may lead to a deadlock.
    MPI_Recv(swap_temp, array_size / 2, MPI_INT, partner, 1, MPI_COMM_WORLD, &status);
    int error_state = MPI_Send(array, array_size / 2, MPI_INT, partner, 1, MPI_COMM_WORLD);
    if (error_state != MPI_SUCCESS) 
    {
        fprintf(stderr, "Error idle. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, error_state);
    }

    if (dir)
    { 
        for(long int i = 0; i < array_size / 2; i++)
            if(array[i + array_size / 2] < swap_temp[i]) 
                swap(&array[i + array_size / 2], &swap_temp[i]);
    }
    else
    {
        for(long int i = 0; i < array_size / 2; i++)
            if(array[i + array_size / 2] > swap_temp[i]) 
                swap(&swap_temp[i], &array[i + array_size / 2]);
    }
    //Proccessing has ended. This guy will first receive and then send to avoid deadlock.
    MPI_Recv(array, array_size / 2, MPI_INT, partner, 2, MPI_COMM_WORLD, &status);
    error_state = MPI_Send(swap_temp, array_size / 2, MPI_INT, partner, 2, MPI_COMM_WORLD);
    if (error_state != MPI_SUCCESS) 
    {
        fprintf(stderr, "Error idle. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, error_state);
    }
}

void test_sort(int array[], long int array_size, int n_processes, int rank)
{
    int flag = 1;
    int min = array[0];
    int max = 0;

    for(long int i = 0; i < array_size - 1; i++)
    {
        if (array[i + 1] < array[i]) 
            flag = 0;
        if (array[i] > max) 
            max = array[i];
        if (array[i] < min) 
            min = array[i];
    }

    if (rank == 0)
    {
        MPI_Status status;
        int other_flag;
        
        int *minimum = (int *) malloc(n_processes * sizeof(int));
        int *maximum = (int *) malloc(n_processes * sizeof(int));
        if (minimum == NULL || maximum == NULL)
            return;

        minimum[0] = min;
        maximum[0] = max;

        for (long int i = 1; i < n_processes; i++)
        {
            //tag = 3 means local sorting flag
            MPI_Recv(&other_flag, 1, MPI_INT, i, 3, MPI_COMM_WORLD, &status);
            flag = flag & other_flag;
            //tag = 4 means minimum
            MPI_Recv(minimum + i, 1, MPI_INT, i, 4, MPI_COMM_WORLD, &status);
            //tag = 5 means maximum
            MPI_Recv(maximum + i, 1, MPI_INT, i, 5, MPI_COMM_WORLD, &status);
        }

        if (flag) 
            fprintf(stderr, "everyone is locally sorted\n");
        else 
            fprintf(stderr, "at least one failed to sort himself locally\n");
    
        //from now on flag has a new interpretetion
        for (long int i = 0; i < n_processes - 1; i++)
            if(maximum[i] > minimum[i + 1]) 
                flag = 0;
        
        //if flag is still 1 at this point, test has been passed
        if (flag) 
            printf("parallel bitonic sort was a success\n");
        else 
            printf("bitonic sort has failed\n");
    }
    else
    {
    //tag = 3 means local sorting flag
        MPI_Send(&flag, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
    //tag = 4 means minimum
        MPI_Send(&min, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
    //tag = 5 means maximum
        MPI_Send(&max, 1, MPI_INT, 0, 5, MPI_COMM_WORLD);
    }
}

