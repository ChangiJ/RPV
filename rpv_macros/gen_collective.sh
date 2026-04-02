#/usr/bin/env bash

# clear datacards folder
setup_directories() {
    mkdir -p datacards/testfile_step
    mkdir -p datacards/testfile
    echo "Directories initialized."
}

# generate datacards
generate_datacards() {
for year in {2016..2018..1}; do
   	for mass in {1000..2200..100}; do
       	echo "Generating datacard for mass ${mass} ${year}..."
     		./run/make_rpv_datacard_newbins_0lstudy.exe ${mass} mconly output_nominal_newnt_${year}_mconly.root ${year} off on
    done
done
}

# collect by steps 
combine_cards_by_step() {
echo "generating combinecard by step"
for year in {2016..2018..1}; do
		for mass in {1000..2200..100}; do
				echo ${year} ${mass}
## step 1
				combineCards.py datacards/datacard_M${mass}_mconly_cr_nb0_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb0_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb1_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb1_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb2_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_sr_nb2_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat > datacards/testfile_step/datacard_M${mass}_mconly_step1_nopdfoutput_nominal_newnt_${year}_mconly.dat


## step 2
								combineCards.py datacards/datacard_M${mass}_mconly_cr_nb0_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb0_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb0_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb1_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb1_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb1_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb2_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_sr_nb2_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat > datacards/testfile_step/datacard_M${mass}_mconly_step2_nopdfoutput_nominal_newnt_${year}_mconly.dat

## step 3
								combineCards.py datacards/datacard_M${mass}_mconly_cr_nb0_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb0_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb1_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb1_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb1_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb2_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_sr_nb2_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb3_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat > datacards/testfile_step/datacard_M${mass}_mconly_step3_nopdfoutput_nominal_newnt_${year}_mconly.dat

## step 4
								combineCards.py datacards/datacard_M${mass}_mconly_cr_nb0_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb0_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb1_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb1_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb1_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_cr_nb2_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat  datacards/datacard_M${mass}_mconly_sr_nb2_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_sr_nb2_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_cr_nb3_lownjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_sr_nb3_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_sr_nb3_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_sr_nb4_mednjets_nopdfoutput_nominal_newnt_${year}_mconly.dat datacards/datacard_M${mass}_mconly_sr_nb4_highnjets_nopdfoutput_nominal_newnt_${year}_mconly.dat > datacards/testfile_step/datacard_M${mass}_mconly_step4_nopdfoutput_nominal_newnt_${year}_mconly.dat
		done
done
}

# collect by year
combine_cards_by_year() {
		echo "Generating combinecards by year"
    for step in {1..4}; do
        for mass in {1000..2200..100}; do
            echo "Combining cards for step ${step}, mass ${mass}..."
            output_file="datacards/testfile/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_201678_mconly.dat"
            combineCards.py \
                datacards/testfile_step/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_2016_mconly.dat \
                datacards/testfile_step/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_2017_mconly.dat \
                datacards/testfile_step/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_2018_mconly.dat \
                > "${output_file}"
            chmod u+w "${output_file}"
            sed -i 's|datacards/testfile_step/datacards/|datacards/|g' "${output_file}"
            echo "Generated: ${output_file}"
        done
    done
}

# main function
main() {
    setup_directories
    generate_datacards
    combine_cards_by_step
    combine_cards_by_year
    echo "All tasks completed successfully!"
}

# start script
main


