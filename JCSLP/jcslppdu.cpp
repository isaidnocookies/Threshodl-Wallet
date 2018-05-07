#include "jcslppdu.h"

#include <QtEndian>

#define     kJCSLPVersion1_Value    0x0

#define     kJCSLPType_SAAnnounce   0x0
#define     kJCSLPType_DAAnnounce   0x1
#define     kJCSLPType_UAQuery      0x2
#define     kJCSLPType_UAReply      0x3

inline static QByteArray __createQStringListPDU(quint8 iType, const QStringList iPayload)
{
    if( iPayload.isEmpty() ) return QByteArray();

    QByteArray  lPDU;
    quint8      lVersion            = qToBigEndian<quint8>( kJCSLPVersion1_Value );
    quint8      lType               = qToBigEndian<quint8>( iType );
    quint16     lPayloadCount       = 0;
    QByteArray  lPayloadBuffer;

    for( QString lP : iPayload )
    {
        QByteArray      lUtf8       = lP.toUtf8();
        lPayloadBuffer = lPayloadBuffer.append(lUtf8);
        if( ! lPayloadBuffer.endsWith('\0') )
            lPayloadBuffer = lPayloadBuffer.append(QChar{'\0'});
        lPayloadCount++;
    }

    lPayloadCount = qToBigEndian<quint16>(lPayloadCount);

    lPDU = QByteArray{ reinterpret_cast<char *>(&lVersion), sizeof(lVersion) };
    lPDU = QByteArray{ reinterpret_cast<char *>(&lType), sizeof(lType) };
    lPDU = lPDU.append( QByteArray{ reinterpret_cast<char *>(&lPayloadCount), sizeof(lPayloadCount) } );
    lPDU = lPDU.append( lPayloadBuffer );

    return lPDU;
}

QByteArray JCSLP::PDU::V1::createSAAnnounce(const QStringList iServices)
{ return __createQStringListPDU(kJCSLPType_SAAnnounce,iServices); }

QByteArray JCSLP::PDU::V1::createDAAnnounce(const QStringList iDAAddresses)
{ return __createQStringListPDU(kJCSLPType_DAAnnounce,iDAAddresses); }

QByteArray JCSLP::PDU::V1::createUARequest(const QStringList iServices)
{ return __createQStringListPDU(kJCSLPType_UAQuery,iServices); }

QByteArray JCSLP::PDU::V1::createUAReply(const QStringList iServices)
{ return __createQStringListPDU(kJCSLPType_UAReply,iServices); }

QByteArray JCSLP::PDU::V1::createUARequest(const QString iService)
{ return createUARequest(QStringList() << iService); }

QByteArray JCSLP::PDU::V1::createDAAnnounce(const QString iDAAddress)
{ return createDAAnnounce(QStringList() << iDAAddress); }

QByteArray JCSLP::PDU::V1::createSAAnnounce(const QString iService)
{ return createSAAnnounce(QStringList() << iService); }

QByteArray JCSLP::PDU::V1::createUAReply(const QString iService)
{ return createUAReply(QStringList() << iService); }

bool JCSLP::PDU::V1::getType(const QByteArray iPDU, JCSLP::PDU::V1::PDUType &oPDUType)
{
    if( iPDU.size() < static_cast<int>((sizeof(quint8) * 2)) ) return false;

    const quint8 *  lPtr = reinterpret_cast<const quint8 *>(iPDU.constData());

    if( qFromBigEndian<quint8>(lPtr[0]) == kJCSLPVersion1_Value ) {
        switch( qFromBigEndian<quint8>(lPtr[1]) ) {
        case kJCSLPType_SAAnnounce:
            oPDUType = PDUType::SAAnnounce;
            return true;

        case kJCSLPType_DAAnnounce:
            oPDUType = PDUType::DAAnnounce;
            return true;

        case kJCSLPType_UAQuery:
            oPDUType = PDUType::UAQuery;
            return true;

        case kJCSLPType_UAReply:
            oPDUType = PDUType::UAReply;
            return true;

        default:
            return false;
        }
    }

    return false;
}

bool JCSLP::PDU::V1::getStringList(const QByteArray iPDU, QStringList &oStringList)
{
    if( iPDU.size() < static_cast<int>((sizeof(quint8) * 2) + sizeof(quint16)) ) return false;

    const quint8 *  lPtr = reinterpret_cast<const quint8 *>(iPDU.constData());
    const quint8 *  lEnd = lPtr + iPDU.size();

    lPtr += (sizeof(quint8) * 2);

    const quint16 * lPayloadCountBEPtr  = reinterpret_cast<const quint16 *>(lPtr);
    quint16         lPayloadCount       = qFromBigEndian<quint16>(*lPayloadCountBEPtr);

    lPtr += sizeof(quint16);

    if( lPayloadCount > 0 ) {
        // We should make sure the PDU size can handle the absolute minimum size
        if( iPDU.size() < static_cast<int>((sizeof(quint8) * 2) + sizeof(quint16) + (sizeof(quint8) * lPayloadCount)) )
            return false;

        QByteArray lWorkingStr = QByteArray();
        for( quint16 lIndex = 0; lIndex < lPayloadCount && lPtr < lEnd; lIndex++ ) {
            // We should be at the start of the first char
            if( *lPtr == '\0' ) {
                oStringList << QString::fromUtf8(lWorkingStr);
                lPtr++;
                continue;
            }

            lWorkingStr = lWorkingStr.append(*lPtr);
            lPtr++;
        }

        if( ! lWorkingStr.isEmpty() ) {
            oStringList << QString::fromUtf8(lWorkingStr);
        }

        if( oStringList.isEmpty() )
            return false;
    }

    return true;
}
