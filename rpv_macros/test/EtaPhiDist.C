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

void EtaPhiDist() {
    std::string inputDir = "/mnt/data3/babies/241201/UL2018/merged_rpvfitnbge0_hem_data";

	int i = 0;

    TChain *ch = new  TChain("tree");
    for (const auto& entry : std::filesystem::directory_iterator(inputDir)) {
        if (entry.path().extension() == ".root") {
			i++;
			std::cout << i << std::endl;
			ch -> Add(entry.path().c_str());
        }
    }
	
	TCanvas *c1 = new TCanvas("eta:phi","eta:phi",1200,900);

	TH2F *hist = new TH2F("hist",
						"No_HEM",
						1000,-3.14,3.14,
						1000,-5,5);

	hist->GetXaxis()->SetTitle("#phi");
    hist->GetYaxis()->SetTitle("#eta");

	ch->Draw("jets_eta:jets_phi>>hist","pass && trig_ht1050 && run<319077","colz");

	c1->SaveAs("test/No_HEM.pdf");
	c1->SaveAs("test/No_HEM.png");
}
