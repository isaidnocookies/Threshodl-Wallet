#include "clientalphahandler.h"
#include "rpcmessagepingrequest.h"
#include "rpcmessagepingreply.h"
#include "rpcmessagecreateaccountrequest.h"
#include "rpcmessagecreateaccountreply.h"
#include "rpcmessagecreatemicrowalletpackagerequest.h"

#include "app.h"
#include "digest.h"
#include "walletgrinderalpha.h"
#include "bitcoinwallet.h"

#include <QDebug>

bool ClientAlphaHandler::handle(ClientConnection *iConnection, const QString iCommand, RPCMessage &iMessage)
{
    if( iCommand == RPCMessagePingRequest::commandValue() ) {
        RPCMessagePingRequest   lPingReq{iMessage};
        QString lReply = RPCMessagePingReply::create(lPingReq.payload(),QStringLiteral("Threshodl"));
        if( ! lReply.isEmpty() ) {
            return iConnection->sendMessage(lReply);
        }else{
            qWarning() << "Failed to generate a reply message, this is a coding issue!";
        }
    }else if( iCommand == RPCMessageCreateAccountRequest::commandValue() ) {
        return createUserAccount(iConnection,iMessage);
    }else if( iCommand == RPCMessageCreateMicroWalletPackageRequest::commandValue() ) {
        return createMicroWalletPackage(iConnection,iMessage);
    }

    return false;
}

bool ClientAlphaHandler::authenticateMessage(RPCMessage &iMessage)
{
    auto            lDBI        = gApp->databaseInterface();
    QByteArray      lPublicKey;

    if( lDBI && ! (lPublicKey = lDBI->publicKeyForAddress(iMessage.username())).isEmpty() ) {
        if( iMessage.signatureKeyEncoding() == RPCMessage::KeyEncoding::SHA512 ) {
            return Digest::verify(lPublicKey, iMessage.dataForSignature(), iMessage.signature(), Digest::HashTypes::SHA512 );
        }else{
            return true;
        }
    }

    return false;
}

bool ClientAlphaHandler::createUserAccount(ClientConnection *iConnection, RPCMessage &iMessage)
{
    auto                                    lDBI        = gApp->databaseInterface();
    RPCMessageCreateAccountReply::ReplyCode lReplyCode  = RPCMessageCreateAccountReply::ReplyCode::UnknownFailure;

    if( lDBI ) {
        RPCMessageCreateAccountRequest  lRequest{iMessage};
        if( lRequest.signatureKeyEncoding() == RPCMessage::KeyEncoding::SHA512 ) {
            QString                         lAddress    = lRequest.username();
            QByteArray                      lPublicKey  = lRequest.publicKey();

            if( Digest::verify(lPublicKey,lRequest.dataForSignature(),lRequest.signature(),Digest::HashTypes::SHA512) ) {
                if( lDBI->addressCreate(lAddress,lPublicKey) ) {
                    lReplyCode = RPCMessageCreateAccountReply::ReplyCode::Success;
                }else{
                    lReplyCode = RPCMessageCreateAccountReply::ReplyCode::UsernameTaken;
                }
            }else{
                lReplyCode = RPCMessageCreateAccountReply::ReplyCode::KeyInvalid;
            }
        }else{
            lReplyCode = RPCMessageCreateAccountReply::ReplyCode::KeyTypeInvalid;
        }
    }else{
        lReplyCode = RPCMessageCreateAccountReply::ReplyCode::InternalServerError1;
    }

    QString lMessage = RPCMessageCreateAccountReply::create(lReplyCode, iMessage.username(), gApp->privateKeyPEM());
    return iConnection->sendMessage(lMessage);
}

bool ClientAlphaHandler::createMicroWalletPackage(ClientConnection *iConnection, RPCMessage &iMessage)
{
    if( authenticateMessage(iMessage) ) {
        RPCMessageCreateMicroWalletPackageRequest   lRequest{iMessage};

        if( lRequest.cryptoTypeShortName() == QStringLiteral("btc") ) {
            std::vector<BValue> *   lValuesPtr = WalletGrinderAlpha::getBreaks(lRequest.cryptoValue());
            std::vector<BValue>     lValues;
            QStringList             lWalletValues;
            QList<BitcoinWallet>    lBTCWallets;
            QString                 lRawBTCTransaction  = QStringLiteral(
                        "{\"jsonrpc\":\"1.0\",\"id\":\"getinfo\",\"method\":\"createrawtransaction\",\"params\":\"[[{\"txid\":\"%1\",\"vout\":%2}],"
                        ).arg(lRequest.txId()).arg(lRequest.vout());

            if( lValuesPtr ) {
                lValues = *lValuesPtr;

                for( size_t lIndex = 0; lIndex < lValues.size(); lIndex++ ) {
                    lWalletValues << QString::number(WalletGrinderAlpha::getValueFromBValue(lValues[lIndex]));
                    lBTCWallets << BitcoinWallet::createNewBitcoinWallet();

                    if( lIndex > 0 ) lRawBTCTransaction = lRawBTCTransaction.append(QStringLiteral(","));
                    lRawBTCTransaction = lRawBTCTransaction.append( QStringLiteral("{\"%1\":%2}").arg(lBTCWallets[lIndex].address().constData()).arg(lWalletValues[lIndex]) );
                }

                if( ! lRequest.outputBalance().isEmpty() && lRequest.outputBalance().toDouble() > 0.0f ) {
                    lRawBTCTransaction = lRawBTCTransaction.append( QStringLiteral(",{\"%1\":%2}").arg(lRequest.outputBalanceAddress()).arg(lRequest.outputBalance()) );
                }

                lRawBTCTransaction = lRawBTCTransaction.append(QStringLiteral("]}"));

                delete lValuesPtr;
            }else{
                // Something bad happened
            }
        }else{
            // Unknown crypto
        }
    }

    return false;
}
