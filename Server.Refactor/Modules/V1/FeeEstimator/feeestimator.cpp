#include "<<HEADER_FILE>>.h"
#include "app.h"
#include "modulelinker.h"

static <<MODULE_NAME>>ML _gRegisterModuleLinker;

<<MODULE_NAME>>ML::<<MODULE_NAME>>ML()
{ ModuleLinker::registerModule(QStringLiteral("Grinder-1"),<<MODULE_NAME>>ML::creator,<<MODULE_NAME>>ML::doInit,<<MODULE_NAME>>ML::start,<<MODULE_NAME>>ML::startInOwnThread); }

void *<<MODULE_NAME>>ML::creator(void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    return new <<MODULE_NAME>>;
}

bool <<MODULE_NAME>>ML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool <<MODULE_NAME>>ML::startInOwnThread()
{ return false; }

bool <<MODULE_NAME>>ML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

<<MODULE_NAME>>::<<MODULE_NAME>>()
{
}

<<MODULE_NAME>>::~<<MODULE_NAME>>()
{
}
