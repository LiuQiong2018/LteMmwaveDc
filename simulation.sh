rm *.txt

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=6 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000" &> result.txt
mkdir 1127_03_03_01/
mv *.txt 1127_03_03_01

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=0 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000" &> result.txt
mkdir 1127_03_03_02/
mv *.txt 1127_03_03_02

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=2 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000" &> result.txt
mkdir 1127_03_03_03/
mv *.txt 1127_03_03_03

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=1 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000" &> result.txt
mkdir 1127_03_03_04/
mv *.txt 1127_03_03_04

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=7 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000" &> result.txt
mkdir 1127_03_03_05/
mv *.txt 1127_03_03_05


./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=6 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=10000000" &> result.txt
mkdir 1127_03_04_01/
mv *.txt 1127_03_04_01

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=0 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=10000000" &> result.txt
mkdir 1127_03_04_02/
mv *.txt 1127_03_04_02

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=2 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=10000000" &> result.txt
mkdir 1127_03_04_03/
mv *.txt 1127_03_04_03

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=1 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=10000000" &> result.txt
mkdir 1127_03_04_04/
mv *.txt 1127_03_04_04

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=7 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=10000000" &> result.txt
mkdir 1127_03_04_05/
mv *.txt 1127_03_04_05


./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=6 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=100000000" &> result.txt
mkdir 1127_03_05_01/
mv *.txt 1127_03_05_01

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=0 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=100000000" &> result.txt
mkdir 1127_03_05_02/
mv *.txt 1127_03_05_02

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=2 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=100000000" &> result.txt
mkdir 1127_03_05_03/
mv *.txt 1127_03_05_03

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=1 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=100000000" &> result.txt
mkdir 1127_03_05_04/
mv *.txt 1127_03_05_04

./waf --run "scratch/simulation-dc-mmwave --simTime=8 --splitAlgorithm=7 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=100000000" &> result.txt
mkdir 1127_03_05_05/
mv *.txt 1127_03_05_05


./waf --run "scratch/simulation-dc-mmwave --simTime=15 --splitAlgorithm=6 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000000" &> result.txt
mkdir 1127_03_06_01/
mv *.txt 1127_03_06_01

./waf --run "scratch/simulation-dc-mmwave --simTime=15 --splitAlgorithm=0 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000000" &> result.txt
mkdir 1127_03_06_02/
mv *.txt 1127_03_06_02

./waf --run "scratch/simulation-dc-mmwave --simTime=15 --splitAlgorithm=2 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000000" &> result.txt
mkdir 1127_03_06_03/
mv *.txt 1127_03_06_03

./waf --run "scratch/simulation-dc-mmwave --simTime=15 --splitAlgorithm=1 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000000" &> result.txt
mkdir 1127_03_06_04/
mv *.txt 1127_03_06_04

./waf --run "scratch/simulation-dc-mmwave --simTime=15 --splitAlgorithm=7 --splitTimerInterval=10 --x2delay=10 --tcpDataRate=2000Mb/s --alpha=0.1 --beta=0.1 --pdcpReorderingTimer=100 --pdcpEarlyRetTimer=70 --dataSize=1000000000" &> result.txt
mkdir 1127_03_06_05/
mv *.txt 1127_03_06_05


