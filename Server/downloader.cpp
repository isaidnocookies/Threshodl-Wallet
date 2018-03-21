#include "downloader.h"

Downloader::Downloader(QObject * iParent)
    : QObject(iParent)
{

}

void Downloader::threadStarted()
{
    mNAM    = QNetworkAccessManager(this);
}
