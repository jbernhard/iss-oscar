# iSS-OSCAR

This is a slighty modified version of the iSS hypersurface sampler, written by Zhi Qiu and Chun Shen at Ohio State University.  It reads 2+1D hydro
freeze-out hypersurfaces in standard OSCAR format and outputs particles in 3+1D.


## Compilation

iSS-OSCAR is built via the standard CMake process:

  1. `mkdir build && cd build`
  2. `cmake ..`
  3. `make`
  4. `make install`

This will placed the compiled `iSS` binary in project root.


## Usage

  * Edit `parameters.dat` as desired.
  * Edit `EOS/chosen_particles.dat` as desired.  This list of Monte Carlo IDs determines the particle types which will be sampled.  Several presets are
     available:
      - `chosen_particles_3.dat`:  pi+, K+, and protons (this is the current setting).
      - `chosen_particles_urqmd.dat`:  All particles known to UrQMD (273).
      - `chosen_particles_all.dat`:  All particles (319).
  * Place an OSCAR hypersurface file in `surface/OSCAR2008H.dat`.
  * Call `./iSS`.  Parameters may be passed on the command line and will override `parameters.dat`.  Results will be placed in `results/`.
