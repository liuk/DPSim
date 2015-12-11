# DPSim - Dark Photon Simulation Package
This package is developed based on the existing Geant4 Monte Carlo simulation software of E906/SeaQuest experiment. It's intended for the Polarized Target (E1039) and dark photon search. The major changes are:

  - the output is in ROOT structure, no more MySQLs!
  - more generators (including dark photon generators) are/will be included;
  - GDML is used as geometry description, so that the persistent geometry could be shared between simulation and reconstruction efforts

### Compile and install

DPSim relies on following packages:

  - Geant4.10, please note that GDML support needs to be turned on when installing Geant4, cmake will search for `geant4-config`;
  - xerces-c, this is the XML parser, it's also needed by Geant4;
  - ROOT v5.34, DPSim has not been tested under ROOT6, cmake will search for `root-config`;
  - Pythia8.1xx, DPSim for now uses deprecated functions of Pythia8 and thus does not work under Pythia8.2 yet, cmake will search for `pythia8-config`;
  - LHAPDF, need to have at least CT10nlo PDF set installed, cmake will search for `lhapdf-config`;
  - MySQL5, no particular requirement, need to have MySQL python interface (MySQLdb) installed if user want to generate GDML file, cmake will search for `mysql_config`;
  - BOOST, if not installed at system area, user needs to set environmental variable `BOOST_ROOT` to point to the BOOST headers

With all the packages above provided, DPSim uses cmake to build. A typical installation process goes like this:

  - `git clone https://github.com/liuk/DPSim.git DPSim`
  - `cd DPSim; mkdir build; cd build`
  - `cmake ..; make`

After that, the executable file `DPSim` will be produced under `build/bin`, and can be used like `./bin/DPSim run_configuration`, an exmaple configuration file is provided under `DPSim/conf/example.conf`, with self-explainary options. 
