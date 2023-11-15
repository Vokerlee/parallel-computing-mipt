#include "device.h"
#include <stdlib.h>
#include <stdio.h>

cl_int lookup_devices()
{
    cl_uint n_platforms = 0;
    cl_platform_id *platforms = NULL;
    cl_device_id *devices = NULL;

    cl_int status = clGetPlatformIDs(0, NULL, &n_platforms);
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
            char     buffer[1024] = {0};
            cl_uint  uint         = 0;
            cl_ulong ulong        = 0;
            size_t   size         = 0;

            status = clGetDeviceInfo(devices[dev], CL_DEVICE_NAME, sizeof(buffer), &buffer, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice name: %s\n", buffer);

            status = clGetDeviceInfo(devices[dev], CL_DEVICE_VENDOR, sizeof(buffer), &buffer, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice vendor: %s\n", buffer);

            status = clGetDeviceInfo(devices[dev], CL_DEVICE_EXTENSIONS, sizeof(buffer), &buffer, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice extensions: %s\n", buffer);

            // Size of global memory cache in bytes.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(ulong), &ulong, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice global memory cache size: %ld\n", (long) ulong);

            // Size of global device memory in bytes.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(ulong), &ulong, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice global memory size: %ld\n", (long) ulong);

            // Size of local memory region in bytes.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(ulong), &ulong, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice local memory size: %ld\n", (long) ulong);

            // The default compute device address space size of the global address space specified as
            // an unsigned integer value in bits. Currently supported values are 32 or 64 bits.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_ADDRESS_BITS, sizeof(uint), &uint, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice address space regime (bits): %ld\n", (long) uint);

            // Maximum configured clock frequency of the device in MHz.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(uint), &uint, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice max frequency: %ld\n", (long) uint);

            // The number of parallel compute units on the OpenCL device.
            // A work-group executes on a single compute unit. The minimum value is 1.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(uint), &uint, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice max compute units: %ld\n", (long) uint);

            // Maximum number of work-items in a work-group that a device is capable of executing on
            // a single compute unit, for any given kernel-instance running on the device.
            // (Refer also to clEnqueueNDRangeKernel) and CL_KERNEL_WORK_GROUP_SIZE). The minimum value is 1.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size), &size, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice max work group size: %ld\n", (long) size);

            // Maximum dimensions that specify the global and local work-item IDs used
            // by the data parallel execution model.
            // (Refer to clEnqueueNDRangeKernel).
            // The minimum value is 3 for devices that are not of type CL_DEVICE_TYPE_CUSTOM.
            status = clGetDeviceInfo(devices[dev], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(uint), &uint, NULL);
            if (status == CL_SUCCESS)
                printf("\t\tDevice max work dimensions: %ld\n", (long) uint);
        }

        free(devices);
    }

    free(platforms);

    return status;
}
