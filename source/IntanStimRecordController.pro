TEMPLATE      = app

QT            += widgets multimedia

CONFIG        += static
QMAKE_CXXFLAGS_RELEASE	-= -O2
QMAKE_CXXFLAGS_RELEASE	-= -O1
QMAKE_CXXFLAGS_RELEASE	-= -O
QMAKE_CXXFLAGS_RELEASE	*= -O3

macx:{
QMAKE_RPATHDIR += /Users/amansehgal/Qt/5.8/clang_64/lib
}

HEADERS       = \
    okFrontPanelDLL.h \
    waveplot.h \
    mainwindow.h \
    signalprocessor.h \
    bandwidthdialog.h \
    renamechanneldialog.h \
    signalchannel.h \
    signalgroup.h \
    signalsources.h \
    spikescopedialog.h \
    spikeplot.h \
    keyboardshortcutdialog.h \
    randomnumber.h \
    impedancefreqdialog.h \
    globalconstants.h \
    triggerrecorddialog.h \
    setsaveformatdialog.h \
    helpdialoghighpassfilter.h \
    helpdialognotchfilter.h \
    helpdialogdacs.h \
    helpdialogcomparators.h \
    helpdialogchipfilters.h \
    cabledelaydialog.h \
    helpdialogfastsettle.h \
    datastreamfifo.h \
    usbdatathread.h \
    helpdialogreference.h \
    rhs2000datablock.h \
    rhs2000evalboard.h \
    rhs2000registers.h \
    helpdialogioexpander.h \
    stimparameters.h \
    stimparamdialog.h \
    stimfigure.h \
    timespinbox.h \
    currentspinbox.h \
    digfigure.h \
    digoutdialog.h \
    abstractfigure.h \
    anoutfigure.h \
    anoutdialog.h \
    voltagespinbox.h \
    startupdialog.h \
    ampsettledialog.h \
    chargerecoverydialog.h \
    filt.h \
    helpdialogspikebandfilter.h \
    spikebandfiltercoeffs.h

SOURCES       = main.cpp \
    okFrontPanelDLL.cpp \
    waveplot.cpp \
    mainwindow.cpp \
    signalprocessor.cpp \
    bandwidthdialog.cpp \
    renamechanneldialog.cpp \
    signalchannel.cpp \
    signalgroup.cpp \
    signalsources.cpp \
    spikescopedialog.cpp \
    spikeplot.cpp \
    keyboardshortcutdialog.cpp \
    randomnumber.cpp \
    impedancefreqdialog.cpp \
    triggerrecorddialog.cpp \
    setsaveformatdialog.cpp \
    helpdialoghighpassfilter.cpp \
    helpdialognotchfilter.cpp \
    helpdialogdacs.cpp \
    helpdialogcomparators.cpp \
    helpdialogchipfilters.cpp \
    cabledelaydialog.cpp \
    helpdialogfastsettle.cpp \
    datastreamfifo.cpp \
    usbdatathread.cpp \
    helpdialogreference.cpp \
    rhs2000datablock.cpp \
    rhs2000evalboard.cpp \
    rhs2000registers.cpp \
    helpdialogioexpander.cpp \
    stimparameters.cpp \
    stimparamdialog.cpp \
    stimfigure.cpp \
    timespinbox.cpp \
    currentspinbox.cpp \
    digfigure.cpp \
    digoutdialog.cpp \
    abstractfigure.cpp \
    anoutfigure.cpp \
    anoutdialog.cpp \
    voltagespinbox.cpp \
    startupdialog.cpp \
    ampsettledialog.cpp \
    chargerecoverydialog.cpp \
    filt.cpp \
    helpdialogspikebandfilter.cpp
    
RESOURCES     = IntanStimRecordController.qrc

macx:{
LIBS += -L$$PWD/ -lokFrontPanel
INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
}
