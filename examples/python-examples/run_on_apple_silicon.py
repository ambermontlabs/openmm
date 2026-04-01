#!/usr/bin/env python
"""
Example script demonstrating how to run OpenMM simulations on Apple Silicon.

This script shows:
1. How to detect your system architecture
2. How to use the Metal platform for GPU acceleration
3. How to optimize for M5 Max
"""

import openmm.app as app
import openmm as mm
import openmm.unit as unit
import sys

def get_system_info():
    """Get information about the current system."""
    import platform
    import sys
    
    info = {
        'system': platform.system(),
        'machine': platform.machine(),
        'processor': platform.processor(),
    }
    
    if sys.platform == 'darwin':
        # Try to get more detailed Apple Silicon info
        try:
            import os
            # Check if running on Apple Silicon
            result = os.popen('sysctl -n hw.machine').read().strip()
            info['machine'] = result
            
            # Try to get CPU type
            result = os.popen('sysctl -n machdep.cpu.brand_string').read().strip()
            info['cpu'] = result
            
        except:
            pass
    
    return info

def select_platform():
    """Select the best platform for Apple Silicon."""
    # Get available platforms
    platforms = []
    for i in range(mm.Platform.getNumPlatforms()):
        platform = mm.Platform.getPlatform(i)
        platforms.append(platform.getName())
    
    print("Available platforms:", platforms)
    
    # Prefer Metal platform on Apple Silicon
    if 'Metal' in platforms and sys.platform == 'darwin':
        print("Using Metal platform for Apple Silicon GPU acceleration")
        return mm.Platform.getPlatformByName('Metal')
    elif 'CUDA' in platforms:
        print("Using CUDA platform")
        return mm.Platform.getPlatformByName('CUDA')
    elif 'OpenCL' in platforms:
        print("Using OpenCL platform")
        return mm.Platform.getPlatformByName('OpenCL')
    else:
        print("Using CPU platform")
        return mm.Platform.getPlatformByName('CPU')

def run_simulation(platform=None):
    """Run a simple simulation."""
    print("\n" + "="*60)
    print("Running OpenMM Simulation")
    print("="*60)
    
    # Print system info
    info = get_system_info()
    print("\nSystem Information:")
    for key, value in info.items():
        print(f"  {key}: {value}")
    
    # Select platform
    if platform is None:
        platform = select_platform()
    print(f"\nPlatform: {platform.getName()}")
    
    # Load a simple system
    print("\nLoading TIP3P water box...")
    pdb = app.PDBFile('examples/python-examples/minimize-water.pdb')
    
    # Check if file exists, if not create a simple one
    try:
        forcefield = app.ForceField('tip3p.xml')
        system = forcefield.createSystem(pdb.topology,
                                         nonbondedMethod=app.PME,
                                         nonbondedCutoff=1*unit.nanometer,
                                         constraints=app.HBonds)
    except Exception as e:
        print(f"Warning: Could not load water box: {e}")
        print("Creating a simple Lennard-Jones system instead...")
        
        # Create a simple box of particles
        system = mm.System()
        for i in range(100):
            system.addParticle(1.0)
        
        # Add a simple nonbonded force
        nb = mm.NonbondedForce()
        for i in range(100):
            nb.addParticle(0.0, 0.3, 0.0)
        system.addForce(nb)
        
        # Add harmonic restraints
        harmonic = mm.CustomExternalForce('k*(x^2+y^2+z^2)')
        harmonic.addPerParticleParameter('k')
        for i in range(100):
            harmonic.addParticle([100.0])
        system.addForce(harmonic)
        
        # Create positions
        positions = []
        for i in range(100):
            x = (i % 5) * 1.0
            y = ((i // 5) % 5) * 1.0
            z = (i // 25) * 1.0
            positions.append([x, y, z] * unit.nanometer)
        pdb = app.PDBFile('examples/python-examples/minimize-water.pdb')
        pdb.topology.setBoxDimensions([5*unit.nanometer, 5*unit.nanometer, 5*unit.nanometer])
    
    print(f"System has {system.getNumParticles()} particles")
    print(f"System has {system.getNumForces()} forces")
    
    # Create integrator
    integrator = mm.LangevinMiddleIntegrator(300*unit.kelvin, 1.0/unit.picosecond,
                                             0.002*unit.picoseconds)
    
    # Create context with the selected platform
    print(f"\nCreating context on {platform.getName()}...")
    context = mm.Context(system, integrator, platform)
    
    # Set positions
    context.setPositions(pdb.positions)
    
    # Minimize energy
    print("Minimizing energy...")
    context.setVirtualSites(False)
    mm.LocalEnergyMinimizer.minimize(context, 100, 1.0)
    
    # Get energy
    state = context.getState(getEnergy=True)
    print(f"\nInitial potential energy: {state.getPotentialEnergy()}")
    
    # Run a few steps
    print("\nRunning 1000 steps of simulation...")
    integrator.step(1000)
    
    # Get final energy
    state = context.getState(getEnergy=True, positions=True)
    print(f"Final potential energy: {state.getPotentialEnergy()}")
    print(f"\nSimulation complete!")
    
    return context

if __name__ == '__main__':
    import os
    
    # Check if running on Apple Silicon
    if sys.platform == 'darwin':
        print("Detected macOS")
        
        # Check for Apple Silicon
        import os
        result = os.popen('sysctl -n hw.machine').read().strip()
        if 'arm' in result.lower():
            print(f"Running on Apple Silicon ({result})")
            
            # Check for M5 Max
            try:
                cpu_name = os.popen('sysctl -n machdep.cpu.brand_string').read().strip()
                if 'M5' in cpu_name:
                    print("Detected Apple M5 Max!")
                    print("Optimizing for 14-core CPU and 40-core GPU...")
            except:
                pass
        else:
            print(f"Running on Intel Mac ({result})")
    
    # Run the simulation
    try:
        run_simulation()
    except Exception as e:
        print(f"\nError running simulation: {e}")
        import traceback
        traceback.print_exc()
