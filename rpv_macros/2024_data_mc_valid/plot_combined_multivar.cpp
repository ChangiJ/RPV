/*
 * Compilation:
 * g++ -O3 -o plot_combined_multivar plot_combined_multivar.cpp $(root-config --cflags --libs)
 *
 * Execution:
 * ./plot_combined_multivar [input_root_file] [output_directory(optional)]
 * Example:
 * ./plot_combined_multivar ../2024_data_mc_valid/output_nominal_multivar_UL201678.root
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
#include "TLatex.h"
#include "TSystem.h"
#include "TROOT.h"

using namespace std;

int main(int argc, char* argv[])
{
    gStyle->SetOptStat(0);
    gStyle->SetPadTickX(1);
    gStyle->SetPadTickY(1);
    TH1::AddDirectory(kFALSE); 

    if(argc < 2) {
        cout << "Usage: ./plot_combined_multivar [input_root_file] [output_dir(optional)]" << endl;
        return 1;
    }

    TString inputName = argv[1];
    TString outDir = (argc >= 3) ? argv[2] : "."; 

    // 출력 디렉터리가 없으면 생성
    if (outDir != ".") {
        gSystem->mkdir(outDir, kTRUE);
    }

    TFile* infile = TFile::Open(inputName, "READ");
    if(!infile || infile->IsZombie()) {
        cout << "Error: Cannot open file " << inputName << endl;
        return 1;
    }

    cout << "Successfully opened combined file: " << inputName << endl;
    cout << "Generating plots..." << endl;

    vector<TString> varNames = {"nleps", "njets", "nbm", "ht", "mj12"};
    vector<TString> varTitles = {"N_{lep}", "N_{jet}", "N_{b}", "H_{T} [GeV]", "M_{J} [GeV]"}; 
    double xmins[5] = {0, 4, 0, 1200, 500};
    double xmaxs[5] = {5, 15, 8, 5000, 2500};
    int ndivs[5] = {5, 11, 8, 505, 4}; 

    for(size_t i = 0; i < varNames.size(); ++i) {
        TString vName = varNames[i];
        TString vTitle = varTitles[i];
        
        TH1F *h_data  = (TH1F*)infile->Get(vName + "/data_obs");
        TH1F *h_qcd   = (TH1F*)infile->Get(vName + "/qcd");
        TH1F *h_ttbar = (TH1F*)infile->Get(vName + "/ttbar");
        TH1F *h_wjets = (TH1F*)infile->Get(vName + "/wjets");
        TH1F *h_other = (TH1F*)infile->Get(vName + "/other");
        TH1F *h_sig   = (TH1F*)infile->Get(vName + "/signal_M1800");

        if(h_data && h_qcd && h_ttbar && h_wjets && h_other && h_sig) {
            
            // 색상 적용 (CMS-PAS-SUS-21-005 Figure 4 규격 맞춤)
            h_qcd->SetFillColor(TColor::GetColor("#ffcc00"));   // 노란색
            h_ttbar->SetFillColor(TColor::GetColor("#3366ff")); // 파란색
            h_wjets->SetFillColor(TColor::GetColor("#9933cc")); // 보라색
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

            THStack *hs = new THStack("hs_" + vName, "");
            hs->Add(h_other);
            hs->Add(h_wjets);
            hs->Add(h_ttbar);
            hs->Add(h_qcd);

            TH1F *h_mc_total = (TH1F*)h_qcd->Clone("h_mc_total_" + vName);
            h_mc_total->Add(h_ttbar);
            h_mc_total->Add(h_wjets);
            h_mc_total->Add(h_other);

            TCanvas *c1 = new TCanvas("canvas_" + vName, vTitle + " Distribution", 800, 800);

            // 상단 패드(분포도)
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

            // CMS 로고 및 루미노시티 라벨 (Full Run-2 기준 표기 적용)
            TLatex* latex = new TLatex();
            latex->SetTextSize(0.05);
            latex->DrawLatexNDC(0.15, 0.93, "CMS #font[52]{Private Work}");
            latex->SetTextAlign(31); // 우측 정렬
            latex->DrawLatexNDC(0.95, 0.93, "138 fb^{-1} (13 TeV)"); 
            latex->SetTextFont(42);

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
            // 하단 패드(비율 그래프)
            TPad *pad2 = new TPad("pad2", "pad2", 0., 0., 1., 0.3);
            pad2->SetLeftMargin(0.15);
            pad2->SetRightMargin(0.05);
            pad2->SetTopMargin(0.0);
            pad2->SetBottomMargin(0.35);
            pad2->Draw();
            pad2->cd();

            TH1F *h_ratio = (TH1F*)h_data->Clone("h_ratio_" + vName);
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

            // PNG 저장
            TString outPath = Form("%s/combined_plot_%s.png", outDir.Data(), vName.Data());
            c1->SaveAs(outPath); 
            cout << "Saved: " << outPath << endl;

            delete latex;
            delete leg;
            delete line;
            delete hs;
            delete h_mc_total;
            delete h_ratio;
            delete c1;
        } else {
            cout << "[Warning] Missing histograms for variable: " << vName << endl;
        }
    }

    infile->Close();
    cout << "All plots generated successfully!" << endl;
    return 0;
}
