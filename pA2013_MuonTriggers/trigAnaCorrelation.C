#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <vector>
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"
#include "TH2.h"
#include "TPaletteAxis.h"
#include "TCanvas.h"
#include "TSystem.h"

using namespace std;

vector <string>* triggers = new vector<string>;
vector <string>* triggerCuts = new vector<string>;
vector< vector <double*> >* results = new vector< vector <double*> >;
TString goutdir;
TString anaMode;


double calcEffErr(double nEvt,double nAcc)
{
  double eff =  nAcc / nEvt;
  return sqrt(eff*(1-eff)/nEvt);
}

double* calcEff(TTree* HltTree, const char *title, double nEvt,TString cut,int flag=1)
{
  double nAcc = HltTree->GetEntries(cut);
  double effErr = calcEffErr(nEvt,nAcc);
  double eff = nAcc/nEvt;
  double *result = new double[2];
  result[0]=eff;
  result[1]=effErr;

  // print out or not
  if (flag) {
    triggers->push_back(string(title));
    triggerCuts->push_back(string(cut));
    cout <<" | "<<setw(20)<<title;
    cout <<" | "<<setw(10)<<fixed<<setprecision(3)<<eff*100.<<"% +-";
    cout        <<setw(10)<<fixed<<setprecision(3)<<effErr*100.<<"%";
    cout <<" | "<<endl;
  }

  return result;
}

void printEff(TTree* HltTree,const char *cut,const char *title, char *projectTitle)
{
  cout <<"   * "<<title<<":"<<endl;
  cout <<"   * " <<cut<<endl;
  cout <<"      * Efficiencies:"<<endl;
  double nEvt = HltTree->GetEntries(cut);

  cout <<" | "<<setw(20)<<"HLT Path";
  cout <<" | "<<setw(25)<<"Efficiency";
  cout <<" | "<<endl;

  triggers->clear();
  triggerCuts->clear();

  vector <double*> effs;
  TString str;    //Should not erase this line to prevent Form() breaks down!

  // calculate the efficiency //
  effs.push_back(calcEff(HltTree,"OR of all triggers",nEvt,Form("(%s)&&1==1",cut)));
  if (anaMode=="L1MB") {
    // BSC Coinc
    effs.push_back(calcEff(HltTree,"L1_BscMinBiasThreshold1_5bx",nEvt,Form("(%s)&&L1_BscMinBiasThreshold1",cut)));
    effs.push_back(calcEff(HltTree,"L1_BscMinBiasThreshold1_BptxAND",nEvt,Form("(%s)&&L1_BscMinBiasThreshold1_BptxAND",cut)));
    // HF Coinc
    effs.push_back(calcEff(HltTree,"L1_HcalHfCoincidencePm_BptxAND",nEvt,Form("(%s)&&L1_HcalHfCoincidencePm_BptxAND",cu)));
    // ZDC
    effs.push_back(calcEff(HltTree,"L1_ZdcCaloPlus_ZdcCaloMinus",nEvt,Form("(%s)&&L1_ZdcCaloPlus_ZdcCaloMinus",cut)));
    // High mul
    effs.push_back(calcEff(HltTree,"L1Tech_BSC_HighMultiplicity.v0",nEvt,Form("(%s)&&L1Tech_BSC_HighMultiplicity.v0",cut)));
    // -- protected w/ bsc2 veto --
    //effs.push_back(calcEff(HltTree,"L1_NotBsc2_BscMinBiasOR",nEvt,Form("(%s)&&L1_NotBsc2_BscMinBiasOR",cut)));
    //effs.push_back(calcEff(HltTree,"L1_NotBsc2_BscMinBiasThreshold1",nEvt,Form("(%s)&&L1_NotBsc2_BscMinBiasThreshold1",cut)));
    //effs.push_back(calcEff(HltTree,"L1_NotBsc2_HcalHfCoincidencePm",nEvt,Form("(%s)&&L1_NotBsc2_HcalHfCoincidencePm",cut)));
    // -- protected w/ bptx and bsc2 veto --
    //effs.push_back(calcEff(HltTree,"L1_NotBsc2_BptxAND_BscMinBiasOR",nEvt,Form("(%s)&&L1_NotBsc2_BptxAND_BscMinBiasOR",cut)));
  }
  if (anaMode=="HLTMB") {
    effs.push_back(calcEff(HltTree,"HLT_HIL1Algo_Unprotected",nEvt,Form("(%s)&&HLT_HIL1Algo_Unprotected",cut)));
    effs.push_back(calcEff(HltTree,"HLT_HIL1Algo_BptxAND",nEvt,Form("(%s)&&HLT_HIL1Algo_BptxAND",cut)));
    effs.push_back(calcEff(HltTree,"HLT_HIL1Algo_NotBSC2",nEvt,Form("(%s)&&HLT_HIL1Algo_NotBSC2",cut)));
    effs.push_back(calcEff(HltTree,"HLT_HIL1Algo_BptxAND_NotBSC2",nEvt,Form("(%s)&&HLT_HIL1Algo_BptxAND_NotBSC2",cut)));
  }
  if (anaMode=="L1Mu") {
    effs.push_back(calcEff(HltTree,"L1_SingleMu0",nEvt,Form("(%s)&&L1_SingleMu0",cut)));
    effs.push_back(calcEff(HltTree,"L1_SingleMuOpen",nEvt,Form("(%s)&&L1_SingleMuOpen",cut)));
    effs.push_back(calcEff(HltTree,"L1_SingleMu3",nEvt,Form("(%s)&&L1_SingleMu3",cut)));
    effs.push_back(calcEff(HltTree,"L1_SingleMu5",nEvt,Form("(%s)&&L1_SingleMu5",cut)));
    effs.push_back(calcEff(HltTree,"L1_SingleMu7",nEvt,Form("(%s)&&L1_SingleMu7",cut)));
    effs.push_back(calcEff(HltTree,"L1_DoubleMuOpen",nEvt,Form("(%s)&&L1_DoubleMuOpen",cut)));
    effs.push_back(calcEff(HltTree,"L1_DoubleMuOpen_BptxAND",nEvt,Form("(%s)&&L1_DoubleMuOpen_BptxAND",cut)));
    effs.push_back(calcEff(HltTree,"L1_DoubleMu3",nEvt,Form("(%s)&&L1_DoubleMu3",cut)));
  }
  if (anaMode=="HLTMu") {
    effs.push_back(calcEff(HltTree,"HLT_PAL1SingleMuOpen_v1",nEvt,Form("(%s)&&HLT_PAL1SingleMuOpen_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAL1SingleMu3_v1",nEvt,Form("(%s)&&HLT_PAL1SingleMu3_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAL1SingleMu7_v1",nEvt,Form("(%s)&&HLT_PAL1SingleMu7_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_HLT_PAL1SingleMu12_v1",nEvt,Form("(%s)&&HLT_PAL1SingleMu12_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAL1DoubleMu0_v1",nEvt,Form("(%s)&&HLT_PAL1DoubleMu0_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PADimuon0_NoVertexing_v1",nEvt,Form("(%s)&&HLT_PADimuon0_NoVertexing_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAMu5_v1",nEvt,Form("(%s)&&HLT_PAMu5_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAMu8_v1",nEvt,Form("(%s)&&HLT_PAMu8_v1",cut)));
  }
  if (anaMode=="HLTPA"){
    effs.push_back(calcEff(HltTree,"HLT_PAMu3_v1",nEvt,Form("(%s)&&HLT_PAMu3_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAMu7_v1",nEvt,Form("(%s)&&HLT_PAMu7_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAMu12_v1",nEvt,Form("(%s)&&HLT_PAMu12_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAL1DoubleMuOpen_v1",nEvt,Form("(%s)&&HLT_PAL1DoubleMuOpen_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAL1DoubleMu0_HighQ_v1",nEvt,Form("(%s)&&HLT_PAL1DoubleMu0_HighQ_v1",cut))); 
    effs.push_back(calcEff(HltTree,"HLT_PAL2DoubleMu3_v1",nEvt,Form("(%s)&&HLT_PAL2DoubleMu3_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAMu3PFJet20_v1",nEvt,Form("(%s)&&HLT_PAMu3PFJet20_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAMu3PFJet40_v1",nEvt,Form("(%s)&&HLT_PAMu3PFJet40_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PAMu7PFJet20_v1",nEvt,Form("(%s)&&HLT_PAMu7PFJet20_v1",cut)));
    effs.push_back(calcEff(HltTree,"HLT_PABTagMu_Jet20_Mu4_v1",nEvt,Form("(%s)&&HLT_PABTagMu_Jet20_Mu4_v1",cut)));
  }
  results->push_back(effs);

  cout <<"      * Correlation Matrix:"<<endl;
  int tsize = (int)triggers->size();
  TH2D *h = new TH2D(Form("h%s",title),"",tsize,0,tsize,tsize,0,tsize);
  TH2D *hct = new TH2D(Form("h%s_ct",title),"",tsize,0,tsize,tsize,0,tsize);

  for (int i=tsize-1;i>=0;i--){
    int nEvtAfterCut = HltTree->GetEntries(((*triggerCuts)[i]).c_str());
    h->GetXaxis()->SetBinLabel(i+1,((*triggers)[i]).c_str());
    h->GetYaxis()->SetBinLabel(i+1,((*triggers)[i]).c_str());
    hct->GetXaxis()->SetBinLabel(i+1,((*triggers)[i]).c_str());
    hct->GetYaxis()->SetBinLabel(i+1,((*triggers)[i]).c_str());
    for (int j=0;j<tsize;j++){
      string cut_ ="("+(*triggerCuts)[i]+")&&("+(*triggerCuts)[j]+")";
      double* eff = calcEff(HltTree,"",nEvtAfterCut,cut_.c_str(),0);
      if (nEvtAfterCut==0) eff[0]=0;
      h->SetBinContent(i+1,j+1,int(eff[0]*1000)/10.);
      hct->SetBinContent(i+1,j+1,HltTree->GetEntries(cut_.c_str()));
    }
  }
  h->GetXaxis()->LabelsOption("v");
  h->SetLabelSize(0.03,"XY");
  hct->GetXaxis()->LabelsOption("v");

  TCanvas *c1 = new TCanvas(Form("c%s",title), Form("c_%s",title),1000,800);
  c1->Range(-3.609756,-1.910995,12.7561,10.60209);
  c1->SetFillColor(0);
  c1->SetBorderMode(0);
  c1->SetBorderSize(0);
  c1->SetTickx();
  c1->SetTicky();
  c1->SetLeftMargin(0.37);
  c1->SetRightMargin(0.1684054);
  c1->SetTopMargin(0.02);
  c1->SetBottomMargin(0.4);
  c1->SetFrameLineColor(0);
  c1->SetFrameBorderMode(0);
  c1->SetFrameLineColor(0);
  c1->SetFrameBorderMode(0);

  h->SetStats(0);
  h->Draw("col text");

  TPaletteAxis *palette = new TPaletteAxis(tsize*1.02,0,tsize*1.1,tsize,h);
  palette->SetLabelColor(1);
  palette->SetLabelFont(42);
  palette->SetLabelOffset(0.005);
  palette->SetLabelSize(0.035);
  palette->SetTitleOffset(1);
  palette->SetTitleSize(0.04);
  palette->SetFillColor(100);
  palette->SetFillStyle(1001);
  h->GetListOfFunctions()->Add(palette,"br");
  h->Draw("col text z");

  string fname(Form("%s/trigCorr_%s_%s.pdf",goutdir.Data(),projectTitle,title));
  // c1->SaveAs(fname.c_str());
  c1->SaveAs(Form("correlOut/trigCorr_210354.pdf"));

//  cout <<"<img src=\"%ATTACHURLPATH%/"<<fname<<"\" alt=\""<<fname<<"\" width='671'   height='478' />"<<endl;   
}

void trigAnaCorrelation(
    TString mode="HLTPA",
    Int_t runNum = 210354,
    Int_t goodLumiStart = 1,
    //store/caf/user/velicanu/PA2013_merged_HiForest/pPb_hiForest2_1_15_test.root
    TString inFile0Name = "root://eoscms//eos/cms/store/caf/user/yjlee/pPb2013/dst/pPb_openHLT_Run210354_veryRAW_43k.root",
    //TString inFile0Name = "/castor/cern.ch/user/k/kimy/openHLT//openhlt_run181531.root",
    //"/castor/cern.ch/user/m/miheejo/openHLT/cms442/r181530_reco_v1_2/HIExpressPhysics_hiexp-hirun2011-r181530-reco-v1_2.root",
//"/castor/cern.ch/user/v/velicanu/HIHLT_Validation_Test_GRIF_v10.root",
    TString outdir="./correlOut",
    char *projectTitle = "HIpARun2013",
    string source="data")
{
  //  gROOT->Macro("/afs/cern.ch/user/k/kyolee/private/cms538HI_test/src/JpsiStyle.C");
  // Load input
  TChain * HltTree = new TChain("hltanalysis/HltTree","HI OpenHLT Tree");
  HltTree->Add(inFile0Name);
  cout << inFile0Name << endl;
  cout << " # entries: " << HltTree->GetEntries() << endl;
  if (HltTree->GetEntries() == 0) {
    cout << "Something is wrong with open input root file.\n";
    return;
  }
  anaMode=mode;

  // Define Output
  // TString evtSel("1"); 
 TString evtSel("HLT_PAMu3_v1||HLT_PAMu7_v1||HLT_PAMu12_v1||HLT_PAL1DoubleMu0_HighQ_v1||HLT_PAL1DoubleMuOpen_v1||HLT_PAL2DoubleMu3_v1||HLT_PABTagMu_Jet20_Mu4_v1||HLT_PAMu3PFJet20_v1||HLT_PAMu3PFJet40_v1||HLT_PAMu7PFJet20_v1");
//  TString evtSel("HLT_PAMinBiasHfOrBSC_v1");
//  TString evtSel("HLT_HIMinBiasHfOrBSC_v1");
//  TString evtSel("L1Tech_BPTX_plus_AND_minus.v0_5bx");
  outdir+=Form("/run%d_%s_%s",runNum,anaMode.Data(),"17012013");
  gSystem->mkdir(outdir.Data(),kTRUE);
  goutdir=outdir;
  TFile *outf = new TFile(Form("%s_%s_hist.root",outdir.Data(),projectTitle),"RECREATE");

  // define event types
  vector<string> evtType;
  vector<string> evtTypeCut;
  evtType.push_back(evtSel.Data()); evtTypeCut.push_back(evtSel.Data());
//  evtType.push_back("BptxAND"); evtTypeCut.push_back(evtSel.Data());
  if (source=="mc") {
  }
  else if (source=="data") {
    for (UInt_t i=0; i<evtTypeCut.size(); ++i) {
      evtTypeCut[i]+=Form("&&Run>=%d&&LumiBlock>=%d",runNum,goodLumiStart);
      cout << "evtTypeCut[" << i << "]: " << evtTypeCut[i] << endl;
    }
//    for (UInt_t i=0; i<evtTypeCut.size(); ++i) evtTypeCut[i]+=Form("&&Run==%d&&LumiBlock>%d",runNum,goodLumiStart);
  }

  // Print out event type fractions
  cout <<"<pre>"<<endl;
  for (unsigned int i=0; i<evtType.size(); ++i) {
    int nSel = HltTree->GetEntries(evtTypeCut[i].c_str());
    cout <<std::right << std::setw(20) << evtType[i] << ": " <<nSel<<endl;
  }
  cout <<"</pre>"<<endl;

  // Calc Efficiencies
  for (unsigned int i=0; i<evtType.size(); ++i) {
    cout << "evtTypeCut[" << i << "]: " << evtTypeCut[i] << endl;
    cout << "evtType[" << i << "]: " << evtType[i] << endl;
    printEff(HltTree,evtTypeCut[i].c_str(),evtType[i].c_str(),projectTitle);
  }

  // Print efficiency results
  cout <<" | "<<setw(20)<<" ";
  for (unsigned int i=0;i<evtType.size();i++) {
    cout <<" | "<<setw(8)<<evtType[i];
  }
  cout <<" | " <<endl;

  for (int k=0;k<(int)triggers->size();k++){
    cout <<" | " <<setw(20)<<(*triggers)[k];
    for (int j=0;j<(int)results->size();j++) {
      double eff=(((*results)[j])[k])[0];
      double effErr=(((*results)[j])[k])[1];
      cout <<" | "<<setw(10)<<fixed<<setprecision(3)<<eff*100.<<"% +-";
      cout        <<setw(10)<<fixed<<setprecision(3)<<effErr*100.<<"% ";
    }
    cout <<" | "<<endl;
  }   

  // save
  outf->Write();
}
