#include "downloadworker.h"
#include "core.h"
#include "globalsandconstants.h"

#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThread>

DownloadWorker::DownloadWorker()
{
    mNetworkManager = new QNetworkAccessManager(this);
}

DownloadWorker::~DownloadWorker()
{

}

void DownloadWorker::startDownloading()
{
    mDownloading = true;
    downloadData();
}

void DownloadWorker::stopDownloading()
{
    mDownloading = false;
    emit finished();
}

void DownloadWorker::downloadData()
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    while (mDownloading) {

        for (auto sn : AppWallets::walletShortnames()) {
            QString lRequestParameter = QString(sn).append("USD");

            if (lRequestParameter.contains("d")) {
                continue;
            }

            qDebug() << sn;

            lReply = mNetworkManager->get(QNetworkRequest(QUrl(QString("%1/indices/global/ticker/%2").arg(BITCOIN_AVERAGE_API_URL).arg(lRequestParameter))));
            lMyEventLoop.exec();

            if (lReply->error() == QNetworkReply::NoError) {
                QByteArray      lReplyText = lReply->readAll();
                auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();
                qDebug() << lMyMap["last"];
            } else {
                qDebug() << "Error.... Can not update market values...";
            }
        }

        QThread::sleep(10);
    }
}
