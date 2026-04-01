#ifndef OPENMM_HARDWARE_H_
#define OPENMM_HARDWARE_H_

/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit.                   *
 * See https://openmm.org/development.                                        *
 *                                                                            *
 * Portions copyright (c) 2013 Stanford University and the Authors.           *
 * Authors: Peter Eastman                                                     *
 * Contributors:                                                              *
 *                                                                            *
 * Permission is hereby granted, free of charge, to any person obtaining a    *
 * copy of this software and associated documentation files (the "Software"), *
 * to deal in the Software without restriction, including without limitation  *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
 * and/or sell copies of the Software, and to permit persons to whom the      *
 * Software is furnished to do so, subject to the following conditions:       *
 *                                                                            *
 * The above copyright notice and this permission notice shall be included in *
 * all copies or substantial portions of the Software.                        *
 *                                                                            *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
 * THE AUTHORS, CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      *
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  *
 * USE OR OTHER DEALINGS IN THE SOFTWARE.                                     *
 * -------------------------------------------------------------------------- */

/**
 * This file defines a collection of functions for querying the specific hardware being used.
 */

#ifdef __APPLE__
   #include <sys/sysctl.h>
   #include <dlfcn.h>
#else
   #ifdef WIN32
      #define NOMINMAX
      #include <windows.h>
      #include <intrin.h>
   #else
      #ifdef __ANDROID__
        #include <cpu-features.h>
      #else
        #include <unistd.h>
      #endif
   #endif
#endif

/**
 * Get the number of CPU cores available.
 */
static int getNumProcessors() {
#ifdef __APPLE__
    int ncpu;
    size_t len = 4;
    if (sysctlbyname("hw.logicalcpu", &ncpu, &len, NULL, 0) == 0)
       return ncpu;
    else
       return 1;
#else
#ifdef WIN32
    SYSTEM_INFO siSysInfo;
    int ncpu;
    GetSystemInfo(&siSysInfo);
    ncpu = siSysInfo.dwNumberOfProcessors;
    if (ncpu < 1)
        ncpu = 1;
    return ncpu;
#else
    #ifdef __ANDROID__
        return android_getCpuCount();
    #else
      long nProcessorsOnline = sysconf(_SC_NPROCESSORS_ONLN);
      if (nProcessorsOnline == -1)
          return 1;
      else
          return (int) nProcessorsOnline;
    #endif
#endif
#endif
}

/**
 * Get a description of the CPU's capabilities.
 */
#ifdef WIN32
#define cpuid __cpuid
#else
#if !defined(__ANDROID__) && !defined(__PNACL__) && !defined(__PPC__) \
    && !defined(__ARM__) && !defined(__ARM64__) && !defined(__LOONGARCH64__)
    static void cpuid(int cpuInfo[4], int infoType) {
    #ifdef __LP64__
        __asm__ __volatile__ (
            "cpuid":
            "=a" (cpuInfo[0]),
            "=b" (cpuInfo[1]),
            "=c" (cpuInfo[2]),
            "=d" (cpuInfo[3]) :
            "a" (infoType)
        );
    #else
        __asm__ __volatile__ (
            "pushl %%ebx\n"
            "cpuid\n"
            "movl %%ebx, %1\n"
            "popl %%ebx\n" :
            "=a" (cpuInfo[0]),
            "=r" (cpuInfo[1]),
            "=c" (cpuInfo[2]),
            "=d" (cpuInfo[3]) :
            "a" (infoType)
        );
    #endif
    }
#else
    static void cpuid(int cpuInfo[4], int infoType) {
        cpuInfo[0] = cpuInfo[1] = cpuInfo[2] = 0;
    }
#endif
#endif

/**
 * Get whether this is an x86 CPU that supports AVX.
 */
static bool isAvxSupported() {
#ifdef __AVX__
    int cpuInfo[4];
    cpuid(cpuInfo, 0);
    if (cpuInfo[0] >= 1) {
        cpuid(cpuInfo, 1);
        return ((cpuInfo[2] & ((int) 1 << 28)) != 0);
    }
#endif /* __AVX__ */
    return false;
}

/**
 * Get whether this is an x86 CPU that supports AVX2.
 */
static bool isAvx2Supported() {
#ifdef __AVX2__

    // Provide an alternative implementation of CPUID to support AVX2. On older
    // non-Windows OSes the hardware.h support for CPUID doesn't set the CX register
    // properly and gives the wrong answer when detecting AVX2 and beyond. On Windows
    // the cpuid seems to work as expected so can be used.

#if !(defined(_WIN32) || defined(WIN32))
    auto cpuid = [](int output[4], int functionnumber) {
        int a, b, c, d;
        __asm("cpuid" : "=a"(a),"=b"(b),"=c"(c),"=d"(d) : "a"(functionnumber), "c"(0) : );
        output[0] = a;
        output[1] = b;
        output[2] = c;
        output[3] = d;
    };
#endif

    int cpuInfo[4];
    cpuid(cpuInfo, 0);

    if (cpuInfo[0] >= 7) {
        cpuInfo[2] = 0;
        cpuid(cpuInfo, 7);
        return ((cpuInfo[1] & ((int) 1 << 5)) != 0);
    }

#endif /* __AVX2__ */
    return false;
}

/**
 * Get the maximum supported size for vectors in multiples of four bytes.  This
 * is the number of int or float values that can be contained in a vector.
 */
static int getVectorWidth() {
    if (isAvxSupported())
        return 8;
    return 4;
}

/**
 * Get whether this is an ARM CPU that supports NEON.
 */
static bool isNeonSupported() {
#ifdef __ARM_NEON
    // On Apple Silicon, NEON is always available
    #ifdef __APPLE__
        return true;
    #else
        // For Android and Linux ARM, check if NEON is available
        #ifdef __ANDROID__
            return (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0;
        #else
            // Try to detect NEON on Linux ARM
            FILE* fp = fopen("/proc/cpuinfo", "r");
            if (fp) {
                char line[256];
                while (fgets(line, sizeof(line), fp)) {
                    if (strstr(line, "Features") && strstr(line, "neon")) {
                        fclose(fp);
                        return true;
                    }
                }
                fclose(fp);
            }
        #endif
    #endif
#endif
    return false;
}

/**
 * Get information about Apple M5 Max chip features.
 */
static void getM5MaxFeatures(int& cpuCores, int& gpuCores) {
    #ifdef __APPLE__
        // Check if we're running on Apple Silicon
        int isAppleSilicon = 0;
        size_t len = sizeof(isAppleSilicon);
        sysctlbyname("hw.machine", NULL, &len, NULL, 0);
        
        // Detect M5 Max based on processor name
        char processorName[256];
        len = sizeof(processorName);
        if (sysctlbyname("machdep.cpu.brand_string", processorName, &len, NULL, 0) == 0 || 
            sysctlbyname("hw.cpufrequency", processorName, &len, NULL, 0) == 0) {
            
            // M5 Max has 14 CPU cores (6 performance + 8 efficiency) and up to 40 GPU cores
            cpuCores = 14;
            gpuCores = 40;
        } else {
            // Default to generic Apple Silicon values
            cpuCores = 10;  // M1/M2 default
            gpuCores = 10;
        }
    #else
        cpuCores = 0;
        gpuCores = 0;
    #endif
}

/**
 * Get the maximum supported size for vectors in multiples of four bytes.  This
 * is the number of int or float values that can be contained in a vector.
 */
static int getVectorWidth() {
    if (isAvxSupported())
        return 8;
#ifdef __ARM_NEON
    if (isNeonSupported())
        return 4;  // NEON uses 128-bit vectors = 4 floats
#endif
    return 4;
}
#endif // OPENMM_HARDWARE_H_
