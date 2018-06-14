import FWCore.ParameterSet.Config as cms
from Configuration.StandardSequences.Eras import eras

process = cms.Process('SliceTestAnalysis',eras.Run2_2018)

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_data', '')
#process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
process.source = cms.Source("PoolSource", fileNames = cms.untracked.vstring())
process.source.fileNames.append("/store/user/jlee/SingleMuon/crab_20180612_230657/180612_140727/0000/gemAOD_81.root")

process.options = cms.untracked.PSet()

process.TFileService = cms.Service("TFileService",fileName = cms.string("histo.root"))

process.SliceTestAnalysis = cms.EDAnalyzer('SliceTestAnalysis',
    gemRecHits = cms.InputTag("gemRecHits"),
)
process.p = cms.Path(process.SliceTestAnalysis)
