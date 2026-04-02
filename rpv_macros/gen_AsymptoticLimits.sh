#!/usr/bin/sh


## do combine
for num in {1,2,3,4}; do 
		for mass in {1500..2500..100}; do
				echo ${num} ${mass}	
				combine -M AsymptoticLimits datacards/testfile_step/2023/datacard_${mass}_2023_step${num}.dat
				mv higgsCombineTest.AsymptoticLimits.mH120.root newbin_Run3/higgsCombine_M${mass}_mconly_step${num}.AsymptoticLimits.mH120.root
		done
done
