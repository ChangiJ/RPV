#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TChain.h"
#include "TCanvas.h"
#include "iostream"
#include "vector"
#include "string"
#include "filesystem"
#include "TLatex.h"

using namespace std;

string HEM = "/mnt/data3/babies/241201/UL2018/merged_rpvfitnbge0_hem_data";
string No_HEM = "/mnt/data3/babies/241201/UL2018/merged_rpvfitnbge0_data";

void Distribution(string inputDir = No_HEM, int phi_bin = 62, int eta_bin = 48) {
    string region = (inputDir == No_HEM) ? "No_HEM" : "HEM";

    TChain *ch = new TChain("tree");
    int i = 0;
    for (const auto& entry : filesystem::directory_iterator(inputDir)) {
        if (entry.path().extension() == ".root") {
            i++;
            //cout << i << endl;
            ch->Add(entry.path().c_str());
        }
    }

    TCanvas *c1 = new TCanvas("eta_phi", "eta_phi", 1200, 900);

    string title = region + " phi_bin=" + to_string(phi_bin) + ",eta_bin=" + to_string(eta_bin);
    TH2F *hist = new TH2F("hist",
                          title.c_str(),
                          phi_bin, -3.14, 3.14,
                          eta_bin, -2.4, 2.4);
    hist->GetXaxis()->SetTitle("#phi");
    hist->GetYaxis()->SetTitle("#eta");
	hist->GetZaxis()->SetRangeUser(0,hist->GetMaximum());
	
//	c1->SetLogz();

//	hist->SetMaximum(50);
//	hist->SetMinimum(10);  


    ch->Draw("jets_eta:jets_phi>>hist", "jets_eta > -2.4 && jets_eta < 2.4 && jets_phi > -3.14 && jets_phi < 3.14 && ht>1200 && nleps == 1 && njets >=4 && pass && trig_ht1050 && run<319077", "colz");

	string pdfFileName = "test/results/" + region + "_phi" + to_string(phi_bin) + "_eta" + to_string(eta_bin) + ".pdf";
    string pngFileName = "test/results/" + region + "_phi" + to_string(phi_bin) + "_eta" + to_string(eta_bin) + ".png";
    //c1->SaveAs(pdfFileName.c_str());
    c1->SaveAs(pngFileName.c_str());
}

