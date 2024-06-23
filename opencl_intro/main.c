#include "device.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    cl_int status = lookup_devices();
    if (status != CL_SUCCESS)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
