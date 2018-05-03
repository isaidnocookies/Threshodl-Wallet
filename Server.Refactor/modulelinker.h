#ifndef MODULELINKER_H
#define MODULELINKER_H

#include <QString>
#include <QList>
#include <QStringList>

class ModuleLinker
{
public:
    typedef void * (*staticMethodToCreator)(void * pointerToAppObject);                                     // Returns a pointer to a new object
    typedef bool (*staticMethodToDoInit)(void * pointerToThis, void * pointerToAppObject);                  // Returns true on DoInit success
    typedef bool (*staticMethodShouldStartInOwnThread)();                                                   // Returns true if should be created and started in own thread
    typedef bool (*staticMethodToStart)(void * pointerToThis, void * pointerToAppObject);                   // Returns true on Start success

    // Returns true if the module was registered, modules can NOT register after the app has loaded fully
    static bool registerModule( const QString iModuleName, staticMethodToCreator iCreator, staticMethodToDoInit iDoInit, staticMethodToStart iStart, staticMethodShouldStartInOwnThread iThreadStart );
    static bool registerModuleWithDependencies( const QString iModuleName, const QStringList iDependencies, staticMethodToCreator iCreator, staticMethodToDoInit iDoInit, staticMethodToStart iStart, staticMethodShouldStartInOwnThread iThreadStart );

    typedef struct {
        QString                                 Name;
        QStringList                             Dependencies;
        staticMethodToCreator                   Creator;
        staticMethodToDoInit                    DoInit;
        staticMethodShouldStartInOwnThread      ThreadStart;
        staticMethodToStart                     Start;
    } ModuleInfo;

    static void preventNewRegistrationOfModules();
    static QList<ModuleInfo *> registeredModules();
    static QList<ModuleInfo *> sortRegisteredModulesByDependencies(const QList<ModuleInfo*> iModules); // On error, returns an empty list;
    static ModuleInfo * moduleInfoForName(const QString iModuleName);
};

#endif // MODULELINKER_H
