cmake_minimum_required (VERSION 3.10)

if(PLATFORM_ANDROID)
    add_subdirectory(Android)
elseif(PLATFORM_LINUX)
    add_subdirectory(Linux)
endif()


project(Diligent-NativeAppBase)

if(PLATFORM_WIN32)

    set(SOURCE 
        src/Win32/WinMain.cpp
    )
    set(INCLUDE
        include/Win32/Win32AppBase.hpp
    )

    function(add_win32_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_executable(${TARGET_NAME} WIN32 ${SOURCE} ${INCLUDE} ${ASSETS})

        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            # libmingw32 must be included BEFORE Diligent-NativeAppBase that contains the definition of WinMain.
            # otherwise WinMain will be stripped out of Diligent-NativeAppBase and will be unresolved.
            target_link_libraries(${TARGET_NAME}
            PRIVATE
                mingw32
            )
        endif()
    endfunction()

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_win32_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

elseif(PLATFORM_UNIVERSAL_WINDOWS)

    set(SOURCE 
        src/UWP/dummy.cpp
    )
    set(INCLUDE
        include/UWP/UWPAppBase.hpp
    )

    # Windows Runtime types cannot be included into static libraries
    # https://social.msdn.microsoft.com/Forums/en-US/269db513-64ef-4817-a025-43954f614eb3/lnk4264-why-are-static-libraries-not-recommended-when-authoring-windows-runtime-types?forum=winappswithnativecode
    # So as a workaround, we will include all source files into the target app project
    function(add_uwp_app TARGET_NAME SOURCE INCLUDE ASSETS)
        get_target_property(NATIVE_APP_SOURCE_DIR Diligent-NativeAppBase SOURCE_DIR)

        set(UWP_SOURCE
            ${NATIVE_APP_SOURCE_DIR}/src/UWP/Common/DeviceResources.cpp
            ${NATIVE_APP_SOURCE_DIR}/src/UWP/App.cpp
            ${NATIVE_APP_SOURCE_DIR}/src/UWP/UWPAppBase.cpp
        )

        set(UWP_INCLUDE
            ${NATIVE_APP_SOURCE_DIR}/src/UWP/Common/DeviceResources.h
            ${NATIVE_APP_SOURCE_DIR}/src/UWP/Common/DirectXHelper.h
            ${NATIVE_APP_SOURCE_DIR}/src/UWP/App.h
            ${NATIVE_APP_SOURCE_DIR}/include/UWP/UWPAppBase.hpp
            ${NATIVE_APP_SOURCE_DIR}/src/UWP/Common/StepTimer.h
        )

        add_executable(${TARGET_NAME} WIN32 ${SOURCE} ${INCLUDE} ${ASSETS} ${UWP_SOURCE} ${UWP_INCLUDE})
        set_source_files_properties(${ASSETS} PROPERTIES VS_DEPLOYMENT_CONTENT 1)

        target_include_directories(${TARGET_NAME} 
        PUBLIC
            ${NATIVE_APP_SOURCE_DIR}/Src/UWP
        )
    
        target_link_libraries(${TARGET_NAME} PRIVATE dxgi.lib)

        source_group("UWP Common\\src" FILES ${UWP_SOURCE})
        source_group("UWP Common\\include" FILES ${UWP_INCLUDE})

    endfunction(add_uwp_app)

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_uwp_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

elseif(PLATFORM_ANDROID)

    set(SOURCE 
        src/Android/AndroidAppBase.cpp
    )
    set(INCLUDE
        include/Android/AndroidAppBase.hpp
    )
    function(add_android_app TARGET_NAME SOURCE INCLUDE ASSETS)
        get_target_property(NATIVE_APP_SOURCE_DIR Diligent-NativeAppBase SOURCE_DIR)
        set(ANDROID_SOURCE
            ${NATIVE_APP_SOURCE_DIR}/src/Android/AndroidMain.cpp
        )
        add_library(${TARGET_NAME} SHARED ${SOURCE} ${ANDROID_SOURCE} ${INCLUDE} ${ASSETS})
        target_link_libraries(${TARGET_NAME} 
        PRIVATE 
            android
            native_app_glue
        )
        # Export ANativeActivity_onCreate(),
        # Refer to: https://github.com/android-ndk/ndk/issues/381.
        set_target_properties(${TARGET_NAME}
        PROPERTIES
          LINK_FLAGS "-u ANativeActivity_onCreate"
        )
        #target_include_directories(${TARGET_NAME} 
        #PRIVATE
        #	${ANDROID_NDK}/sources/android/cpufeatures
        #)
        source_group("Android" FILES ${ANDROID_SOURCE})
    endfunction()

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_android_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

elseif(PLATFORM_LINUX)

    set(SOURCE 
        src/Linux/LinuxMain.cpp
    )
    set(INCLUDE
        include/Linux/LinuxAppBase.hpp
    )
    function(add_linux_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_executable(${TARGET_NAME} ${SOURCE} ${INCLUDE} ${ASSETS})
    endfunction()

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_linux_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

elseif(PLATFORM_MACOS)

    set(SOURCE
        src/MacOS/MacOSAppBase.cpp
    )
    set(INCLUDE
        include/MacOS/MacOSAppBase.hpp
    )

    function(add_macos_app TARGET_NAME SOURCE INCLUDE ASSETS)
        get_target_property(NATIVE_APP_SOURCE_DIR Diligent-NativeAppBase SOURCE_DIR)

        set(APPLE_SOURCE
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/main.m
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/WindowController.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/AppDelegate.m
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/FullscreenWindow.m
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/GLView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/MetalView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/MVKView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/ViewBase.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/ViewController.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/ModeSelectionViewController.mm
        )

        set(APPLE_INCLUDE
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/WindowController.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/AppDelegate.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/FullscreenWindow.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/GLView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/MetalView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/MVKView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/ViewBase.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/ViewController.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX/ModeSelectionViewController.h
        )

        set(APPLE_RESOURCES
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/OSX/Base.lproj/Main.storyboard
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/OSX/Images.xcassets/AppIcon.appiconset/dg.icns
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/OSX/Images.xcassets/opengl-logo.png
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/OSX/Images.xcassets/vulkan-logo.png
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/OSX/Images.xcassets/metal-logo.png
        )

        set(APPLE_INFO_PLIST
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/OSX/Info.plist
        )

        set(APPLE_INCLUDE_DIRS
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/OSX
        )

        add_executable(${TARGET_NAME} MACOSX_BUNDLE ${SOURCE} ${APPLE_SOURCE} ${INCLUDE} ${APPLE_INCLUDE} ${ASSETS} ${APPLE_RESOURCES})
        string(REPLACE "_" "-" BUNDLE_NAME ${TARGET_NAME})
        set_target_properties(${TARGET_NAME} PROPERTIES
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.diligentengine.samples.${BUNDLE_NAME}"
            MACOSX_BUNDLE_INFO_PLIST "${APPLE_INFO_PLIST}"
            RESOURCE "${APPLE_RESOURCES}"
        )
        source_group("MacOS" FILES ${APPLE_SOURCE})
        source_group("MacOS" FILES ${APPLE_INCLUDE})
        source_group("Resources" FILES ${APPLE_RESOURCES})
        target_include_directories(${TARGET_NAME} PRIVATE ${APPLE_INCLUDE_DIRS})

        find_package(OpenGL REQUIRED)

        find_library(CORE_VIDEO CoreVideo)
        if (NOT CORE_VIDEO)
                message(FATAL_ERROR "CoreVideo is not found")
        endif()

        find_library(METAL_FRAMEWORK Metal)
        if (NOT METAL_FRAMEWORK)
            message(FATAL_ERROR "Metal framework is not found")
        endif()

        find_library(CORE_ANIMATION QuartzCore)
        if (NOT CORE_ANIMATION)
            message(FATAL_ERROR "QuartzCore (CoreAnimation) is not found")
        endif()

        target_link_libraries(${TARGET_NAME} PRIVATE ${OPENGL_LIBRARY} ${CORE_VIDEO} ${METAL_FRAMEWORK} ${CORE_ANIMATION})
        # Silence OpenGL deprecation warnings
        target_compile_definitions(${TARGET_NAME} PUBLIC GL_SILENCE_DEPRECATION)

    endfunction()

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_macos_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

elseif(PLATFORM_IOS)

    set(SOURCE
        src/IOS/IOSAppBase.cpp
    )
    set(INCLUDE
        include/IOS/IOSAppBase.hpp
    )

    function(add_ios_app TARGET_NAME SOURCE INCLUDE ASSETS)
        get_target_property(NATIVE_APP_SOURCE_DIR Diligent-NativeAppBase SOURCE_DIR)

        set(APPLE_SOURCE
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/main.m
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/AppDelegate.m
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/BaseView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/EAGLView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/AppViewBase.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/ModeSelectionViewController.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/MetalView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/MVKView.mm
        )

        set(APPLE_INCLUDE
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/AppDelegate.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/BaseView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/EAGLView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/AppViewBase.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/ModeSelectionViewController.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/MetalView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS/MVKView.h
        )

        set(APPLE_RESOURCES
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/iOS/Base.lproj/Main.storyboard
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/iOS/Base.lproj/LaunchScreen.xib
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/iOS/Images.xcassets/AppIcon.appiconset/dg-icon.png
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/iOS/Images.xcassets/opengles-logo.png
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/iOS/Images.xcassets/vulkan-logo.png
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/iOS/Images.xcassets/metal-logo.png
        )

        set(APPLE_INFO_PLIST
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/iOS/info.plist
        )

        set(APPLE_INCLUDE_DIRS
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/iOS
        )

        add_executable(${TARGET_NAME} MACOSX_BUNDLE ${SOURCE} ${APPLE_SOURCE} ${INCLUDE} ${APPLE_INCLUDE} ${ASSETS} ${APPLE_RESOURCES})
        string(REPLACE "_" "-" BUNDLE_NAME ${TARGET_NAME})
        set_target_properties(${TARGET_NAME} PROPERTIES
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.diligentengine.samples.${BUNDLE_NAME}"
            MACOSX_BUNDLE_INFO_PLIST "${APPLE_INFO_PLIST}"
            RESOURCE "${APPLE_RESOURCES}"
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
            # XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "Dev Team"
            BUILD_RPATH "@executable_path"
        )
        source_group("iOS" FILES ${APPLE_SOURCE})
        source_group("iOS" FILES ${APPLE_INCLUDE})
        source_group("Resources" FILES ${APPLE_RESOURCES})
        target_include_directories(${TARGET_NAME} PRIVATE ${APPLE_INCLUDE_DIRS})

        find_library(OPENGLES OpenGLES)
        if (NOT OPENGLES)
            message(FATAL_ERROR "OpenGLES is not found")
        endif()

        find_library(UIKIT UIKit)
        if (NOT UIKIT)
            message(FATAL_ERROR "UIKIT is not found")
        endif()

        find_library(CORE_ANIMATION QuartzCore)
        if (NOT CORE_ANIMATION)
            message(FATAL_ERROR "QuartzCore (CoreAnimation) is not found")
        endif()

        target_link_libraries(${TARGET_NAME} PRIVATE ${OPENGLES} ${UIKIT} ${CORE_ANIMATION})
        # Silence OpenGLES deprecation warnings
        target_compile_definitions(${TARGET_NAME} PUBLIC GLES_SILENCE_DEPRECATION)

    endfunction()

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_ios_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

elseif(PLATFORM_TVOS)

    set(SOURCE
        src/TVOS/TVOSAppBase.cpp
    )
    set(INCLUDE
        include/TVOS/TVOSAppBase.hpp
    )

    function(add_tvos_app TARGET_NAME SOURCE INCLUDE ASSETS)
        get_target_property(NATIVE_APP_SOURCE_DIR Diligent-NativeAppBase SOURCE_DIR)

        set(APPLE_SOURCE
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/main.m
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/AppDelegate.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/MainUIView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/MetalView.mm
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/MetalViewController.mm
        )

        set(APPLE_INCLUDE
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/AppDelegate.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/MainUIView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/MetalView.h
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS/MetalViewController.h
        )

        set(APPLE_RESOURCES
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/tvOS/Base.lproj/Main.storyboard
        )

        set(APPLE_INFO_PLIST
            ${NATIVE_APP_SOURCE_DIR}/Apple/Data/tvOS/info.plist
        )

        set(APPLE_INCLUDE_DIRS
            ${NATIVE_APP_SOURCE_DIR}/Apple/Source/Classes/tvOS
        )

        add_executable(${TARGET_NAME} MACOSX_BUNDLE ${SOURCE} ${APPLE_SOURCE} ${INCLUDE} ${APPLE_INCLUDE} ${ASSETS} ${APPLE_RESOURCES})
        string(REPLACE "_" "-" BUNDLE_NAME ${TARGET_NAME})
        set_target_properties(${TARGET_NAME} PROPERTIES
            XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.diligentengine.samples.${BUNDLE_NAME}"
            MACOSX_BUNDLE_INFO_PLIST "${APPLE_INFO_PLIST}"
            RESOURCE "${APPLE_RESOURCES}"
            XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
            # XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "Dev Team"
            BUILD_RPATH "@executable_path"
        )
        source_group("tvOS" FILES ${APPLE_SOURCE})
        source_group("tvOS" FILES ${APPLE_INCLUDE})
        source_group("Resources" FILES ${APPLE_RESOURCES})
        target_include_directories(${TARGET_NAME} PRIVATE ${APPLE_INCLUDE_DIRS})

        find_library(UIKIT UIKit)
        if (NOT UIKIT)
            message(FATAL_ERROR "UIKIT is not found")
        endif()

        find_library(CORE_ANIMATION QuartzCore)
        if (NOT CORE_ANIMATION)
            message(FATAL_ERROR "QuartzCore (CoreAnimation) is not found")
        endif()

        target_link_libraries(${TARGET_NAME} PRIVATE ${UIKIT} ${CORE_ANIMATION})
        # Silence OpenGLES deprecation warnings (we don't use GLES, but still XCode produces errors)
        target_compile_definitions(${TARGET_NAME} PUBLIC GLES_SILENCE_DEPRECATION)

    endfunction()

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_tvos_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

elseif(PLATFORM_EMSCRIPTEN)
    set(SOURCE 
        src/Emscripten/EmscriptenAppBase.cpp
        src/Emscripten/EmscriptenMain.cpp
    )
    set(INCLUDE
        include/Emscripten/EmscriptenAppBase.hpp
    )

    function(add_emscripten_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_executable(${TARGET_NAME} ${SOURCE} ${INCLUDE} ${ASSETS})   
         
        # The initial amount of memory to use. Using more memory than this will cause us to expand the heap,
        # which can be costly with typed arrays. If ALLOW_MEMORY_GROWTH is set, this initial amount of memory can
        # increase later; if not, then it is the final and total amount of memory.
        #target_link_options(${TARGET_NAME} PRIVATE "SHELL: -s INITIAL_MEMORY=32MB")

        # If false, the app is aborted with an error if it tries to allocate more memory than INITIAL_MEMORY.
        # If true, the memory arrays will grow at runtime, seamlessly and dynamically.
        # Related: MEMORY_GROWTH_GEOMETRIC_STEP, MEMORY_GROWTH_GEOMETRIC_CAP, MEMORY_GROWTH_LINEAR_STEP
        target_link_options(${TARGET_NAME} PRIVATE "SHELL: -s ALLOW_MEMORY_GROWTH=1")
        
        # Default stack size is only 64 Kb
        # https://emscripten.org/docs/tools_reference/settings_reference.html?highlight=environment#default-pthread-stack-size
        target_link_options(${TARGET_NAME} PRIVATE "SHELL: -s STACK_SIZE=5MB")

        # The default implementation of malloc/free (dlmalloc) uses a single global lock,
        # which creates contention in multithreaded applications.
        # Use mimalloc that scales a lot better.
        target_link_options(${TARGET_NAME} PRIVATE "SHELL: -s MALLOC=mimalloc")
    endfunction()

    function(add_target_platform_app TARGET_NAME SOURCE INCLUDE ASSETS)
        add_emscripten_app("${TARGET_NAME}" "${SOURCE}" "${INCLUDE}" "${ASSETS}")
    endfunction()

else()
    message(FATAL_ERROR "Unknown platform")
endif()

list(APPEND INCLUDE 
    include/AppBase.hpp
    include/NativeAppBase.hpp
    include/CommandLineParser.hpp
)

add_library(Diligent-NativeAppBase STATIC ${SOURCE} ${INCLUDE})
set_common_target_properties(Diligent-NativeAppBase)

if(MSVC)
    target_compile_options(Diligent-NativeAppBase PRIVATE -DUNICODE)

    if(PLATFORM_UNIVERSAL_WINDOWS)
        # Disable w4189: local variable is initialized but not referenced
        # Disable w4063: case is not a valid value for switch of enum
        # Consume the windows runtime extensions (/ZW)
        target_compile_options(Diligent-NativeAppBase INTERFACE /wd4189 /wd4063)
    endif()
endif()

target_include_directories(Diligent-NativeAppBase 
PUBLIC
    include
)

target_link_libraries(Diligent-NativeAppBase 
PRIVATE 
    Diligent-BuildSettings
    Diligent-Common
)

if(PLATFORM_WIN32)

    target_include_directories(Diligent-NativeAppBase
    PUBLIC 
        include/Win32
    )

elseif(PLATFORM_UNIVERSAL_WINDOWS)

    target_include_directories(Diligent-NativeAppBase 
    PUBLIC
        include/UWP 
        src/UWP
    )

elseif(PLATFORM_ANDROID)
    target_link_libraries(Diligent-NativeAppBase PUBLIC NDKHelper native_app_glue PRIVATE android)
    target_include_directories(Diligent-NativeAppBase
    PUBLIC
        include/Android
    )
elseif(PLATFORM_LINUX)
    find_package(X11 REQUIRED)
    target_link_libraries(Diligent-NativeAppBase 
    PRIVATE 
        X11::X11
    )
    if(GL_SUPPORTED)
        find_package(OpenGL REQUIRED)
        target_link_libraries(Diligent-NativeAppBase
        PRIVATE
            OpenGL::GL
            OpenGL::GLX
        )
    endif()
    target_include_directories(Diligent-NativeAppBase 
    PUBLIC 
        include/Linux
    )
    if(VULKAN_SUPPORTED)
        find_library(XCB_LIBRARY xcb)
        target_link_libraries(Diligent-NativeAppBase
        PRIVATE 
            ${XCB_LIBRARY}
        )
    endif()
elseif(PLATFORM_MACOS)
    target_include_directories(Diligent-NativeAppBase PUBLIC
        src/MacOS
        include/MacOS
    )
elseif(PLATFORM_IOS)
    target_include_directories(Diligent-NativeAppBase PUBLIC
        include/IOS
    )
elseif(PLATFORM_TVOS)
    target_include_directories(Diligent-NativeAppBase PUBLIC
        include/TVOS
    )
elseif(PLATFORM_EMSCRIPTEN)
    target_include_directories(Diligent-NativeAppBase PUBLIC
        include/Emscripten
    )
endif()

source_group("src" FILES ${SOURCE})
source_group("include" FILES ${INCLUDE})

set_target_properties(Diligent-NativeAppBase PROPERTIES
    FOLDER DiligentTools
)
