#-------------------------------------------------
#
# Project created by QtCreator 2014-01-28T09:00:01
#
#-------------------------------------------------

QT       += core gui xml opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#DEFINES += GLEW_STATIC
QMAKE_CXXFLAGS += -std=c++11 -fopenmp
QMAKE_LFLAGS += -fopenmp

win32:LIBS += \
    -lopengl32 \
    -lglu32 \
    -L../External/glew/lib/win32 \
    -lglew32 \
    -lz

unix:LIBS += \
    -lGL \
    -lGLU \
    -lGLEW \
    -lpng \
    -lz


include(../Athena/AthenaCore.pri)

TARGET    = RetroView
CONFIG   += console
CONFIG   += app_bundle

TEMPLATE = app

#DEFINES += ATHENA_NO_SAVES ATHENA_NO_ZQUEST

unix {
    CONFIG += link_pkgconfig
}

INCLUDEPATH += include
win32:INCLUDEPATH += ../External/glm ../External/glew/include

SOURCES += \
    src/GXCommon.cpp \
    src/RetroCommon.cpp \
    src/MREADecompress.cpp \
    src/CCamera.cpp \
    src/CTEVStage.cpp \
    src/CGLViewer.cpp \
    src/CMaterialReader.cpp \
    src/CMaterialCache.cpp \
    src/CMaterialSet.cpp \
    src/CMaterial.cpp \
    src/CMesh.cpp \
    src/CMainWindow.cpp \
    src/CModelData.cpp \
    src/CModelLoader.cpp \
    src/CModelLoaderThead.cpp \
    src/CResourceManager.cpp \
    src/CScene.cpp \
    src/CModelFile.cpp \
    src/CModelReader.cpp \
    src/CAreaFile.cpp \
    src/CAreaReader.cpp

!contains(DEFINES, NO_MAIN): {
    SOURCES += src/main.cpp
}

HEADERS += \
    include/GXCommon.hpp \
    include/IRenderableModel.hpp \
    include/GXCommon.hpp \
    include/GXTypes.hpp \
    include/RetroCommon.hpp \
    include/MREADecompress.hpp \
    include/CCamera.hpp \
    include/CScene.hpp \
    include/CTEVStage.hpp \
    include/CGLViewer.hpp \
    include/CMesh.hpp \
    include/CAreaReader.hpp \
    include/CAreaFile.hpp \
    include/CModelData.hpp \
    include/CMaterialSet.hpp \
    include/CMaterialReader.hpp \
    include/CMaterialCache.hpp \
    include/CMainWindow.hpp \
    include/CModelLoader.hpp \
    include/CMaterial.hpp \
    include/CModelLoaderThead.hpp \
    include/CResourceManager.hpp \
    include/SBoundingBox.hpp \
    include/CModelFile.hpp \
    include/CModelReader.hpp

FORMS += \
    forms/MainWindow.ui

RESOURCES += \
    resources/resources.qrc

win32:RC_FILE += \
    resources/mainicon.rc
