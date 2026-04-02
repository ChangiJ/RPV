// plot_dps: Macro that plots variables both for comissioning DPS

#include <iostream>
#include <vector>

#include "TChain.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TString.h"
#include "TColor.h"

#include "styles.hpp"
#include "utilities.hpp"
#include "utilities_macros.hpp"
#include "utilities_macros_rpv.hpp"

using namespace std;

TString plot_type=".pdf";
TString plot_style="CMSPaper_Preliminary";

void hem();

// data for HEM Issue
TString folder_dat_18_without_hem = "/mnt/data3/babies/241201/UL2018/merged_rpvfitnbge0_data/";
TString folder_dat_18_with_hem    = "/mnt/data3/babies/241201/UL2018/merged_rpvfitnbge0_hem_data/";
vector<TString> s_data_2018_without_hem = getRPVProcess(folder_dat_18_without_hem,"data");
vector<TString> s_data_2018_with_hem    = getRPVProcess(folder_dat_18_with_hem,   "data");

vector<sfeats> Samples;
vector<hfeats> vars;

void test() {

  TChain *ch = new TChain("tree");
	ch->Add("/mnt/data3/babies/241201/UL2018/merged_rpvfitnbge0_data/*.root");
	ch->Draw("");
}

void hem() {

  TString luminosity="1.";

//  Samples.push_back(sfeats(s_data_2018_with_hem, "2018 data without HEM objects", kRed,1,"pass && trig_ht1050 && run<319077 && Sum$((jets_eta>-3.0 && jets_eta<-1.3 && jets_phi>-1.57 && jets_phi <-0.87) && jets_pt>30 && abs(jets_eta)<2.4 && jets_id==1)==0")); Samples.back().isData=true;
  Samples.push_back(sfeats(s_data_2018_without_hem, "2018 data without HEM objects", kRed,1,"pass && trig_ht1050 && run<319077")); Samples.back().isData=true;
  Samples.push_back(sfeats(s_data_2018_with_hem, "2018 data with HEM objects", kBlue,1,"pass && trig_ht1050 && run<319077"));
  Samples.back().doBand = true;
  vector<int> data;
  data.push_back(0);
  data.push_back(1);

  std::vector<TString> cuts = {"nleps==1&&mj12>500&&ht>1200&&nbm==0&&njets>=4&&njets<=5", // bin22
                               "nleps==1&&mj12>500&&ht>1200&&nbm==0&&njets>=6&&njets<=7", // bin23
                               "nleps==1&&mj12>500&&ht>1200&&nbm==0&&njets>=8",           // bin24
                               "nleps==1&&mj12>500&&ht>1200&&nbm==1&&njets>=4&&njets<=5", // bin25
                               "nleps==1&&mj12>500&&ht>1200&&nbm==1&&njets>=6&&njets<=7", // bin26
                               "nleps==1&&mj12>500&&ht>1200&&nbm==1&&njets>=8"};          // bin27

  for(auto icut : cuts) {
    vars.push_back(hfeats("mj12", 3, 500, 1400, data, "mj12", icut,-1,"data"));
    vars.back().normalize = true;
  }

  plot_distributions(Samples, vars, luminosity, plot_type, plot_style, "dataquality/hem", true, true);
}



int main(int argc, char *argv[]){
  
  TString issue = argv[1];
  TString year  = argv[2];

  test();
  // HEM
  if((issue=="hem") && (year=="2018")) {
    cout << "//////////////////////////////////////////////////" << endl;
    cout << "//////////////////// HEM Issue ///////////////////" << endl;
    cout << "//////////////////////////////////////////////////" << endl;
    cout << endl;
    cout << "!!! PLEASE fix L438 and L481 in src/utilities_macros.cpp !!!" << endl;

    hem();

    cout << "!!! PLEASE fix L438 and L481 in src/utilities_macros.cpp !!!" << endl;
  }

  else {
    cout << "ERROR::Please input argument as hem/l1/l1pre/l1prefire" << endl;
    cout << "ERROR::Please input year     as 2016 or 2017 for l1prefire" << endl;
    cout << "ERROR::Please input year     as 2018 for hem" << endl;
  }


}
