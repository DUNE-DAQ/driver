source setup_build_environment

echo Set up artdaq
setup artdaq v3_09_01 -q e19:s96:prof

echo Set up artdaq_demo_hdf5
setup artdaq_demo_hdf5 v1_01_00a -q e19:s96:prof

echo Set up artdaq_demo
setup artdaq_demo v3_09_01 -q e19:s96:prof
