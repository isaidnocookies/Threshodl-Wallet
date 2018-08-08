#include "threshodltools.h"

#include <QClipboard>
#include <QGuiApplication>
#include <QDebug>

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

QString ThreshodlTools::formatMarketValueString(QString iInputValue)
{
    if (iInputValue.contains(".")) {
        int lIndexOfDot = iInputValue.indexOf(".");
        if (lIndexOfDot > iInputValue.size() - 2) {
            return iInputValue;
        } else {
            return iInputValue.remove(lIndexOfDot + 3, iInputValue.size() - 1 - lIndexOfDot + 1);
        }
    } else {
        return iInputValue.append(".00");
    }
}
