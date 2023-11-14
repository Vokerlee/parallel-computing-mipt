#include "device.h"
#include <stdlib.h>
#include <stdio.h>

cl_int lookup_devices()
{
    cl_uint n_platforms = 0;
    cl_platform_id *platforms = NULL;
    cl_device_id *devices = NULL;

    cl_int status = clGetPlatformIDs(0, NULL, &n_platforms);
    printf("STATUS %d\n", status);
    if (status != CL_SUCCESS)
        return status;

    if (n_platforms > 0)
    {
        platforms = (cl_platform_id *) calloc(n_platforms, sizeof(cl_platform_id));
        status = clGetPlatformIDs(n_platforms, platforms, NULL);
    }
    else
        return CL_DEVICE_NOT_FOUND;

    printf("Amount of platforms: %ld\n", (long) n_platforms);

    for (size_t pf = 0; pf < n_platforms; ++pf)
    {
        cl_uint n_devices = 0;
        status = clGetDeviceIDs(platforms[pf], CL_DEVICE_TYPE_ALL, 0, NULL, &n_devices);
        if (status != CL_SUCCESS)
            continue;

        printf("\tAmount of devices[platform %ld]: %ld\n", (long) pf, (long) n_devices);

        devices = (cl_device_id *) calloc(n_devices, sizeof(cl_device_id));
        status = clGetDeviceIDs(platforms[pf], CL_DEVICE_TYPE_ALL, n_devices, devices, NULL);
        if (status != CL_SUCCESS)
            continue;

        for (size_t dev = 0; dev < n_devices; ++dev)
        {
            char buffer[1024] = {0};
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_NAME, sizeof(buffer), &buffer, NULL);
            if (status == CL_SUCCESS)
            {
                printf("\t\tDevice name: %s\n", buffer);
            }
        }

        free(devices);
    }

    free(platforms);
}
