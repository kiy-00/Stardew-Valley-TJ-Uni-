#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.1)

set(APP_NAME TemplateCpp)


# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 指定使用GCC 13编译器
set(CMAKE_C_COMPILER /usr/bin/gcc-13)
set(CMAKE_CXX_COMPILER /usr/bin/g++-13)

project(${APP_NAME})

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)
if(NOT USE_COCOS_PREBUILT)
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)
endif()

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

include_directories(
        Classes
        Classes/map
        Classes/map/core
        Classes/map/utils
        Classes/map/events
        Classes/map/builder
        Classes/map/state
        ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
)
# Scene module
set(SCENE_SOURCE
    Classes/scenes/BaseScene.cpp
    Classes/scenes/GameScene.cpp
    Classes/HelloWorldScene.cpp
)
set(SCENE_HEADER
    Classes/scenes/BaseScene.h
    Classes/scenes/GameScene.h
    Classes/HelloWorldScene.h
)

# Map modules
set(MAP_CORE_SOURCE
    Classes/map/core/MapManager.cpp
    Classes/map/core/MapLayer.cpp
    Classes/map/core/GameMap.cpp
    Classes/map/core/Tile.cpp
)
set(MAP_CORE_HEADER
    Classes/map/core/MapManager.h
    Classes/map/core/MapLayer.h
    Classes/map/core/GameMap.h
    Classes/map/core/Tile.h
)

set(MAP_UTILS_SOURCE
    Classes/map/utils/MapUtils.cpp
)
set(MAP_UTILS_HEADER
    Classes/map/utils/MapUtils.h
    Classes/map/utils/MapConstants.h
)

set(MAP_PROPERTIES_SOURCE
    Classes/map/properties/MapProperties.cpp
    Classes/map/properties/TileProperties.cpp
)
set(MAP_PROPERTIES_HEADER
    Classes/map/properties/MapProperties.h
    Classes/map/properties/TileProperties.h
)

set(MAP_OTHER_SOURCE
    Classes/map/builder/MapBuilder.cpp
    Classes/map/state/TileState.cpp
    Classes/map/events/MapEventManager.cpp
)
set(MAP_OTHER_HEADER
    Classes/map/builder/MapBuilder.h
    Classes/map/state/TileState.h
    Classes/map/events/MapEventManager.h
)

# Core module
set(CORE_SOURCE
    Classes/AppDelegate.cpp
)
set(CORE_HEADER
    Classes/AppDelegate.h
)

# Append all modules to main game sources
list(APPEND GAME_SOURCE
    ${CORE_SOURCE}
    ${SCENE_SOURCE}
    ${MAP_CORE_SOURCE}
    ${MAP_UTILS_SOURCE}
    ${MAP_PROPERTIES_SOURCE}
    ${MAP_OTHER_SOURCE}
)

list(APPEND GAME_HEADER
    ${CORE_HEADER}
    ${SCENE_HEADER}
    ${MAP_CORE_HEADER}
    ${MAP_UTILS_HEADER}
    ${MAP_PROPERTIES_HEADER}
    ${MAP_OTHER_HEADER}
)

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(APP_SRC
    ${GAME_SOURCE}
    ${GAME_HEADER}
    )
cocos_build_app(${APP_NAME}
                APP_SRC "${APP_SRC}"
                DEPEND_COMMON_LIBS "cocos2d"
                DEPEND_ANDROID_LIBS "cocos2d_android"
                )

if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist"
                              )
    elseif(IOS)
        cocos_pak_xcode(${APP_NAME} INFO_PLIST "iOSBundleInfo.plist.in")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME} COPY_TO ${APP_BIN_DIR})
endif()

if(LINUX OR WINDOWS)
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
