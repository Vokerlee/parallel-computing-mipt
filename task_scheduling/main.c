#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

#ifndef OMP_SCHEDLING_CLASS
#define OMP_SCHEDLING_CLASS static
#endif

#ifndef OMP_SCHEDLING_CHUNKS
#define OMP_SCHEDLING_CHUNKS 2
#endif

#ifndef N_USED_THREADS
#define N_USED_THREADS 8
#endif

int main(int argc, char *argv[])
{
    omp_set_num_threads(N_USED_THREADS);

    #pragma omp parallel for schedule(OMP_SCHEDLING_CLASS, OMP_SCHEDLING_CHUNKS)
    for (size_t i = 0; i < 2 * OMP_SCHEDLING_CHUNKS * omp_get_num_threads(); ++i)
    {
        printf("%d %ld\n", omp_get_thread_num(), (long) i);
    }

    return EXIT_SUCCESS;
}
