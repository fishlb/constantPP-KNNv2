#!/bin/bash

# ==============================================================================
# Script to run result summary scripts
# ==============================================================================

echo ">>> Starting data summary extraction..."

# 1. Run Table V summary (Accuracy)
echo "Processing Table V..."
python tableV_accuracy_summary.py

# 2. Run Fig 3 summary (Offline Cost)
echo "Processing Fig 3..."
python fig3_offline_cost_summary.py

# 3. Run Fig 4 summary (Latency)
echo "Processing Fig 4..."
python fig4_latency_result_summary.py

echo "---------------------------------------------------"
echo "All scripts have finished!"
echo "Check the following files in root directory:"
echo " - TableV_Accuracy_Summary.txt"
echo " - Fig3_OfflineCost_Summary.txt"
echo " - Fig4_LatencyResult_Summary.txt"
echo "==================================================="