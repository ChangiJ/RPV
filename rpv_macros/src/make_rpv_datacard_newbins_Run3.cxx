// ./run/make_rpv_datacard_newbins_Run3.exe 2000 mconly variations/output_nominal_newnt_2023.root 2023 off on
// makes a datacard for RPV analysis, simplified for Run 3
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "TH1.h"
#include "TString.h"
#include "TFile.h"
#include "TSystem.h"

// --- Simplified Systematics Functions ---
// Outputs a simple trigger efficiency systematic.
void outputSimpleSystematics(std::ofstream &file, const std::vector<std::string> &bins, TString year);
// Outputs the autoMCStats directive.
void outputautoMCStats(std::ofstream &file);

namespace {
  unsigned int nbins;
  unsigned int nprocesses;
}

using namespace std;

int main(int argc, char *argv[])
{
  TString year;
  std::string gluinoMass;
  std::string signalBinName;
  std::string cardType;
  TString inputname;
  TString sig_onoff;

  // --- Argument Parsing ---
  if(argc < 7) {
    std::cout << "Syntax: ./run/make_rpv_datacard_newbins_Run3.exe [gluino mass] [cardType] [filename] [year] [merge_flag] [syst_on/off]" << std::endl;
    std::cout << "Example: ./run/make_rpv_datacard_newbins_Run3.exe 2000 mconly variations/output_nominal_newnt_2023.root 2023 off on" << std::endl;
    return 1;
  } else {
    gluinoMass = argv[1];
    cardType = argv[2];
    inputname = argv[3];
    year = argv[4];
    // argv[5] (merge_flag) is ignored for Run 3 analysis but kept for argument consistency.
    sig_onoff = argv[6];
  }

  // --- Process and Bin Definitions ---
  std::stringstream ss;
  ss << "signal_M" << gluinoMass;
  signalBinName = ss.str();
  
  std::vector<std::string> processes = { signalBinName, "qcd", "ttbar", "wjets", "other"};
  nprocesses = processes.size();

  // Binning structure from the original file to generate multiple datacards.
  std::vector<std::vector<std::string>> bins;
  std::vector<std::string> bins_cr_nb0_lownjets    = {"bin37","bin22"}; 
  std::vector<std::string> bins_cr_nb1_lownjets    = {"bin40","bin25"}; 
  std::vector<std::string> bins_cr_nb2_lownjets    = {"bin43","bin28"}; 
  std::vector<std::string> bins_cr_nb3_lownjets    = {"bin46","bin31"}; 
  std::vector<std::string> bins_cr_nb0_mednjets    = {"bin38","bin23"}; 
  std::vector<std::string> bins_cr_nb1_mednjets    = {"bin41","bin26"}; 
  std::vector<std::string> bins_sr_nb2_mednjets    = {"bin44","bin29"}; 
  std::vector<std::string> bins_sr_nb3_mednjets    = {"bin47","bin32"}; 
  std::vector<std::string> bins_sr_nb4_mednjets    = {"bin50","bin35"}; 
  std::vector<std::string> bins_cr_nb0_highnjets   = {"bin39","bin24"}; 
  std::vector<std::string> bins_cr_nb1_highnjets   = {"bin42","bin27"}; 
  std::vector<std::string> bins_sr_nb2_highnjets   = {"bin45","bin30"}; 
  std::vector<std::string> bins_sr_nb3_highnjets   = {"bin48","bin33"}; 
  std::vector<std::string> bins_sr_nb4_highnjets   = {"bin51","bin36"}; 
  
  std::vector<std::string> bins_all = {"bin37","bin22","bin38","bin23","bin39","bin24","bin40","bin25","bin43","bin28","bin41","bin26","bin42","bin27","bin44","bin29"};
  if(cardType=="default" || cardType=="mconly"){
    bins_all.push_back("bin46"); bins_all.push_back("bin31");
    bins_all.push_back("bin47"); bins_all.push_back("bin32");
    bins_all.push_back("bin50"); bins_all.push_back("bin35");
    bins_all.push_back("bin45"); bins_all.push_back("bin30");
    bins_all.push_back("bin48"); bins_all.push_back("bin33");
    bins_all.push_back("bin51"); bins_all.push_back("bin36");
  }
  
  bins.push_back(bins_cr_nb0_lownjets);
  bins.push_back(bins_cr_nb1_lownjets);
  bins.push_back(bins_cr_nb2_lownjets);
  bins.push_back(bins_cr_nb3_lownjets);
  bins.push_back(bins_cr_nb0_mednjets);
  bins.push_back(bins_cr_nb1_mednjets);
  bins.push_back(bins_cr_nb0_highnjets);
  bins.push_back(bins_cr_nb1_highnjets);

  if(cardType=="default" || cardType=="mconly"){
      bins.push_back(bins_sr_nb2_mednjets);
      bins.push_back(bins_sr_nb3_mednjets);
      bins.push_back(bins_sr_nb4_mednjets);
      bins.push_back(bins_sr_nb2_highnjets);
      bins.push_back(bins_sr_nb3_highnjets);
      bins.push_back(bins_sr_nb4_highnjets);
  }
  bins.push_back(bins_all);
 
  // --- Main Loop to Generate Datacards ---
  for(unsigned int ipair=0; ipair<bins.size(); ipair++) { 
    if (bins.at(ipair).empty()) continue;
    
    nbins = bins.at(ipair).size(); 
    
    // Open ROOT file
    TFile *variations = TFile::Open(inputname);
    if (!variations || variations->IsZombie()) {
        std::cerr << "Error: Could not open input file " << inputname << std::endl;
        return 1;
    }

    // --- Datacard Naming ---
    std::string filename("datacards/datacard_M");
    filename += gluinoMass;
    if(cardType=="control") filename+="_control";
    else if(cardType=="mconly") filename+="_mconly";
    
    // Append bin combination name
    if(ipair==0) filename+="_cr_nb0_lownjets";
    if(ipair==1) filename+="_cr_nb1_lownjets";
    if(ipair==2) filename+="_cr_nb2_lownjets";
    if(ipair==3) filename+="_cr_nb3_lownjets";
    if(ipair==4) filename+="_cr_nb0_mednjets";
    if(ipair==5) filename+="_cr_nb1_mednjets";
    if(ipair==6) filename+="_cr_nb0_highnjets";
    if(ipair==7) filename+="_cr_nb1_highnjets";
    if(cardType!="control") {
        if(ipair==8) filename+="_sr_nb2_mednjets";
        if(ipair==9) filename+="_sr_nb3_mednjets";
        if(ipair==10) filename+="_sr_nb4_mednjets";
        if(ipair==11) filename+="_sr_nb2_highnjets";
        if(ipair==12) filename+="_sr_nb3_highnjets";
        if(ipair==13) filename+="_sr_nb4_highnjets";
    }
    // Final combined card is the last one in the vector
    if(ipair==bins.size()-1) filename.erase(filename.find("_"), std::string::npos);


    filename+="_nopdf"; // Kept from original for consistency
    TString tmpname = inputname;
    tmpname.ReplaceAll("variations/", "").ReplaceAll(".root","");
    filename += tmpname;
    filename += ".dat";
    
    // Create directory if it doesn't exist
    gSystem->Exec("mkdir -p datacards");
    std::ofstream file;
    file.open(filename);
    if (!file.is_open()) {
      std::cerr << "Error creating file: " << filename << std::endl;
      continue;
    }
    
    // --- Write Datacard Content ---
    file << "imax " << nbins << " number of bins" << std::endl;
    file << "jmax " << nprocesses-1 << std::endl;
    file << "kmax * number of nuisance parameters" << std::endl;
    file << "------------------------------------" << std::endl;
    
    for(unsigned int ibin=0; ibin<nbins; ibin++) {
        file << "shapes * " << bins.at(ipair).at(ibin) << " " << inputname << " " << bins.at(ipair).at(ibin);
        file << "/$PROCESS " << bins.at(ipair).at(ibin) << "/$PROCESS_$SYSTEMATIC" << std::endl;
    }
    file << "------------------------------------" << std::endl;  

    file << "bin\t";
    for(unsigned int ibin=0; ibin<nbins; ibin++) {
        file << bins.at(ipair).at(ibin) << "\t";
    }
    file << "\n";
    file << "observation\t";
    for(unsigned int ibin=0; ibin<nbins; ibin++) {
        TString binName(bins.at(ipair).at(ibin));
        TH1F *hist = static_cast<TH1F*>(variations->Get(Form("%s/data_obs",binName.Data())));
        file << (hist ? hist->Integral() : 0.0) << "\t";
        if(hist) delete hist;
    }
    file << "\n";
    file << "------------------------------------" << std::endl;

    file << "bin\t";
    for(unsigned int ibin=0; ibin<nbins; ibin++) {
        for(unsigned int iprocess=0; iprocess<nprocesses; iprocess++) {
            file << bins.at(ipair).at(ibin) << "\t";
        }
    }
    file << "\n";
    file << "process\t";
    for(unsigned int ibin=0; ibin<nbins; ibin++) {
        for(unsigned int iprocess=0; iprocess<nprocesses; iprocess++) {
            file << processes.at(iprocess) << "\t";
        }
    }
    file << "\n";
    file << "process\t";
    for (unsigned int ibin=0; ibin<nbins; ibin++) {
        for (unsigned int iproc=0; iproc<nprocesses; iproc++) {
            file << iproc << "\t";
        }
    }
    file << "\n";
    file << "rate\t";
    for(unsigned int ibin=0; ibin<nbins; ibin++) {
        for(unsigned int iprocess=0; iprocess<nprocesses; iprocess++) {
            TString histName(Form("%s/%s", bins.at(ipair).at(ibin).c_str(), processes.at(iprocess).c_str()));
            TH1F *hist = static_cast<TH1F*>(variations->Get(histName));
            double rate = (hist ? hist->Integral() : 0.0);
            file << (rate > 0 ? rate : 0.0) << "\t";
            if(hist) delete hist;
        }
    }
    file << "\n------------------------------------" << std::endl;

    if(sig_onoff == "on") {
        outputSimpleSystematics(file, bins.at(ipair), year);
    }
    
    outputautoMCStats(file);

    file.close();
    variations->Close();
    cout << "Created: " << filename << endl;
  } // End of loop over bin combinations
  return 0;
}

void outputSimpleSystematics(std::ofstream &file, const std::vector<std::string> &bins, TString year)
{
    // Example systematic: Trigger efficiency
    // Applies 1% uncertainty (1.01) to signal and 'other' processes.
    file << "trigeff_" << year << "\tlnN\t";
    for(unsigned int ibin=0; ibin<bins.size(); ibin++) {
      // Format: signal  qcd  ttbar  wjets  other
      file << "1.01\t-\t-\t-\t1.01\t";
    }
    file << std::endl;
    
    // You can add other simple lnN systematics here, for example:
    // file << "lumi_" << year << "\tlnN\t";
    // for(unsigned int ibin=0; ibin<bins.size(); ibin++) {
    //   file << "1.018\t-\t-\t-\t1.018\t"; // 1.8% lumi uncertainty for 2023
    // }
    // file << std::endl;
}


void outputautoMCStats(std::ofstream &file)
{
 // Use a threshold of 0 to include all bins with non-zero statistical uncertainty
 file << "* autoMCStats 0" << endl;
}


