TEMPLATE = app
TARGET = voyager
mac { 
    TARGET = Voyager
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
}
DEFINES += QT_NO_CAST_FROM_ASCII \
    QT_STRICT_ITERATORS
include(src.pri)
SOURCES += main.cpp
DESTDIR = ../
unix { 
    INSTALLS += target \
        translations \
        desktop \
        iconxpm \
        iconsvg \
        icon16 \
        icon32 \
        icon128 \
        man \
        man-compress
    target.path = $$BINDIR
    translations.path = $$PKGDATADIR
    translations.files += .qm/locale
    desktop.path = $$DATADIR/applications
    desktop.files += voyager.desktop
    iconxpm.path = $$DATADIR/pixmaps
    iconxpm.files += data/voyager.xpm
    iconsvg.path = $$DATADIR/icons/hicolor/scalable/apps
    iconsvg.files += data/voyager.svg
    icon16.path = $$DATADIR/icons/hicolor/16x16/apps
    icon16.files += data/16x16/voyager.png
    icon32.path = $$DATADIR/icons/hicolor/32x32/apps
    icon32.files += data/32x32/voyager.png
    icon128.path = $$DATADIR/icons/hicolor/128x128/apps
    icon128.files += data/voyager.png
    man.path = $$DATADIR/man/man1
    man.files += data/voyager.1
    man-compress.path = $$DATADIR/man/man1
    man-compress.extra = "" \
        "gzip -9 -f \$(INSTALL_ROOT)/$$DATADIR/man/man1/voyager.1" \
        ""
}
HEADERS += 
FORMS += 
