/*
 * ==============================================================================
 * Script Name: make_nominal_multivar_cr.cxx
 * Description:
 * 1. Extracts nominal kinematic distributions specifically for Control Regions (CR) 
 * where background dominates over signal.
 * 2. Depending on the target N_lep (0 or 1):
 * - For N_lep = 1, defines three inclusive Control Regions:
 * 'nb0'       : N_b = 0 and N_jet >= 4
 * 'njet4_5'   : 4 <= N_jet <= 5 and N_b >= 0
 * 'inclusive' : Union of 'nb0' and 'njet4_5'
 * - For N_lep = 0, only 'nb0' is analyzed.
 * 3. Applies global baseline cuts (M_J > 500, H_T >= 1200) 
 * with correct event weights (Luminosity, Pass filter, and Triggers).
 * 4. Generates 1D histograms for 5 key variables and saves them into a ROOT file.
 * 5. Automatically generates and saves customized Data/MC stacked plots 
 * as PNG files.
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
#include "utilities_macros_rpv_Run3.hpp"

using namespace std;

struct winsize w;

void appendTree(vector<TString> s_process, small_tree &tree_in);
void getNominal(small_tree_rpv &tree, TString year, TString str_year, TFile *f, TString procname, int target_nleps);
TString color(TString procname);
vector<TString> getCutLines(TString region, int target_nleps);

TString str_year("");
float lumi = 1.; 

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
  int target_nleps = 1;

  if(argc < 3) {
    cout << "Usage: ./run/make_nominal_multivar_cr.exe [nleps (0 or 1)] [year] [luminosity(optional)] [str_year(optional)]" << endl;
    return 1;
  }

  target_nleps = atoi(argv[1]);
  year = argv[2];
  
  if(argc >= 4) lumi = atof(argv[3]);
  else {
    if(year=="UL2016_preVFP") lumi = 19.5;
    else if(year=="UL2016_postVFP") lumi = 16.8;
    else if(year=="UL2017") lumi = 41.5;
    else if(year=="UL2018") lumi = 59.8;
    else if(year=="2024") lumi = 109.95; 
    else if(year=="2023") lumi = 300;
  }
  
  if(argc == 5) str_year = argv[4];

  cout << "=========================================" << endl;
  cout << "Running CR NOMINAL + PLOTTING" << endl;
  cout << "Target N_lep      : " << target_nleps << endl;
  cout << "Year              : " << year << endl;
  cout << "Luminosity        : " << lumi << " fb-1" << endl;
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
  vector<TString> s_rpv_m1800 = getRPVProcess(folder_sig,"rpv_m1800");

  small_tree_rpv data((static_cast<std::string>(s_jetht.at(0))));   appendTree(s_jetht, data); 
  small_tree_rpv qcd((static_cast<std::string>(s_qcd.at(0))));      appendTree(s_qcd, qcd);
  small_tree_rpv ttbar((static_cast<std::string>(s_tt.at(0))));     appendTree(s_tt, ttbar);
  small_tree_rpv wjets((static_cast<std::string>(s_wjets.at(0))));  appendTree(s_wjets, wjets);
  small_tree_rpv other((static_cast<std::string>(s_other.at(0))));  appendTree(s_other, other);

  small_tree_rpv rpv_m1000((static_cast<std::string>(s_rpv_m1000.at(0)))); appendTree(s_rpv_m1000, rpv_m1000);
  small_tree_rpv rpv_m1800((static_cast<std::string>(s_rpv_m1800.at(0)))); appendTree(s_rpv_m1800, rpv_m1800);

  TString out_dir = "/home/jchg3876/RPV/rpv_macros/2024_data_mc_valid/2024_CR";
  gSystem->mkdir(out_dir, kTRUE);

  TString output_filename = Form("%s/output_nominal_multivar_cr_nleps%d_%s_%.0f.root", out_dir.Data(), target_nleps, year.Data(), lumi);
  if(str_year != "") output_filename = Form("%s/output_nominal_multivar_cr_nleps%d_%s_%s.root", out_dir.Data(), target_nleps, year.Data(), str_year.Data());
  
  TFile *f = new TFile(output_filename, "recreate");

  getNominal(data,  year, str_year, f, "data_obs", target_nleps);
  getNominal(qcd,   year, str_year, f, "qcd", target_nleps);
  getNominal(ttbar, year, str_year, f, "ttbar", target_nleps);
  getNominal(wjets, year, str_year, f, "wjets", target_nleps);
  getNominal(other, year, str_year, f, "other", target_nleps);

  getNominal(rpv_m1000, year, str_year, f, "signal_M1000", target_nleps);
  getNominal(rpv_m1800, year, str_year, f, "signal_M1800", target_nleps);

  cout << "\nGenerating and saving styled canvases for CR regions..." << endl;
  
  gStyle->SetOptStat(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  vector<TString> varNames = {"nleps", "njets", "nbm", "ht", "mj12"};
  vector<TString> varTitles = {"N_{lep}", "N_{jet}", "N_{b}", "H_{T} [GeV]", "M_{J} [GeV]"}; 
  
  double xmins[5] = {0, 4, 0, 1200, 500};
  double xmaxs[5] = {5, 12, 8, 3500, 1200};
  int ndivs[5]    = {5, 8, 8, 505, 505}; 

  TString regions[3] = {"nb0", "njet4_5", "inclusive"};
  int num_regions = (target_nleps == 0) ? 1 : 3;

  for(int r = 0; r < num_regions; ++r) {
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
          latex->DrawLatexNDC(0.15, 0.93, "CMS #font[52]{Private Work (Simulation)}");
          latex->SetTextAlign(31);
          latex->DrawLatexNDC(0.95, 0.93, lumi_label);

          vector<TString> cut_lines = getCutLines(region, target_nleps);
          TLatex* cut_text = new TLatex();
          cut_text->SetNDC();
          cut_text->SetTextSize(0.04);
          cut_text->SetTextFont(42);
          cut_text->SetTextAlign(13); 
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
          
          c1->SaveAs(Form("%s/nominal_cr_nleps%d_%s_%s_%s.png", out_dir.Data(), target_nleps, region.Data(), vName.Data(), year.Data())); 

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

void getNominal(small_tree_rpv &tree, TString year, TString str_year, TFile *f, TString procname, int target_nleps)
{
  ioctl(0,TIOCGWINSZ, &w);
  int cols = w.ws_col;

  TH1F *h_nleps[3];
  TH1F *h_njets[3];
  TH1F *h_nbm[3];
  TH1F *h_ht[3];
  TH1F *h_mj12[3];
  
  TString regions[3] = {"nb0", "njet4_5", "inclusive"};
  int num_regions = (target_nleps == 0) ? 1 : 3;

  for(int i=0; i<num_regions; i++) {
      TString prefix = regions[i] + "_";
      h_nleps[i] = new TH1F(prefix + procname + "_nleps", procname, 5, 0, 5);
      h_njets[i] = new TH1F(prefix + procname + "_njets", procname, 8, 4, 12);
      h_nbm[i]   = new TH1F(prefix + procname + "_nbm",   procname, 8, 0, 8);
      h_ht[i]    = new TH1F(prefix + procname + "_ht",    procname, 23, 1200, 3500);
      h_mj12[i]  = new TH1F(prefix + procname + "_mj12",  procname, 7, 500, 1200);

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
    if(tree.nleps() != target_nleps) continue; 

    float v_nleps = tree.nleps() >= 5.0    ? 4.5     : tree.nleps();
    float v_njets = tree.njets() >= 12.0   ? 11.5    : tree.njets();
    float v_nbm   = tree.nbm() >= 8.0      ? 7.5     : tree.nbm();
    float v_ht    = tree.ht() >= 3500.0    ? 3499.0  : tree.ht();
    float v_mj12  = tree.mj12() >= 1200.0  ? 1199.0  : tree.mj12();

    bool is_nb0 = (tree.nbm() == 0);
    bool is_njet4_5 = (tree.njets() >= 4 && tree.njets() <= 5);

    if (is_nb0) {
        h_nleps[0]->Fill(v_nleps, nominalweight); 
        h_njets[0]->Fill(v_njets, nominalweight); 
        h_nbm[0]->Fill(v_nbm, nominalweight); 
        h_ht[0]->Fill(v_ht, nominalweight); 
        h_mj12[0]->Fill(v_mj12, nominalweight); 
    }

    if (target_nleps == 1) {
        if (is_njet4_5) {
            h_nleps[1]->Fill(v_nleps, nominalweight); 
            h_njets[1]->Fill(v_njets, nominalweight); 
            h_nbm[1]->Fill(v_nbm, nominalweight); 
            h_ht[1]->Fill(v_ht, nominalweight); 
            h_mj12[1]->Fill(v_mj12, nominalweight); 
        }

        if (is_nb0 || is_njet4_5) {
            h_nleps[2]->Fill(v_nleps, nominalweight); 
            h_njets[2]->Fill(v_njets, nominalweight); 
            h_nbm[2]->Fill(v_nbm, nominalweight); 
            h_ht[2]->Fill(v_ht, nominalweight); 
            h_mj12[2]->Fill(v_mj12, nominalweight); 
        }
    }
  } 

  TString vars[5] = {"nleps", "njets", "nbm", "ht", "mj12"};
  TH1F** hist_ptrs_arr[5] = {h_nleps, h_njets, h_nbm, h_ht, h_mj12};

  f->cd();
  for(int r=0; r<num_regions; r++) {
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

  for(int i=0; i<num_regions; i++) {
      delete h_nleps[i]; delete h_njets[i]; delete h_nbm[i]; 
      delete h_ht[i]; delete h_mj12[i];
  }
}

vector<TString> getCutLines(TString region, int target_nleps) {
    vector<TString> lines;
    lines.push_back("H_{T} #geq 1200 GeV, M_{J} > 500 GeV");
    lines.push_back(Form("N_{lep} = %d", target_nleps));
    
    if(region == "nb0") {
        lines.push_back("N_{jet} #geq 4");
        lines.push_back("N_{b} = 0");
    } else if(region == "njet4_5") {
        lines.push_back("4 #leq N_{jet} #leq 5");
        lines.push_back("N_{b} #geq 0");
    } else if(region == "inclusive") {
        lines.push_back("N_{b} = 0 OR 4 #leq N_{jet} #leq 5");
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
