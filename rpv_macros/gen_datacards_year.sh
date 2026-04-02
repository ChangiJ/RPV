#!/usr/bin/sh

mkdir -p datacards/testfile

for step in {1..4..1}; do
		for mass in {1000..2200..100}; do
				echo "Processing step ${step}, mass ${mass}"
			
				output_file="datacards/testfile/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_201678_mconly.dat"
				combineCards.py datacards/testfile_step/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_2016_mconly.dat datacards/testfile_step/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_2017_mconly.dat datacards/testfile_step/datacard_M${mass}_mconly_step${step}_nopdfoutput_nominal_newnt_2018_mconly.dat > "${output_file}"

				chmod u+w "${output_file}"

				sed -i 's|datacards/testfile_step/datacards/|datacards/|g' "${output_file}"
		done
done
