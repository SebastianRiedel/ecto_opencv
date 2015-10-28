#!/usr/bin/env python
import ecto
from ecto_opencv.highgui import imshow
from ecto_opencv.highgui import ImageReader
import time
import argparse

parser = argparse.ArgumentParser(description='ListReader: Displays the given images.')
parser.add_argument('filenames', type=str, nargs='+', help='Filename(s) of the images to display.')
parser.add_argument('--wait', type=int, default=0, help='Number of milliseconds each image is displayed. A value of 0 means it is displayed indefinitely (close window to display next image).')

args = parser.parse_args()


image_list = args.filenames
images = ImageReader(file_list=ecto.list_of_strings(image_list))

#this is similar to a slide show... Wait for half a second
imshow = imshow(name='image', waitKey=args.wait)

plasm = ecto.Plasm()
plasm.connect(images['image'] >> imshow['image'])

ecto.view_plasm(plasm)
plasm.execute(niter=0)
# delay necessary, otherwise we get seg faults during deconstruction of everything
time.sleep(0.5)