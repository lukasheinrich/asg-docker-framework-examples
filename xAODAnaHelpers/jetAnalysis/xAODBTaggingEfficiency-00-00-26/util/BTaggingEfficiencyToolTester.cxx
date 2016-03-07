#include "xAODBTaggingEfficiency/BTaggingEfficiencyTool.h"

#include <string>
#include <iomanip>

using CP::CorrectionCode;
using CP::SystematicCode;

int main() {
  
  xAOD::TStore store;

  BTaggingEfficiencyTool * tool = new BTaggingEfficiencyTool("BTagTest");
  // tagger name as specified in the CDI file
  tool->setProperty("TaggerName",          "MV2c20");
  // operating point as specified in the CDI file
  tool->setProperty("OperatingPoint",      "FixedCutBEff_77");
  // jet collection name as specified in the CDI file (for 2012, this contains also the JVF specification)
  tool->setProperty("JetAuthor",           "AntiKt4EMTopoJets");
  // name of the CDI file
  tool->setProperty("ScaleFactorFileName", "13TeV/2015-PreRecomm-13TeV-MC12-CDI-October23_v1.root");
  // calibration specification (there should always be a "default" available so this doesn't need to be set
  // tool->setProperty("ScaleFactorBCalibration", "ttbar_PDF_7b_SF");
  // specify an aggressive ("Tight") or not-so-aggressive ("Medium") eigenvector suppression scheme
  //         (this needs information stored in the calibration objects which will not be available for older files)
  // tool->setProperty("EigenvectorReductionB", "Medium");
  // tool->setProperty("EigenvectorReductionC", "Medium");
  // tool->setProperty("EigenvectorReductionLight", "Medium");
  // specify that the file is to be looked for in the PathResolver "development" area (don't use this for official productions)
  // tool->setProperty("UseDevelopmentFile",  true);
  // specify the use of the cone-based labelling rather than the ghost association
  tool->setProperty("ConeFlavourLabel", true);
  // uncomment this to use the Run-1 style cone labelling (parton based, inclusive) instead of the Run-2 default (hadron based, exclusive)
  // tool->setProperty("OldConeFlavourLabel", true);
  // uncomment this if the "Envelope" systematics model is to be used instead of the eigenvector variations
  // tool->setProperty("SystematicsStrategy", "Envelope");
  // A successful initialisation ought to be checked for
  StatusCode code = tool->initialize();
  if (code != StatusCode::SUCCESS) {
    std::cout << "Initialization of tool " << tool->name() << " failed! Subsequent results may not make sense." << std::endl;
  }

  std::cout << "-----------------------------------------------------" << std::endl;
  const std::map<CP::SystematicVariation, std::vector<std::string> > allowed_variations = tool->listSystematics();
  std::cout << "Allowed systematics variations for tool " << tool->name() << ":" << std::endl;
  for (auto var : allowed_variations) {
    std::cout << std::setw(40) << std::left << var.first.name() << ":";
    for (auto flv : var.second) std::cout << " " << flv;
    std::cout << std::endl;
  }
  std::cout << "-----------------------------------------------------" << std::endl;

  bool retval = true;

  std::cout << "Creating a jet" << std::endl;
  xAOD::JetFourMom_t p4(25000.,0.7,0.3,1000.);

  xAOD::Jet * jet = new xAOD::Jet();
  jet->makePrivateStore();
  std::cout << "Setting jet 4 momentum" << std::endl;
  jet->setJetP4(p4);
  std::cout << "Setting jet attribute" << std::endl;
  // light quark. Note that here and in the following, we fill both the Run-1 and Run-2 style flavour label variables
  jet->setAttribute("ConeTruthLabelID", 0);
  jet->setAttribute("HadronConeExclTruthLabelID", 0);
  float sf=0;
  CorrectionCode result;
  std::cout << "\nTesting function calls without systematics..." << std::endl;
  result = tool->getEfficiency(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Light quark get efficiency failed"<<std::endl; retval=false;}
  else {
    std::cout << "Light quark get efficiency succeeded: " << sf << std::endl;
  }
  result = tool->getScaleFactor(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Light quark get scale factor failed"<<std::endl; retval=false;}
  else {
    std::cout << "Light quark get scale factor succeeded: " << sf << std::endl;
  }

  jet->setAttribute("ConeTruthLabelID", 5);
  jet->setAttribute("HadronConeExclTruthLabelID", 5);
  sf=0;
  result = tool->getEfficiency(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Bottom quark get efficiency failed"<<std::endl; retval=false;}
  else {
    std::cout << "Bottom quark get efficiency succeeded: " << sf << std::endl;
  }
  result = tool->getScaleFactor(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Bottom quark get scale factor failed"<<std::endl; retval=false;}
  else {
    std::cout << "Bottom quark get scale factor succeeded: " << sf << std::endl;
  }

  jet->setAttribute("ConeTruthLabelID", 4);
  jet->setAttribute("HadronConeExclTruthLabelID", 4);
  sf=0;
  result = tool->getEfficiency(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Charm quark get efficiency failed"<<std::endl; retval=false;}
  else {
    std::cout << "Charm quark get efficiency succeeded: " << sf << std::endl;
  }
  result = tool->getScaleFactor(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Charm quark get scale factor failed"<<std::endl; retval=false;}
  else {
    std::cout << "Charm quark get scale factor succeeded: " << sf << std::endl;
  }

  jet->setAttribute("ConeTruthLabelID", 15);
  jet->setAttribute("HadronConeExclTruthLabelID", 15);
  sf=0;
  result = tool->getEfficiency(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Tau quark get efficiency failed"<<std::endl; retval=false;}
  else {
    std::cout << "Tau quark get efficiency succeeded: " << sf << std::endl;
  }
  result = tool->getScaleFactor(*jet,sf);
  if( result!=CorrectionCode::Ok) { std::cout << "Tau quark get scale factor failed"<<std::endl; retval=false;}
  else {
    std::cout << "Tau quark get scale factor succeeded: " << sf << std::endl;
  }

  // systematics interface
  jet->setAttribute("ConeTruthLabelID", 5);
  jet->setAttribute("HadronConeExclTruthLabelID", 5);
  std::cout << "\nTesting application of systematics to b jets..." << std::endl;
  CP::SystematicSet systs = tool->affectingSystematics();
  for( CP::SystematicSet::const_iterator iter = systs.begin();
       iter!=systs.end(); ++iter) {
    CP::SystematicVariation var = *iter;
    CP::SystematicSet set;
    set.insert(var);
    SystematicCode sresult = tool->applySystematicVariation(set);
    if( sresult !=SystematicCode::Ok) {
      std::cout << var.name() << " apply systematic variation FAILED " << std::endl;
	}
    result = tool->getScaleFactor(*jet,sf);
    if( result!=CorrectionCode::Ok) {
      std::cout << var.name() << " getScaleFactor FAILED" << std::endl;
    } else {
      std::cout << var.name() << " " << sf << std::endl;
    }
  }
  // don't forget to switch back off the systematics...
  CP::SystematicSet defaultSet;
  SystematicCode dummyResult = tool->applySystematicVariation(defaultSet);
  if (dummyResult != SystematicCode::Ok)
    std::cout << "problem disabling systematics setting!" << std::endl;

  // out-of-bounds tests:
  // (1) b jet outside the calibration range (300 GeV, at present) but inside the extrapolation range
  xAOD::JetFourMom_t p4Extrapolated(500000.,0.7,0.3,1000.);
  xAOD::Jet* extrapolatedJet = new xAOD::Jet;
  extrapolatedJet->makePrivateStore();
  std::cout << "\nSetting jet 4 momentum for b jet outside calibration but inside extrapolation range" << std::endl;
  extrapolatedJet->setJetP4(p4Extrapolated);
  extrapolatedJet->setAttribute("ConeTruthLabelID", 5);
  extrapolatedJet->setAttribute("HadronConeExclTruthLabelID", 5);
  result = tool->getScaleFactor(*extrapolatedJet,sf);
  switch (result) {
  case CorrectionCode::Error:
    std::cout << "extrapolated b-jet get scale factor failed" << std::endl; break;
  case CorrectionCode::OutOfValidityRange:
    std::cout << "extrapolated b-jet get scale factor should not be flagged as out-of-validity!" << std::endl; break;
  case CorrectionCode::Ok:
    std::cout << "extrapolated b-jet get scale factor OK" << std::endl; break;
  default:
    break;
  }
  // (2) b jet outside the extrapolation range (1200 GeV, at present)
  xAOD::JetFourMom_t p4Invalid(1500000.,0.7,0.3,1000.);
  xAOD::Jet* invalidJet = new xAOD::Jet;
  invalidJet->makePrivateStore();
  std::cout << "\nSetting jet 4 momentum for b jet outside extrapolation range" << std::endl;
  invalidJet->setJetP4(p4Invalid);
  invalidJet->setAttribute("ConeTruthLabelID", 5);
  invalidJet->setAttribute("HadronConeExclTruthLabelID", 5);
  result = tool->getScaleFactor(*invalidJet,sf);
  if (result != CorrectionCode::OutOfValidityRange) {
    std::cout << "invalid b-jet get scale factor should have been flagged as out-of-validity but is not!" << std::endl;
  } else {
    std::cout << "invalid b-jet get scale factor correctly flagged as out-of-validity" << std::endl;
  }

  // test tool copying

  // BTaggingEfficiencyTool* copiedTool = new BTaggingEfficiencyTool(*tool);
  // std::cout << "-----------------------------------------------------" << std::endl;
  // const std::map<CP::SystematicVariation, std::vector<std::string> > copied_variations = copiedTool->listSystematics();
  // std::cout << "Allowed systematics variations for tool " << copiedTool->name() << ":" << std::endl;
  // for (auto var : copied_variations) {
  //   std::cout << std::setw(40) << std::left << var.first.name() << ":";
  //   for (auto flv : var.second) std::cout << " " << flv;
  //   std::cout << std::endl;
  // }
  // std::cout << "-----------------------------------------------------" << std::endl;
  // sf=0;
  // result = copiedTool->getEfficiency(*jet,sf);
  // if( result!=CorrectionCode::Ok) { std::cout << "Bottom quark get efficiency failed"<<std::endl; retval=false;}
  // else {
  //   std::cout << "Bottom quark get efficiency succeeded: " << sf << std::endl;
  // }
  // result = copiedTool->getScaleFactor(*jet,sf);
  // if( result!=CorrectionCode::Ok) { std::cout << "Bottom quark get scale factor failed"<<std::endl; retval=false;}
  // else {
  //   std::cout << "Bottom quark get scale factor succeeded: " << sf << std::endl;
  // }


  // release our resources
  delete tool;

  return retval;
}
