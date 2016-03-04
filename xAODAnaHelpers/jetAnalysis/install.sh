#!/bin/bash
source /atlas-asg/get_rcsetup.sh 
rcSetup Base,2.3.38
git clone https://github.com/UCATLAS/xAODAnaHelpers
git clone https://github.com/kratsg/OriginCorrectedJets
rc find_packages
rc build
