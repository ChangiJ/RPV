/*
 * Compilation:
 * g++ -O3 -o plot_yields_nominal plot_yields_nominal.cxx $(root-config --cflags --libs)
 *
 * Execution:
 * ./plot_yields_nominal [input_root_file] [0 or 1 (lepton channel)]
 * Example:
 * ./plot_yields_nominal ../variations/output_nominal_newnt_UL2017.root 0
 */

#include <iostream>
#include <vector>

#include "TStyle.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "THStack.h"
#include "TFile.h"
#include "TLegend.h"
#include "TPad.h"
#include "TLine.h"
#include "TMath.h"
#include "TColor.h"
#include "TROOT.h"

using namespace std;

int main(int argc, char* argv[])
{
    gStyle->SetOptStat(0);
    TH1::AddDirectory(kFALSE); // Prevent ROOT from managing histograms in memory automatically

    if(argc < 3) {
        cout << "Usage: ./plot_yields_nominal [input_root_file] [0 or 1]" << endl;
        return 1;
    }

    TString inputName = argv[1];
    int nleps = atoi(argv[2]);

    if(nleps != 0 && nleps != 1) {
        cout << "Error: Lepton channel must be 0 or 1." << endl;
        return 1;
    }

    TFile* infile = TFile::Open(inputName, "READ");
    if(!infile || infile->IsZombie()) {
        cout << "Error: Cannot open file " << inputName << endl;
        return 1;
    }

    // 채널별 Bin 범위 설정
    int start_bin = (nleps == 1) ? 22 : 37;
    int end_bin   = (nleps == 1) ? 36 : 51;

    for(int ibin = start_bin; ibin <= end_bin; ibin++) {
        // 채널에 따른 특정 Bin 제외 로직
        if(nleps == 1 && ibin == 34) continue;
        if(nleps == 0 && ibin == 49) continue;

        TString bName = Form("bin%d", ibin);
        
        // Define histograms for 3 MJ bins
        TH1F *h_data  = new TH1F(Form("h_data_%d", ibin),  "Data",   3, 0, 3);
        TH1F *h_qcd   = new TH1F(Form("h_qcd_%d", ibin),   "QCD",    3, 0, 3);
        TH1F *h_ttbar = new TH1F(Form("h_ttbar_%d", ibin), "t#bar{t}", 3, 0, 3);
        TH1F *h_wjets = new TH1F(Form("h_wjets_%d", ibin), "W+jets", 3, 0, 3);
        TH1F *h_other = new TH1F(Form("h_other_%d", ibin), "Other",  3, 0, 3);
        TH1F *h_sig   = new TH1F(Form("h_sig_%d", ibin),   "m_{#tilde{g}}=1800 GeV", 3, 0, 3);

        for(int inb = 0; inb < 3; inb++) {
            TH1F* h_in_data  = (TH1F*)infile->Get(Form("%s/data_obs", bName.Data()));
            TH1F* h_in_qcd   = (TH1F*)infile->Get(Form("%s/qcd", bName.Data()));
            TH1F* h_in_ttbar = (TH1F*)infile->Get(Form("%s/ttbar", bName.Data()));
            TH1F* h_in_wjets = (TH1F*)infile->Get(Form("%s/wjets", bName.Data()));
            TH1F* h_in_other = (TH1F*)infile->Get(Form("%s/other", bName.Data()));
            TH1F* h_in_sig   = (TH1F*)infile->Get(Form("%s/signal_M1800", bName.Data()));

            int plot_idx = inb + 1;

            if(h_in_data) {
                float val = h_in_data->GetBinContent(plot_idx);
                h_data->SetBinContent(plot_idx, val);
                h_data->SetBinError(plot_idx, TMath::Sqrt(val)); 
            }
            if(h_in_qcd) {
                h_qcd->SetBinContent(plot_idx, h_in_qcd->GetBinContent(plot_idx));
                h_qcd->SetBinError(plot_idx, h_in_qcd->GetBinError(plot_idx));
            }
            if(h_in_ttbar) {
                h_ttbar->SetBinContent(plot_idx, h_in_ttbar->GetBinContent(plot_idx));
                h_ttbar->SetBinError(plot_idx, h_in_ttbar->GetBinError(plot_idx));
            }
            if(h_in_wjets) {
                h_wjets->SetBinContent(plot_idx, h_in_wjets->GetBinContent(plot_idx));
                h_wjets->SetBinError(plot_idx, h_in_wjets->GetBinError(plot_idx));
            }
            if(h_in_other) {
                h_other->SetBinContent(plot_idx, h_in_other->GetBinContent(plot_idx));
                h_other->SetBinError(plot_idx, h_in_other->GetBinError(plot_idx));
            }
            if(h_in_sig) {
                h_sig->SetBinContent(plot_idx, h_in_sig->GetBinContent(plot_idx));
                h_sig->SetBinError(plot_idx, h_in_sig->GetBinError(plot_idx));
            }
        }

        // Styles
        h_qcd->SetFillColor(TColor::GetColor("#ffcc00"));
        h_ttbar->SetFillColor(TColor::GetColor("#3366ff"));
        h_wjets->SetFillColor(TColor::GetColor("#33cc33"));
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

        // Stack
        THStack *hs = new THStack(Form("hs_%d", ibin), "");
        hs->Add(h_other);
        hs->Add(h_wjets);
        hs->Add(h_ttbar);
        hs->Add(h_qcd);

        TH1F *h_mc_total = (TH1F*)h_qcd->Clone(Form("h_mc_total_%d", ibin));
        h_mc_total->Add(h_ttbar);
        h_mc_total->Add(h_wjets);
        h_mc_total->Add(h_other);

        // Canvas
        TCanvas *c1 = new TCanvas(Form("c_%d", ibin), bName, 800, 800);
        
        TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
        pad1->SetBottomMargin(0.02);
        pad1->SetLogy(); 
        pad1->Draw();
        pad1->cd();

        hs->Draw("HIST");
        
        // 이벤트가 0일 때 Logy 에러 방지용 Safeguard
        float max_val = TMath::Max(h_data->GetMaximum(), h_mc_total->GetMaximum());
        if (max_val <= 0.0) max_val = 1.0; 
        
        hs->SetMaximum(max_val * 500); 
        hs->SetMinimum(0.1);
        hs->GetYaxis()->SetTitle("Events");
        hs->GetYaxis()->SetTitleSize(0.05);
        hs->GetYaxis()->SetLabelSize(0.04);

        h_sig->Draw("HIST SAME");
        h_data->Draw("PE SAME");

        TLegend *leg = new TLegend(0.55, 0.65, 0.88, 0.88);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetNColumns(2);
        leg->AddEntry(h_data, "Data", "pe");
        leg->AddEntry(h_sig, h_sig->GetTitle(), "l");
        leg->AddEntry(h_qcd, "QCD", "f");
        leg->AddEntry(h_ttbar, "t#bar{t}", "f");
        leg->AddEntry(h_wjets, "W+jets", "f");
        leg->AddEntry(h_other, "Other", "f");
        leg->Draw();

        // Ratio Pad
        c1->cd();
        TPad *pad2 = new TPad("pad2", "pad2", 0, 0.0, 1, 0.3);
        pad2->SetTopMargin(0.02);
        pad2->SetBottomMargin(0.35);
        pad2->Draw();
        pad2->cd();

        TH1F *h_ratio = (TH1F*)h_data->Clone(Form("h_ratio_%d", ibin));
        h_ratio->Divide(h_mc_total);
        h_ratio->SetTitle("");
        
        h_ratio->GetYaxis()->SetTitle("Data / MC");
        h_ratio->GetYaxis()->SetRangeUser(0.0, 2.5);
        h_ratio->GetYaxis()->SetNdivisions(505);
        
        h_ratio->GetXaxis()->SetTitle(Form("M_{J} [GeV] (%s)", bName.Data()));
        h_ratio->GetXaxis()->SetTitleSize(0.12);
        h_ratio->GetXaxis()->SetLabelSize(0.14);
        h_ratio->GetYaxis()->SetTitleSize(0.12);
        h_ratio->GetYaxis()->SetLabelSize(0.1);
        h_ratio->GetYaxis()->SetTitleOffset(0.4);

        // Set X-axis labels for MJ bins
        h_ratio->GetXaxis()->SetBinLabel(1, "500-800");
        h_ratio->GetXaxis()->SetBinLabel(2, "800-1100");
        h_ratio->GetXaxis()->SetBinLabel(3, "#geq 1100");

        h_ratio->SetMarkerStyle(20);
        h_ratio->Draw("PE");

        TLine *line = new TLine(0, 1, 3, 1);
        line->SetLineStyle(2);
        line->SetLineWidth(2);
        line->SetLineColor(kBlack);
        line->Draw();

        c1->SaveAs(Form("bin%d.png", ibin));

        // Clean up memory
        delete c1; 
        delete leg; 
        delete line;
        delete hs; 
        delete h_mc_total; 
        delete h_ratio;
        delete h_data; 
        delete h_qcd; 
        delete h_ttbar; 
        delete h_wjets; 
        delete h_other; 
        delete h_sig;
    }

    infile->Close();
    return 0;
}
