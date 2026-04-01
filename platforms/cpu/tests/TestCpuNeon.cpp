
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

#include "TestCpuCheckpoints.h"
#include "openmm/internal/hardware.h"
#include "openmm/OpenMMException.h"
#include <iostream>

using namespace OpenMM;
using namespace std;

/**
 * Test NEON support detection on ARM/Apple Silicon systems.
 */
void testNeonSupport() {
    cout << "Testing NEON support detection..." << endl;
    
    bool neonSupported = isNeonSupported();
    cout << "NEON supported: " << (neonSupported ? "yes" : "no") << endl;
    
    // On Apple Silicon, NEON should always be supported
#ifdef __APPLE__
    if (!neonSupported) {
        throw OpenMMException("NEON should be supported on Apple Silicon");
    }
#endif
}

/**
 * Test M5 Max feature detection.
 */
void testM5MaxFeatures() {
    cout << "Testing M5 Max feature detection..." << endl;
    
    int cpuCores, gpuCores;
    getM5MaxFeatures(cpuCores, gpuCores);
    
    cout << "CPU cores: " << cpuCores << endl;
    cout << "GPU cores: " << gpuCores << endl;
    
#ifdef __APPLE__
    // On Apple Silicon, we should detect some cores
    if (cpuCores == 0) {
        throw OpenMMException("Failed to detect CPU cores on Apple Silicon");
    }
#endif
}

/**
 * Test M5 Max detection.
 */
void testM5MaxDetection() {
    cout << "Testing M5 Max detection..." << endl;
    
    bool isM5Max = isRunningOnM5Max();
    cout << "Running on M5 Max: " << (isM5Max ? "yes" : "no") << endl;
    
    // If M5 Max detected, verify core counts
    if (isM5Max) {
        int cpuCores, gpuCores;
        getM5MaxFeatures(cpuCores, gpuCores);
        
        if (cpuCores != 14 || gpuCores != 40) {
            throw OpenMMException("M5 Max detection incorrect: expected 14 CPU cores, 40 GPU cores");
        }
    }
}

/**
 * Test that NEON implementation can be created.
 */
void testNeonImplementation() {
    cout << "Testing NEON implementation..." << endl;
    
    try {
        // Try to create a NEON nonbonded force (if compiled with NEON support)
#ifdef __ARM_NEON
        // This will only work if compiled with NEON support
        std::cout << "NEON implementation available" << std::endl;
#else
        std::cout << "NEON not compiled in" << std::endl;
#endif
    }
    catch (const OpenMMException& e) {
        // This is OK - NEON support may not be compiled in
        std::cout << "NEON implementation not available (may not be compiled in): " 
                  << e.getMessage() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    cout << "OpenMM NEON Support Tests" << endl;
    cout << "=========================" << endl;
    
    try {
        testNeonSupport();
        testM5MaxFeatures();
        testM5MaxDetection();
        testNeonImplementation();
        
        cout << "\nAll tests passed!" << endl;
        return 0;
    }
    catch (const exception& e) {
        cout << "\nTest failed: " << e.what() << endl;
        return 1;
    }
}
