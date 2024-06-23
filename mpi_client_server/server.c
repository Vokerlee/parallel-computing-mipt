#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <gmp.h>
#include <math.h>

int visualize(int *line)
{
	int table[3][3];
	table[0][0] = line[0];
	table[0][1] = line[1];
	table[0][2] = line[2];
	table[1][0] = line[3];
	table[1][1] = line[4];
	table[1][2] = line[5];
	table[2][0] = line[6];
	table[2][1] = line[7];
	table[2][2] = line[8];

	for (int i = 0; i < 3; i++)
    {
		for (int j = 0; j < 3; j++)
			printf("%d ", table[i][j]);
		
        printf("\n");
    }

	return 0;
}

int check(int *line)
{
	int sum1 = 0;
    int sum2 = 0;

	int table[3][3];
	table[0][0] = line[0];
	table[0][1] = line[1];
	table[0][2] = line[2];
	table[1][0] = line[3];
	table[1][1] = line[4];
	table[1][2] = line[5];
	table[2][0] = line[6];
	table[2][1] = line[7];
	table[2][2] = line[8];

	for (int i = 0; i < 3; i++)
	{	
		sum1 = 0;
		sum2 = 0;
		
        for (int j = 0; j < 3; j++)
		{
			if (table[i][j] == 1) 
                sum1++;
			if (table[i][j] == 2) 
                sum2++;
		}
		if (sum1 == 3) 
        {
			printf("First won \n");
			return 1;
        }
		if (sum2 == 3) 
        {
			printf("Second won \n");
			return 1;
        }
	}

	// check vertical
	for (int i = 0; i < 3; i++)
	{	
		sum1 = 0;
		sum2 = 0;
		
        for (int j = 0; j < 3; j++)
		{
			if (table[j][i] == 1) 
                sum1++;
			if (table[j][i] == 2) 
                sum2++;
		}
		if (sum1 == 3) 
        {
			printf("First won \n");
			return 1;
        }
		if (sum2 == 3) 
        {
			printf("Second won \n");
			return 1;
        }
	}

	// check diag
	sum1 = 0;
	sum2 = 0;
	
    for (int j = 0; j < 3; j++)
	{	

		if (table[j][j] == 1) 
            sum1++;
		if (table[j][j] == 2) 
            sum2++;
		
        if (sum1 == 3) 
        {
			printf("First won \n");
			return 1;
        }
		if (sum2 == 3) 
        {
			printf("Second won \n");
			return 1;
        }
	}

	sum1 = 0;
	sum2 = 0;
	
    for (int j = 2; j > -1; j--)
	{
		if (table[j][j] == 1) 
            sum1++;
		if (table[j][j] == 2) 
            sum2++;
		
        if (sum1 == 3) 
        {
			printf("First won \n");
			return 1;
        }
		if (sum2 == 3) 
        {
			printf("Second won \n");
			return 1;
        }
	}

	if (table[2][0] == 1 && table[1][1] == 1 && table[0][2] == 1)
	{
		printf("First won \n");
		return 1;
    }
	if (table[2][0] == 2 && table[1][1] == 2 && table[0][2] == 2)
	{
		printf("Second won \n");
		return 1;
    }

	return 0;
}

int main(int argc, char **argv) 
{
    int error_value = MPI_Init(NULL, NULL);
    if (error_value != MPI_SUCCESS)
        errx(EX_OSERR, "error %d: MPI_Init()", error_value);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);

	int turn = 0;
	int finish = 0;

	turn = rand() % 2;

    int table[9] = {0};

	int r = 0, l = 0; // printable coordinates l-server, r - client
	char port_name[MPI_MAX_PORT_NAME] = {0}; 
	MPI_Status status; 
	int message = 0; 
	MPI_Comm intercomm; 

    MPI_Open_port(MPI_INFO_NULL, port_name); 
	printf("portname: %s\n", port_name); 
	printf("Waiting for the player...\n"); 
	
    MPI_Comm_accept(port_name, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &intercomm); 
	// Message about turn priority 
	MPI_Send(&turn, 1, MPI_INT, 0, 6, intercomm); 

    while (finish != 1)
	{
         MPI_Comm_accept(port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercomm); 
		if (turn == 1)
		{
			// MPI_Send(&finish, 1, MPI_INT, 0, 6, intercomm);
			printf("First turn \n");
			scanf("%d", &l); 
			MPI_Send(&l, 1, MPI_INT, 0, 6, intercomm); 
			MPI_Recv(&r, 1, MPI_INT, 0, 5, intercomm, &status); 
			// filling values
			table[r] = 2;
			table[l] = 1;
		}	
		else
		{
			// MPI_Send(&finish, 1, MPI_INT, 0, 6, intercomm);
			printf("Second turn \n");
			scanf("%d", &l); 
			MPI_Recv(&r, 1, MPI_INT, 0, 5, intercomm, &status); 
			MPI_Send(&l, 1, MPI_INT, 0, 6, intercomm); 
			// filling values
			table[l] = 1;
			table[r] = 2;
		}

		MPI_Comm_free(&intercomm); 
		visualize(table);
		finish = check(table);
		// MPI_Send(&finish, 1, MPI_INT, 0, 6, intercomm);
	}

	if (finish == 1)
	{
		MPI_Comm_accept(port_name, MPI_INFO_NULL, 0, MPI_COMM_SELF, &intercomm); 
		printf("The game is over \n");
		finish = 999;
		MPI_Send(&finish, 1, MPI_INT, 0, 6, intercomm);
		MPI_Close_port(port_name); 
		
        MPI_Finalize(); 
		
        return 0; 
	}
} 
