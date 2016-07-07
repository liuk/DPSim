export DPSIM_ROOT=$(cd $(dirname ${BASH_SOURCE[0]}); pwd)
export PATH=$PATH:$DPSIM_ROOT/build/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DPSIM_ROOT/build/lib
