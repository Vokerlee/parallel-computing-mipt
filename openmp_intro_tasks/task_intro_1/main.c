#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[])
{
    omp_set_num_threads(omp_get_num_procs());

    #pragma omp parallel
    {
        printf("Total number of therads is %d [#%d]\n",
            omp_get_num_threads(), omp_get_thread_num());
    }

    return EXIT_SUCCESS;
}
