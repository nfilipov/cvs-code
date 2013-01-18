#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TROOT.h>

#include <TH1.h>
#include <TH2D.h>

#include <TCanvas.h>
#include "TChain.h"
#include <TDirectory.h>
#include <TFile.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TMath.h>
#include <TStyle.h>
#include <TSystem.h>
#include "TTree.h"
#include "TString.h"

// miscellaneous  
#include <fstream>
#include <map>
#include <string>
#include <iostream>
#include <stdio.h>

#endif

/*
This macro will calculate and plot different trigger rates vs time (s)
 */

void calculateTriggerRates(
			   TString inFile0Name = "root://eoscms//eos/cms/store/caf/user/velicanu/PA2013_merged_HiForest/pPb_hiForest2_pilotRun_200kHz_v3.root",
//         TString inFile0Name = "/castor/cern.ch/user/m/miheejo/openHLT/cms442/r181530_reco_v1_2/HIExpressPhysics_hiexp-hirun2011-r181530-reco-v1_2.root",
//         "/castor/cern.ch/user/k/kimy/openHLT//openhlt_run181531.root",
//         "/castor/cern.ch/user/v/velicanu/HIHLT_Validation_Test_GRIF_v10.root",
			   Int_t runNum        = 202792,
			   TString outdir      = "output",
			   char *projectTitle  = "HIpARun2013",
			   string source       = "data"
			   )
{
  char szBuf[256];
  int scale = 23;


  // event selectoin
  //Form("&&Run==%d&&LumiBlock>%d",runNum,goodLumiStart)
  // trigger under investigation
 //  const int ntrigs = 8;
//   const char* triggerPath[ntrigs] = {"","HLT_HIMinBiasHfOrBSC",
// 				     "L1_SingleMu3_BptxAND","HLT_HIL1SingleMu3","HLT_HIL2Mu3",
// 				     "L1_DoubleMuOpen_BptxAND","HLT_HIL1DoubleMuOpen","HLT_HIL2DoubleMu3"};

  /* const int ntrigs = 9;
  const char* triggerPath[ntrigs] = {
    "",
    "HLT_PAL1SingleMuOpen_v1",
    "HLT_PAL1SingleMu3_v1",
    "HLT_PAL1SingleMu7_v1",
    "HLT_PAL1SingleMu12_v1",
    "HLT_PAL1DoubleMu0_v1",
    "HLT_PADimuon0_NoVertexing_v1",
    "HLT_PAMu5_v1",
    "HLT_PAMu8_v1"
    }; */
  //trigger list for singleMu rate plot
  const int ntrigs = 4 ;
  const char* triggerPath[ntrigs] = {
    "",
    "HLT_PAMu3_v1",
    "HLT_PAMu7_v1",
    "HLT_PAMu12_v1"
  };
  /*
  //trigger list for DoubleMu rate plot
  const int ntrigs = 4 ;
  const char* triggerPath[ntrigs] = {
    "",
    "HLT_PAL1DoubleMuOpen_v1",
    "HLT_PAL1DoubleMu0_HighQ_v1",
    "HLT_PAL2DoubleMu3_v1"
  };
  
  //trigger list fo bJet+Mu rate plot
  const int ntrigs =5;
  const char* triggerPath[ntrigs] = {
    "",
    "HLT_PAMu3PFJet20_v1",
    "HLT_PAMu3PFJet40_v1",
    "HLT_PAMu7PFJet20_v1",
    "HLT_PABTagMu_Jet20_Mu4_v1"    
    };*/
  
  TString str;
  TH1D *ahTemp[ntrigs];
  double ahTempRate[ntrigs];  //Rates (Integrated over lumisections)
  // Load input
  TChain * HltTree = new TChain("hltanalysis/HltTree","HI OpenHLT Tree");
  HltTree->Add(inFile0Name);
  cout << inFile0Name << endl;
  cout << " # entries: " << HltTree->GetEntries() << endl;
  int nEvents = HltTree->GetEntries();
  for(int it=1; it<ntrigs; it++)
  {
    
    TH1D *ph  = new TH1D("ph",";Lumi Section; Counts ",1100,0,1100);
    HltTree->Draw("LumiBlock>>ph",str.Format("%s",triggerPath[it]));
    TLegend *lTemp= new TLegend(0.2,0.8,0.8,0.9);
    lTemp->SetHeader(str.Format("Run : %d",runNum));
    lTemp->SetMargin(0.05);
    lTemp->SetFillStyle(0);
    lTemp->SetLineColor(0);
    lTemp->SetLineWidth(5.0);
    lTemp->SetTextSize(0.03); 
    lTemp->AddEntry(ph,str.Format("%s",triggerPath[it],"l")); 
    lTemp->Draw("same");
    c1->SaveAs(str.Format("%d_%s.pdf",runNum,triggerPath[it]));
    TH1D *phLumi = (TH1D*)gDirectory->Get("ph");
    phLumi->SetDirectory(0);
    phLumi->Scale(1./(phLumi->GetBinWidth(1)*23));// 1lumi unit=23 sec
    ahTempRate[it] = phLumi->Integral()/phLumi->GetNbinsX();
    ahTemp[it] = phLumi;

    cout<< triggerPath[it]<<"\t"<<phLumi->GetEntries()<< "\t" << ahTempRate[it] << endl;
   
  }
     
  //----------------------------
  // drawing part
  // axis
  //  TH1D * phLumiAxis = new TH1D("phLumiAxis",";Lumi Section;dEvt/dLumiSec",1100,0,1100);
  TH1D * phLumiAxis = new TH1D("phLumiAxis",";Lumi Section;Rate [Hz]",1,0,1200);
  phLumiAxis->SetMinimum(0.01);
  phLumiAxis->SetMaximum(1e+3);
  gStyle->SetOptStat(kFALSE);

  // legend
  TLegend *l0= new TLegend(0.2,0.7,0.8,0.9);
  l0->SetHeader(str.Format("Run : %d",runNum));
  l0->SetMargin(0.03);
  l0->SetFillStyle(0);
  l0->SetLineColor(0);
  l0->SetLineWidth(1.0);
  l0->SetTextSize(0.03); 
  
  // canvas
  TCanvas *pcLumi = new TCanvas("pcLumi","pcLumi");
  pcLumi->cd();
  phLumiAxis->Draw();
  pcLumi->SetLogy();
 
  for(int it=1; it<ntrigs; it++)
    {
      TH1 *phLocal = (TH1 *)(ahTemp[it]->Clone("phLocal"));
      phLocal->SetDirectory(0); 
      phLocal->SetLineColor(it);
      if (it >= 10) phLocal->SetLineColor(it+20);
      if(it==5)	phLocal->SetLineColor(kOrange+2);
      phLocal->Draw("same");
      l0->AddEntry(phLocal,str.Format("%s: %.2f Hz",triggerPath[it],ahTempRate[it]),"l");
      pcLumi->Update();
     
    }
  l0->Draw("same");
  pcLumi->SaveAs(str.Format("%d_ratedMu.png",runNum));
}
