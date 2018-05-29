#include "threshodltools.h"

#include <QClipboard>
#include <QGuiApplication>

ThreshodlTools::ThreshodlTools(QObject *parent) : QObject(parent){}

QString ThreshodlTools::stringAdd(QString iOne, QString iTwo)
{
    QStringMath lOne (iOne);
    QStringMath lTwo (iTwo);

    return QStringMath(lOne + lTwo).toString();
}

void ThreshodlTools::copyToClipboard(QString iDataToCopy)
{
    QClipboard *lClipboard = QGuiApplication::clipboard();
    lClipboard->setText(iDataToCopy);
    qDebug() << "Text copied";
}

QString ThreshodlTools::getClipboardText()
{
    QClipboard *lClipboard = QGuiApplication::clipboard();
    qDebug() << "Text returned from clipboard";
    return lClipboard->text();
}
