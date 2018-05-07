#ifndef JCSLPPDU_H
#define JCSLPPDU_H

#include <QByteArray>
#include <QString>
#include <QStringList>

namespace JCSLP
{
namespace PDU
{
namespace V1
{
typedef enum PDUType {
    SAAnnounce  = 0x0,
    DAAnnounce  = 0x1,
    UAQuery     = 0x2,
    UAReply     = 0x3
} PDUType;

bool            getType(const QByteArray iPDU, PDUType &oPDUType);
bool            getStringList(const QByteArray iPDU, QStringList &oStringList);

QByteArray      createSAAnnounce(const QString iService);
QByteArray      createSAAnnounce(const QStringList iServices);
QByteArray      createDAAnnounce(const QString iDAAddress);
QByteArray      createDAAnnounce(const QStringList iDAAddresses);
QByteArray      createUARequest(const QString iService);
QByteArray      createUARequest(const QStringList iServices);
QByteArray      createUAReply(const QString iService);
QByteArray      createUAReply(const QStringList iServices);

}
}
}

#endif // JCSLPPDU_H
