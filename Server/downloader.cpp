#include "downloader.h"

Downloader::Downloader(QObject * iParent)
    : QObject(iParent)
{

}

void Downloader::threadStarted()
{
    mNAM    = new QNetworkAccessManager(this);
}

void Downloader::timerEvent()
{

}
