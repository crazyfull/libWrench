#DEFINES += libWrenchApp

contains(DEFINES, libWrenchApp){
    message( "Application" )
    ####App
    TEMPLATE = app
    CONFIG += console
    CONFIG -= app_bundle
    CONFIG -= qt

} else {
    message( "Library" )
    ####Library
    TEMPLATE = lib
    CONFIG -= qt
    DEFINES += libWrench
    #CONFIG += c++11 staticlib
    TARGET = libWrench
    VERSION = 1.5.14
}

#c++ 11 & static build
CONFIG += c++11 staticlib

#NDEBUG
#NOLOG
#DEFINES += "NDEBUG"
#DEFINES += "NONEEDLOG"

#DEFINES += USE_SSL
#LIBS += -lssl -lcrypto      #openssl

#
#unix:DESTDIR = /usr/lib/libTCPServer
DESTDIR=../bin #Target file directory
OBJECTS_DIR=../objs #Intermediate object files directory
MOC_DIR=../moc

#libs
unix:LIBS += -pthread
CONFIG(release, debug|release) {
    LIBS += -static -static-libgcc -static-libstdc++
    message("release static mod" )
} else  {
    message("debug mod")
}

#source Headers
INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

#source Headers
INCLUDEPATH += $$PWD/src/cJSON
DEPENDPATH += $$PWD/src/cJSON

#source Headers
INCLUDEPATH += $$PWD/src/Files
DEPENDPATH += $$PWD/src/Files

#source Headers
INCLUDEPATH += $$PWD/src/Cryptography/cipher
DEPENDPATH += $$PWD/src/Cryptography/cipher

#source Headers
INCLUDEPATH += $$PWD/src/Cryptography/hash
DEPENDPATH += $$PWD/src/Cryptography/hash

#source Headers
INCLUDEPATH += $$PWD/src/Network/TCPServer
DEPENDPATH += $$PWD/src/Network/TCPServer

#source Headers
INCLUDEPATH += $$PWD/src/Network/UDPServer
DEPENDPATH += $$PWD/src/Network/UDPServer

#source Headers
INCLUDEPATH += $$PWD/src/Network/UDPClient
DEPENDPATH += $$PWD/src/Network/UDPClient

#source Headers
INCLUDEPATH += $$PWD/src/Network/DNSLookup
DEPENDPATH += $$PWD/src/Network/DNSLookup

#source Headers
INCLUDEPATH += $$PWD/src/LinuxService
DEPENDPATH += $$PWD/src/LinuxService


SOURCES += main.cpp \
    src/Cryptography/cipher/Base64/base64.c \
    src/Cryptography/cipher/Base64/clsBase64.cpp \
    src/Cryptography/cipher/clsMask.cpp \
    src/Cryptography/hash/byte_order.c \
    src/Cryptography/hash/clsHash.cpp \
    src/Cryptography/hash/md5.c \
    src/Cryptography/hash/sha1.c \
    src/Cryptography/hash/sha256.c \
    src/Cryptography/hash/sha512.c \
    src/Files/clsFileDirectory.cpp \
    src/LinuxService/clsLinuxService.cpp \
    src/Network/DNSLookup/clsDNSHeader.cpp \
    src/Network/DNSLookup/clsDNSLookup.cpp \
    src/Network/DNSLookup/clsDNSPackets.cpp \
    src/Network/TCPServer/clsEpollManager.cpp \
    src/Network/TCPServer/clsIDType.cpp \
    src/Network/TCPServer/clsSecureSocket.cpp \
    src/Network/TCPServer/clsTCPListener.cpp \
    src/Network/TCPServer/clsTCPServer.cpp \
    src/Network/TCPServer/clsTCPSocket.cpp \
    src/Network/UDPClient/UDPClient.cpp \
    src/Network/UDPServer/clsUDPListener.cpp \
    src/Network/UDPServer/clsUDPServer.cpp \
    src/Network/UDPServer/clsUDPSocket.cpp \
    src/Network/pipe/clsPipe.cpp \
    src/cJSON/cJSON.c \
    src/clsCString.cpp \
    src/clsThread.cpp \
    src/clsThreadTimer.cpp \
    src/iniSettings/ConvertUTF.c



#includes
HEADERS +=  src/clsThread.h \
    src/Cryptography/cipher/Base64/base64.h \
    src/Cryptography/cipher/Base64/clsBase64.h \
    src/Cryptography/cipher/clsMask.h \
    src/Cryptography/hash/byte_order.h \
    src/Cryptography/hash/clsHash.h \
    src/Cryptography/hash/md5.h \
    src/Cryptography/hash/sha1.h \
    src/Cryptography/hash/sha256.h \
    src/Cryptography/hash/sha512.h \
    src/Files/clsFileDirectory.h \
    src/LinuxService/clsLinuxService.h \
    src/Network/DNSLookup/clsDNSHeader.h \
    src/Network/DNSLookup/clsDNSLookup.h \
    src/Network/DNSLookup/clsDNSPackets.h \
    src/Network/TCPServer/SocketHeader.h \
    src/Network/TCPServer/clsEpollManager.h \
    src/Network/TCPServer/clsIDType.h \
    src/Network/TCPServer/clsSecureSocket.h \
    src/Network/TCPServer/clsTCPListener.h \
    src/Network/TCPServer/clsTCPServer.h \
    src/Network/TCPServer/clsTCPSocket.h \
    src/Network/UDPClient/UDPClient.h \
    src/Network/UDPServer/clsUDPListener.h \
    src/Network/UDPServer/clsUDPServer.h \
    src/Network/UDPServer/clsUDPSocket.h \
    src/Network/pipe/clsPipe.h \
    src/cJSON/cJSON.h \
    src/clsCString.h \
    src/clsThreadTimer.h \
    src/iniSettings/ConvertUTF.h \
    src/iniSettings/SimpleIni.h \
    src/log.h


###################################################################
target.path = /usr/lib/
INSTALLS += target

headers.files += $$HEADERS
headers.path = /usr/include/Wrench
INSTALLS += headers

