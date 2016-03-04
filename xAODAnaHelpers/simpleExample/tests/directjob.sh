#!/bin/bash

source rcSetup.sh 
source $ROOTSYS/bin/thisroot.sh 
curl -O http://physics.nyu.edu/~lh1132/test_ttbar_AOD.pool.root
xAH_run.py --files test_ttbar_AOD.pool.root --config simpleconfig.py direct
