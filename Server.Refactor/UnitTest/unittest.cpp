#include "../config.h"

void UnitTestInjectCommandConfiguration(Config * iConfiguration)
{
    iConfiguration->setDisableSave(true);
    iConfiguration->setValue(QStringLiteral("LogOutputFormat"),QStringLiteral("json"));

    // Do nothing for now, later we can inject custom configuration settings and filters here

    return;
}
