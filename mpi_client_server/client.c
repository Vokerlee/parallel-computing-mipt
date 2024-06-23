#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <gmp.h>
#include <math.h>

int main(int argc, char* argv[])
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

    // Get comm_size and comm_rank

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_size()", error_value);

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_rank()", error_value);
 
    int turn    = 0;
    int cell    = 0;
    int finish  = 0;
    int message = 0; 

    char port_name[MPI_MAX_PORT_NAME + 1] = {0};
    MPI_Comm intercomm; 
    MPI_Status status;

    strcpy(port_name, argv[1]); 
    
    error_value = MPI_Comm_connect(port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm); 
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_connect", error_value);

    MPI_Recv(&turn, 1, MPI_INT, 0, 6, intercomm, &status);      
    printf("First turn is for player #%d\n", turn); 
    
    while (finish != 1)
	{
        MPI_Comm_connect(port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercomm); 
        MPI_Recv(&cell, 1, MPI_INT, 0, 6, intercomm, &status);         
        
        scanf("%d", &message); 
        MPI_Send(&message, 1, MPI_INT, 0, 5, intercomm);  
        // MPI_Recv(&finish, 1, MPI_INT, 0, 6, intercomm, &status); 
        printf("Player picked: %d \n", cell);
       
        if (cell == 999)
        {
            printf("Game is over\n");
            MPI_Comm_disconnect(&intercomm);
        }
    }

    MPI_Finalize();
 
    return EXIT_SUCCESS;
}
