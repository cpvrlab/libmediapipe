set -e
shopt -s nullglob

VERSION="v0.8.11"
OPENCV_DIR=""
CONFIG="debug"

while [[ "$#" -gt 0 ]]; do
	case $1 in
		--version)
			VERSION="$2"
			shift
			shift
			;;
		--config)
			CONFIG="$2"
			shift
			shift
			;;
		--opencv_dir)
			OPENCV_DIR="$(cygpath -m $(realpath "$2"))"
			shift
			shift
			;;
	esac
done

if [ "$OPENCV_DIR" = "" ]; then
	echo "ERROR: Missing argument 'opencv_dir'"
	exit 1
fi

if [ "$CONFIG" != "debug" ] && [ "$CONFIG" != "release" ]; then
	echo "ERROR: Argument 'config' must be one of {debug, release}"
	exit 1
fi

echo "--------------------------------"
echo "CONFIGURATION"
echo "--------------------------------"

echo "MediaPipe version: $VERSION"
echo "OpenCV directory: $OPENCV_DIR"
echo "Build configuration: $CONFIG"

echo "--------------------------------"

OUTPUT_DIR="output"
PACKAGE_DIR="$OUTPUT_DIR/libmediapipe-$VERSION-x86_64-windows"
DATA_DIR="$OUTPUT_DIR/data"

echo -n "Checking Clang - "
CLANG_BIN_PATH="$(type -P clang-cl)"
if [ -z "$CLANG_BIN_PATH" ]; then
	echo "ERROR: Clang is not installed"
	echo "Download Clang from: https://github.com/llvm/llvm-project/releases"
	exit 1
fi
export BAZEL_LLVM="$(realpath "$(dirname "$CLANG_BIN_PATH")/../")"
echo "OK (Found at $CLANG_BIN_PATH)"

echo -n "Checking Bazel - "
BAZEL_BIN_PATH="$(type -P bazel)"
if [ -z "$BAZEL_BIN_PATH" ]; then
	echo "ERROR: Bazel is not installed"
	echo "Download Bazel from: https://github.com/bazelbuild/bazel/releases"
	exit 1
fi
echo "OK (Found at $BAZEL_BIN_PATH)"

echo -n "Checking Python - "
PYTHON_BIN_PATH="$(type -P python)"
if [ -z "$PYTHON_BIN_PATH" ]; then
	echo "ERROR: Python is not installed"
	echo "Download Python from: https://www.python.org/downloads/"
	exit 1
fi
echo "OK (Found at $PYTHON_BIN_PATH)"

echo "--------------------------------"
echo "CLONING MEDIAPIPE"
echo "--------------------------------"

if [ ! -d "mediapipe" ]; then
	git clone https://github.com/google/mediapipe.git
else
	echo "Repository already cloned"
fi

cd mediapipe
git checkout "$VERSION"

echo -n "Setting up OpenCV - "

LINE=$(grep -n windows_opencv WORKSPACE | cut -d : -f1)
LINE=$(($LINE + 2))
sed -i "$LINE"'s;C:\\\\opencv\\\\build;'"$OPENCV_DIR;" WORKSPACE

cp ../patches/opencv_windows.BUILD third_party

echo "Done"

echo "--------------------------------"
echo "BUILDING C API"
echo "--------------------------------"

if [ -d "mediapipe/c" ]; then
	echo -n "Removing old C API - "
	rm -r mediapipe/c
	echo "Done"
fi

echo -n "Copying C API - "
cp -r ../c mediapipe/c
echo "Done"

bazel build -c opt \
	--action_env PYTHON_BIN_PATH="$PYTHON_BIN_PATH" \
	--define MEDIAPIPE_DISABLE_GPU=1 \
	--compiler=clang-cl \
	--verbose_failures \
	mediapipe/c:mediapipe

cd ..

if [ -d "$OUTPUT_DIR" ]; then
	echo -n "Removing existing output directory - "
	rm -rf "$OUTPUT_DIR"
	echo "Done"
fi

echo -n "Creating output directory - "
mkdir "$OUTPUT_DIR"
echo "Done"

echo -n "Creating library directories - "
mkdir "$PACKAGE_DIR"
mkdir "$PACKAGE_DIR/include"
mkdir "$PACKAGE_DIR/bin"
mkdir "$PACKAGE_DIR/lib"
echo "Done"

echo -n "Copying libraries - "
cp mediapipe/bazel-bin/mediapipe/c/mediapipe.dll "$PACKAGE_DIR/bin"
cp mediapipe/bazel-bin/mediapipe/c/mediapipe.if.lib "$PACKAGE_DIR/lib/mediapipe.lib"
echo "Done"

echo -n "Copying header - "
cp mediapipe/mediapipe/c/mediapipe.h "$PACKAGE_DIR/include"
echo "Done"

echo -n "Copying data - "

for DIR in mediapipe/bazel-bin/mediapipe/modules/*; do
	MODULE=$(basename "$DIR")
	mkdir -p "$DATA_DIR/mediapipe/modules/$MODULE"

	for FILE in "$DIR"/*.binarypb; do
		cp "$FILE" "$DATA_DIR/mediapipe/modules/$MODULE/$(basename "$FILE")"
	done

	for FILE in "$DIR"/*.tflite; do
		cp "$FILE" "$DATA_DIR/mediapipe/modules/$MODULE/$(basename "$FILE")"
	done
done

cp mediapipe/mediapipe/modules/hand_landmark/handedness.txt "$DATA_DIR/mediapipe/modules/hand_landmark"

echo "Done"
