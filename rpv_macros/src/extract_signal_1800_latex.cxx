#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

#include "TString.h"
#include "TFile.h"
#include "TH1F.h"

using namespace std;

// 요청하신 유효숫자 규칙을 적용하는 함수
// 1 미만 : 소수점 3자리 (예: 0.343)
// 1 이상 10 미만 : 소수점 2자리 (예: 3.31)
// 10 이상 100 미만 : 소수점 2자리 (예: 10.17, 13.36 -> 총 4자리)
// 100 이상 1000 미만 : 소수점 1자리 (예: 123.4 -> 총 4자리)
// 1000 이상 : 소수점 없음 (예: 1234 -> 총 4자리)
TString formatYield(float val) 
{
    float abs_val = std::abs(val);
    if (abs_val == 0.0) return "0.00";
    else if (abs_val < 1.0) return Form("%.3f", val);
    else if (abs_val < 10.0) return Form("%.2f", val);
    else if (abs_val < 100.0) return Form("%.2f", val);
    else if (abs_val < 1000.0) return Form("%.1f", val);
    else return Form("%.0f", val);
}

int main(int argc, char* argv[])
{
    if(argc < 2) {
        cout << "Usage: " << argv[0] << " <input_root_file>" << endl;
        cout << "Example: " << argv[0] << " variations/output_nominal_newnt_UL2017.root" << endl;
        return 1;
    }

    TString inputName = argv[1];
    TFile* infile = TFile::Open(inputName, "READ");
    if(!infile || infile->IsZombie()) {
        cout << "[Error] Cannot open file: " << inputName << endl;
        return 1;
    }

    const int nbins = 52;
    TString binLatex[nbins] = {
        // [0] ~ [21]번 인덱스는 현재 출력 범위(22~36)에 들어가지 않으므로 패딩 처리
        "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},4\\leq N_{jets}\\leq5,N_{b}=0",    // 22
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},6\\leq N_{jets}\\leq7,N_{b}=0",    // 23
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},N_{jets}\\geq8,N_{b}=0",           // 24
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},4\\leq N_{jets}\\leq5,N_{b}=1",    // 25
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},6\\leq N_{jets}\\leq7,N_{b}=1",    // 26
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},N_{jets}\\geq8,N_{b}=1",           // 27
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},4\\leq N_{jets}\\leq5,N_{b}=2",    // 28
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},6\\leq N_{jets}\\leq7,N_{b}=2",    // 29
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV}, N_{jets}\\geq8,N_{b}=2",          // 30
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},4\\leq N_{jets}\\leq5,N_{b}\\geq3",// 31
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},6\\leq N_{jets}\\leq7,N_{b}=3",    // 32
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV}, N_{jets}\\geq8,N_{b}=3",          // 33
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},4\\leq N_{jets}\\leq5,N_{b}\\geq4",// 34 (제외 대상)
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV},6\\leq N_{jets}\\leq7,N_{b}\\geq4",// 35
        "N_{leps}=1,H_{T}>1200~\\textrm{GeV}, N_{jets}\\geq8,N_{b}\\geq4"       // 36
    };

    cout << "\\begin{table}[htbp!]" << endl;
    cout << "\\centering" << endl;
    cout << "\\caption{Expected signal yields for $m_{\\tilde{g}} = 1.8$ TeV.}" << endl;
    
    // 표가 페이지를 넘어가지 않게 압축 (행간 간격 0.8배, 폰트 크기 축소)
    cout << "\\renewcommand{\\arraystretch}{0.8}" << endl;
    cout << "\\footnotesize" << endl;
    cout << "\\begin{tabular}{ c | c }" << endl;
    cout << "\\hline\\hline" << endl;
    cout << "$M_{J}$ (GeV) & $m_{\\tilde{g}}=1.8$ TeV \\\\"  << endl;
    cout << "\\hline" << endl;

    // 1-lepton 구간만 출력 (22 ~ 36번, 34번 제외)
    for(int ibin = 22; ibin < 37; ibin++) 
    {
        if(ibin == 34) continue;

        // 원본 테이블의 스타일처럼 조건식(Nlep, Njet 등)을 하나의 가로행으로 병합하여 출력
        cout << "\\multicolumn{2}{c}{$" << binLatex[ibin].Data() << "$} \\\\" << endl;
        cout << "\\hline" << endl;

        // Histogram 읽어오기 (table_rpv_newnt.cxx 방식)
        TH1F* h_sig1800 = static_cast<TH1F*>(infile->Get(Form("bin%i/signal_M1800", ibin)));
        
        float sig_val[3] = {0.0, 0.0, 0.0};
        if (h_sig1800) {
            sig_val[0] = h_sig1800->GetBinContent(1);
            sig_val[1] = h_sig1800->GetBinContent(2);
            sig_val[2] = h_sig1800->GetBinContent(3);
        }

        // 유효숫자 변환 함수(formatYield)를 적용하여 출력 (오차 미포함)
        cout << "$500 - 800$ & "  << formatYield(sig_val[0]) << " \\\\" << endl;
        cout << "$800 - 1100$ & " << formatYield(sig_val[1]) << " \\\\" << endl;
        cout << "$1100 \\le$ & "  << formatYield(sig_val[2]) << " \\\\" << endl;
        cout << "\\hline" << endl;
    }

    cout << "\\end{tabular}" << endl;
    cout << "\\end{table}\n" << endl;

    infile->Close();
    return 0;
}
