#ifndef WCPMESSAGECREATEMICROWALLETPACKAGEREPLY_H
#define WCPMESSAGECREATEMICROWALLETPACKAGEREPLY_H

#include "wcpmessage.h"

class WCPMessageCreateMicroWalletPackageReply : public WCPMessage
{
public:
    enum ReplyCode {
        UnknownFailure                  = 0x0,
        Success                         = 0x1,
        Unauthorized                    = 0x2,
        UnhandledCryptoType             = 0x3,
        UnableToGrindUpCryptoCurrency   = 0x4,
        InternalServerError1            = 0xA0
    };

    typedef enum ReplyCode ReplyCode;

    WCPMessageCreateMicroWalletPackageReply();
    WCPMessageCreateMicroWalletPackageReply(const WCPMessage &iOther);
    WCPMessageCreateMicroWalletPackageReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();              // Untouched by the server
    static QString microWalletsDataKey();

    ReplyCode replyCode() const;
    QString transactionId() const;
    QList<QByteArray> microWalletsData() const;

    // For Bitcoin
    static QString estimatedFeesKey();
    QString estimatedFees() const;

    static QString create(
            const ReplyCode iReplyCode, const QList<QByteArray> iMicroWalletDatas, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );

    static QString createBtc(
            const ReplyCode iReplyCode, const QList<QByteArray> iMicroWalletDatas, const QString iTransactionId,
            const QString iEstimatedFees,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGECREATEMICROWALLETPACKAGEREPLY_H
