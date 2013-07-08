# iSS-OSCAR

This is a slighty modified version of the iSS hypersurface sampler, written by Zhi Qiu and Chun Shen at Ohio State University.  It reads 2+1D hydro freeze-out
hypersurfaces in standard [OSCAR](https://wiki.bnl.gov/TECHQM/index.php/OSCAR_Standard_Output_Format_for_Hydro_Codes) format and outputs particles in 3+1D.


## Compilation

iSS-OSCAR is built via the standard CMake process:

  1. `mkdir build && cd build`
  2. `cmake ..`
  3. `make`
  4. `make install`

This will placed the compiled `iSS` binary in the project root.


## Usage

  * Edit `parameters.dat` as desired.  Some notable options:
    * MC\_sampling:  Might want to set this 2 for testing, since it is much faster, but produces less accurate flows.
    * number\_of\_repeated\_sampling:  How many times to sample the hypersurface.
  * Edit `EOS/chosen_particles.dat` as desired.  This list of Monte Carlo IDs determines the particle types which will be sampled.  Several presets are
     available:
      - `chosen_particles_3.dat`:  pi+, K+, and protons (this is the current setting).
      - `chosen_particles_urqmd.dat`:  All particles known to UrQMD (273).
      - `chosen_particles_all.dat`:  All particles (319).
  * Place an OSCAR hypersurface file in `surface/OSCAR2008H.dat`.
  * Call `./iSS`.  Parameters may be passed on the command line and will override `parameters.dat`.  Results will be placed in `results/`.


## Notes

Basic parsing of the OSCAR header is implemented.  Currently, three items are checked:

  * Geometry must be 'scaling2d'.
  * Grid must be 'Euler'.
  * Viscosity must be 'none' or 'shear viscosity only'.  If 'none', all components of the shear tensor are set to zero, else the components are expected in the
    OSCAR file following the normal vector.  The order of the components must be `pi00 pi01 pi02 pi11 pi12 pi22 pi33`.
