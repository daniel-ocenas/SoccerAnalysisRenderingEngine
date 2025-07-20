This repository contains the source codes for my bachelor thesis Displaying Virtual Graphics and Animations for Sports Analysis. 
Thesis text: https://www.fit.vut.cz/study/thesis/23140/.en

The aim of this bachelor thesis is to create graphic tools used for the analysis of video recordings of sports events, especially football matches. The user can use the tools to highlight selected football players, which are automatically tracked and highlighted with three-dimensional objects rendered into video scene. Secondly, user can manually create pointed arrows representing player or football movement. Three-dimensional objects are created by perspective distortion corresponding with the scene taken by camera. The theoretical part of this thesis focuses on the proper rendering of graphical objects into calibrated scene. The practical part focuses on implementation and visualization of mentioned tools by using C++, OpenGL and OpenCV library. 

Author: Daniel Ocenas

On linux build with
```console
sudo apt install libglu1-mesa-dev freeglut3-dev
cmake -S "~/source" -B "C:/BP-xocena06-build" -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
make
```

On Windows with Visual Studio 2019 use CMake tool.
```console
cmake -S "~/source" -B "C:/BP-xocena06-build" -G "Visual Studio 16 2019"
```
Open file /x64/Release/Engine.exe
