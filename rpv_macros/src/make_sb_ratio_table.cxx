/*
 * ==============================================================================
 * Script Name: make_sb_summary_plot.cxx
 * Description:
 * 1. Generates a Search Region (SR) Summary Plot (1D Histogram with Bins on X-axis).
 * 2. Plots Stacked Backgrounds and Signal Overlays.
 * 3. Adds S/B Ratio (%) as text directly above each bin in the top pad.
 * 4. Plots the S/B Ratio (%) in the bottom panel.
 * 5. Handles merged bins (Bin 31 and 46) and excluded bins automatically.
 * 6. Outputs the plots to a high-quality PDF.
 * ==============================================================================
 */

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TColor.h"
#include "TStyle.h"
#include "TMath.h"

#include "small_tree_rpv.hpp"
#include "utilities.hpp"
#include "utilities_macros.hpp"
//#include "utilities_macros_rpv.hpp"
#include "utilities_macros_rpv_Run3.hpp"

using namespace std;

void appendTree(vector<TString> s_process, small_tree &tree_in);
int getBin(int nleps, int njets, int nb);
void fillHistograms(small_tree_rpv &tree, float lumi, TString procname, TH1F* h_1lep, TH1F* h_0lep);
void drawSummaryPlot(TString pdf_name, TString region_name, TString sig_mass, float lumi,
                     TH1F* h_qcd, TH1F* h_ttbar, TH1F* h_wjets, TH1F* h_other, TH1F* h_sig,
                     const vector<TString>& bin_labels, bool isFirst, bool isLast);

int main(int argc, char *argv[])
{
  int nthreads = 16;
  ROOT::EnableImplicitMT(nthreads);
  gErrorIgnoreLevel = kError + 1;
  TH1::AddDirectory(kFALSE);
  gStyle->SetOptStat(0);

  TString year;
  TString sig_mass;
  float lumi = 1.;

  if (argc < 3) {
    cout << "Usage: ./run/make_sb_summary_plot.exe [year] [signal_mass] [luminosity(optional)]" << endl;
    cout << "Example: ./run/make_sb_summary_plot.exe 2024 1800" << endl;
    return 1;
  }

  year = argv[1];
  sig_mass = argv[2];

  if (argc >= 4) lumi = atof(argv[3]);
  else {
    if (year == "UL2016_preVFP") lumi = 19.5;
    else if (year == "UL2016_postVFP") lumi = 16.8;
    else if (year == "UL2017") lumi = 41.5;
    else if (year == "UL2018") lumi = 59.8;
    else if (year == "2024") lumi = 109.95; 
    else if (year == "2023") lumi = 300;
  }

  cout << "=========================================" << endl;
  cout << "Generating SR Summary Plot (PDF)" << endl;
  cout << "Year        : " << year << endl;
  cout << "Signal Mass : " << sig_mass << " GeV" << endl;
  cout << "Luminosity  : " << lumi << " fb-1" << endl;
  cout << "=========================================" << endl;

  TString folder_bkg = folder_year(year, false).at(0);
  TString folder_sig = folder_year(year, false).at(2);

  vector<TString> s_qcd   = getRPVProcess(folder_bkg, "qcd");
  vector<TString> s_tt    = getRPVProcess(folder_bkg, "ttbar");
  vector<TString> s_wjets = getRPVProcess(folder_bkg, "wjets");
  vector<TString> s_other = getRPVProcess(folder_bkg, "other_public");

  TString sig_name = "rpv_m" + sig_mass;
  vector<TString> s_rpv = getRPVProcess(folder_sig, sig_name);

  small_tree_rpv qcd((static_cast<std::string>(s_qcd.at(0))));      appendTree(s_qcd, qcd);
  small_tree_rpv ttbar((static_cast<std::string>(s_tt.at(0))));     appendTree(s_tt, ttbar);
  small_tree_rpv wjets((static_cast<std::string>(s_wjets.at(0))));  appendTree(s_wjets, wjets);
  small_tree_rpv other((static_cast<std::string>(s_other.at(0))));  appendTree(s_other, other);
  small_tree_rpv rpv((static_cast<std::string>(s_rpv.at(0))));      appendTree(s_rpv, rpv);

  // Define 14-bin Histograms for 1-Lepton and 0-Lepton
  int nbins = 14;
  TH1F* h1_qcd   = new TH1F("h1_qcd",   "", nbins, 0, nbins);
  TH1F* h1_ttbar = new TH1F("h1_ttbar", "", nbins, 0, nbins);
  TH1F* h1_wjets = new TH1F("h1_wjets", "", nbins, 0, nbins);
  TH1F* h1_other = new TH1F("h1_other", "", nbins, 0, nbins);
  TH1F* h1_sig   = new TH1F("h1_sig",   "", nbins, 0, nbins);

  TH1F* h0_qcd   = new TH1F("h0_qcd",   "", nbins, 0, nbins);
  TH1F* h0_ttbar = new TH1F("h0_ttbar", "", nbins, 0, nbins);
  TH1F* h0_wjets = new TH1F("h0_wjets", "", nbins, 0, nbins);
  TH1F* h0_other = new TH1F("h0_other", "", nbins, 0, nbins);
  TH1F* h0_sig   = new TH1F("h0_sig",   "", nbins, 0, nbins);

  // Fill Histograms
  fillHistograms(qcd,   lumi, "qcd",   h1_qcd,   h0_qcd);
  fillHistograms(ttbar, lumi, "ttbar", h1_ttbar, h0_ttbar);
  fillHistograms(wjets, lumi, "wjets", h1_wjets, h0_wjets);
  fillHistograms(other, lumi, "other", h1_other, h0_other);
  fillHistograms(rpv,   lumi, sig_name, h1_sig,   h0_sig);

  // Bin Labels
  vector<TString> labels_1lep = {"22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "35", "36"};
  vector<TString> labels_0lep = {"37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "50", "51"};

  TString pdf_filename = Form("SR_SummaryPlot_M%s_%s.pdf", sig_mass.Data(), year.Data());

  // Draw and Save
  drawSummaryPlot(pdf_filename, "1-Lepton Search Regions", sig_mass, lumi, 
                  h1_qcd, h1_ttbar, h1_wjets, h1_other, h1_sig, labels_1lep, true, false);
                  
  drawSummaryPlot(pdf_filename, "0-Lepton Search Regions", sig_mass, lumi, 
                  h0_qcd, h0_ttbar, h0_wjets, h0_other, h0_sig, labels_0lep, false, true);

  cout << "\nPlot successfully saved to: " << pdf_filename << endl;

  return 0;
}

void appendTree(vector<TString> s_process, small_tree &tree_in) {
  if (s_process.size() > 1) {
    for (size_t i = 1; i < s_process.size(); i++) {
      tree_in.Add((static_cast<std::string>(s_process.at(i))));
    }
  }
}

int getBin(int nleps, int njets, int nb) {
  if (nleps == 1) {
    if (nb == 0) {
      if (njets >= 4 && njets <= 5) return 22;
      if (njets >= 6 && njets <= 7) return 23;
      if (njets >= 8) return 24;
    } else if (nb == 1) {
      if (njets >= 4 && njets <= 5) return 25;
      if (njets >= 6 && njets <= 7) return 26;
      if (njets >= 8) return 27;
    } else if (nb == 2) {
      if (njets >= 4 && njets <= 5) return 28;
      if (njets >= 6 && njets <= 7) return 29;
      if (njets >= 8) return 30;
    } else if (nb == 3) {
      if (njets >= 4 && njets <= 5) return 31; 
      if (njets >= 6 && njets <= 7) return 32;
      if (njets >= 8) return 33;
    } else if (nb >= 4) {
      if (njets >= 4 && njets <= 5) return 31; // Merged
      if (njets >= 6 && njets <= 7) return 35;
      if (njets >= 8) return 36;
    }
  } else if (nleps == 0) {
    if (nb == 0) {
      if (njets >= 6 && njets <= 7) return 37;
      if (njets >= 8 && njets <= 9) return 38;
      if (njets >= 10) return 39;
    } else if (nb == 1) {
      if (njets >= 6 && njets <= 7) return 40;
      if (njets >= 8 && njets <= 9) return 41;
      if (njets >= 10) return 42;
    } else if (nb == 2) {
      if (njets >= 6 && njets <= 7) return 43;
      if (njets >= 8 && njets <= 9) return 44;
      if (njets >= 10) return 45;
    } else if (nb == 3) {
      if (njets >= 4 && njets <= 5) return 46; 
      if (njets >= 6 && njets <= 7) return 47;
      if (njets >= 8 && njets <= 9) return 48; 
    } else if (nb >= 4) {
      if (njets >= 4 && njets <= 5) return 46; // Merged
      if (njets >= 6 && njets <= 7) return 50;
      if (njets >= 8) return 51;
    }
  }
  return -1; 
}

void fillHistograms(small_tree_rpv &tree, float lumi, TString procname, TH1F* h_1lep, TH1F* h_0lep) {
  cout << "Processing: " << procname << " ... ";
  cout.flush();

  for (unsigned int ientry = 0; ientry < tree.GetEntries(); ientry++) {
    tree.GetEntry(ientry);

    float nominalweight = lumi * tree.weight() * tree.pass();
    if (nominalweight == 0) continue;

    if (tree.mj12() < 1100) continue;
    if (tree.ht() < 1200) continue;

    int bin_idx = getBin(tree.nleps(), tree.njets(), tree.nbm());
    
    // Map bin_idx to TH1F bins (1 to 14) -> Fill value is (hbin - 0.5)
    if (bin_idx >= 22 && bin_idx <= 36 && bin_idx != 34) {
      int hbin = bin_idx - 21;
      if (bin_idx >= 35) hbin -= 1;
      h_1lep->Fill(hbin - 0.5, nominalweight);
    } else if (bin_idx >= 37 && bin_idx <= 51 && bin_idx != 49) {
      int hbin = bin_idx - 36;
      if (bin_idx >= 50) hbin -= 1;
      h_0lep->Fill(hbin - 0.5, nominalweight);
    }
  }
  cout << "Done." << endl;
}

void drawSummaryPlot(TString pdf_name, TString region_name, TString sig_mass, float lumi,
                     TH1F* h_qcd, TH1F* h_ttbar, TH1F* h_wjets, TH1F* h_other, TH1F* h_sig,
                     const vector<TString>& bin_labels, bool isFirst, bool isLast) {
    
    // Apply styling
    h_qcd->SetFillColor(TColor::GetColor("#ffcc00"));   
    h_ttbar->SetFillColor(TColor::GetColor("#3366ff")); 
    h_wjets->SetFillColor(TColor::GetColor("#9933cc")); 
    h_other->SetFillColor(kGray+1);

    h_qcd->SetLineColor(kBlack);
    h_ttbar->SetLineColor(kBlack);
    h_wjets->SetLineColor(kBlack);
    h_other->SetLineColor(kBlack);

    h_sig->SetLineColor(kRed);
    h_sig->SetLineWidth(3);
    h_sig->SetFillStyle(0);

    THStack *hs = new THStack("hs", "");
    hs->Add(h_other);
    hs->Add(h_wjets);
    hs->Add(h_ttbar);
    hs->Add(h_qcd);

    TH1F *h_bkg_tot = (TH1F*)h_qcd->Clone("h_bkg_tot");
    h_bkg_tot->Add(h_ttbar);
    h_bkg_tot->Add(h_wjets);
    h_bkg_tot->Add(h_other);

    // Canvas setup
    TCanvas *c1 = new TCanvas("c1", region_name, 1200, 800);

    // Top Pad
    TPad *pad1 = new TPad("pad1", "pad1", 0., 0.3, 1., 1.);
    pad1->SetLeftMargin(0.10);
    pad1->SetRightMargin(0.05);
    pad1->SetTopMargin(0.08);
    pad1->SetBottomMargin(0.02);
    pad1->SetLogy();
    pad1->Draw();
    pad1->cd();

    hs->Draw("HIST");
    
    float max_val = TMath::Max(h_bkg_tot->GetMaximum(), h_sig->GetMaximum());
    if (max_val <= 0.0) max_val = 1.0;
    
    hs->SetMaximum(max_val * 1000); // Plenty of room for text
    hs->SetMinimum(0.05);

    hs->GetYaxis()->SetTitle("Events / Bin");
    hs->GetYaxis()->SetTitleSize(0.06);
    hs->GetYaxis()->SetTitleOffset(0.7);
    hs->GetXaxis()->SetLabelSize(0);
    hs->GetYaxis()->SetLabelSize(0.05);

    h_sig->Draw("HIST SAME");

    // S/B Ratio Text on Top Pad
    TLatex *tex_sb = new TLatex();
    tex_sb->SetTextSize(0.035);
    tex_sb->SetTextFont(42);
    tex_sb->SetTextAngle(45); // Tilted for better readability
    tex_sb->SetTextAlign(11); // Bottom-Left

    for (int i = 1; i <= h_bkg_tot->GetNbinsX(); i++) {
        double bkg = h_bkg_tot->GetBinContent(i);
        double sig = h_sig->GetBinContent(i);
        if (bkg > 0 && sig > 0) {
            double sb = (sig / bkg) * 100.0;
            double x_pos = hs->GetXaxis()->GetBinCenter(i) - 0.2;
            double y_pos = TMath::Max(bkg, sig) * 1.8; // Float slightly above the highest bar/line
            tex_sb->DrawLatex(x_pos, y_pos, Form("%.2f%%", sb));
        }
    }

    // Legend & Headers
    TLegend *leg = new TLegend(0.65, 0.65, 0.92, 0.88);
    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetNColumns(2);
    leg->AddEntry(h_sig, Form("m_{#tilde{g}}=%s GeV", sig_mass.Data()), "l");
    leg->AddEntry(h_qcd, "QCD", "f");
    leg->AddEntry(h_ttbar, "t#bar{t}", "f");
    leg->AddEntry(h_wjets, "W+jets", "f");
    leg->AddEntry(h_other, "Other", "f");
    leg->Draw();

    TLatex* latex = new TLatex();
    latex->SetTextSize(0.05);
    latex->DrawLatexNDC(0.10, 0.94, "CMS #font[52]{Private Work}");
    latex->SetTextAlign(31);
    latex->DrawLatexNDC(0.95, 0.94, Form("%.1f fb^{-1} (13.6 TeV) - %s", lumi, region_name.Data()));

    // Bottom Pad
    c1->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0., 0., 1., 0.3);
    pad2->SetLeftMargin(0.10);
    pad2->SetRightMargin(0.05);
    pad2->SetTopMargin(0.0);
    pad2->SetBottomMargin(0.35);
    pad2->SetGridy();
    pad2->Draw();
    pad2->cd();

    TH1F *h_ratio = (TH1F*)h_sig->Clone("h_ratio");
    h_ratio->Divide(h_bkg_tot);
    h_ratio->Scale(100.0); // Convert to Percentage
    
    h_ratio->SetTitle("");
    h_ratio->GetXaxis()->SetTitle("Search Region Bin");
    h_ratio->GetXaxis()->SetLabelSize(0.15);
    h_ratio->GetXaxis()->SetTitleSize(0.15); 
    h_ratio->GetXaxis()->SetTitleOffset(1.0);
    h_ratio->GetXaxis()->SetTitleFont(42);

    h_ratio->GetYaxis()->SetTitle("S / B (%)");
    h_ratio->GetYaxis()->SetNdivisions(505);
    h_ratio->GetYaxis()->SetLabelSize(0.12);
    h_ratio->GetYaxis()->SetTitleSize(0.12);
    h_ratio->GetYaxis()->CenterTitle(true);
    h_ratio->GetYaxis()->SetTitleOffset(0.4);

    // Set Bin Labels
    for(size_t i = 0; i < bin_labels.size(); i++) {
        h_ratio->GetXaxis()->SetBinLabel(i+1, bin_labels[i]);
    }

    h_ratio->SetMarkerStyle(20);
    h_ratio->SetMarkerSize(1.5);
    h_ratio->SetLineColor(kRed);
    h_ratio->SetMarkerColor(kRed);
    
    // Auto-scale Ratio Y-axis based on max S/B
    float max_sb = h_ratio->GetMaximum();
    h_ratio->GetYaxis()->SetRangeUser(0.0, max_sb > 0 ? max_sb * 1.5 : 10.0);
    
    h_ratio->Draw("P HIST");

    // PDF Print Logic
    if (isFirst && isLast) {
        c1->Print(pdf_name); 
    } else if (isFirst) {
        c1->Print(pdf_name + "("); 
    } else if (isLast) {
        c1->Print(pdf_name + ")"); 
    } else {
        c1->Print(pdf_name);
    }

    delete hs;
    delete h_bkg_tot;
    delete h_ratio;
    delete c1;
}
