TARGET	= ananas4-test 
include(../lib/lib.pri)
include(../plugins/plugins.pri)
include(../ananas.pri )

SOURCES	+= \
	    main.cpp \ 
	    testametadata.cpp

HEADERS	+=  \
	    test.h \
	    testametadata.h
TRANSLATES = 

CONFIG +=qtestlib
FORMS	= 
TEMPLATE	=app
INCLUDEPATH	+= ../lib ../plugins
LIBS	+= -L$(QTDIR)/lib -L../../lib -lananas4
LIBS	+= -L../../lib/designer -lananasplugin4

unix {
    LIBS += -L../../lib -lqt4-qdataschema
}

win32 {
    LIBS += -L../../lib -lqt4-qdataschema1
}