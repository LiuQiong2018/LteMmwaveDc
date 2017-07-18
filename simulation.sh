rm *.txt

./waf --run "scratch/simulation-dc-mmwave --simTime=3 --nodeNum=4" &> result.txt
mkdir test
mv *.txt test/


