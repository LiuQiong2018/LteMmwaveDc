rm *.txt

./waf --run "scratch/simulation-dc-mmwave-multi --simTime=10 --splitAlgorithm=6 --splitTimerInterval=10 --x2delay=5 --tcpDataRate=4000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=50 --pdcpEarlyRetTimer=60" &> result.txt
mkdir 1121_01_01/
mv *.txt 1121_01_01

./waf --run "scratch/simulation-dc-mmwave-multi --simTime=10 --splitAlgorithm=6 --splitTimerInterval=10 --x2delay=5 --tcpDataRate=4000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=50 --pdcpEarlyRetTimer=40" &> result.txt
mkdir 1121_01_02/
mv *.txt 1121_01_02


