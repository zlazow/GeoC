import gdal
from gdalconst import *
import os.path
from PIL import Image
import PIL.ImageOps
import numpy as np
import time

def load(filename):
    """
    Opens filename, and returns its dataset.
    """
    script_dir = os.path.dirname(os.path.abspath(__file__))
    dataset = gdal.Open(os.path.join(script_dir, filename))
    print type(dataset)
    return dataset
    
def ndviCalc(red,nir,height,width):
    """
    Calculate NDVI
    NDVI = (NIR-red)/(NIR+red)
    """
    #Creat Holder Arrays
    ndviAdj = np.zeros(shape=(height,width), dtype='float32')
    ndviNum = np.zeros(shape=(height,width), dtype='float32')
    ndviDen = np.zeros(shape=(height,width), dtype='float32')
        
    np.subtract(nir,red,ndviNum)
    np.add(nir,red,ndviDen)
    np.divide(ndviNum,ndviDen, ndviAdj)
    #rescale
    
    np.clip(ndviAdj,0,1)
    np.multiply(ndviAdj,255,ndviAdj)

    
    return ndviAdj
    
def createImage(arr,out):
    """
    Take a numpy array and convert it into an image.
    """
    arr=arr.astype('uint8')
    im = Image.fromarray(arr)
    #im = PIL.ImageOps.invert(im)
    im.save(out, "tiff")
    return im
    
filename = raw_input("What is the file name? ")
start = time.clock()
dataset = load(filename)
band1 = dataset.GetRasterBand(3) #3 is red
band2 = dataset.GetRasterBand(4) #4 is NIR
fileOutName= "Fig_NDVIImage_34_"+filename
    
redArray = band1.ReadAsArray()
redArray = redArray.astype(float)
nirArray = band2.ReadAsArray()
nirArray = nirArray.astype(float)
ndviMatrix = ndviCalc(redArray,nirArray, redArray.shape[0],redArray.shape[1])
stop = time.clock()
print "NDVI Array Time:" + str(stop - start)
        
createImage(ndviMatrix,fileOutName)
#createImageSegment(ndviMatrix,x0,y0,x1,y1fileOutName)
stop = time.clock()
print "Total Time:" + str(stop - start)