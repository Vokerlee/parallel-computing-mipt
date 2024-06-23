#ifndef SIMPLE_DEVICE_EXAMPLE_H
#define SIMPLE_DEVICE_EXAMPLE_H

#ifdef __APPLE__
#include<OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

cl_int lookup_devices();

#endif // SIMPLE_DEVICE_EXAMPLE_H
