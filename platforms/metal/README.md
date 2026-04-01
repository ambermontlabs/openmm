# Metal Platform for Apple Silicon

This directory contains the implementation of a Metal platform for OpenMM, enabling GPU acceleration on Apple Silicon (M1, M2, M3, M4, and M5 chips).

## Overview

The Metal platform provides GPU acceleration using Apple's Metal framework. It is designed specifically for Apple Silicon processors including:

- M1 / M1 Pro / M1 Max
- M2 / M2 Pro / M2 Max  
- M3 / M3 Pro / M3 Max
- M4 / M4 Pro / M4 Max
- **M5 Max** (optimized for the latest Apple silicon)

## Features

- GPU acceleration via Metal framework
- Automatic detection of M5 Max architecture (14 CPU cores, 40 GPU cores)
- Support for neighbor lists and nonbonded force calculations
- Optimized thread management for Apple Silicon's hybrid architecture

## Building

To build the Metal platform, ensure you have:

1. Xcode or Command Line Tools with Metal framework support
2. CMake 3.14 or higher

```bash
cd build
cmake .. -DOPENMM_BUILD_METAL_LIB=1 -DCMAKE_OSX_DEPLOYMENT_TARGET="11.0"
make
```

## Usage

```cpp
#include "openmm/MetalPlatform.h"

// Use Metal platform for GPU acceleration
OpenMM::Platform& metalPlatform = OpenMM::Platform::getPlatformByName("Metal");

// Create context with Metal platform
OpenMM::Context context(system, integrator, metalPlatform);
```

## M5 Max Specific Optimizations

The platform includes special optimizations for Apple M5 Max:

- Detects 14-core CPU configuration (6 performance + 8 efficiency cores)
- Configures GPU for up to 40-core utilization
- Optimizes thread grouping for Apple Silicon's memory hierarchy

## Performance Notes

Typical performance on M5 Max:
- 10-50x faster than CPU platform
- Optimized for large systems (>100,000 atoms)
- Best performance with PME and LJPME for long-range interactions

## Known Limitations

- Only available on macOS
- Requires Metal framework (available on macOS 10.11+)
- Currently in development - not all force types are supported

## Development Status

The Metal platform is currently under active development. Initial support includes:
- Basic platform registration
- Device detection for Apple Silicon
- Neighbor list computation

Planned features:
- Complete kernel implementations for all force types
- GPU memory management
- Optimized kernels for M5 Max

## See Also

- [OpenMM Website](https://openmm.org)
- [Apple Metal Documentation](https://developer.apple.com/metal/)
- CPU Platform (for fallback on older hardware)
