# DPSim - Dark Photon Simulation Package
This package is developed based on the existing Geant4 Monte Carlo simulation software of E906/SeaQuest experiment. It's intended for the Polarized Target (E1039) and dark photon search. The major changes are:

  - the output is in ROOT structure, no more MySQLs, yay!
  - more generators (including dark photon generators) are/will be included;
  - GDML is used as geometry description, so that the persistent geometry could be shared between simulation and reconstruction efforts

***

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

  ```
  git clone https://github.com/liuk/DPSim.git DPSim
  cd DPSim; mkdir build; cd build
  cmake ..; make
  ```

Following arguments can be specified in cmake:

  - `-DLIBONLY=ON (default: OFF)`: this will skip the simulation and only build the ROOT event structure, and thus only requires ROOT, for users who only want to analyze the output of DPSim
  - `-DDEBUG=ON (default: OFF)`: this will remove the `-DNDEBUG` in cflags, and thus enable all the assertions, which are disabled in the Release mode
  - `-DDEBUG_IN=ON (default: OFF)`: this will enable debugging output during initialization process
  - `-DDEBUG_TR=ON (default: OFF)`: this will enable debugging output during tracking process
  - `-DDEBUG_IO=ON (default: OFF)`: this will enable debugging output during I/O process

After that, the executable file `DPSim` will be produced under `build/bin`, and can be used like `./bin/DPSim run_configuration`, an exmaple configuration file is provided under `DPSim/conf/example.conf`, with self-explainary options. A shared library `libRawMCEvent` will be produced under `build/lib`, optionally one can use `make install` to install the header and shared library to ROOT header and library places, as specified by `root-config --prefix`.

***

### Generating GDML files and set target type

Currently there are two sources of geometry needed for DPSim to run:

  1. mysql schema from MySQL database (this part will be replaced by sqlite3 in future, so that we do not rely on any kind of external server to run), this schema contains all the details of the spectrometer setup;
  2. GDML file, generated from the MySQL-based geometry setup. This file only contains one target, and is used as the input for DetectorConstruction;

#### Making MySQL schema

A `.sql` file comes with the DPSim package under `geometry` directory, one can run `mysql -u user -p -h host geometry_schema_name < geometry_file_name.sql` to generate the MySQL-based geometry schema.

#### Making GDML file

A GDML file is needed in the configuration file to start `DPSim` simulation job (in this way the target type is implicitly specified). A python script `gdmlWriter.py` is provided to read the MySQL geometry schema and make the GDML file. This script requires Python package `MySQLdb` and `xml` to work properly.

For instance, if one would like to generate the GDML file with LD2 target from the geometry schema `geometry_v1` on `localhost`, the command looks like this:

  ```
  ./gdmlWriter --input=geometry_v1 --output=geom_LD2.gdml --target=LD2 --server=localhost --port=3306
  ```

***

### Adding additional (trigger) detector

DPSim is designed for R&D of dark photon instrumentation. It is very straightforward to add detectors into the spectrometer and even include it in the trigger. For instance, if one wants to add an addition layer of Y-measuring hodoscopes in front of KMag, one can follow the steps below:

  - in geometry schema: add one entry in the `Planes` and `Alignments` table, with incrementing detectorID, if the new detector is intended for trigger, an appropriate triggerLv field needs to be assigned, otherwise it should be set to -1;
  - in `inc/DPMCRawEvent.h`: change line 14 to the new total number of detector planes (default is 48);
  - in `inc/DPTriggerAnalyzer.h`: change line 14 to the new total number of trigger planes (default is 4);

After the above steps, re-run the `gdmlWriter.py` to generate the new GDML file and specify it in the conf file. DPSim will take care of the simulation and digitization.
