QT += quick qml charts

CONFIG += qmltypes
QML_IMPORT_NAME = fr.ecgviewer.qmlcomponents
QML_IMPORT_MAJOR_VERSION = 1

SOURCES += \
        controller.cpp \
        main.cpp \
        marker.cpp \
        sequence.cpp \
        tool.cpp

resources.files = main.qml 
resources.prefix = /$${TARGET}
RESOURCES += resources

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    controller.h \
    marker.h \
    sequence.h \
    tool.h
