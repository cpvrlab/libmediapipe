# libmediapipe

## About
libmediapipe is a C wrapper around [Google's MediaPipe framework](https://github.com/google/mediapipe/).
This repository contains scripts that clone MediaPipe, copy the libmediapipe source files into the workspace and build it with Bazel.
This produces a shared library that can subsequently be used in CMake/Visual Studio/Xcode/etc. projects without ever touching Bazel again.

## Building

### Linux

#### Requirements
- Python 3: ```apt install python3```
- Bazel 5.2.0: https://bazel.build/install/ubuntu
- Numpy: ```pip3 install numpy```

#### Build and install OpenCV 4.7.0
```
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.7.0.zip
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.7.0.zip
unzip opencv.zip
unzip opencv_contrib.zip
mkdir -p opencv_build && cd opencv_build
cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.7.0/modules ../opencv-4.7.0
cmake --build .
cmake --install .
```

#### Run the build script
```
cd <path to libmediapipe>
./build-x86_64-linux.sh --version v0.8.11 --config release --opencv_dir /usr/local
```

### Windows

#### Requirements
- Python
- Bazel
- Clang
- Visual Studio
- Powershell
- Git with Bash
- Numpy: ```pip install numpy```

#### Install OpenCV 4.7.0
The installer can be downloaded from https://github.com/opencv/opencv/releases/download/4.7.0/opencv-4.7.0-windows.exe

#### Run the build script
```cd <path to libmediapipe>
./build-x86_64-windows.ps1 --version v0.8.11 --config release --opencv_dir <path to opencv build>
```

### MacOS

#### Requirements
- Python: ```brew install python```
- Bazelisk: ```brew install bazelisk```
- Xcode
- Numpy: ```pip install numpy``` or ```pip3 install numpy```

#### Build and install OpenCV 4.7.0
```
curl -L -o opencv.zip https://github.com/opencv/opencv/archive/4.7.0.zip
curl -L -o opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.7.0.zip
unzip opencv.zip
unzip opencv_contrib.zip
mkdir -p opencv_build && cd opencv_build
cmake -DBUILD_ZLIB=false -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.7.0/modules ../opencv-4.7.0
cmake --build . -j6
cmake --install .
```

#### Run the build script

##### Architectures
- ```x86_64```: x86_64/AMD64 processors
- ```aarch64```: AArch64/ARM64/Apple silicon processors

```
cd <path to libmediapipe>
./build-<arch>-macos.sh --version v0.8.11 --config release --opencv_dir /usr/local
```

### Android

#### Requirements
- Python 3: ```apt install python3```
- Bazel 5.2.0: https://bazel.build/install/ubuntu
- Numpy: ```pip3 install numpy```
- Android SDK
- Android NDK 21

#### Environment variables
```
export ANDROID_HOME=<path to android sdk>
export ANDROID_NDK_HOME=<path to android ndk>
```

#### Run the build script
```
cd <path to libmediapipe>
./build-aarch64-android.sh --version v0.8.11 --config release
```

## Usage
The build script generates an ```output``` directory, which contains the build directory (```libmediapipe-<version>-<arch>-<os>```) and the ```data``` directory (protobuf files and TensorFlow models).
To use the library, add ```include``` to your include path and link with ```libmediapipe.so```/```mediapipe.lib```/```libmediapipe.dylib```.
See ```example/example.cpp``` for an example of how to use the library.

## Running the Example

### Linux & MacOS

#### Requirements
- CMake

```
cd <path to libmediapipe>/example
mkdir build && cd build
cmake -DMediaPipe_DIR=<libmediapipe build dir> -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
chmod +x ./example
./example <libmediapipe data dir>
```

### Windows

#### Requirements
- CMake

```
cd <path to libmediapipe>/example
mkdir build; cd build
cmake -DMediaPipe_DIR=<libmediapipe build dir> -DOpenCV_DIR=<opencv build dir> ..
cmake --build . --config Release
cd Release
./example.exe <libmediapipe data dir>
```
