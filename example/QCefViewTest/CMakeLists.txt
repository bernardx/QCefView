cmake_minimum_required(VERSION 3.4.1)
project(QCefViewTest)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if (OS_WINDOWS)
  find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
elseif (OS_MACOS)
  find_program(DEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
elseif (OS_LINUX)
elseif (OS_MACOS)
else()
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}/include
)

file(GLOB_RECURSE _SRC_FILES
  "*.h"
  "*.cpp"
)

file(GLOB_RECURSE _UI_FILES
  "*.ui"
)
source_group("Form Files" ${_UI_FILES})

configure_file(index.in.html webres/index.html COPYONLY)
configure_file(tutorial.in.html webres/tutorial.html COPYONLY)

file(GLOB_RECURSE _WEB_FILES 
  "*.html"
)
source_group("Webres Files" ${_WEB_FILES})

if (OS_WINDOWS)
  file(GLOB_RECURSE _RES_FILES
    "*.qrc"
    "*.rc"
  )
  source_group("Resource Files" ${_RES_FILES})

  add_executable(${PROJECT_NAME} WIN32 
    ${_SRC_FILES}
    ${_UI_FILES}
    ${_RES_FILES}
    ${_WEB_FILES}
  )

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      QCefView
  )

  target_compile_definitions(${PROJECT_NAME} PRIVATE
    UNICODE
    _UNICODE
  )

  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY 
      VS_STARTUP_PROJECT ${PROJECT_NAME}
  )

  set_target_properties(${PROJECT_NAME} 
    PROPERTIES
      VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
  )

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Embed the manifest file into the target
    COMMAND mt.exe 
      -manifest \"${CMAKE_CURRENT_SOURCE_DIR}\\${PROJECT_NAME}.manifest\" 
      -inputresource:\"$<TARGET_FILE:${PROJECT_NAME}>\" 
      -outputresource:\"$<TARGET_FILE:${PROJECT_NAME}>\"
    
    # Copy the webres directory to output folder
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
      ${CMAKE_CURRENT_BINARY_DIR}/webres 
      $<TARGET_FILE_DIR:${PROJECT_NAME}>/webres
    
    # Deploy the Qt Application
    COMMAND ${DEPLOYQT_EXECUTABLE} 
      --no-svg 
      --no-translations 
      --no-compiler-runtime 
      $<TARGET_FILE:${PROJECT_NAME}>
  )
endif() # OS_WINDOWS

if (OS_LINUX)
  file(GLOB_RECURSE _RES_FILES
    "*.qrc"
  )
  source_group("Resource Files" ${_RES_FILES})

  add_executable(${PROJECT_NAME} 
    ${_SRC_FILES}
    ${_UI_FILES}
    ${_RES_FILES}
    ${_WEB_FILES}
  )

  set_target_properties(${PROJECT_NAME} 
    PROPERTIES 
      INSTALL_RPATH "$ORIGIN"
      BUILD_WITH_INSTALL_RPATH TRUE
  )

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      # On linux platform and debug mode, libcef.so MUST be loaded before libc.so.
      # Gnerally we can achive this by using LD_PRELOAD=libcef.so, but this is not 
      # convenience for debugging. 
      # Thus, here we do a trick to make sure the load order
      "-Wl,--no-as-needed $<TARGET_FILE_DIR:QCefView>/$<TARGET_FILE_NAME:libcef_lib>"

      QCefView
  )

  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # Copy the webres directory to output folder
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
      ${CMAKE_CURRENT_BINARY_DIR}/webres 
      $<TARGET_FILE_DIR:${PROJECT_NAME}>/webres

    # Deploy the Qt Application
    # COMMAND ${DEPLOYQT_EXECUTABLE} 
    # --no-svg 
    # --no-translations 
    # --no-compiler-runtime 
    # $<TARGET_FILE:${PROJECT_NAME}>
  )
endif() # OS_LINUX

if (OS_MACOS)
  file(GLOB_RECURSE _RES_FILES
    "*.qrc"
  )
  source_group("Resource Files" ${_RES_FILES})

  set(QCefViewTest_INFO_PLIST_FILE 
    "${CMAKE_CURRENT_LIST_DIR}/mac/Info.plist"
  )
  
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
    ${_SRC_FILES}
    ${_UI_FILES}
    ${_RES_FILES}
    ${_WEB_FILES}
  )

  set_target_properties(${PROJECT_NAME} 
    PROPERTIES 
      ARCHIVE_OUTPUT_DIRECTORY                  "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/demo"
      LIBRARY_OUTPUT_DIRECTORY                  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/demo"
      RUNTIME_OUTPUT_DIRECTORY                  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/demo"
      CLANG_ENABLE_OBJC_ARC                     "YES"
      MACOSX_BUNDLE_INFO_PLIST                  "${QCefViewTest_INFO_PLIST_FILE}"
      XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS   "@executable_path/../Frameworks"
      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.cefview.qcefviewtest"
      XCODE_LINK_BUILD_PHASE_MODE KNOWN_LOCATION
  )

  add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD

    # remove the old framework from the bundle if exists
    COMMAND rm -fr "$<TARGET_BUNDLE_CONTENT_DIR:${PROJECT_NAME}>/Frameworks/QCefView.framework"

    # copy the latest framework to the bundle
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>/../../QCefView.framework"
        "$<TARGET_BUNDLE_CONTENT_DIR:${PROJECT_NAME}>/Frameworks/QCefView.framework"

    # copy the webres to the bundle
    COMMAND ${CMAKE_COMMAND} -E copy_directory
       "${CMAKE_CURRENT_BINARY_DIR}/webres"
       "$<TARGET_BUNDLE_CONTENT_DIR:${PROJECT_NAME}>/Resources/webres"

    # deploy the Qt Application
    # COMMAND ${DEPLOYQT_EXECUTABLE} 
    #   "$<TARGET_BUNDLE_DIR:${PROJECT_NAME}>"
    #   "-codesign=-"

    VERBATIM
  )

  find_library(COCOA_FRAMEWORK Cocoa)
  find_library(APPKIT_FRAMEWORK Appkit)
  target_link_libraries(${PROJECT_NAME} 
    PRIVATE
      $<TARGET_FILE:QCefView>
      ${COCOA_FRAMEWORK}
      ${APPKIT_FRAMEWORK}
  )
endif()

set_target_properties(${PROJECT_NAME} 
  PROPERTIES
    FOLDER example
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
