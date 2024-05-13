import os
import random
import cv2 as cv

PATH_P00_S00 = "data/EDM_P00_S00"
PATH_P01_S00 = "data/EDM_P01_S00"
PATH_P02_S00 = "data/EDM_P02_S00"
PATH_P03_S00 = "data/EDM_P03_S00"
PATH_P04_S00 = "data/EDM_P04_S00"

fileEx = '.txt'
LIST_P00_S00 = [file for file in os.listdir(PATH_P00_S00) if file.endswith(fileEx)]
LIST_P01_S00 = [file for file in os.listdir(PATH_P01_S00) if file.endswith(fileEx)]
LIST_P02_S00 = [file for file in os.listdir(PATH_P02_S00) if file.endswith(fileEx)]
LIST_P03_S00 = [file for file in os.listdir(PATH_P03_S00) if file.endswith(fileEx)]
LIST_P04_S00 = [file for file in os.listdir(PATH_P04_S00) if file.endswith(fileEx)]

FILE_P00_S00, FILE_P01_S00, FILE_P02_S00, FILE_P03_S00, FILE_P04_S00 = [], [], [], [], []
for ind in range(len(LIST_P00_S00)): FILE_P00_S00 = FILE_P00_S00 + ["%s/%s"%(PATH_P00_S00,LIST_P00_S00[ind])]
for ind in range(len(LIST_P01_S00)): FILE_P01_S00 = FILE_P01_S00 + ["%s/%s"%(PATH_P01_S00,LIST_P01_S00[ind])]
for ind in range(len(LIST_P02_S00)): FILE_P02_S00 = FILE_P02_S00 + ["%s/%s"%(PATH_P02_S00,LIST_P02_S00[ind])]
for ind in range(len(LIST_P03_S00)): FILE_P03_S00 = FILE_P03_S00 + ["%s/%s"%(PATH_P03_S00,LIST_P03_S00[ind])]
for ind in range(len(LIST_P04_S00)): FILE_P04_S00 = FILE_P04_S00 + ["%s/%s"%(PATH_P04_S00,LIST_P04_S00[ind])]

trainList = FILE_P01_S00 + FILE_P02_S00 + FILE_P03_S00 + FILE_P00_S00
testList = FILE_P04_S00
random.seed(0)
randList = random.sample(trainList, len(trainList))
trainList = randList

file = open("data/trainList.txt", "w")
file.write( "%d %d\n" % (len(trainList),105) )
for ind in range(0,len(trainList)):
    file.write( "%s\n" % (trainList[ind]) )
file.close()
os.system(".\\bin\\edm2rf.exe data/trainList.txt train")

file = open("data/testList.txt", "w")
file.write( "%d %d\n" % (len(testList),105) )
for ind in range(0,len(testList)):
    file.write( "%s\n" % (testList[ind]) )
file.close()
os.system(".\\bin\\edm2rf.exe data/testList.txt")