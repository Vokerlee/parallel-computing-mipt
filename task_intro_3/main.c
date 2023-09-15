#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    omp_set_num_threads(omp_get_num_procs());

    long int sum = 0;

    #pragma omp parallel for ordered
    for (size_t i = 0; i < omp_get_num_threads(); ++i)
    {
        #pragma omp ordered
        sum += 1;

        printf("%d %ld\n", omp_get_thread_num(), sum);
    }

    printf("Total sum is %ld\n", sum);

    return EXIT_SUCCESS;
}
