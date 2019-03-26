// cd /cms/ldap_home/iawatson/scratch/GEM/CMSSW_10_1_5/src/ && eval `scramv1 runtime -sh` && eval `scramv1 runtime -sh` && scram b -j 10
// cd ../../.. && source /cvmfs/cms.cern.ch/cmsset_default.sh && eval `scramv1 runtime -sh` && eval `scramv1 runtime -sh` && scram b -j 10
// system include files
#include <memory>
#include <cmath>
#include <iostream>
#include <sstream>
#include<map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

//#include "DQMServices/Core/interface/DQMStore.h"
//#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
//#include "DQMServices/Core/interface/MonitorElement.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
// ME0
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartition.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartitionSpecs.h"
// GEM
#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartition.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/Associations/interface/MuonToTrackingParticleAssociator.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "SimMuon/MCTruth/interface/MuonToSimAssociatorByHits.h"
#include "SimTracker/Common/interface/TrackingParticleSelector.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"

#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"
#include "Geometry/CommonTopologies/interface/RadialStripTopology.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Run.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TString.h"
#include "TGraphAsymmErrors.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TDatabasePDG.h"
#include <tuple>

using namespace std;
using namespace edm;

class MuonSimAnalyser : public edm::EDAnalyzer {
public:
  explicit MuonSimAnalyser(const edm::ParameterSet&);
  ~MuonSimAnalyser();

private:
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void beginJob() override;
  virtual void endJob() override;

  virtual void beginRun(Run const&, EventSetup const&) override;
  virtual void endRun(Run const&, EventSetup const&) override;

  void initValue();

  enum detType {DET_NONE = 0, DET_ME0, DET_GEM, DET_CSC, DET_DT, DET_RPC};

  bool isSimTrackGood(const SimTrack & , int = -1, detType = DET_NONE);
  std::tuple<SimTrack, unsigned int, unsigned int, int> matchSimHitToSimTrack(const PSimHit & sh, const edm::SimTrackContainer & sim_track, int pdg=-1, detType = DET_NONE); 
  int getMotherPdgId(const SimTrack & trk, const edm::PSimHitContainer & sim_hit, const edm::SimVertexContainer & sim_vertex);

  // ----------member data ---------------------------
  edm::ParameterSet cfg_;
  edm::EDGetToken ME0SimHitsToken_;
  edm::EDGetToken GEMSimHitsToken_;
  edm::EDGetToken simTracksToken_;
  edm::EDGetToken simVerticesToken_;
  
  edm::Service<TFileService> fs;

  float GEM_minEta_ = 1.5; float GEM_maxEta_ = 2.5;
  float ME0_minEta_ = 1.9; float ME0_maxEta_ = 3.;

  TTree *t_event;
  int b_nME0SimHits, b_nGEMSimHits;

  /* ME */
  TTree *t_ME0_simhit;
  int   b_ME0_SimHit_region, b_ME0_SimHit_chamber, b_ME0_SimHit_layer, b_ME0_SimHit_etaPartition, b_ME0_SimHit_pdgId, b_ME0_SimHit_momPdgId;
  float b_ME0_SimHit_pt,     b_ME0_SimHit_eta,     b_ME0_SimHit_phi,   b_ME0_SimHit_E;

  /* GEM */
  TTree *t_GEM_simhit;
  int   b_GEM_SimHit_region, b_GEM_SimHit_station, b_GEM_SimHit_ring, b_GEM_SimHit_chamber, b_GEM_SimHit_layer, b_GEM_SimHit_etaPartition, b_GEM_SimHit_pdgId, b_GEM_SimHit_momPdgId;
  float b_GEM_SimHit_pt,     b_GEM_SimHit_eta,     b_GEM_SimHit_phi,  b_GEM_SimHit_E; 

};

MuonSimAnalyser::MuonSimAnalyser(const edm::ParameterSet& iConfig)
{ 

  ME0SimHitsToken_ = consumes<edm::PSimHitContainer>(iConfig.getParameter<edm::InputTag>("ME0SimInputLabel"));
  GEMSimHitsToken_ = consumes<edm::PSimHitContainer>(iConfig.getParameter<edm::InputTag>("GEMSimInputLabel"));
  simTracksToken_ = consumes< edm::SimTrackContainer >(iConfig.getParameter<edm::InputTag>("simTrackCollection"));
  simVerticesToken_ = consumes< edm::SimVertexContainer >(iConfig.getParameter<edm::InputTag>("simVertexCollection"));
  cfg_ = iConfig;



  t_event = fs->make<TTree>("Event", "Event");
  t_event->Branch("nME0SimHits",   &b_nME0SimHits,   "nME0SimHits/I");
  t_event->Branch("nGEMSimHits",   &b_nGEMSimHits,   "nGEMSimHits/I");

  /*ME0*/
  t_ME0_simhit = fs->make<TTree>("ME0_SimHit", "ME0_SimHit");
  t_ME0_simhit->Branch("SimHit_pdgId",        &b_ME0_SimHit_pdgId,        "SimHit_pdgId/I");
  t_ME0_simhit->Branch("SimHit_momPdgId",     &b_ME0_SimHit_momPdgId,     "SimHit_momPdgId/I");
  t_ME0_simhit->Branch("SimHit_pt",           &b_ME0_SimHit_pt,           "SimHit_pt/F");
  t_ME0_simhit->Branch("SimHit_eta",          &b_ME0_SimHit_eta,          "SimHit_eta/F");
  t_ME0_simhit->Branch("SimHit_phi",          &b_ME0_SimHit_phi,          "SimHit_phi/F");
  t_ME0_simhit->Branch("SimHit_E",            &b_ME0_SimHit_E,            "SimHit_E/F");
  t_ME0_simhit->Branch("SimHit_region",       &b_ME0_SimHit_region,       "SimHit_region/I");
  t_ME0_simhit->Branch("SimHit_chamber",      &b_ME0_SimHit_chamber,      "SimHit_chaber/I");
  t_ME0_simhit->Branch("SimHit_layer",        &b_ME0_SimHit_layer,        "SimHit_layer/I");
  t_ME0_simhit->Branch("SimHit_etaPartition", &b_ME0_SimHit_etaPartition, "SimHit_etaParition/I");

  /*GEM*/
  t_GEM_simhit = fs->make<TTree>("GEM_SimHit", "GEM_SimHit");
  t_GEM_simhit->Branch("SimHit_pdgId",        &b_GEM_SimHit_pdgId,        "SimHit_pdgId/I");
  t_GEM_simhit->Branch("SimHit_momPdgId",     &b_GEM_SimHit_momPdgId,     "SimHit_momPdgId/I");
  t_GEM_simhit->Branch("SimHit_pt",           &b_GEM_SimHit_pt,           "SimHit_pt/F");
  t_GEM_simhit->Branch("SimHit_eta",          &b_GEM_SimHit_eta,          "SimHit_eta/F");
  t_GEM_simhit->Branch("SimHit_phi",          &b_GEM_SimHit_phi,          "SimHit_phi/F");
  t_GEM_simhit->Branch("SimHit_E",            &b_GEM_SimHit_E,            "SimHit_E/F");
  t_GEM_simhit->Branch("SimHit_region",       &b_GEM_SimHit_region,       "SimHit_region/I");
  t_GEM_simhit->Branch("SimHit_station",      &b_GEM_SimHit_station,      "SimHit_station/I");
  t_GEM_simhit->Branch("SimHit_ring",         &b_GEM_SimHit_ring,         "SimHit_ring/I");
  t_GEM_simhit->Branch("SimHit_chamber",      &b_GEM_SimHit_chamber,      "SimHit_chaber/I");
  t_GEM_simhit->Branch("SimHit_layer",        &b_GEM_SimHit_layer,        "SimHit_layer/I");
  t_GEM_simhit->Branch("SimHit_etaPartition", &b_GEM_SimHit_etaPartition, "SimHit_etaParition/I");

}

MuonSimAnalyser::~MuonSimAnalyser()
{
}

void
MuonSimAnalyser::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  /* ME0 Geometry */
  edm::ESHandle<ME0Geometry> hME0Geom;
  iSetup.get<MuonGeometryRecord>().get(hME0Geom);
  const ME0Geometry* ME0Geometry_ = &*hME0Geom;

  /* GEM Geometry */
  edm::ESHandle<GEMGeometry> hGEMGeom;
  iSetup.get<MuonGeometryRecord>().get(hGEMGeom);
  const GEMGeometry* GEMGeometry_ = &*hGEMGeom;

  edm::Handle<edm::PSimHitContainer> ME0SimHits;
  edm::Handle<edm::PSimHitContainer> GEMSimHits;
  edm::Handle<edm::SimTrackContainer> simTracks;
  edm::Handle<edm::SimVertexContainer> simVertices;
  iEvent.getByToken(ME0SimHitsToken_, ME0SimHits);
  iEvent.getByToken(GEMSimHitsToken_, GEMSimHits);
  iEvent.getByToken(simTracksToken_, simTracks);
  iEvent.getByToken(simVerticesToken_, simVertices);
  //if ( !simHits.isValid() || !simTracks.isValid() || !simVertices.isValid()) return;

  initValue();

  const edm::SimTrackContainer & sim_tracks = *simTracks.product();
  const edm::SimVertexContainer & sim_vertices = *simVertices.product();


  /* ME0 */
  if (ME0SimHits.isValid()) {
    const edm::PSimHitContainer & ME0_sim_hits = *ME0SimHits.product();
    for (auto& sh : ME0_sim_hits){
      ME0DetId det_id(sh.detUnitId());
      auto vec                   = sh.momentumAtEntry();
      auto shLp                  = sh.localPosition();
      const BoundPlane & surface = ME0Geometry_->idToDet(det_id)->surface();
      auto shGp                  = surface.toGlobal(shLp);

      b_ME0_SimHit_pdgId        = sh.particleType();
      auto sim_track_tuple = matchSimHitToSimTrack(sh, sim_tracks);
      b_ME0_SimHit_momPdgId = getMotherPdgId(std::get<0>(sim_track_tuple), ME0_sim_hits, sim_vertices);
      std::cout << " >>>>>>> TUPLE : [ " << std::get<0>(sim_track_tuple) << " ] / [ " << std::get<1>(sim_track_tuple) << " ] / [ " << std::get<2>(sim_track_tuple) << " ] / [ " << std::get<3>(sim_track_tuple) << " ] " <<  std::endl;

      TDatabasePDG *pdg = TDatabasePDG::Instance();
      if (auto particle_pdg = pdg->GetParticle(b_ME0_SimHit_pdgId)) {
        std::cout << particle_pdg->GetName() << " " << b_ME0_SimHit_pdgId << " mom pdg : " << b_ME0_SimHit_momPdgId << std::endl;
      } else {
        std::cout << "Cannot understand!!! " << b_ME0_SimHit_pdgId << std::endl;
      }

      b_ME0_SimHit_pt           = vec.perp();
      b_ME0_SimHit_eta          = shGp.eta();
      b_ME0_SimHit_phi          = shGp.phi();
      b_ME0_SimHit_E            = sh.pabs();

      b_ME0_SimHit_region       = det_id.region();
      b_ME0_SimHit_chamber      = det_id.chamber();
      b_ME0_SimHit_layer        = det_id.layer();
      b_ME0_SimHit_etaPartition = det_id.roll();

      ++b_nME0SimHits;
      t_ME0_simhit->Fill();
    }
  } else return;


  /* GEM */
  if (GEMSimHits.isValid()) {
    const edm::PSimHitContainer & GEM_sim_hits = *GEMSimHits.product();
    for (auto& sh : GEM_sim_hits){
      GEMDetId det_id(sh.detUnitId());
      auto vec                   = sh.momentumAtEntry();
      auto shLp                  = sh.localPosition();
      const BoundPlane & surface = GEMGeometry_->idToDet(det_id)->surface();
      auto shGp                  = surface.toGlobal(shLp);
      
      b_GEM_SimHit_pdgId        = sh.particleType();
      auto sim_track_tuple = matchSimHitToSimTrack(sh, sim_tracks);
      b_GEM_SimHit_momPdgId = getMotherPdgId(std::get<0>(sim_track_tuple), GEM_sim_hits, sim_vertices);
      std::cout << " >>>>>>> TUPLE : [ " << std::get<0>(sim_track_tuple) << " ] / [ " << std::get<1>(sim_track_tuple) << " ] / [ " << std::get<2>(sim_track_tuple) << " ] / [ " << std::get<3>(sim_track_tuple) << " ] " <<  std::endl;
 
      TDatabasePDG *pdg = TDatabasePDG::Instance();
      if (auto particle_pdg = pdg->GetParticle(b_GEM_SimHit_pdgId)) {
        std::cout << particle_pdg->GetName() << " " << b_GEM_SimHit_pdgId << " mom pdg : " << b_GEM_SimHit_momPdgId << std::endl;    
      } else {
        std::cout << "Cannot understand!!! " << b_GEM_SimHit_pdgId << std::endl;
      }
  
      b_GEM_SimHit_pt           = vec.perp();
      b_GEM_SimHit_eta          = shGp.eta();
      b_GEM_SimHit_phi          = shGp.phi();
      b_GEM_SimHit_E            = sh.pabs();

      b_GEM_SimHit_region       = det_id.region();
      b_GEM_SimHit_station      = det_id.station();
      b_GEM_SimHit_ring         = det_id.ring();
      b_GEM_SimHit_chamber      = det_id.chamber();
      b_GEM_SimHit_layer        = det_id.layer();
      b_GEM_SimHit_etaPartition = det_id.roll();
  
      ++b_nGEMSimHits;
      t_GEM_simhit->Fill();
    }
  } else return;
  t_event->Fill();
}

void MuonSimAnalyser::beginJob(){}
void MuonSimAnalyser::endJob(){}

void MuonSimAnalyser::beginRun(const edm::Run& run, const edm::EventSetup& iSetup){//(Run const& run, EventSetup const&){
}
void MuonSimAnalyser::endRun(Run const&, EventSetup const&){}

void MuonSimAnalyser::initValue() {
  b_nME0SimHits = -1; b_nGEMSimHits = -1;

  /*ME0 */
  b_ME0_SimHit_region = -9; b_ME0_SimHit_chamber = -9; b_ME0_SimHit_layer = -9; b_ME0_SimHit_etaPartition = -9; b_ME0_SimHit_pdgId = -99; b_ME0_SimHit_momPdgId = -99;
  b_ME0_SimHit_pt     = -9; b_ME0_SimHit_eta     = -9; b_ME0_SimHit_phi  = -9;  b_ME0_SimHit_E = -9;

  /*GEM */
  b_GEM_SimHit_region = -9; b_GEM_SimHit_station = -9; b_GEM_SimHit_ring = -9; b_GEM_SimHit_chamber = -9; b_GEM_SimHit_layer = -9; b_GEM_SimHit_etaPartition = -9; b_GEM_SimHit_pdgId = -99; b_GEM_SimHit_momPdgId = -99;
  b_GEM_SimHit_pt     = -9; b_GEM_SimHit_eta     = -9; b_GEM_SimHit_phi  = -9; b_GEM_SimHit_E = -9;
}

bool MuonSimAnalyser::isSimTrackGood(const SimTrack &t, int pdg, detType detName) {
  // SimTrack selection
  if (t.noVertex()) return false;
  //if (t.noGenpart()) return false;
  if (pdg != -1 ) {
    if (t.type() != pdg) return false; // only interested in pdg matched particle
  }
  //if (t.momentum().pt() < minPt_ ) return false;
  const float eta(std::abs(t.momentum().eta()));
  if (detName == DET_GEM) {
    if (eta > GEM_maxEta_ || eta < GEM_minEta_ ) return false; // no GEMs could be in such eta
  } else if (detName == DET_ME0) {
    if (eta > ME0_maxEta_ || eta < ME0_minEta_ ) return false; // no ME0 could be in such eta
  }
  return true;
}

std::tuple<SimTrack, unsigned int, unsigned int, int> MuonSimAnalyser::matchSimHitToSimTrack(const PSimHit & sh, const edm::SimTrackContainer & sim_track, int pdg, detType detName) {
  int pdgId = sh.particleType();
  for (auto & trk : sim_track) {
    if (!isSimTrackGood(trk, pdg, detName)) continue;
    if (sh.trackId() != trk.trackId()) continue;
    return std::make_tuple(trk, trk.vertIndex(), trk.genpartIndex(), pdgId);
  }
  return std::make_tuple(SimTrack(), -1, -1, pdgId);
}

int MuonSimAnalyser::getMotherPdgId(const SimTrack & trk, const edm::PSimHitContainer & sim_hit, const edm::SimVertexContainer & sim_vertex){
  unsigned int p_vtx_id = 0;
  for (auto & vtx : sim_vertex) {
    if (vtx.noParent()) continue;
    if((int)vtx.vertexId() == trk.vertIndex()) p_vtx_id = vtx.parentIndex();
  }
  for (auto & sh : sim_hit) {
    if (sh.trackId() == p_vtx_id) return sh.particleType();
  } 
  return -999;
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonSimAnalyser);
