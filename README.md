# DPSim - Dark Photon Simulation Package
This package is developed based on the existing Geant4 Monte Carlo simulation software of E906/SeaQuest experiment. It's intended for the Polarized Target (E1039) and dark photon search. The major changes are:

  - the output is in ROOT structure, no more MySQLs!
  - more generators (including dark photon generators) are/will be included;
  - GDML is used as geometry description, so that the persistent geometry could be shared between simulation and reconstruction efforts
