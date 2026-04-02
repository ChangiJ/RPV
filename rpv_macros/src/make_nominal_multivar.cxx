/*
 * ==============================================================================
 * Script Name: make_nominal_multivar.cxx
 * * Description:
 * 1. Processes skimmed RPV SUSY ntuples to extract nominal kinematic distributions.
 * 2. Applies global baseline cuts (Mj > 500, HT >= 1200, Njet >= 4, Nlep == 1) and proper event weights (Luminosity, Pass filter, and Trigger selection).
 * 3. Categorizes events into 15 distinct analysis regions (Bins 22-36, excluding 34, and one inclusive "all" region) based on Njet and Nb multiplicity.
 * 4. Generates 1D histograms for 5 key variables (nleps, njets, nbm, ht, mj12) and saves them into a nested ROOT file structure (e.g., bin22/mj12/ttbar).
 * 5. Automatically generates and saves 75 fully styled Data/MC stacked plots (with ratio panels and dynamic event cut labels) as PNG files.
 * ==============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include "TROOT.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TMath.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TPad.h"
#include "TLine.h"
#include "TColor.h"
#include "TLatex.h"
#include <TSystem.h>

#include "small_tree_rpv.hpp"
#include "utilities.hpp"
#include "utilities_macros.hpp"
//#include "utilities_macros_rpv.hpp"
#include "utilities_macros_rpv_Run3.hpp"

using namespace std;

struct winsize w;

void appendTree(vector<TString> s_process, small_tree &tree_in);
void getNominal(small_tree_rpv &tree, TString year, TString str_year, TFile *f, TString procname);
TString color(TString procname);
vector<TString> getCutLines(TString region);

TString str_year("");
bool nl0shape = true;
float lumi = 1.; // fb-1

TString red = "\033[0;31m";
TString green = "\033[1;32m";
TString blue = "\033[1;34m";
TString yellow = "\033[1;33m";
TString gray = "\033[0;90m";
TString cyan = "\033[0;36m";
TString reset = "\033[0m";

int main(int argc, char *argv[])
{
  int nthreads=16;
  ROOT::EnableImplicitMT(nthreads);
  gErrorIgnoreLevel=kError+1;
  TH1::SetDefaultSumw2();
  
  TH1::AddDirectory(kFALSE); 

  ioctl(0,TIOCGWINSZ, &w);

  TString year;
  TString onoff = "on";

  if(argc < 3) {
    cout << "Usage: ./run/make_nominal_multivar.exe [0 Lepton Shape on/off] [year] [luminosity(optional)] [str_year(optional)]" << endl;
    return 1;
  }

  onoff = argv[1];
  year = argv[2];
  
  if(argc >= 4) lumi = atof(argv[3]);
  else {
    if(year=="UL2016_preVFP") lumi = 19.5;
    else if(year=="UL2016_postVFP") lumi = 16.8;
    else if(year=="UL2017") lumi = 41.5;
    else if(year=="UL2018") lumi = 59.8;
    else if(year=="2024") lumi = 124.0; // https://cds.cern.ch/record/2952191
    else if(year=="2023") lumi = 300;
  }
  
  if(argc == 5) str_year = argv[4];
  if(onoff=="off") nl0shape = false; 

  cout << "=========================================" << endl;
  cout << "Running NOMINAL ONLY (Bins 22-36 & All) + PLOTTING" << endl;
  cout << "Year              : " << year << endl;
  cout << "Luminosity        : " << lumi << " fb-1" << endl;
  cout << "0 Lepton shape    : " << (nl0shape?"on":"off") << endl;
  cout << "=========================================" << endl;

  TString folder_bkg = folder_year(year,false).at(0);
  TString folder_dat = folder_year(year,false).at(1);
  TString folder_sig = folder_year(year,false).at(2);

  vector<TString> s_jetht = getRPVProcess(folder_dat,"data");
  vector<TString> s_qcd   = getRPVProcess(folder_bkg,"qcd");
  vector<TString> s_tt    = getRPVProcess(folder_bkg,"ttbar");
  vector<TString> s_wjets = getRPVProcess(folder_bkg,"wjets");
  vector<TString> s_other = getRPVProcess(folder_bkg,"other_public");

  vector<TString> s_rpv_m1000 = getRPVProcess(folder_sig,"rpv_m1000");
  vector<TString> s_rpv_m1100 = getRPVProcess(folder_sig,"rpv_m1100");
  vector<TString> s_rpv_m1200 = getRPVProcess(folder_sig,"rpv_m1200");
  vector<TString> s_rpv_m1300 = getRPVProcess(folder_sig,"rpv_m1300");
  vector<TString> s_rpv_m1400 = getRPVProcess(folder_sig,"rpv_m1400");
  vector<TString> s_rpv_m1500 = getRPVProcess(folder_sig,"rpv_m1500");
  vector<TString> s_rpv_m1600 = getRPVProcess(folder_sig,"rpv_m1600");
  vector<TString> s_rpv_m1700 = getRPVProcess(folder_sig,"rpv_m1700");
  vector<TString> s_rpv_m1800 = getRPVProcess(folder_sig,"rpv_m1800");
  vector<TString> s_rpv_m1900 = getRPVProcess(folder_sig,"rpv_m1900");
  vector<TString> s_rpv_m2000 = getRPVProcess(folder_sig,"rpv_m2000");
  vector<TString> s_rpv_m2100 = getRPVProcess(folder_sig,"rpv_m2100");
  vector<TString> s_rpv_m2200 = getRPVProcess(folder_sig,"rpv_m2200");
  vector<TString> s_rpv_m2300 = getRPVProcess(folder_sig,"rpv_m2300");
  vector<TString> s_rpv_m2400 = getRPVProcess(folder_sig,"rpv_m2400");
  vector<TString> s_rpv_m2500 = getRPVProcess(folder_sig,"rpv_m2500");

  small_tree_rpv data((static_cast<std::string>(s_jetht.at(0))));   appendTree(s_jetht, data); 
  small_tree_rpv qcd((static_cast<std::string>(s_qcd.at(0))));      appendTree(s_qcd, qcd);
  small_tree_rpv ttbar((static_cast<std::string>(s_tt.at(0))));     appendTree(s_tt, ttbar);
  small_tree_rpv wjets((static_cast<std::string>(s_wjets.at(0))));  appendTree(s_wjets, wjets);
  small_tree_rpv other((static_cast<std::string>(s_other.at(0))));  appendTree(s_other, other);

  small_tree_rpv rpv_m1000((static_cast<std::string>(s_rpv_m1000.at(0)))); appendTree(s_rpv_m1000, rpv_m1000);
  small_tree_rpv rpv_m1100((static_cast<std::string>(s_rpv_m1100.at(0)))); appendTree(s_rpv_m1100, rpv_m1100);
  small_tree_rpv rpv_m1200((static_cast<std::string>(s_rpv_m1200.at(0)))); appendTree(s_rpv_m1200, rpv_m1200);
  small_tree_rpv rpv_m1300((static_cast<std::string>(s_rpv_m1300.at(0)))); appendTree(s_rpv_m1300, rpv_m1300);
  small_tree_rpv rpv_m1400((static_cast<std::string>(s_rpv_m1400.at(0)))); appendTree(s_rpv_m1400, rpv_m1400);
  small_tree_rpv rpv_m1500((static_cast<std::string>(s_rpv_m1500.at(0)))); appendTree(s_rpv_m1500, rpv_m1500);
  small_tree_rpv rpv_m1600((static_cast<std::string>(s_rpv_m1600.at(0)))); appendTree(s_rpv_m1600, rpv_m1600);
  small_tree_rpv rpv_m1700((static_cast<std::string>(s_rpv_m1700.at(0)))); appendTree(s_rpv_m1700, rpv_m1700);
  small_tree_rpv rpv_m1800((static_cast<std::string>(s_rpv_m1800.at(0)))); appendTree(s_rpv_m1800, rpv_m1800);
  small_tree_rpv rpv_m1900((static_cast<std::string>(s_rpv_m1900.at(0)))); appendTree(s_rpv_m1900, rpv_m1900);
  small_tree_rpv rpv_m2000((static_cast<std::string>(s_rpv_m2000.at(0)))); appendTree(s_rpv_m2000, rpv_m2000);
  small_tree_rpv rpv_m2100((static_cast<std::string>(s_rpv_m2100.at(0)))); appendTree(s_rpv_m2100, rpv_m2100);
  small_tree_rpv rpv_m2200((static_cast<std::string>(s_rpv_m2200.at(0)))); appendTree(s_rpv_m2200, rpv_m2200);
  small_tree_rpv rpv_m2300((static_cast<std::string>(s_rpv_m2300.at(0)))); appendTree(s_rpv_m2300, rpv_m2300);
  small_tree_rpv rpv_m2400((static_cast<std::string>(s_rpv_m2400.at(0)))); appendTree(s_rpv_m2400, rpv_m2400);
  small_tree_rpv rpv_m2500((static_cast<std::string>(s_rpv_m2500.at(0)))); appendTree(s_rpv_m2500, rpv_m2500);

  TString out_dir = Form("2024_data_mc_valid/%s", year.Data());
  gSystem->mkdir(out_dir, kTRUE);

  TString shape = nl0shape ? "_nl0shape" : "";
  TString output_filename = Form("%s/output_nominal_multivar%s_%s_%.0f.root", out_dir.Data(), shape.Data(), year.Data(), lumi);
  if(str_year != "") output_filename = Form("%s/output_nominal_multivar%s_%s_%s.root", out_dir.Data(), shape.Data(), year.Data(), str_year.Data());
  
  TFile *f = new TFile(output_filename, "recreate");

  getNominal(data,  year, str_year, f, "data_obs");
  getNominal(qcd,   year, str_year, f, "qcd");
  getNominal(ttbar, year, str_year, f, "ttbar");
  getNominal(wjets, year, str_year, f, "wjets");
  getNominal(other, year, str_year, f, "other");

  getNominal(rpv_m1000, year, str_year, f, "signal_M1000");
  getNominal(rpv_m1100, year, str_year, f, "signal_M1100");
  getNominal(rpv_m1200, year, str_year, f, "signal_M1200");
  getNominal(rpv_m1300, year, str_year, f, "signal_M1300");
  getNominal(rpv_m1400, year, str_year, f, "signal_M1400");
  getNominal(rpv_m1500, year, str_year, f, "signal_M1500");
  getNominal(rpv_m1600, year, str_year, f, "signal_M1600");
  getNominal(rpv_m1700, year, str_year, f, "signal_M1700");
  getNominal(rpv_m1800, year, str_year, f, "signal_M1800");
  getNominal(rpv_m1900, year, str_year, f, "signal_M1900");
  getNominal(rpv_m2000, year, str_year, f, "signal_M2000");
  getNominal(rpv_m2100, year, str_year, f, "signal_M2100");
  getNominal(rpv_m2200, year, str_year, f, "signal_M2200");
  getNominal(rpv_m2300, year, str_year, f, "signal_M2300");
  getNominal(rpv_m2400, year, str_year, f, "signal_M2400");
  getNominal(rpv_m2500, year, str_year, f, "signal_M2500");

  cout << "\nGenerating and saving styled canvases for all regions and variables..." << endl;
  
  gStyle->SetOptStat(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  vector<TString> varNames = {"nleps", "njets", "nbm", "ht", "mj12"};
  vector<TString> varTitles = {"N_{lep}", "N_{jet}", "N_{b}", "H_{T} [GeV]", "M_{J} [GeV]"}; 
  double xmins[5] = {0, 4, 0, 1200, 500};
  double xmaxs[5] = {5, 15, 8, 5000, 1500};
  int ndivs[5] = {5, 11, 8, 505, 4}; 

  TString regions[16];
  for(int i=0; i<15; i++) regions[i] = Form("bin%d", i+22);
  regions[15] = "all";

  for(int r = 0; r < 16; ++r) {
      if(r == 12) continue; // bin34
      TString region = regions[r];

      for(size_t i = 0; i < varNames.size(); ++i) {
          TString vName = varNames[i];
          TString vTitle = varTitles[i];
          
          TString path = region + "/" + vName + "/";
          TH1F *h_data  = (TH1F*)f->Get(path + "data_obs");
          TH1F *h_qcd   = (TH1F*)f->Get(path + "qcd");
          TH1F *h_ttbar = (TH1F*)f->Get(path + "ttbar");
          TH1F *h_wjets = (TH1F*)f->Get(path + "wjets");
          TH1F *h_other = (TH1F*)f->Get(path + "other");
          TH1F *h_sig   = (TH1F*)f->Get(path + "signal_M1800");

          if(!h_data || !h_qcd || !h_ttbar || !h_wjets || !h_other || !h_sig) continue;
          if(h_data->GetEntries() == 0 && h_ttbar->GetEntries() == 0) continue; 
              
          h_qcd->SetFillColor(TColor::GetColor("#ffcc00"));   
          h_ttbar->SetFillColor(TColor::GetColor("#3366ff")); 
          h_wjets->SetFillColor(TColor::GetColor("#9933cc")); 
          h_other->SetFillColor(kGray+1);

          h_qcd->SetLineColor(kBlack);
          h_ttbar->SetLineColor(kBlack);
          h_wjets->SetLineColor(kBlack);
          h_other->SetLineColor(kBlack);

          h_data->SetMarkerStyle(20);
          h_data->SetMarkerSize(1.2);
          h_data->SetLineColor(kBlack);

          h_sig->SetLineColor(kRed);
          h_sig->SetLineWidth(3);
          h_sig->SetFillStyle(0);

          THStack *hs = new THStack("hs_" + region + "_" + vName, "");
          hs->Add(h_other);
          hs->Add(h_wjets);
          hs->Add(h_ttbar);
          hs->Add(h_qcd);

          TH1F *h_mc_total = (TH1F*)h_qcd->Clone("h_mc_total_" + region + "_" + vName);
          h_mc_total->Add(h_ttbar);
          h_mc_total->Add(h_wjets);
          h_mc_total->Add(h_other);

          TCanvas *c1 = new TCanvas("canvas_" + region + "_" + vName, vTitle + " Distribution", 800, 800);

          TPad *pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.);
          pad1->SetLeftMargin(0.15);
          pad1->SetRightMargin(0.05);
          pad1->SetTopMargin(0.08);
          pad1->SetBottomMargin(0.02);
          pad1->SetLogy();
          pad1->Draw();
          pad1->cd();

          hs->Draw("HIST");

          float max_val = TMath::Max(h_data->GetMaximum(), h_mc_total->GetMaximum());
          if (max_val <= 0.0) max_val = 1.0;

          hs->SetMaximum(max_val * 500);
          hs->SetMinimum(0.1);

          hs->GetYaxis()->SetTitle("Events");
          hs->GetYaxis()->SetTitleSize(0.08);
          hs->GetYaxis()->SetTitleOffset(0.8);
          hs->GetXaxis()->SetLabelSize(0);
          hs->GetYaxis()->SetLabelSize(0.07);
          hs->GetXaxis()->SetNdivisions(ndivs[i]);

          h_sig->Draw("HIST SAME");
          h_data->Draw("PE SAME");

          TString lumi_label = Form("%.1f fb^{-1} (13.6 TeV)", lumi);
          if (year != "2024" && year != "2023") lumi_label = Form("%.1f fb^{-1} (13 TeV)", lumi);

          TLatex* latex = new TLatex();
          latex->SetTextSize(0.05);
          latex->DrawLatexNDC(0.15, 0.93, "CMS #font[52]{Private Work}");
          latex->SetTextAlign(31);
          latex->DrawLatexNDC(0.95, 0.93, lumi_label);

          vector<TString> cut_lines = getCutLines(region);
          TLatex* cut_text = new TLatex();
          cut_text->SetNDC();
          cut_text->SetTextSize(0.04);
          cut_text->SetTextFont(42);
          cut_text->SetTextAlign(13); // Top-Left
          float text_y = 0.88;
          for(size_t line_idx = 0; line_idx < cut_lines.size(); ++line_idx) {
              cut_text->DrawLatex(0.18, text_y - (line_idx * 0.06), cut_lines[line_idx]);
          }

          TLegend *leg = new TLegend(0.55, 0.65, 0.88, 0.88);
          leg->SetBorderSize(0);
          leg->SetFillStyle(0);
          leg->SetNColumns(2);
          leg->AddEntry(h_data, "Data", "pe");
          leg->AddEntry(h_sig, "m_{#tilde{g}}=1800 GeV", "l");
          leg->AddEntry(h_qcd, "QCD", "f");
          leg->AddEntry(h_ttbar, "t#bar{t}", "f");
          leg->AddEntry(h_wjets, "W+jets", "f");
          leg->AddEntry(h_other, "Other", "f");
          leg->Draw();

          c1->cd();
          TPad *pad2 = new TPad("pad2", "pad2", 0., 0., 1., 0.3);
          pad2->SetLeftMargin(0.15);
          pad2->SetRightMargin(0.05);
          pad2->SetTopMargin(0.0);
          pad2->SetBottomMargin(0.35);
          pad2->Draw();
          pad2->cd();

          TH1F *h_ratio = (TH1F*)h_data->Clone("h_ratio_" + region + "_" + vName);
          h_ratio->Divide(h_mc_total);
          h_ratio->SetTitle("");

          h_ratio->GetXaxis()->SetTitle(vTitle);
          h_ratio->GetXaxis()->SetLabelSize(0.12);
          h_ratio->GetXaxis()->SetTitleSize(0.16); 
          h_ratio->GetXaxis()->SetTitleOffset(1.0);
          h_ratio->GetXaxis()->SetNdivisions(ndivs[i]);
          h_ratio->GetXaxis()->SetTitleFont(62);

          h_ratio->GetYaxis()->SetTitle("Data / MC");
          h_ratio->GetYaxis()->SetRangeUser(0.1, 1.9); 
          h_ratio->GetYaxis()->SetNdivisions(505);
          h_ratio->GetYaxis()->SetLabelSize(0.15);
          h_ratio->GetYaxis()->SetTitleSize(0.13);
          h_ratio->GetYaxis()->CenterTitle(true);
          h_ratio->GetYaxis()->SetTitleOffset(0.5);

          h_ratio->SetMarkerStyle(20);
          h_ratio->Draw("ep");

          TLine *line = new TLine(xmins[i], 1., xmaxs[i], 1.);
          line->SetLineColor(kBlack);
          line->SetLineWidth(1);
          line->SetLineStyle(2);
          line->Draw("same");

          f->cd(path);
          c1->Write("canvas_" + vName);
          
          c1->SaveAs(Form("%s/nominal_%s_%s_%s.png", out_dir.Data(), region.Data(), vName.Data(), year.Data())); 

          delete latex;
          delete cut_text;
          delete leg;
          delete line;
          delete hs;
          delete h_mc_total;
          delete h_ratio;
          delete c1;
      }
  }

  f->Close();
  cout << "Saved successfully to: " << output_filename << endl;
  return 0;
} 

void appendTree(vector<TString> s_process, small_tree &tree_in)
{
  if(s_process.size()>1) {
    for(int i=1; i<s_process.size(); i++) {
      tree_in.Add((static_cast<std::string>(s_process.at(i))));
    }
  }
}

void getNominal(small_tree_rpv &tree, TString year, TString str_year, TFile *f, TString procname)
{
  ioctl(0,TIOCGWINSZ, &w);
  int cols = w.ws_col;

  TH1F *h_nleps[16];
  TH1F *h_njets[16];
  TH1F *h_nbm[16];
  TH1F *h_ht[16];
  TH1F *h_mj12[16];

  for(int i=0; i<16; i++) {
      TString prefix = (i == 15) ? "all_" : Form("bin%d_", i+22);
      h_nleps[i] = new TH1F(prefix + procname + "_nleps", procname, 5, 0, 5);
      h_njets[i] = new TH1F(prefix + procname + "_njets", procname, 11, 4, 15);
      h_nbm[i]   = new TH1F(prefix + procname + "_nbm",   procname, 8, 0, 8);
      h_ht[i]    = new TH1F(prefix + procname + "_ht",    procname, 20, 1200, 5000);
      h_mj12[i]  = new TH1F(prefix + procname + "_mj12",  procname, 20, 500, 1500);

      h_nleps[i]->Sumw2(); 
      h_njets[i]->Sumw2(); 
      h_nbm[i]->Sumw2(); 
      h_ht[i]->Sumw2(); 
      h_mj12[i]->Sumw2(); 
  }

  for(unsigned int ientry=0; ientry<tree.GetEntries(); ientry++)
  {
    float progress = 0.0;
    if(ientry%int(tree.GetEntries()/1000) == 0 || ientry+1 == tree.GetEntries()){
      ioctl(0,TIOCGWINSZ, &w);
      cols = w.ws_col;
      progress = float(ientry+1)/float(tree.GetEntries());
      int barWidth = cols - 79;
      TString space = "";
      for(int sp = 0 ; sp < 20-strlen(procname.Data()) ; sp++) space += " "; 
      cout << "Process name      : " << procname.Data() << space.Data() <<  "[";
      int pos = barWidth*progress;
      for(int i = 0; i < barWidth;++i){
        if(i <= pos) cout << color(procname).Data() << "■";
        else cout << " ";
      } 
      cout<<"\033[0m]";
      cout << "   " << ientry+1 << " / " << tree.GetEntries() << "   (" << Form("%5.1f",progress*100.0) <<"%)\r";
      cout.flush();
    }
    
    tree.GetEntry(ientry); 

    float nominalweight = lumi*tree.weight()*tree.pass(); 
    if (procname=="data_obs" && (year=="UL2016_preVFP"||year=="UL2016_postVFP"||year=="UL2016")) 
      nominalweight = tree.pass() * (tree.trig_ht900()||tree.trig_jet450());
    else if (procname=="data_obs") 
      nominalweight = tree.pass() * tree.trig_ht1050(); 

    if(nominalweight == 0) continue; 

    if(tree.mj12() <= 500) continue; 
    if(tree.ht() < 1200) continue; 
    if(tree.njets() < 4) continue;   
    if(tree.nleps() != 1) continue; 

    float v_nleps = tree.nleps() >= 5.0   ? 4.5    : tree.nleps();
    float v_njets = tree.njets() >= 15.0  ? 14.5   : tree.njets();
    float v_nbm   = tree.nbm() >= 8.0     ? 7.5    : tree.nbm();
    float v_ht    = tree.ht() >= 5000.0   ? 4999.0 : tree.ht();
    float v_mj12  = tree.mj12() >= 1500.0 ? 2499.0 : tree.mj12();

    h_nleps[15]->Fill(v_nleps, nominalweight); 
    h_njets[15]->Fill(v_njets, nominalweight); 
    h_nbm[15]->Fill(v_nbm, nominalweight); 
    h_ht[15]->Fill(v_ht, nominalweight); 
    h_mj12[15]->Fill(v_mj12, nominalweight); 

    for(int ibin = 22; ibin <= 36; ibin++) {
        if (ibin == 34) continue; // bin 34 
        if (passBinCut(ibin, tree.nleps(), tree.ht(), tree.njets(), tree.mj12(), tree.nbm())) {
            int idx = ibin - 22;
            h_nleps[idx]->Fill(v_nleps, nominalweight); 
            h_njets[idx]->Fill(v_njets, nominalweight); 
            h_nbm[idx]->Fill(v_nbm, nominalweight); 
            h_ht[idx]->Fill(v_ht, nominalweight); 
            h_mj12[idx]->Fill(v_mj12, nominalweight); 
        }
    }
  } 

  TString regions[16];
  for(int i=0; i<15; i++) regions[i] = Form("bin%d", i+22);
  regions[15] = "all";
  TString vars[5] = {"nleps", "njets", "nbm", "ht", "mj12"};
  TH1F** hist_ptrs_arr[5] = {h_nleps, h_njets, h_nbm, h_ht, h_mj12};

  f->cd();
  for(int r=0; r<16; r++) {
      if(r == 12) continue; // bin34
      if(!f->GetDirectory(regions[r])) f->mkdir(regions[r]);
      f->cd(regions[r]);
      for(int v=0; v<5; v++) {
          if(!gDirectory->GetDirectory(vars[v])) gDirectory->mkdir(vars[v]);
          gDirectory->cd(vars[v]);
          
          TH1F* h_to_write = hist_ptrs_arr[v][r];
          h_to_write->SetName(procname); 
          h_to_write->Write();
          
          f->cd(regions[r]);
      }
      f->cd();
  }

  cout<<"\n"; 
  for(int al=0 ; al<cols ; al++) cout << "=";
  cout<<endl;

  for(int i=0; i<16; i++) {
      delete h_nleps[i]; delete h_njets[i]; delete h_nbm[i]; 
      delete h_ht[i]; delete h_mj12[i];
  }
}

vector<TString> getCutLines(TString region) {
    vector<TString> lines;
    lines.push_back("H_{T} #geq 1200 GeV, M_{J} > 500 GeV");
    lines.push_back("N_{lep} = 1");
    
    if(region == "all") {
        lines.push_back("N_{jet} #geq 4 (Inclusive)");
        lines.push_back("N_{b} #geq 0 (Inclusive)");
    } else {
        int ibin = region.ReplaceAll("bin", "").Atoi();
        int njbin = (ibin%3==1) ? 0 : ((ibin%3==2) ? 1 : 2);
        int nbbin = (ibin-22)/3; 
        
        if(njbin == 0) lines.push_back("4 #leq N_{jet} #leq 5");
        else if(njbin == 1) lines.push_back("6 #leq N_{jet} #leq 7");
        else lines.push_back("N_{jet} #geq 8");
        
        if(nbbin >= 3) lines.push_back(Form("N_{b} #geq %d", nbbin));
        else lines.push_back(Form("N_{b} = %d", nbbin));
    }
    return lines;
}

TString color(TString procname){
  if(procname == "data_obs") return reset;
  else if(procname == "qcd") return yellow;
  else if(procname == "ttbar") return blue;
  else if(procname == "wjets") return green;
  else if(procname == "other") return gray;
  else return red;
}
