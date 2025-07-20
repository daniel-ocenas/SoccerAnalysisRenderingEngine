This repository contains the source codes for my bachelor thesis Displaying Virtual Graphics and Animations for Sports Analysis. 
Thesis text: https://www.fit.vut.cz/study/thesis/23140/.en

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
