import os
import cv2 as cv

for pind in range(0,5):
    sind = 0

    imgPath = "data/IMG_P%02d_S%02d"%(pind,sind)
    edmPath = "data/EDM_P%02d_S%02d"%(pind,sind)
    if (not os.path.exists(edmPath)): os.mkdir(edmPath)

    imgFiles = os.listdir(imgPath)
    file = open("data/imgList.txt", "w")
    for find in range(0,len(imgFiles)):
        lab = int( imgFiles[find][9:11] )
        if   lab==15: lab=6
        elif lab==16: lab=3
        elif lab==17: lab=11
        elif lab==18: lab=6
        elif lab==19: lab=11
        elif lab==21: lab=4   # right hand
        elif lab==22: lab=12
        elif lab==24: lab=12
        file.write( "%s %s %s %d\n" % (imgPath,edmPath,imgFiles[find],lab) )
    file.close()
    os.system(".\\bin\\img2edm.exe data/imgList.txt")