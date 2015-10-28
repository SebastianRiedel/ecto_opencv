#!/usr/bin/env python
import ecto
from ecto_opencv.highgui import imshow
from ecto_opencv.opencv_test import ImageGen, ImageCmp, ImageDelay

plasm = ecto.Plasm()

d1,d2,d3 = ImageDelay(),ImageDelay(),ImageDelay()
img_gen = ImageGen()[:]
img_cmp = ImageCmp()
plasm.connect(
              img_gen >> d1[:],
              d1[:] >> d2[:],
              d2[:] >> d3[:],
              d3[:] >> img_cmp['image1'],
              img_gen >> img_cmp['image2'],
              )

ecto.view_plasm(plasm)
plasm.execute(niter=10)
