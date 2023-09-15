#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

#define N_NESTED_LEVEL 3

int main(int argc, char *argv[])
{
    omp_set_nested(N_NESTED_LEVEL);

    size_t i = 0;
    size_t j = 0;
    size_t k = 0;

    #pragma omp parallel num_threads(N_NESTED_LEVEL) default(shared)
    {
        #pragma omp for
        for (i = 0; i < N_NESTED_LEVEL; ++i)
        {
            #pragma omp parallel num_threads(N_NESTED_LEVEL)
            {
                #pragma omp for
                for (j = 0; j < N_NESTED_LEVEL; ++j)
                {
                    #pragma omp parallel num_threads(N_NESTED_LEVEL)
                    {
                        #pragma omp for
                        for (k = 0; k < N_NESTED_LEVEL; ++k)
                        {
                            printf("    2 %d %d\n", omp_get_thread_num(), omp_get_num_threads());
                        }
                    }

                    printf("  1 %d %d\n", omp_get_thread_num(), omp_get_num_threads());
                }
            }

            printf("0 %d %d\n", omp_get_thread_num(), omp_get_num_threads());
        }
    }

    return EXIT_SUCCESS;
}
