#!/bin/bash -e

echo "Setup environment"

[ "$FO_ROOT" ] || { [[ -e CMakeLists.txt ]] && { export FO_ROOT=. || true ;} ;} || export FO_ROOT=../
[ "$FO_BUILD_DEST" ] || export FO_BUILD_DEST=Build
[ "$FO_INSTALL_PACKAGES" ] || export FO_INSTALL_PACKAGES=1

export FO_ENV_VERSION="10.02.2020"
export ROOT_FULL_PATH=$(cd $FO_ROOT; pwd)
export EMSCRIPTEN_VERSION="1.39.4"
export ANDROID_NDK_VERSION="android-ndk-r18b"
export ANDROID_SDK_VERSION="tools_r25.2.3"
export ANDROID_NATIVE_API_LEVEL_NUMBER=21
export ANDROID_HOME="$PWD/$FO_BUILD_DEST/android-sdk"

echo "- FO_ENV_VERSION=$FO_ENV_VERSION"
echo "- FO_ROOT=$FO_ROOT"
echo "- FO_BUILD_DEST=$FO_BUILD_DEST"
echo "- FO_INSTALL_PACKAGES=$FO_INSTALL_PACKAGES"
echo "- ROOT_FULL_PATH=$ROOT_FULL_PATH"
echo "- EMSCRIPTEN_VERSION=$EMSCRIPTEN_VERSION"
echo "- ANDROID_NDK_VERSION=$ANDROID_NDK_VERSION"
echo "- ANDROID_SDK_VERSION=$ANDROID_SDK_VERSION"
echo "- ANDROID_NATIVE_API_LEVEL_NUMBER=$ANDROID_NATIVE_API_LEVEL_NUMBER"
echo "- ANDROID_HOME=$ANDROID_HOME"
