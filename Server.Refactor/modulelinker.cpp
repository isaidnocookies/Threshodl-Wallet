#include "modulelinker.h"

#include <QDebug>
#include <QList>
#include <QReadWriteLock>

#define kMaxNumberOfModules             256
#define kMaxNumberOfLinkerPassTries     20

static QReadWriteLock                           _gLock;
static bool                                     _gAllowRegistration         = true;
static ModuleLinker::ModuleInfo *               _gRegisteredModules[kMaxNumberOfModules];
static int                                      _gRegisteredModulesCount    = 0;

bool ModuleLinker::registerModule(const QString iModuleName, ModuleLinker::staticMethodToCreator iCreator, ModuleLinker::staticMethodToDoInit iDoInit, ModuleLinker::staticMethodToStart iStart, staticMethodShouldStartInOwnThread iThreadStart)
{ return registerModuleWithDependencies(iModuleName, QStringList(), iCreator, iDoInit, iStart, iThreadStart); }

bool ModuleLinker::registerModuleWithDependencies(const QString iModuleName, const QStringList iDependencies, ModuleLinker::staticMethodToCreator iCreator, ModuleLinker::staticMethodToDoInit iDoInit, ModuleLinker::staticMethodToStart iStart, staticMethodShouldStartInOwnThread iThreadStart)
{
    QWriteLocker l{&_gLock};
    if( ! _gAllowRegistration || _gRegisteredModulesCount >= kMaxNumberOfModules )
        return false;

    for( int lIndex = 0; lIndex < _gRegisteredModulesCount && lIndex < kMaxNumberOfModules; lIndex++ )
    {
        // Name already registered
        if( _gRegisteredModules[lIndex]->Name == iModuleName )
            return false;
    }

    ModuleLinker::ModuleInfo *  lInfo = new ModuleLinker::ModuleInfo;
    lInfo->Name          = iModuleName;
    lInfo->Dependencies  = iDependencies;
    lInfo->Creator       = iCreator;
    lInfo->DoInit        = iDoInit;
    lInfo->ThreadStart   = iThreadStart;
    lInfo->Start         = iStart;
    _gRegisteredModules[_gRegisteredModulesCount++] = lInfo;
    return true;
}

void ModuleLinker::preventNewRegistrationOfModules()
{
    QWriteLocker l{&_gLock};
    _gAllowRegistration = false;
}

QList<ModuleLinker::ModuleInfo *> ModuleLinker::registeredModules()
{
    QReadLocker                         l{&_gLock};
    QList<ModuleLinker::ModuleInfo *>   lResult;
    for( int lIndex = 0; lIndex < _gRegisteredModulesCount; lIndex++ ) {
        lResult << _gRegisteredModules[lIndex];
    }
    return lResult;
}

QList<ModuleLinker::ModuleInfo *> ModuleLinker::sortRegisteredModulesByDependencies(const QList<ModuleLinker::ModuleInfo *> iModules)
{
    QList<ModuleLinker::ModuleInfo *>   lResults;
    QList<ModuleLinker::ModuleInfo *>   lModules        = iModules;
    QStringList                         lModuleNames;
    QStringList                         lInResults;
    int                                 lPassCount;
    bool                                lNotFound;

    try{
        // Build a list of all the module names
        for( auto lE : lModules )
        { lModuleNames << lE->Name; }

        // Now make sure we can actually satisfy all the dependencies
        for( auto lE : lModules )
        {
            if( ! lE->Dependencies.isEmpty() )
            {
                for( auto lD : lE->Dependencies )
                {
                    if( ! lModuleNames.contains(lD) )
                        throw __LINE__;
                }
            }
        }

        // First insert all the entries that have no dependencies
        for( auto lE : lModules )
        {
            if( lE->Dependencies.isEmpty() )
            {
                lResults << lE;
                lInResults << lE->Name;
            }
        }

        for( lPassCount = 0; lPassCount < kMaxNumberOfLinkerPassTries && lInResults.size() < lModules.size(); lPassCount++ )
        {
            for( auto lE : lModules )
            {
                if( lE->Dependencies.isEmpty() )     continue;
                if( lInResults.contains(lE->Name) )  continue;

                lNotFound = false;
                for( auto lD : lE->Dependencies )
                {
                    if( ! lInResults.contains(lD) ) {
                        lNotFound = true;
                        break;
                    }
                }

                if( lNotFound ) continue;

                lResults << lE;
                lInResults << lE->Name;
            }
        }

        if( lPassCount == kMaxNumberOfLinkerPassTries && lInResults.size() < lModules.size() )
            throw __LINE__;

    }catch(int iLineNumber){
        qWarning() << "Module Linker error at line" << iLineNumber;
        return QList<ModuleLinker::ModuleInfo *>();
    }


    return lResults;
}

ModuleLinker::ModuleInfo *ModuleLinker::moduleInfoForName(const QString iModuleName)
{
    QReadLocker l{&_gLock};
    for( int lIndex = 0; lIndex < _gRegisteredModulesCount && lIndex < kMaxNumberOfModules; lIndex++ )
    {
        if( _gRegisteredModules[lIndex]->Name == iModuleName )
            return _gRegisteredModules[lIndex];
    }
    return nullptr;
}
