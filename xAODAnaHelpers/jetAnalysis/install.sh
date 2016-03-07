#!/bin/bash
source /atlas-asg/get_rcsetup.sh 
rcSetup Base,2.3.38
git clone https://github.com/UCATLAS/xAODAnaHelpers
cd xAODAnaHelpers && git checkout 00-03-28 && cd -
git clone https://github.com/kratsg/OriginCorrectedJets
git clone https://github.com/kratsg/xAODJetReclustering
cd xAODJetReclustering && git checkout useJetRecTrimmer && cd ../
sed -i 's|Jelpers|Helpers|g' xAODAnaHelpers/scripts/checkoutASGtags.py
python xAODAnaHelpers/scripts/checkoutASGtags.py 2.3.38
rc find_packages
rc build
