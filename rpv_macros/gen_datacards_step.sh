#!/bin/bash

# This script combines datacards in four different steps as specified.
# It iterates through signal masses from 1500 to 2500 GeV.

# --- Configuration ---
YEAR="2023"
INPUT_DIR="datacards"
OUTPUT_BASE_DIR="datacards/testfile_step"
OUTPUT_DIR="${OUTPUT_BASE_DIR}/${YEAR}"
# Suffix for the input datacard filenames
FILENAME_SUFFIX="_nopdfoutput_nominal_newnt_${YEAR}.dat"
# --- End Configuration ---

# Create the output directory if it doesn't exist
mkdir -p ${OUTPUT_DIR}

echo "Starting datacard combination for all steps..."

# Loop over signal masses from 1500 to 2500 in steps of 100
for m in $(seq 1500 100 2500); do
    echo "-----------------------------------------------------"
    echo "Processing Mass = ${m} GeV"
    echo "-----------------------------------------------------"

    # Define the base name for input files for this mass
    BASE_IN_PREFIX="${INPUT_DIR}/datacard_M${m}_mconly"

    # --- Step 1: Nb = {0,1,2} * {lownjet, midnjet} (6 files total) ---
    echo "  -> Combining cards for Step 1..."
    combineCards.py \
        ${BASE_IN_PREFIX}_cr_nb0_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb0_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb2_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb2_mednjets${FILENAME_SUFFIX} \
        > ${OUTPUT_DIR}/datacard_${m}_${YEAR}_step1.dat

    # --- Step 2: Nb = {0,1,2} * {low, med, high} njet (9 files total) ---
    echo "  -> Combining cards for Step 2..."
    combineCards.py \
        ${BASE_IN_PREFIX}_cr_nb0_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb0_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb0_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb2_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb2_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb2_highnjets${FILENAME_SUFFIX} \
        > ${OUTPUT_DIR}/datacard_${m}_${YEAR}_step2.dat

    # --- Step 3: Step 2 files + {Nb=3 & lownjet} (10 files total) ---
    echo "  -> Combining cards for Step 3..."
    combineCards.py \
        ${BASE_IN_PREFIX}_cr_nb0_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb0_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb0_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb2_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb2_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb2_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb3_lownjets${FILENAME_SUFFIX} \
        > ${OUTPUT_DIR}/datacard_${m}_${YEAR}_step3.dat

    # --- Step 4: All 14 individual datacards ---
    echo "  -> Combining cards for Step 4..."
    combineCards.py \
        ${BASE_IN_PREFIX}_cr_nb0_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb0_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb0_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb1_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb2_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb2_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb2_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_cr_nb3_lownjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb3_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb3_highnjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb4_mednjets${FILENAME_SUFFIX} \
        ${BASE_IN_PREFIX}_sr_nb4_highnjets${FILENAME_SUFFIX} \
        > ${OUTPUT_DIR}/datacard_${m}_${YEAR}_step4.dat

done

echo ""
echo "Combination complete. Combined files are located in: ${OUTPUT_DIR}"


