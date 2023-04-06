# SPH Fluid-Simulation
- Navier-Stokes Equations: particle acceleration calculation includes
  - Laplacian of the velocity field 
  - Pressure gradient
  - Diffusion / Divergence
  - Curl
  - Surface tensor
  - External force (gradient)
  
- Kernel weighted effects is applied
  - k-d tree algorithm is applied to expedite computation
 
- Support liquid with different density, kinetic, and surface tensor

- Integrator (Solver)
  - Verlet

- Dependency
  - OpenGL
  - [cyCodeBase](http://www.cemyuksel.com/cyCodeBase/)

- Demo
  
  ![](figure/fluid-sim.gif)
  
  - Some numerical cases were not handled perfectly, causing leakage at the edge of the container

> TAMU CSCE649, Fall 2022
