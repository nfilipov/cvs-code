import FWCore.ParameterSet.Config as cms

process = cms.Process("HIOnia")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.destinations = ['cout', 'cerr']
process.MessageLogger.cerr.FwkReport.reportEvery = 100

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#process.GlobalTag.globaltag = 'GR_R_39X_V6B::All' #re-reco
process.GlobalTag.globaltag = 'GR_R_44_V15::All' # prompt reco

process.HeavyIonGlobalParameters = cms.PSet(
    centralityVariable = cms.string("HFtowers"), #HFhits for prompt reco
    nonDefaultGlauberModel = cms.string(""),
    centralitySrc = cms.InputTag("hiCentrality")
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    "file:onia2MuMuPAT_112_1_8Ff.root"
    )
)

process.hltDoubleMuOpen = cms.EDFilter("HLTHighLevel",
                 TriggerResultsTag = cms.InputTag("TriggerResults","","HLT"),
                 HLTPaths = cms.vstring("HLT_DoubleMu3_Upsilon_v*"),
                 eventSetupPathsKey = cms.string(''),
                 andOr = cms.bool(True),
                 throw = cms.bool(False)
)

process.hionia = cms.EDAnalyzer('HiOniaAnalyzer',
                                srcMuon = cms.InputTag("patMuonsWithTrigger"),
                                srcMuonNoTrig = cms.InputTag("patMuonsWithTrigger"),
                                src = cms.InputTag("onia2MuMuPatTrkTrk"),
                                genParticles = cms.InputTag("genMuons"),
                                primaryVertexTag = cms.InputTag("offlinePrimaryVertices"),

                                #-- Reco Details
                                useBeamSpot = cms.bool(False),
                                useRapidity = cms.bool(True),
                                
                                #--
                                maxAbsZ = cms.double(24.0),
                                
                                pTBinRanges = cms.vdouble(0.0, 6.0, 8.0, 9.0, 10.0, 12.0, 15.0, 40.0),
                                etaBinRanges = cms.vdouble(0.0, 2.5),
                                centralityRanges = cms.vdouble(),

                                onlyTheBest = cms.bool(False),		
                                applyCuts = cms.bool(True),			
                                storeEfficiency = cms.bool(False),
                      
                                removeSignalEvents = cms.untracked.bool(False),
                                removeTrueMuons = cms.untracked.bool(False),
                                storeSameSign = cms.untracked.bool(True),
                                
                                #-- Gen Details
                                oniaPDG = cms.int32(553),
                                isHI = cms.untracked.bool(False),
                                isMC = cms.untracked.bool(False),
                                isPromptMC = cms.untracked.bool(False),

                                #-- Histogram configuration
                                combineCategories = cms.bool(False),
                                fillRooDataSet = cms.bool(False),
                                fillTree = cms.bool(True),
                                minimumFlag = cms.bool(True),
                                fillSingleMuons = cms.bool(True),
                                histFileName = cms.string("Upsilon_Histos.root"),		
                                dataSetName = cms.string("Upsilon_DataSet.root"),
                                
                                #--
                                NumberOfTriggers = cms.uint32(3),
                                )


#process.p = cms.Path(process.hltDoubleMuOpen + process.hionia)
process.p = cms.Path(process.hionia)
