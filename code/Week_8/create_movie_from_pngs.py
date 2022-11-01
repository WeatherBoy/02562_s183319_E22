# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# Create movies from a lot of pngs using openCV
# >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
# Created by Felix Bo Caspersen, s183319 on Thu Oct 27 2022

import cv2
import os

print(os.getcwd())
image_folder = "../../Images_for_handIn/CornelSpheres_movie"
image_name = "cornellrightsphere_"
video_name = "CornellSpheres_rendered.avi"

FPS = 30
NUM_DIVISIONS = 4
N = len([img for img in os.listdir(image_folder) if img.endswith(".png")])
images = []

for j in range(NUM_DIVISIONS):
    # This in particular is weird code...
    #
    # Because there is a lot which happens in the beginning, then
    # we want there to be more frames early on and less frames later
    # on. So we take larger steps, meaning fewer pictures included,
    # towards the end of the "list of images".
    images += [image_name + str(i) + ".png" for i in range((N//NUM_DIVISIONS)*j, (N//NUM_DIVISIONS)*(j+1), (j+1)**2)]
print(images)

frame = cv2.imread(os.path.join(image_folder, images[0]))
height, width, layers = frame.shape

video = cv2.VideoWriter(video_name, 0, FPS, (width,height))

for image in images:
    video.write(cv2.imread(os.path.join(image_folder, image)))

cv2.destroyAllWindows()
video.release()