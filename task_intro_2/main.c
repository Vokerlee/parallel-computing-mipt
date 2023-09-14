#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
        errx(EX_USAGE, "Invalid amount of arguments [need 1, instead %d]", argc - 1);

    omp_set_num_threads(omp_get_num_procs());

    char *endptr = NULL;
    long n_summands = strtol(argv[1], &endptr, 10);
    if (errno == ERANGE)
        errx(EX_DATAERR, "Argument value is out of range");
    else if (*endptr != '\0')
        errx(EX_DATAERR, "Argument value: no conversion can be performed");

    double sum = 0;

    #pragma omp parallel for reduction(+: sum)
        for (size_t i = 1; i <= n_summands; ++i)
        {
            sum += 1. / i;
        }

    printf("Total sum is %lf\n", sum);

    return EXIT_SUCCESS;
}
