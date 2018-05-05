!contains( WALLETSERVERMODULES, DatabaseV1 ) {
    WALLETSERVERMODULES += DatabaseV1

    !contains(QT,sql): QT += sql

    HEADERS += \
        $$PWD/databasepriv.h \
        $$PWD/databaseprivpsql.h \
        $$PWD/database.h

    SOURCES += \
        $$PWD/databaseprivpsql.cpp \
        $$PWD/database.cpp
}
