#!/bin/sh
ZSIMPATH=/home/temp/whb/HSCC_climber/zsim-nvmain
PINPATH=$ZSIMPATH/pin_kit
ZSIMBIN=$ZSIMPATH/bin/zsim
NVMAINPATH=$ZSIMPATH/nvmain
BOOST=/usr/local/boost-1.59
HDF5=/usr/local/hdf5-1.8.3
MPICHPATH=/home/wxy/mpich
GUPSPATH=/home/wxy/gups
PATH=/home/wxy/mpich/lib:$PATH
PYTHONPATH=/usr/include/python2.7
PYTHONBIN=/usr/bin/lib/python2.7
LIBCONFIG=/usr/local/libconfig-1.5
LD_LIBRARY_PATH=$PINPATH/intel64/lib:$PINPATH/intel64/runtime:$PINPATH/intel64/lib:$PINPATH/intel64/lib-ext:$BOOST/lib:$HDF5/lib:/usr/local/gmp/lib:/usr/local/mpfr/lib:/usr/local/mpc/lib:$LIBCONFIG/lib:/usr/local/lib:/home/wxy/mpich/lib:/usr/lib64:
INCLUDE=$INCLUDE:$HDF5/include:$LIBCONFIG/include:/usr/local/lib:$PYTHONPATH:/usr/lib64
LIBRARY_PATH=$LIBRARY_PATH:$HDF5/lib:$LIBCONFIG/lib:$PYTHONBIN:/usr/lib64:
CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:$HDF5/include:$LIBCONFIG/include:$PYTHONPATH

HSCC_CONFIG=/home/whb/config
PBBPATH=/home/temp/whb/benchmark/pbbs
CPUSPECPATH=/home/temp/whb/benchmark/spec_cpu2006/benchspec/CPU2006
PARSECPATH=/home/temp/whb/benchmark/parsec-3.0/pkgs
SIMPOINT_DIR=/home/whb/hscc/simpoints
#export PBBPATH CPUSPECPATH PARSECPATH
export PATH MPICHPATH GUPSPATH ZSIMPATH PINPATH NVMAINPATH LD_LIBRARY_PATH BOOST CPLUS_INCLUDE_PATH LIBRARY_PATH HSCC_CONFIG PBBPATH CPUSPECPATH PARSECPATH SIMPOINT_DIR ZSIMBIN
