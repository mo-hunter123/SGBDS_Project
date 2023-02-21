#!/bin/bash 

export C_A="0.2"
export C_V="0.4"
export FIX_COST="1000.0"
export MIN_WAIT="45"
export PATTERN_CLUSTERS="cluster [0-9][0-9]* : (T_[0-9][0-9]*( HLP | WS ))*T_[0-9][0-9]* ;"
export PARTIAL_TRIPS="650"

# gdb --args VRPsimplex2 ../data/partialTest.txt ../output/clusters/exact_meth_out.txt
./PartialSimplex ../data/partialTest.txt ../output/clusters/exact_meth_out.txt
