from PIL import Image
import numpy as np
import matplotlib

"""
This is Module Py2C for c++
"""

class A: pass

def ShowImage(image, width, height):
    img = [[]] * height
    img_img = [[]] * height
    if type(image[0]) is type(1+2j):
        for x in range(height):
            for y in range(width):
                img[x] = img[x] + [image[x * width + y].real]
        for x in range(height):
            for y in range(width):
                img_img[x] = img_img[x] + [image[x * width + y].imag]
    else:
        for x in range(height):
            for y in range(width):
                img[x] = img[x] + [image[x * width + y]]
    if (len(img_img[0]) != 0):
        npimg = np.array(img)
        npimg = npimg / npimg.max() *255
        pil_image = Image.fromarray(npimg)
        pil_image.show()
    npimg = np.array(img)
    npimg = npimg / npimg.max() *255
    pil_image = Image.fromarray(npimg)
    pil_image.show()
    width = len(img)
    height = len(img[0])
    print('Invoking Method: [ShowImage]')
    return [image,width,height]

def ShowComplexImage(image, width, height):
        img = [[]] * width
        if type(image[0]) is type(1+2j):
                for x in range(width):
                        for y in range(height):
                                img[x] = img[x] + [ image[x * width + y].real ]
        print(img)
        width = len(img)
        height = len(img[0])
        return [image, width, height]

def ShowCharImage(image, width, height):
        print(image[0],image[width*height-1])
        return [image, width, height]


## image is 2d list
def ShowImage2d(image, width, height):
        print('Invoking Method: [ShowImage2d].')
        print(len(image))
        print(len(image[0]))
        pil_image = Image.fromarray(np.array(image))
        pil_image2 = Image.fromarray(np.array(image)*2)
        pil_image.show()
        pil_image2.show()
        # image = image.tolist()
        return list([image, width, height])


def ShowImage3d(image,width,height,slice):
        print('Invoking Method: [ShowImage3d].')
        print(len(image))
        print(len(image[0]))
        print(len(image[0][0]))
        return [image, width, height, slice]


def ShowImage4d(image,width,height,slice,time):
	print('Invoking Method: [ShowImage4d].')
	print( len(image) )
	print( len(image[0]) )
	print( len(image[0][0]) )
	print( len(image[0][0][0]) )
	return [image, width, height, slice, time]


# if __name__=='__main__':
#         print('############################################################')
#         doNothing()
#         width = 256
#         height = 256
#         li = [i for i in range(width*height)]
#         image = ShowImage(li, width, height)
#         #li2d = [[i for j in range(height)] for i in range(width)] # *width+j)*255/(width*height)
#         #image2d = ShowImage2D(li2d,width, height)
#         p = Py4C()
#         p.ShowImage3D(image, width, height, 1)
#         print('############################################################')
