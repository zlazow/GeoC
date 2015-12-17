#!/usr/bin/python

#
# Implements 8-connectivity connected component labeling
# 
# Algorithm obtained from "Optimizing Two-Pass Connected-Component Labeling 
# by Kesheng Wu, Ekow Otoo, and Kenji Suzuki
#

import Image, ImageDraw
from skimage.filter import threshold_otsu, threshold_adaptive
from skimage.util import img_as_ubyte
import numpy as np
from numpy import array
import sys
import math, random
from itertools import product
from ufarray import *
from matplotlib import pyplot as plt
import copy
import time

labels={}
uf = UFarray()

def run(img):
    data=img.load()
    width, height = img.size 
    
    # Union find data structure
    #uf = UFarray()
 
    #
    # First pass
    #
 
    # Dictionary of point:label pairs
    #labels = {}
 
    for y, x in product(range(height), range(width)):

        #
        # Pixel names were chosen as shown:
        #
        #   -------------
        #   | a | b | c |
        #   -------------
        #   | d | e |   |
        #   -------------
        #   |   |   |   |
        #   -------------
        #
        # The current pixel is e
        # a, b, c, and d are its neighbors of interest
        #
        # 255 is white, 0 is black
        # White pixels part of the background, so they are ignored
        # If a pixel lies outside the bounds of the image, it default to white
        #
 
        # If the current pixel is white, it's obviously not a component...
        if data[x, y] == 255:
            pass
 
        # If pixel b is in the image and black:
        #    a, d, and c are its neighbors, so they are all part of the same component
        #    Therefore, there is no reason to check their labels
        #    so simply assign b's label to e
        elif y > 0 and data[x, y-1] == 0:
            labels[x, y] = labels[(x, y-1)]
            labels[x,y][1].addPoint()
 
        # If pixel c is in the image and black:
        #    b is its neighbor, but a and d are not
        #    Therefore, we must check a and d's labels
        elif x+1 < width and y > 0 and data[x+1, y-1] == 0:
 
            c = labels[(x+1, y-1)]
            labels[x, y] = c
            labels[x,y][1].addPoint()
            
            # If pixel a is in the image and black:
            #    Then a and c are connected through e
            #    Therefore, we must union their sets
            if x > 0 and data[x-1, y-1] == 0:
                a = labels[(x-1, y-1)]
                uf.union(c[0], a[0])
                labels[x,y][1].union(labels[x-1,y-1][1])                
 
            # If pixel d is in the image and black:
            #    Then d and c are connected through e
            #    Therefore we must union their sets
            elif x > 0 and data[x-1, y] == 0:
                d = labels[(x-1, y)]
                uf.union(c[0], d[0])
                labels[x,y][1].union(labels[x-1,y][1]) 
                  
        # If pixel a is in the image and black:
        #    We already know b and c are white
        #    d is a's neighbor, so they already have the same label
        #    So simply assign a's label to e
        elif x > 0 and y > 0 and data[x-1, y-1] == 0:
            labels[x, y] = labels[(x-1, y-1)]
            labels[x,y][1].addPoint()
        # If pixel d is in the image and black
        #    We already know a, b, and c are white
        #    so simpy assign d's label to e
        elif x > 0 and data[x-1, y] == 0:
            labels[x, y] = labels[(x-1, y)]
            labels[x,y][1].addPoint()
        # All the neighboring pixels are white,
        # Therefore the current pixel is a new component
        else: 
            labels[x, y] = [uf.makeLabel(), Counter()]
 
    
    #
    # Get rid of small regions
    # Doesn't delete every key... That i think is the issue
    

    
    uf.flatten()
    

        
    
    colors = {}

    # Image to display the components in a nice, colorful way
    output_img = Image.new("RGB", (width, height))
    outdata = output_img.load()
    
        
    


    for (coords) in labels.keys():
        #check=False
        # Name of the component the current point belongs to
        component = uf.find(labels[coords][0])
        #size = labels[coords][1].getSize()
        #if size<=25: #and size>=2000
        check=True

        # Update the labels with correct information
        labels[coords][0] = component
 
        # Associate a random color with this component 
        if component not in colors: 
            if check==True:
                colors[component]=(0,0,0)
            else:    
                colors[component] = (random.randint(0,255), random.randint(0,255),random.randint(0,255))
            #colors[component] = (255,0,0)         
        # Colorize the image
        outdata[coords] = colors[component]   


    return (labels, output_img)
 
#def main():
# Open the image
filename=raw_input("Filename: ")
start = time.clock()
img = Image.open(filename)    

# Threshold the image, this implementation is designed to process b+w
# images only

#use 201 for .tif // use 190 for .png copies
img = img.point(lambda p: (p < 144 or p>200) and 255) 
#img.show()
#img.save("BW.png")
#img = img.convert('1')

# labels is a dictionary of the connected component data in the form:
#     (x_coordinate, y_coordinate) : component_id
#
# if you plan on processing the component data, this is probably what you
# will want to use
#
# output_image is just a frivolous way to visualize the components.
(labels, output_img) = run(img)
#output_img = output_img.convert("1")
output_img.show()
filename = "cclabel_"+filename
output_img.save(filename)
stop = time.clock()
print "Total Time:" + str(stop - start)

#if __name__ == "__main__": main()