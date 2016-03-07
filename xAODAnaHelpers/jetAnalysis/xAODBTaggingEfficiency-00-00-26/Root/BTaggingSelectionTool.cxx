#include "xAODBTaggingEfficiency/BTaggingSelectionTool.h"
#include "xAODBTagging/BTagging.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"

#include "PATInterfaces/SystematicRegistry.h"
#include "PathResolver/PathResolver.h"

#include "TFile.h"

#include <algorithm>

using std::string;

using CP::CorrectionCode;
using CP::SystematicSet;
using CP::SystematicVariation;
using CP::SystematicCode;
using CP::SystematicRegistry;

using Analysis::Uncertainty;
using Analysis::CalibrationDataVariables;
using Analysis::CalibrationDataContainer;
using Analysis::CalibResult;
using Analysis::CalibrationStatus;
using Analysis::Total;
using Analysis::SFEigen;
using Analysis::SFNamed;
using Analysis::None;

using xAOD::IParticle;



BTaggingSelectionTool::BTaggingSelectionTool( const std::string & name)
  : asg::AsgTool( name ), m_accept( "JetSelection" ), m_spline(nullptr), m_constcut(nullptr)
{
  m_initialised = false;
  declareProperty( "MaxEta", m_maxEta = 2.5 );
  declareProperty( "MinPt", m_minPt = 20000 /*MeV*/);
  declareProperty( "MaxRangePt", m_maxRangePt = 1000000 /*MeV*/);
  declareProperty( "FlvTagCutDefinitionsFileName", m_CutFileName = "", "name of the files containing official cut definitions (uses PathResolver)");
  declareProperty("TaggerName",                    m_taggerName="",    "tagging algorithm name");
  declareProperty("OperatingPoint",                m_OP="",            "operating point");
  declareProperty("JetAuthor",                     m_jetAuthor="",     "jet collection");
}

StatusCode BTaggingSelectionTool::initialize() {
  m_initialised = true;

  TString pathtofile =  PathResolverFindCalibFile(m_CutFileName);
  m_inf = TFile::Open(pathtofile, "read");
  if (0==m_inf) {
    ATH_MSG_ERROR( "BTaggingSelectionTool couldn't access tagging cut definitions" );
    return StatusCode::FAILURE;
  }
  
  // The tool only support flat efficiencies, Akt4TopoEM jets with MV2c20 for now
  if ("MV2c20"!=m_taggerName){
    ATH_MSG_ERROR( "BTaggingSelectionTool doesn't support tagger: "+m_taggerName );
    return StatusCode::FAILURE;
  }
  if ("AntiKt4EMTopoJets"  != m_jetAuthor &&
      "AntiKt2PV0TrackJets"!= m_jetAuthor &&
      "AntiKt3PV0TrackJets"!= m_jetAuthor
      ){
    ATH_MSG_ERROR( "BTaggingSelectionTool doesn't support jet collection: "+m_jetAuthor );
    return StatusCode::FAILURE;
  }

  // Change the minPt cut if the user didn't touch it
  if (20000==m_minPt){// is it still teh default value
    if ("AntiKt2PV0TrackJets"== m_jetAuthor) m_minPt=10000 ;
    if ("AntiKt3PV0TrackJets"== m_jetAuthor) m_minPt= 7000 ;
  }
  
  
  TString cutname = m_OP;
  if ("FlatBEff"==cutname(0,8)){
    cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_OP+"/cutprofile";
    m_spline = (TSpline3*) m_inf->Get(cutname);
    if (m_spline == nullptr) ATH_MSG_ERROR( "Invalid operating point" );
  }
  else {
    cutname = m_taggerName+"/"+m_jetAuthor+"/"+m_OP+"/cutvalue";
    m_constcut = (TVector*) m_inf->Get(cutname);
    if (m_constcut == nullptr) ATH_MSG_ERROR( "Invalid operating point" );
  }

  m_inf->Close();

  m_accept.addCut( "Eta", "Selection of jets according to their pseudorapidity" );
  m_accept.addCut( "Pt",  "Selection of jets according to their transverse momentum" );
  m_accept.addCut( "WorkingPoint",  "Working point for flavour-tagging of jets according to their b-tagging weight" );

  return StatusCode::SUCCESS;
}
const Root::TAccept& BTaggingSelectionTool::getTAccept() const {
  return m_accept;
}

const Root::TAccept& BTaggingSelectionTool::accept( const xAOD::IParticle* p ) const { 
  // Reset the result:
  m_accept.clear();
 
  // Check if this is a jet:
  if( p->type() != xAOD::Type::Jet ) {
    ATH_MSG_ERROR( "accept(...) Function received a non-jet" );
    return m_accept;
  }
 
  // Cast it to a jet:
  const xAOD::Jet* jet = dynamic_cast< const xAOD::Jet* >( p );
  if( ! jet ) {
    ATH_MSG_FATAL( "accept(...) Failed to cast particle to jet" );
    return m_accept;
  }
 
  // Let the specific function do the work:
  return accept( *jet );
}

const Root::TAccept& BTaggingSelectionTool::accept( const xAOD::Jet& jet ) const {
  m_accept.clear();

  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingSelectionTool has not been initialised");
    return m_accept;
  }


  if ("AntiKt2PV0TrackJets"== m_jetAuthor ||
      "AntiKt3PV0TrackJets"== m_jetAuthor
      ){
    // We want at least 2 tracks in a track jet
    m_accept.setCutResult( "NConstituents", jet.numConstituents() >= 2 );    
  }

  double pT = jet.pt();
  double eta = jet.eta();

  // Retrieve the tagger weight which was assigned to the jet
  double weight_mv2(-10.);
  const xAOD::BTagging* btag = jet.btagging();
  if ((!btag) || (!btag->MVx_discriminant(m_taggerName, weight_mv2))){
    ATH_MSG_ERROR("Failed to retrieve "+m_taggerName+" weight!");
  }
  ATH_MSG_VERBOSE( "MV2c20 " <<  weight_mv2 );
  
  return accept(pT, eta, weight_mv2);
}

const Root::TAccept& BTaggingSelectionTool::accept(double pT, double eta, double weight_mv2) const
{
  m_accept.clear();

  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingSelectionTool has not been initialised");
    return m_accept;
  }

  // flat cut for out of range pTs
  if (pT>m_maxRangePt)
    pT = m_maxRangePt-500; // 500 MeV below the maximum authorized range

  eta = std::fabs(eta);

  if (! checkRange(pT, eta))
    return m_accept;

  // After initialization, either m_spline or m_constcut should be non-zero
  // Else, the initialization was incorrect and should be revisited
  double cutvalue(DBL_MAX);
  if (m_spline != nullptr && m_constcut == nullptr) {
    cutvalue = m_spline->Eval(pT/1000.);
  }
  else if (m_constcut != nullptr && m_spline == nullptr) {
    cutvalue = m_constcut[0](0);
  }
  else ATH_MSG_ERROR( "Bad cut configuration!" );
  ATH_MSG_VERBOSE( "Cut value " << cutvalue );

  if (  weight_mv2 < cutvalue ){
    return m_accept;
  }
  m_accept.setCutResult( "WorkingPoint", true );
    
  // Return the result:
  return m_accept;
}

int BTaggingSelectionTool::getQuantile( const xAOD::Jet& jet ) const
{
  if (! m_initialised) {
    ATH_MSG_ERROR("BTaggingSelectionTool has not been initialised");
  }

  double bins[]={-0.9678, -0.9266, -0.7887, -0.5911,
                 -0.3734, -0.0436, 0.1886, 0.4496, 0.6062,
                 0.7404, 0.8302, 0.889, 0.9272, 0.9524,
                 0.9688, 0.9796, 0.9866, 0.9912, 0.9944,1.01};

  // Retrieve the tagger weight which was assigned to the jet
  double weight_mv2(-10.);
  if (!jet.btagging()->MVx_discriminant(m_taggerName, weight_mv2)){
    ATH_MSG_ERROR("Failed to retrieve "+m_taggerName+" weight!");
  }
  ATH_MSG_VERBOSE( "MV2c20 " <<  weight_mv2 );
 
  int bin_index(-1);
  for (int i=0; i<=19; ++i) {
    if (weight_mv2 < bins[i]) {
      bin_index = i;
      break;
    }
  }
  return bin_index;
}

bool BTaggingSelectionTool::checkRange(double pT, double eta) const
{
  // Do the |eta| cut:
  if( eta > m_maxEta ) {
    return false;
  }
  m_accept.setCutResult( "Eta", true );

  // Do the pT cut:
  ATH_MSG_VERBOSE( "Jet pT: " << pT );
  if( pT < m_minPt ) {
    return false;
  }
  m_accept.setCutResult( "Pt", true );

  return true;
}
