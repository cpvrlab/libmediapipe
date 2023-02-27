# libmediapipe

## About
libmediapipe is a C wrapper around [Google's Mediapipe framework](https://github.com/google/mediapipe/).
This repository contains scripts that clone MediaPipe, copy the libmediapipe source files into the Bazel workspace and build it.
This produces a shared library that can subsequently be used in CMake/Visual Studio/XCode/etc. projects without ever touching Bazel again.

## Building

### Linux
1. Install the following dependencies:
    - Python 3:
        ```
        apt install python3
        ```
    - Bazel 5.2.0: https://bazel.build/install/ubuntu
    - Clang:
        ```
        apt install clang
        ```
    - Numpy:
        ```
        pip3 install numpy
        ```
2. Build and install OpenCV 4.5.5:
    ```
    wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.5.zip
    wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.5.zip
    unzip opencv.zip
    unzip opencv_contrib.zip
    mkdir -p opencv_build && cd opencv_build
    cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.5.5/modules ../opencv-4.5.5
    cmake --build .
    cmake --install .
    ```
3. Run the build script:
    ```
    cd <path to libmediapipe>
    ./build-x86_64-linux.sh --version v0.8.11 --config release --opencv_dir /usr/local
    ```

### Windows
1. Install the following dependencies:
    - Python
    - Bazel
    - Clang
    - Powershell
    - Git with Bash
    - Numpy
2. Install OpenCV 4.5.5: https://sourceforge.net/projects/opencvlibrary/files/4.5.5/opencv-4.5.5-vc14_vc15.exe/download
3. Run the build script:
    ```cd <path to libmediapipe>
    ./build-x86_64-windows.ps1 --version v0.8.11 --config release --opencv_dir <path to opencv build>
    ```

### MacOS
1. Install the following tools and libraries:
    - Python:
        ```
        brew install python
        ```
    - Bazelisk:
        ```
        brew install bazelisk
        ```
    - Xcode
    - Numpy:
        ```
        pip install numpy
        ```
2. Build and install OpenCV 4.5.5:
    ```
    curl -L -o opencv.zip https://github.com/opencv/opencv/archive/4.5.5.zip
    curl -L -o opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.5.zip
    unzip opencv.zip
    unzip opencv_contrib.zip
    mkdir -p opencv_build && cd opencv_build
    cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.5.5/modules ../opencv-4.5.5
    cmake --build .
    cmake --install .
    ```
3. Run the build script:
    - On x86_64/AMD64 processors, ```arch``` is ```x86_64```
    - On AArch64/ARM64/Apple silicon processors, ```arch``` is ```aarch64```
    ```
    cd <path to libmediapipe>
    ./build-<arch>-macos.sh --version v0.8.11 --config release --opencv_dir /usr/local
    ```

## Usage
The build scripts generate a ```output``` directory, which contains the build directory (```libmediapipe-<version>-<arch>-<os>```) and the ```data``` directory (protobuf files and TensorFlow models).
To use the library, add ```include``` to your include path and link with ```libmediapipe.so```/```mediapipe.lib```/```libmediapipe.dylib```.
See ```example/example.cpp``` for an example of how to use the library.

## Building the Example

## Linux & MacOS
1. Build libmediapipe
2. ```cd <path to libmediapipe>/example```
3. ```mkdir build && cd build```
4. ```cmake -DMediaPipe_DIR=<libmediapipe build dir> -DCMAKE_BUILD_TYPE=Release ..```
5. ```cmake --build .```
6. ```chmod +x ./example```
7. ```./example <libmediapipe data dir>```

## Windows
1. Build libmediapipe
2. ```cd <path to libmediapipe>/example```
3. ```mkdir build; cd build```
4. ```cmake -DMediaPipe_DIR=<libmediapipe build dir> -DOpenCV_DIR=<opencv build dir> ..```
5. ```cmake --build . --config Release```
6. ```cd Release```
7. ```./example.exe <libmediapipe data dir>```