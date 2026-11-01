#!/bin/bash
# Kullanim: run_sim.sh <kok> <tuslar>
export NEST_ROOT=$1 NEST_KEYS=$2
"$(dirname "$0")/sim" 2>&1 | grep -v '^$'
