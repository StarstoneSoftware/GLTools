# OpenGL Rendering Tools
# This is an "include" file for Qt .PRO files (using qmake)
# Regardless of whether you use this or not, be sure and
#define QT_IS_AVAILABLE

#define OPENGL_ES (if using OpenGL ES 3, otherwise assumes OpenGL 4 on the desktop)

HEADERS += $$PWD/include/GLBatch.h \
           $$PWD/include/GLBatchBase.h \
           $$PWD/include/GLShaderManager.h \
           $$PWD/include/GLTools.h \
           $$PWD/include/GLTriangleBatch.h

SOURCES += $$PWD/src/GLBatch.cpp \
           $$PWD/src/GLShaderManager.cpp \
           $$PWD/src/GLTriangleBatch.cpp \
           $$PWD/src/GLTools.cpp
