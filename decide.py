import os

# detect
testList = "data/testList.txt"
finp = open("data/testList.txt", "r")
fout = open("data/dataList.txt", "w")
lines = finp.readlines()

fout.write("out/rf.txt\n")
cnt = 0;
labList = []
for l in lines:
    line = l.strip()
    if cnt>0:
        name = line[17:34]
        lab  = int(line[26:28])
        if(lab==21): lab=16
        elif(lab==22): lab=17
        elif(lab==24): lab=19
        labList = labList + [lab]
        fout.write("data/IMG_%s/%s.jpg out/exp/labels/%s.txt\n"%(name[0:7],name,name))
    cnt = cnt+1
finp.close()
fout.close()

# decide
os.system(".\\bin\\decideAction.exe data/dataList.txt")

cnt=0; sum=0;
rinp = open("out/result.txt", "r")
aout = open("out/summary.txt", "w")
predictions = rinp.readlines()
for p in predictions:
    p = int(p.strip())
    if(p==labList[cnt]): sum=sum+1
    aout.write("%d %d %d\n"%(labList[cnt],p,p==labList[cnt]))
    cnt = cnt+1
print(sum/cnt)
aout.write("acc.: %f\n"%(sum/cnt))
rinp.close()
aout.close()