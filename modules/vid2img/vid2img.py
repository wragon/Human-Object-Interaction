import os
import cv2 as cv

path = "videos"
if (not os.path.exists("images")): os.mkdir("images")

for id_person in range(2,5):
    if (not os.path.exists("images/P%02d"%id_person)): os.mkdir("images/P%02d"%id_person)

    dirPath = "%sP%02d_S00"%(path,id_person)
    listFiles = os.listdir(dirPath)

    for id_action in range(0,21):
      fullPath = "%s/P%02d_S00_A%02d.mp4"%(dirPath,id_person,id_action)
      vidcap = cv.VideoCapture(fullPath)
      success,image = vidcap.read()
      count = 0
      while success:
        cv.imwrite("images/P%02d/P%02d_S00_A%02d_F%04d.jpg"%(id_person,id_person,id_action,count), image ) # save frame as JPEG file
        success,image = vidcap.read()
        count += 1
      print("finish! convert %s to frames" % fullPath)
