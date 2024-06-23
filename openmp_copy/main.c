#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <err.h>
#include <sysexits.h>
#include <errno.h>

// The copyin clause is used to initialize threadprivate data upon entry to a parallel region. The
// value of the threadprivate variable in the primary thread is copied to the threadprivate variable of
// each other team member.

// The copyprivate clause can be used to broadcast values acquired by a single thread directly to
// all instances of the private variables in the other threads. In this example, if the routine is called
// from the sequential part, its behavior is not affected by the presence of the directives. If it is called
// from a parallel region, then the actual arguments with which a and b are associated must be
// private.

int some_var_1 = -1;
int some_var_2 = -1;

int main(int argc, char *argv[])
{
    printf("copyin() demonstration\n");

    #pragma omp threadprivate(some_var_1, some_var_2)

    some_var_1 = 2; // just any number
    some_var_2 = 9; // just any number

    #ifdef COPYIN_ON
    #pragma omp parallel copyin(some_var_1, some_var_2)
    {
        printf("some_var_1 = %d, some_var_2 = %d\n", some_var_1, some_var_2);
    }
    #else
    #pragma omp parallel
    {
        printf("some_var_1 = %d, some_var_2 = %d\n", some_var_1, some_var_2);
    }
    #endif

    printf("\ncopyprivate() demonstration\n");

    #pragma omp parallel
    {
        #pragma omp single copyprivate(some_var_1, some_var_2)
        scanf("%d%d", &some_var_1, &some_var_2);
    }

    #pragma omp parallel
    {
        printf("some_var_1 = %d, some_var_2 = %d\n", some_var_1, some_var_2);
    }

    return EXIT_SUCCESS;
}
