#!/bin/sh
voms-proxy-init --voms atlas
source ./rcSetup.sh 
source /usr/local/bin/thisroot.sh 
echo "root://faxbox.usatlas.org//user/kratsg/pflowTest.root" >> files.list
xAH_run.py --files files.list --inputList --config OriginCorrectedJets/data/config.py direct
