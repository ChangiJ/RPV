#include <TFile.h>
#include <TH1F.h>
#include <TString.h>
#include <iostream>

void MergeHistogram() {
    TFile *inputFile = new TFile("variations/output_impact_UL201678_UL201678.root", "READ");
    if (!inputFile || inputFile->IsZombie()) {
        std::cerr << "Error: Cannot open input file!" << std::endl;
        return;
    }

    TFile *outputFile = new TFile("variations/output_impact_merged_UL201678.root", "RECREATE");
    if (!outputFile || outputFile->IsZombie()) {
        std::cerr << "Error: Cannot create output file!" << std::endl;
        inputFile->Close();
        return;
    }

    int ibin_start = 22, ibin_end = 36;
    TString processes[] = {"qcd", "wjets", "ttbar"};
    int kappa_vals[] = {1, 2};
    int r_vals[] = {1, 2}; 
    TString yearVariants[] = {"Up", "Down"};

    TString histPatterns[] = {
        "%s_kappa%d_njets%d_%s_%s",            // kappa
        "%s_mjsyst_r%d_njets%d_%s_%s",         // mjsyst_r
        "%s_MC_kappa%d_njets%d_%s",           // MC_kappa
        "%s_MC_kappa%d_jec_njets%d_%s",       // MC_kappa_jec
        "%s_MC_kappa%d_jer_njets%d_%s",       // MC_kappa_jer
        "%s_kappa%d_unc_dy_njets%d_%s_%s"     // kappa_unc_dy
    };

    for (int ibin = ibin_start; ibin <= ibin_end; ++ibin) {
        int ijet = (ibin - 1) % 3;
        int ind_ijet = 10 * (2 * ijet + 4) + 2 * ijet + 5;
        if (ind_ijet == 89) ind_ijet = 8;

        TString dirName = Form("bin%i", ibin);
        TDirectory *dir = outputFile->mkdir(dirName);
        if (!dir) {
            dir = outputFile->GetDirectory(dirName);
        }
        dir->cd(); 

		// nominal
		for (const auto &procname : processes) {
				TString histPath = Form("bin%i/%s", ibin, procname.Data());
				TH1F *hist = (TH1F*)inputFile->Get(histPath);
				if (hist) {
						TH1F *clonedHist = (TH1F*)hist->Clone();
						clonedHist->Write();
				}
		}

		// up and down
        for (const auto &procname : processes) {
            for (int ikap : kappa_vals) {
                for (const auto &var : yearVariants) {
                    for (const auto &pattern : histPatterns) {
                        TString histName2016, histName1718, mergedHistName;

                        if (pattern.Contains("mjsyst_r")) { 
                            for (int r : r_vals) {
								histName2016 = Form("bin%i/%s", ibin, Form(pattern.Data(), procname.Data(), r, ind_ijet, procname.Data(), Form("2016%s", var.Data())));
								histName1718 = Form("bin%i/%s", ibin, Form(pattern.Data(), procname.Data(), r, ind_ijet, procname.Data(), Form("1718%s", var.Data())));
								mergedHistName = Form(pattern.Data(), procname.Data(), r, ind_ijet, procname.Data(), Form("201678%s", var.Data()));
                                
							cout << histName2016 << endl;

                            TH1F *hist2016 = (TH1F*)inputFile->Get(histName2016);
                            TH1F *hist1718 = (TH1F*)inputFile->Get(histName1718);
                            TH1F *mergedHist = nullptr;
                            if (hist2016) mergedHist = (TH1F*)hist2016->Clone(mergedHistName);
                                else mergedHist = (TH1F*)hist1718->Clone(mergedHistName);

                                if (hist2016 && hist1718) mergedHist->Add(hist1718);

                                mergedHist->Write();
                            }
                        }
						else { 
							histName2016 = Form("bin%i/%s", ibin, Form(pattern.Data(), procname.Data(), ikap, ind_ijet, procname.Data(), Form("2016%s", var.Data())));
                            histName1718 = Form("bin%i/%s", ibin, Form(pattern.Data(), procname.Data(), ikap, ind_ijet, procname.Data(), Form("1718%s", var.Data())));
                            mergedHistName = Form(pattern.Data(), procname.Data(), ikap, ind_ijet, procname.Data(), Form("201678%s", var.Data()));
                            
                            TH1F *hist2016 = (TH1F*)inputFile->Get(histName2016);
                            TH1F *hist1718 = (TH1F*)inputFile->Get(histName1718);
                            if (hist2016 || hist1718) {
                                TH1F *mergedHist = nullptr;
                                if (hist2016) mergedHist = (TH1F*)hist2016->Clone(mergedHistName);
                                else mergedHist = (TH1F*)hist1718->Clone(mergedHistName);

                                if (hist2016 && hist1718) mergedHist->Add(hist1718);

                                mergedHist->Write();
                            }
                        }
                    }
                }
            }
        }
    }

    inputFile->Close();
    outputFile->Close();

    std::cout << "Merged histograms saved to merged_output.root" << std::endl;
}

