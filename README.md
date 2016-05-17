# Panorama

The program requires a config.cfg file with either the filename of an image
or a .hdrgen filename. If a .hdrgen file is used, there must also be a file of
the same name but with ending .txt that contains the response curve of the
camera. An example text file, to see the required format, can be found in the
root directory.

It will write out the resulting panorama (either as png or exr) to the
directory from which the original image was taken. It will also show an
interacitve render of the captured scene, which can be navigated with the arrow
keys, as well as a render of how the objects in the supplied file
"normals.png" (a normal map) would look, if they were in the scene.
