#!/usr/bin/sh

for mass in {1000..2200..100}; do
	echo ${mass}
./run/make_rpv_datacard_newbins_0lstudy.exe ${mass} mconly output_nominal_newnt_2018_mconly.root 2018 off on
done
