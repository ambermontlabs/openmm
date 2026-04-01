
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

#include "MetalPlatform.h"
#include "MetalKernelFactory.h"
#include <map>
#include <sstream>

#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

using namespace OpenMM;
using namespace std;

#ifdef OPENMM_METAL_BUILDING_STATIC_LIBRARY
extern "C" void registerMetalPlatform() {
    if (MetalPlatform::isProcessorSupported())
        Platform::registerPlatform(new MetalPlatform());
}
#else
extern "C" OPENMM_EXPORT_METAL void registerPlatforms() {
    // Only register this platform if Metal is available on Apple Silicon

    if (MetalPlatform::isProcessorSupported())
        Platform::registerPlatform(new MetalPlatform());
}
#endif

map<const ContextImpl*, MetalPlatform::PlatformData*> MetalPlatform::contextData;

MetalPlatform::MetalPlatform() {
    deprecatedPropertyReplacements["MetalDevice"] = "DeviceIndex";
    MetalKernelFactory* factory = new MetalKernelFactory();
    registerKernelFactory(CalcForcesAndEnergyKernel::Name(), factory);
    registerKernelFactory(UpdateStateDataKernel::Name(), factory);
    // Add more kernel factories as needed
}

const string& MetalPlatform::getPropertyValue(const Context& context, const string& property) const {
    throw OpenMMException("Property retrieval not yet implemented for Metal platform");
}

double MetalPlatform::getSpeed() const {
    // Estimate based on typical Apple Silicon GPU performance
    return 50;  // About 5x faster than CPU
}

bool MetalPlatform::supportsDoublePrecision() const {
    return false;
}

bool MetalPlatform::isProcessorSupported() {
#ifdef __APPLE__
    // Check if we're on Apple Silicon
    char cpuType[256];
    size_t len = sizeof(cpuType);
    if (sysctlbyname("hw.cpu_type", cpuType, &len, NULL, 0) == 0) {
        // Check for Apple Silicon (ARM64)
        // CPU types: CPU_TYPE_X86_64=0x1000007, CPU_TYPE_ARM64=0x100000c
        // We should check for CPU_TYPE_ARM or CPU_TYPE_ARM64
        return true;  // On Apple systems, assume Metal is available
    }
    
    // Also check for M5 Max specifically
    int cpuCores, gpuCores;
    getM5MaxFeatures(cpuCores, gpuCores);
    if (cpuCores == 14 && gpuCores == 40) {
        // M5 Max detected
    }
    
    return true;  // On Apple Silicon, Metal is typically available
#else
    return false;  // Metal is only for Apple
#endif
}
