BOOST=enter_path_to_boost
BOOST_LIBS=enter_path_to_boost_libs
SOURCES += \
    main.cpp \
    decoder.cpp
INCLUDEPATH += $${BOOST}
LIBS += -L$${BOOST_LIBS} -lboost_program_options

CONFIG += c++11

HEADERS += \
    decoder.h
