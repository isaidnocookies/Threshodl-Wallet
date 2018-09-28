#ifndef DARKWALLETTOOLS_H
#define DARKWALLETTOOLS_H

#include "core.h"
#include <QObject>

class DarkWalletTools : public QObject
{
    Q_OBJECT
public:
    explicit DarkWalletTools(QObject *parent = nullptr);

    void setUserDetails(QString iUsername, QString iPublicKey, QString iPrivateKey);

    QString estimateFeesForDark(QString iAmount, QString iShortname);
    bool sendWallets(QVariantList iWallets, QString fromUser, QString iCryptoType, QString toEmailAddress, QString toUser, ErrorCodes::DarkErrors &oError);
    bool completeWallets(QVariantList lWallets);
//    bool breakdownWallets(...)

signals:

public slots:

private:
    bool getAttachmentFile(QVariantList iWallets, QString toUser, QString iCryptoShortname, QString &oAmount, QByteArray &oAttachment);
    bool sendEmail(QByteArray iAttachment, QString iAmount, QString toEmail, QString toUser, QString fromUser, QString iCryptoShortname);
    bool sendFailureEmail(QString toEmail, QString toUser, QString fromUser, QString iCryptoShortname);
    bool transferDarkWallets(QVariantList iWallets, QString iFromUser, QString iPublicKey, QString iPrivateKey, QString toUser, QString iAuthMessage);

    QString mPrivateKey;
    QString mPublicKey;
    QString mUsername;
};

#endif // DARKWALLETTOOLS_H
