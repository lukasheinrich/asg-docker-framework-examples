#!/bin/bash
source /atlas-asg/get_rcsetup.sh 
rcSetup Base,2.3.38
git clone https://github.com/UCATLAS/xAODAnaHelpers
cd xAODAnaHelpers && git checkout 00-03-28 && cd -
git clone https://github.com/kratsg/OriginCorrectedJets
rc find_packages
rc build
