# demo

# vid2img (data.zip)--> yolov7.bat # detect.py
# unzip data.zip into data/, so that you will see the dirs: "yolov7+openpose/data/IMG_P00~04_S00"

# run the following scripts by yolov7+openpose/demo.py
exec(open("img2edm.py").read()) #1. We compute edm vectors from sampled images.
exec(open("edm2rf.py").read()) #2. We train/test a RF classifier for initial action labels.
exec(open("decide.py").read()) #3. We decide human-object interactions using action & detection labels.
