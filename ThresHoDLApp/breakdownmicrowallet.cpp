#include "breakdownmicrowallet.h"

BreakdownMicroWallet::BreakdownMicroWallet(QObject *parent) : QObject(parent)
{
    mConnection = new WCPConnection{this};

    mCurrentAction = ActionCommands::None;
    mConnectionAttempts = 5;

    connect( mConnection, &WCPConnection::connected, this, &BreakdownMicroWallet::connectedToServer );
    connect( mConnection, &WCPConnection::disconnected, this, &BreakdownMicroWallet::disconnectedFromServer );
    connect( mConnection, &WCPConnection::socketError, this, &BreakdownMicroWallet::socketError );
    connect( mConnection, &WCPConnection::sslErrors, this, &BreakdownMicroWallet::sslErrors );
    connect( mConnection, &WCPConnection::failedToSendTextMessage, this, &BreakdownMicroWallet::failedToSendMessage );
    connect( mConnection, &WCPConnection::sentTextMessage, this, &BreakdownMicroWallet::sentMessage );
    connect( mConnection, &WCPConnection::textMessageReceived, this, &BreakdownMicroWallet::receivedMessage );

    QFile lFile{QStringLiteral(":/ca.pem")};
    if( lFile.open(QIODevice::ReadOnly) ) {
        mSslConfiguration = QSslConfiguration::defaultConfiguration();
        mSslConfiguration.setCaCertificates(mSslConfiguration.caCertificates() << QSslCertificate{lFile.readAll(),QSsl::Pem});

        lFile.close();
        mConnection->setSslConfiguration(mSslConfiguration);
    }else{
        qFatal("Failed to open CA cert.");
    }
}

void BreakdownMicroWallet::getMicroWallets(BitcoinWallet iWallet)
{
    //complete microwallet
    mBitcoinWalletToComplete = iWallet;
    mCurrentAction = ActionCommands::CompleteWallet;
    QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
    mConnection->open(lUrl);
}

void BreakdownMicroWallet::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
}

void BreakdownMicroWallet::connectedToServer()
{
    qDebug() << "Connected to server.";

    mTransactionID = QString("%1.%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(QString::number(qrand() % 10000));

    if (mCurrentAction == ActionCommands::CompleteWallet) {
        QStringList lWalletList;
        lWalletList.append(mBitcoinWalletToComplete.walletId());
        mConnection->sendTextMessage(WCPMessageCompleteMicroWalletsRequest::create(lWalletList, mTransactionID, mActiveUser->getUsername(), mActiveUser->getPrivateKey()));
    } else if (mCurrentAction == ActionCommands::BreakdownWallet) {
        mConnection->sendTextMessage(WCPMessageCreateMicroWalletPackageRequest::createBtc(
                        mBitcoinWalletToComplete.value(),
                        1,
                        1,
                        currentChain(),
                        mTransactionID,
                        mActiveUser->getUsername(),
                        mActiveUser->getPrivateKey()));
    }
}

void BreakdownMicroWallet::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";

    // try again...
    if (mCurrentAction != ActionCommands::Complete && mCurrentAction != ActionCommands::None && mConnectionAttempts >= 0) {
        mConnectionAttempts--;
        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
        mConnection->open(lUrl);
    }
}

void BreakdownMicroWallet::failedToSendMessage()
{
    if (mCurrentAction == ActionCommands::CompleteWallet) {
        qDebug() << "Failed to send message for completing wallet";
        emit completeMicroWallet(false);
    } else if (mCurrentAction == ActionCommands::BreakdownWallet ) {
        qDebug() << "Failed to send message for breakingdown wallet";
        emit newMicroWallets(false);
    } else {
        qDebug() << "Failed to send message: for None ActionCommand";
        emit microWalletBreakdownComplete(false);
    }
}

void BreakdownMicroWallet::sentMessage()
{
    qDebug() << "Sent message. Boom.";
}

void BreakdownMicroWallet::receivedMessage()
{
    qDebug() << "Received message.";

    QString lMessage = mConnection->nextTextMessage();
    bool lTreatAsFailure = false;

    if (mCurrentAction == ActionCommands::CompleteWallet) {
        WCPMessageCompleteMicroWalletsReply lReply {lMessage};
        QString lCommand = lReply.fieldValue(QStringLiteral(kFieldKey_Command)).toString();

        if (lCommand == WCPMessageCompleteMicroWalletsReply::commandValue()) {
            switch(lReply.replyCode()) {
            case WCPMessageCompleteMicroWalletsReply::ReplyCode::Success:
                qDebug() << "Successfully completed wallet";
                break;
            case WCPMessageCompleteMicroWalletsReply::ReplyCode::OneOrMoreUnauthorized:
                qDebug() << "OneOrMoreUnauthorized Error";
                lTreatAsFailure = true;
                break;
            case WCPMessageCompleteMicroWalletsReply::ReplyCode::OneOrMoreMicroWalletsDoNotExist:
                qDebug() << "OneOrMoreMicroWalletsDoNotExist Error";
                lTreatAsFailure = true;
                break;
            case WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError1:
                // Database error, cant talk to database
                qDebug() << "InternalServerError1";
                lTreatAsFailure = true;
                break;
            case WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError2:
                // Some wallets were completed.. Result data--if size of data is the error code, its bad. if its not bad, its not bad (size of error is char)
                qDebug() << "InternalServerError2 Error";
                lTreatAsFailure = true; // we are only requesting one wallet...
                break;
            case WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError3:
                // Can't delete microwallet from internal database. Microwallets were complete tho... Treat as success.
                qDebug() << "InternalServerError3 Error";
                break;
            default:
                //WCPMessageCompleteMicroWalletsReply::ReplyCode::UnknownFailure
                qDebug() << "UnknownFailure Error";
                lTreatAsFailure = true;
                break;
            }

            if (!lTreatAsFailure) {
                completeWallet(lReply.walletPartialKeys());
            } else {
                //failure....
                //TODO: Complete failure...
            }
        }
    } else {
        WCPMessageCreateMicroWalletPackageReply lReply {lMessage};
        QString lCommand = lReply.fieldValue(QStringLiteral(kFieldKey_Command)).toString();

        if (lCommand == WCPMessageCreateMicroWalletPackageReply::commandValue()) {
            switch(lReply.replyCode()) {
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::Success:
                qDebug() << "Success -- WCPMessageCreateMicroWalletPackageReply";
                getMicroWalletsFromReply(lReply.microWalletsData());
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::UnableToGrindUpCryptoCurrency:
                qDebug() << "Unable to grind up crypto";
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::Unauthorized:
                qDebug() << "Unauthorized during create";
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::UnhandledCryptoType:
                qDebug() << "Unhandled Crypto";
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::InternalServerError1:
                qDebug() << "Internal Server 1 -- create package";
                break;
            default:
                qDebug() << "Fuck, unknown error from create";
                break;
            }
        }
    }
}

void BreakdownMicroWallet::socketError(QAbstractSocket::SocketError iError)
{
    if (mCurrentAction == ActionCommands::CompleteWallet) {
        qDebug() << "SocketError for completing wallet" << iError;;
    } else if (mCurrentAction == ActionCommands::BreakdownWallet ) {
        qDebug() << "SocketError for breakingdown wallet" << iError;;
    } else {
        qDebug() << "SocketError: for None ActionCommand" << iError;;
    }
}

void BreakdownMicroWallet::sslErrors(const QList<QSslError> iErrors)
{
    Q_UNUSED(iErrors)

    if (mCurrentAction == ActionCommands::CompleteWallet) {
        qDebug() << "Ssl Errors for completing wallet";
    } else if (mCurrentAction == ActionCommands::BreakdownWallet ) {
        qDebug() << "Ssl Errors for breakingdown wallet";
    } else {
        qDebug() << "Ssl Error: for None ActionCommand";
    }
}

void BreakdownMicroWallet::completeWallet(QMap<QString, QByteArray> iData)
{
    mActiveUser->removeMicroWallet(mBitcoinWalletToComplete);
    mBitcoinWalletToComplete.setPrivateKey(mBitcoinWalletToComplete.privateKey().append(iData[mBitcoinWalletToComplete.walletId()]));
    mBitcoinWalletToComplete.setOwner(mActiveUser->getUsername());
    mBitcoinWalletToComplete.setIsMicroWallet(false);
    mBitcoinWalletToComplete.setWif(BitcoinWallet::generateWifFromPrivateKey(mBitcoinWalletToComplete.privateKey()));
    mActiveUser->addBrightWallet(mBitcoinWalletToComplete);
    mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), "Bitcoin was added to bright during breakdown of dark wallet");
    mBitcoinWasCompleted = true;
    mCurrentAction = ActionCommands::BreakdownWallet;

    qDebug() << "Wallet was actually completed. Continue to make change...";

    if (mConnection->isConnected()) {
        connectedToServer();
    } else {
        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
        mConnection->open(lUrl);
    }

}

void BreakdownMicroWallet::getMicroWalletsFromReply(QList<QByteArray> iData)
{
    // complete a transaction to remove bright wallet once testnet coins are introduced...
    mActiveUser->removeBrightWallets(mBitcoinWalletToComplete.value());

    for (auto w : iData) {
        BitcoinWallet lNewWallet (w);
        lNewWallet.setOwner(mActiveUser->getUsername());
        lNewWallet.setIsMicroWallet(true);

        qDebug() << "Adding.... " << BitcoinWallet(w).walletId();
        mActiveUser->addMicroWallet(lNewWallet);
    }

    mCurrentAction = ActionCommands::Complete;

    emit microWalletBreakdownComplete(true);
}
