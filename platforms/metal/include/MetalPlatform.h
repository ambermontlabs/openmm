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

#ifndef OPENMM_METALPLATFORM_H_
#define OPENMM_METALPLATFORM_H_

#include "openmm/Platform.h"

namespace OpenMM {

/**
 * This class is a Platform that uses Metal for GPU acceleration on Apple Silicon.
 */

class OPENMM_EXPORT_METAL MetalPlatform : public Platform {
public:
    MetalPlatform();
    
    /**
     * Get the name of this platform.
     */
    const std::string& getName() const {
        static const std::string name = "Metal";
        return name;
    }
    
    /**
     * Get an estimate of how fast this Platform class is.
     */
    double getSpeed() const;
    
    /**
     * Get whether this Platform supports double precision arithmetic.
     */
    bool supportsDoublePrecision() const {
        return false;
    }
    
    /**
     * Check whether this platform is supported on the current system.
     */
    static bool isProcessorSupported();
    
private:
    friend class MetalKernelFactory;
};

} // namespace OpenMM

#endif // OPENMM_METALPLATFORM_H_
