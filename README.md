# DPSim - Dark Photon Simulation Package
This package is developed based on the existing Geant4 Monte Carlo simulation software of E906/SeaQuest experiment. It's intended for the Polarized Target (E1039) and dark photon search. The major changes are:

  - the output is in ROOT structure, no more MySQLs, yay!
  - more generators (including dark photon generators) are/will be included;
  - GDML is used as geometry description, so that the persistent geometry could be shared between simulation and reconstruction efforts

***

### Compile, install and run

DPSim relies on following packages:

  - Geant4.10, please note that GDML support needs to be turned on when installing Geant4, cmake will search for `geant4-config`;
  - xerces-c, this is the XML parser, it's also needed by Geant4;
  - ROOT v5.34, although there shouldn't be a problem, DPSim has not been tested under ROOT6. cmake will search for `root-config`;
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

After that, the executable file `DPSim` will be produced under `build/bin`, and can be used like `./bin/DPSim run_configuration`, an example configuration file is provided under `DPSim/conf/example.conf`, with self-explainary options. A shared library `libRawMCEvent` will be produced under `build/lib`, optionally one can use `make install` to install the header and shared library to ROOT header and library places, as specified by `root-config --prefix`.

A sample ROOT macro is provided under the directory `analysis_example` for some very basic analysis using DPSim MC event structure.

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

  - in geometry schema, add one entry in the `Planes` and `Alignments` table, with incrementing detectorID, if the new detector is intended for trigger, an appropriate triggerLv field needs to be assigned, otherwise it should be set to -1;
  - in `inc/DPMCRawEvent.h`, change line 14 to the new total number of detector planes (default is 48);
  - optional: in `inc/DPTriggerAnalyzer.h`, change line 14 to the new total number of trigger planes (default is 4). The input trigger matrix table needs to include the new information as well;

After the above steps, re-compile and re-run the `gdmlWriter.py` to generate the new GDML file and specify it in the conf file. DPSim will take care of the simulation and digitization.

***

### FermiGrid support

In scripts directory, a python script `runDPSim.py` is provided to support running DPSim batch jobs either locally or on FermiGrid. Run `./runDPSim.py -h` for more details.

### Line-by-line explaination of the conf file

```
# example configuration file to be parsed by DPSim
# all blank lines or lines begin with # will be ignored
# all attributes need to be initialized from this file,
# thus no line can be deleted

# normal lines with attributes like key-val pair should look like this
#       key  val

# boolean fields can be set to true by Yes/No, capital insensitive

# environmental variables are supported, e.g.:
#       magnetPath $HOME/resources

# random seed, used as seed for Geant4 and pythia, also used as runID
seed 1

# number of events to run
nEvents 200

# print frequency on event level, will print time info every printFreq events
printFreq 500

# output file name
outputFileName output.root

# output mode (all, primary, hits, dimuon)
## all means everything (tracks even if there is no hits)
## primary means only the tracks generated by generator will be saved (even it's not within acceptance),
##   secondaries will be discarded to save space
## hits means it will only save events and tracks with at least on hit in the detector
## dimuon is only useful in dimuon mode, and will only save a event if both muon tracks are accepted
outputMode hits

# beam setup
## bucket_size is only useful in singles mode, and the amount of bucket_size pp collisions is merged into  
##   one events
bucket_size  40000

## beam momentum is needed to calculate the center-of-mass frame energy, should almost always be 120 GeV
beamMomentum 120.
beamCurrent  2.E12

# geometry setup
## This is the input file of GDML file, target selection is implied in this option
geometryGDMLInput $DPSIM_ROOT/geometry/test.gdml

## This table includes the resolution and efficiency of all channles of all detectors, if
##   not provided, perfect efficiency/resolution will be assumed
detectorEffResol  $DPSIM_ROOT/geometry/detectorEff.txt

## This MySQL schema, and corresponding MySQL server is neede to store detailed specs of digitization
geometrySchema    geometry_ver1
mysqlServer       localhost
mysqlPort         3306
login             seaguest
password          qqbar2mu+mu-

# trigger matrix setup, if not provided, only NIM trigger bits are set
triggerMatrix     $DPSIM_ROOT/conf/trigger_57.txt

# magnetic field setup, one can change the polarity and absolute scale by the multipliers
fMagMap         $DPSIM_ROOT/resources/tab.Fmag
kMagMap         $DPSIM_ROOT/resources/tab.Kmag
fMagMultiplier  1.0
kMagMultiplier  1.0

# event generator setup
## generatorType will specify the generator type, available options include dimuon, single, external
## generatorEng will specify the generator engine used

### dimuon means dimuon generator, available engines include:
#### legacyDY, legacyJPsi, legacyPsip, these are the original E906 GMC generators
#### PHSP, this stands for pure phase space generator
#### pythia, this stands for the pythia generator (one can specify whatever generator in pythia cards)
#### DarkPhotonFromEta, this stands for A' generated by eta decay, which is generated by pp collisions
#### custom, this generator generates phasespace dimuon pairs, and set cross section by external 2D look-up tables

### single means single generator, available engines include:
#### pythia, this generator reads in pythia cards and simulate single pp collisions
#### geant, this generator simply shoots 120 GeV (or wharever specified by beamMomentum) to the spectrometer
#### test, this generator shoots single particles within kinematic constrains

### external generator will read externalInput file and feed to Geant4
generatorType     dimuon
generatorEng      legacyDY

## This option is only needed when generatorEng is pythia
pythiaConfig      $DPSIM_ROOT/conf/pythia8_inclusive.cfg

## This option is needed when the generator type is external. The input file needs to contain a tree named save,
##   with the following branches:
##     eventID     int, unique event identifier, does not have to be continuous
##     n           int, number of particles in this event
##     pdg         int[], pdg code of all particles
##     pos         TClonesArray of TVector3, initial positions of all particles
##     mom         TClonesArray of TVector3, initial momenta of all particles
externalInput     external.root

## This option is needed when the generatorEng is custom
customLUT         $DPSIM_ROOT/conf/DY.lut

## This option specifies the particle type in single particle test generator
testParticle      mu

## This option sets the physics list used for material effects, default is QGSP_BERT
physicsList       QGSP_BERT

# beam line component toggle
## include target
targetInBeam yes
## include beam dump
dumpInBeam   yes
## include the instrumentation package
instruInBeam no
## include the air gap between various components
airInBeam    no

# vertex generator bias, don't change, will be discarded
biasVertexGen no

# force pion/kaon decay, don't change, will be discarded
forcePionDecay no
forceKaonDecay no

# optional analysis module enable
## Dummy recon will perform the track fitting part and
enableDummyRecon no

# optional z-vertex offset --- this will only be applied to dimuons if z_min < zmax
zOffsetMin 1.
zOffsetMax -1.

# phase space constrains
x1Min 0.
x1Max 1.
x2Min 0.
x2Max 1.
xfMin -1.
xfMax 1.
massMin 4.
massMax 10.
cosThetaMin -1.
cosThetaMax 1.

# whether to ignore the conflicts/warnings in the configuration
ignoreWarnings no
```
