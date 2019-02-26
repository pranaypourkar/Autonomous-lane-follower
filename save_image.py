import cv2
import time
print(cv2.__version__)
vidcap = cv2.VideoCapture(0);
success,image = vidcap.read()
count = 0
success = True
while success:
  cv2.imwrite("frame%d.png" % count, image)
  time.sleep(1)
  success,image = vidcap.read()
  print 'Read a new frame: ', success
  count += 1