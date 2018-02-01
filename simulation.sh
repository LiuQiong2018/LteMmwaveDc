rm *.txt

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=6 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=169000" &> result.txt
mkdir 0124_01_16_01/
mv *.txt 0124_01_16_01

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=0 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=169000" &> result.txt
mkdir 0124_01_16_02/
mv *.txt 0124_01_16_02

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=2 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=169000" &> result.txt
mkdir 0124_01_16_03/
mv *.txt 0124_01_16_03

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=1 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=169000" &> result.txt
mkdir 0124_01_16_04/
mv *.txt 0124_01_16_04

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=7 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=169000" &> result.txt
mkdir 0124_01_16_05/
mv *.txt 0124_01_16_05


