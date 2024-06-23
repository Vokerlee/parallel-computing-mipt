#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>
#include <gmp.h>
#include <math.h>

long int estimate_n_series_members_alt(long int decimal_precision);
long int calculate_n_series_members(long int decimal_precision);
long int  estimate_n_series_members(long int decimal_precision);

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

    char *endptr = NULL;
    long int n_decimal_places = strtol(argv[1], &endptr, 10);
    if (errno != 0)
        errx(EX_DATAERR, "invalid argument: %s", strerror(errno));
    else if (n_decimal_places <= 0)
        errx(EX_DATAERR, "invalid argument: should be positive");
    
    size_t format_str_size = strlen(argv[1]) + 8; // with a margin
    
    // Get comm_size and comm_rank

    int comm_size = 0;
    int comm_rank = 0;

    error_value = MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_size()", error_value);

    error_value = MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);
    if (error_value != MPI_SUCCESS)
        errx(error_value, "error %d: MPI_Comm_rank()", error_value);

    long int n_series_members = 0;
    if (comm_rank == 0)
    {
        n_series_members = estimate_n_series_members(n_decimal_places);
        // n_series_members = calculate_n_series_members(n_decimal_places); // too slow
        
        for(int i = 1; i <= comm_size - 1; i++)
        {
            error_value = MPI_Send(&n_series_members, 1, MPI_LONG, i, 0, MPI_COMM_WORLD);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Send()", error_value);
        }
    }
    else 
    {
        error_value = MPI_Recv(&n_series_members, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (error_value != MPI_SUCCESS)
            errx(error_value, "error %d: MPI_Recv()", error_value);
    }

    const long int min_size_per_rank  = n_series_members / comm_size;
    long int start = min_size_per_rank * comm_rank;
    long int end   = start + min_size_per_rank - 1;

    if (n_series_members % comm_size != 0) 
    {
        if (comm_rank < n_series_members % comm_size) 
        {
            start += comm_rank;
            end += comm_rank + 1;
        }
        else
        {
            start += (n_series_members % comm_size);
            end   += (n_series_members % comm_size);
        }
    }

    mpz_t local_sum;
    mpz_t local_composition;
    mpz_init_set_ui(local_composition, 1);
    mpz_init_set_ui(local_sum, 1);

    for(int i = end; i > start; i--)
    {
        mpz_mul_ui(local_composition, local_composition, i);
        mpz_add(local_sum, local_sum, local_composition);
    }

    if (comm_rank > 0)
        mpz_mul_ui(local_composition, local_composition, start);

    if (comm_rank == 0 && comm_size > 1)
    {
        char *local_comp_str = mpz_get_str(NULL, 10, local_composition);
        error_value = MPI_Send(local_comp_str, strlen(local_comp_str) + 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        if (error_value != MPI_SUCCESS)
            errx(error_value, "error %d: MPI_Send()", error_value);
    }
    else if (comm_rank > 0)
    {
        MPI_Status status;

        int recv_size = 0;
        MPI_Probe(comm_rank - 1, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &recv_size);

        char *prev_local_comp_str = (char *) calloc(recv_size + 1, sizeof(char));
        if (prev_local_comp_str == NULL)
            errx(EX_OSERR, "error %d: calloc()", errno);

        MPI_Recv(prev_local_comp_str, recv_size, MPI_CHAR, comm_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        mpz_t prev_local_composition;
        mpz_init_set_str(prev_local_composition, prev_local_comp_str, 10);

        mpz_mul(local_composition, local_composition, prev_local_composition);

        if (comm_rank != (comm_size - 1))
        {
            char *local_comp_str = mpz_get_str(NULL, 10, local_composition);
            error_value = MPI_Send(local_comp_str, strlen(local_comp_str) + 1, MPI_CHAR, comm_rank + 1, 0, MPI_COMM_WORLD);
            if (error_value != MPI_SUCCESS)
                errx(error_value, "error %d: MPI_Send()", error_value);
        }

        mpz_clear(prev_local_composition);
        free(prev_local_comp_str);
    }

    mpf_set_default_prec(64 + 8 * n_decimal_places);

    mpf_t f_local_sum;
    mpf_init(f_local_sum);
    mpf_set_z(f_local_sum, local_sum);

    mpf_t f_local_composition;
    mpf_init(f_local_composition);
    mpf_set_z(f_local_composition, local_composition);

    mpf_div(f_local_sum, f_local_sum, f_local_composition);

    mpz_clear(local_composition);
    mpz_clear(local_sum);

    if (comm_rank > 0)
    {
        char *mpi_buf    = (char *) calloc(n_decimal_places + 8, sizeof(char));
        char *format_str = (char *) calloc(format_str_size  + 1, sizeof(char));
        
        snprintf(format_str, format_str_size, "%%.%ldFf", n_decimal_places);
        gmp_snprintf(mpi_buf, n_decimal_places + 8, format_str, f_local_sum);

        MPI_Send(mpi_buf, strlen(mpi_buf) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

        free(format_str);
        free(mpi_buf);
    }
    else if (comm_rank == 0)
    {
        for(int i = 1; i < comm_size; i++)
        {
            MPI_Status status;

            int recv_size = 0;
            MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &recv_size);

            char *sum_str_i = (char *) calloc(recv_size, sizeof(char));
            if (sum_str_i == NULL)
                errx(EX_OSERR, "error %d: calloc()", errno);

            MPI_Recv(sum_str_i, recv_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            mpf_t f_sum_i;
            mpf_init_set_str(f_sum_i, sum_str_i, 10);
            free(sum_str_i);

            mpf_add(f_local_sum, f_local_sum, f_sum_i);
            mpf_clear(f_sum_i);
        }

        char *format_str = (char *) calloc(format_str_size + 1, sizeof(char));

        snprintf(format_str, format_str_size, "%%.%ldFf\n", n_decimal_places);
        gmp_printf(format_str, f_local_sum);

        free(format_str);
    }

    mpf_clear(f_local_sum);
    mpf_clear(f_local_composition);

    MPI_Finalize();

    return 0;
}

long int calculate_n_series_members(long int decimal_precision)
{
    mpz_t required_10_degree;
    mpz_init_set_ui(required_10_degree, 10);
    mpz_pow_ui(required_10_degree, required_10_degree, decimal_precision);

    mpz_t n_series_member;
    mpz_init_set_ui(n_series_member, 1);

    long int n = 0;
    while(mpz_cmp(required_10_degree, n_series_member) > 0) 
    {
        n++;
        mpz_mul_ui(n_series_member, n_series_member, n);
    }

    mpz_clear(required_10_degree);
    mpz_clear(n_series_member);

    return n;
}

long int estimate_n_series_members(long int decimal_precision)
{
    double n_series_members = 1;
    double k_ln_10 = decimal_precision * M_LN10;

    for (int i = 0; i < 20; ++i)
    {
        n_series_members -= (log(2 * M_PI * n_series_members) + n_series_members * (log(n_series_members) - 1) - k_ln_10) / 
                            (0.5 / n_series_members + log(n_series_members));
    }
    
    return (long int) (n_series_members * 1.03);     
}

