/* -------------------------------------------------------------------------- *
 *                                   OpenMM                                   *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit.                   *
 * See https://openmm.org/development.                                        *
 *                                                                            *
 * Portions copyright (c) 2025 Stanford University and the Authors.           *
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

#ifndef OPENMM_M5MAXUTILS_H_
#define OPENMM_M5MAXUTILS_H_

/**
 * Utility functions for Apple M5 Max optimization
 */

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

namespace OpenMM {

/**
 * Get information about Apple M5 Max chip features.
 */
static void getM5MaxFeatures(int& cpuCores, int& gpuCores) {
    #ifdef __APPLE__
        // Detect M5 Max based on processor name
        char processorName[256];
        size_t len = sizeof(processorName);
        
        // Try to get processor name
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
 * Get number of CPU cores optimized for M5 Max
 */
static int getM5MaxCpuCores() {
    int cpuCores, gpuCores;
    getM5MaxFeatures(cpuCores, gpuCores);
    return cpuCores;
}

/**
 * Get number of GPU cores optimized for M5 Max
 */
static int getM5MaxGpuCores() {
    int cpuCores, gpuCores;
    getM5MaxFeatures(cpuCores, gpuCores);
    return gpuCores;
}

/**
 * Check if running on M5 Max
 */
static bool isRunningOnM5Max() {
    int cpuCores, gpuCores;
    getM5MaxFeatures(cpuCores, gpuCores);
    return (cpuCores == 14 && gpuCores == 40);
}

} // namespace OpenMM

#endif // OPENMM_M5MAXUTILS_H_
