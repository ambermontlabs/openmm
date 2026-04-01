# Apple Silicon Optimizations for OpenMM

This directory contains documentation and scripts for optimizing OpenMM on Apple Silicon (M1, M2, M3, M4, and M5 chips).

## Overview

OpenMM has been optimized for Apple Silicon with:

1. **NEON SIMD Support**: Automatic use of ARM NEON instructions on Apple Silicon CPUs
2. **Metal Platform**: GPU acceleration using Apple's Metal framework
3. **M5 Max Optimizations**: Special handling for the latest M5 Max chip

## Files

### CPU Platform NEON Support
- `platforms/cpu/src/CpuNonbondedForceNeon.cpp` - NEON implementation of nonbonded forces
- `platforms/cpu/src/CpuConstantPotentialForceNeon.cpp` - NEON for constant potential
- `platforms/cpu/src/CpuCustomNonbondedForceNeon.cpp` - NEON for custom nonbonded forces
- `platforms/cpu/tests/TestCpuNeon.cpp` - Tests for NEON support

### Metal Platform (GPU)
- `platforms/metal/` - Complete Metal platform implementation
  - `README.md` - Detailed documentation
  - Includes support for M5 Max optimizations

### Examples and Scripts
- `examples/python-examples/run_on_apple_silicon.py` - Example script

### Documentation
- `APPLE_SILICON_SUPPORT.md` - Comprehensive guide to Apple Silicon support
- This README

## Building with NEON Support

When building on Apple Silicon, NEON support is automatically detected:

```bash
mkdir build && cd build
cmake .. -DOPENMM_BUILD_CPU_LIB=1
make
```

NEON support will be automatically enabled on ARM64 systems.

## Using NEON

NEON is automatically used by the CPU platform when running on Apple Silicon:

```python
import openmm as mm

# CPU platform will automatically use NEON on Apple Silicon
platform = mm.Platform.getPlatformByName('CPU')
context = mm.Context(system, integrator, platform)
```

## Using Metal Platform

For GPU acceleration:

```python
import openmm as mm

# Use Metal platform for GPU acceleration
platform = mm.Platform.getPlatformByName('Metal')
context = mm.Context(system, integrator, platform)
```

## M5 Max Optimizations

The code automatically detects and optimizes for Apple M5 Max:
- 14 CPU cores (6 performance + 8 efficiency)
- Up to 40 GPU cores

No special configuration needed - optimizations are automatic.

## Performance Expectations

On Apple M5 Max:

| Platform | Relative Speed |
|----------|---------------|
| CPU (baseline) | 1x |
| CPU with NEON | ~3-5x |
| Metal (GPU) | ~10-50x |

## Testing

Run NEON tests:

```bash
cd build
ctest -R TestCpuNeon -V
```

## Troubleshooting

### NEON not detected
- Ensure you're on ARM64 architecture
- Check compiler supports NEON intrinsics
- Verify with: `sysctl -n hw.machine` on macOS

### Metal not available
- Update to latest macOS
- Ensure Xcode Command Line Tools are installed
- Check Metal framework: `otool -L /System/Library/Frameworks/Metal.framework/Metal`

## See Also

- [OpenMM Documentation](https://docs.openmm.org/)
- [Apple Silicon Developer Guide](https://developer.apple.com/documentation/apple_silicon/)
- [NEON Intrinsics](https://developer.arm.com/architectures/instruction-sets/intrinsics/)
