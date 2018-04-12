#include "clientalphahandler.h"
#include "rpcmessagepingrequest.h"
#include "rpcmessagepingreply.h"
#include "rpcmessagecreateaccountrequest.h"
#include "rpcmessagecreateaccountreply.h"
#include "rpcmessagereassignmicrowalletsrequest.h"
#include "rpcmessagereassignmicrowalletsreply.h"
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
    }else if( iCommand == RPCMessageReassignMicroWalletsRequest::commandValue() ) {
        return reassignMicroWallets(iConnection,iMessage);
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
#warning THIS IS BROKEN
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

bool ClientAlphaHandler::reassignMicroWallets(ClientConnection *iConnection, RPCMessage &iMessage)
{
    if( authenticateMessage(iMessage) ) {
        RPCMessageReassignMicroWalletsRequest               lRequest{iMessage};
        RPCMessageReassignMicroWalletsReply::ReplyCode      lReplyCode  =   RPCMessageReassignMicroWalletsReply::ReplyCode::InternalServerError1;

        auto                                    lDBI    = gApp->databaseInterface();

        if( lDBI ) {
            lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::UnknownFailure;
            // Preflight check
            // 1. Does the destination address exist?
            if( lDBI->addressExists(lRequest.destination()) ) {
                // 2. Does the source address exist?
                if( lDBI->addressExists(lRequest.username()) ) {
                    // 3. Does each wallet id exist and does the ownership match?
                    for( QString lWID : lRequest.microWalletIds() ) {
                        if( ! lDBI->microWalletExists(lWID) ) {
                            lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::OneOrMoreWalletsDoNotExist;
                            break;
                        }

                        if( ! lDBI->microWalletOwnershipCheck(lWID,lRequest.username()) ) {
                            lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::OneOrMoreWalletsUnauthorized;
                            break;
                        }
                    }

                    if(
                            lReplyCode != RPCMessageReassignMicroWalletsReply::ReplyCode::OneOrMoreWalletsDoNotExist &&
                            lReplyCode != RPCMessageReassignMicroWalletsReply::ReplyCode::OneOrMoreWalletsUnauthorized
                    ) {
                        // Ok lets do this
                        QStringList lMicroWalletsDone;
                        bool        lAbort = false;
                        for( QString lWID : lRequest.microWalletIds() ) {
                            if( lDBI->microWalletChangeOwnership(lWID,lRequest.username(),lRequest.destination()) ) {
                                lMicroWalletsDone << lWID;
                            }else{
                                qCritical() << "Attempted to transfer ownership of micro-wallet" << lWID << "from" << lRequest.username() << "to" << lRequest.destination() << "after all safety checks were performed but it failed. Manual intervention maybe needed.";
                                lAbort = true;
                                break;
                            }
                        }

                        if( lAbort ) {
                            lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::InternalServerError2;
                            // Ran into a problem, roll back the ownership if possible
                            for( QString lWID : lMicroWalletsDone ) {
                                qWarning() << "Rolling back transfer of ownership of micro-wallet" << lWID << "to original owner" << lRequest.username() << "from new owner" << lRequest.destination() << "due to failed transfer after safety checks.";
                                if( ! lDBI->microWalletChangeOwnership(lWID,lRequest.destination(),lRequest.username()) ) {
                                    lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::InternalServerError3;
                                    qCritical() << "Roll back of ownership of micro-wallet" << lWID << "to original owner" << lRequest.username() << "from new owner" << lRequest.destination() << "due to failed transfer after safety checks FAILED!!! Now we are in a critial state of this micro-wallet please manually correct ASAP.";
                                }
                            }
                        }else{
                            // We are good!
                            lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::Success;
                        }
                    }
                }else{
                    lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::SourceDoesNotExist;
                }
            }else{
                lReplyCode = RPCMessageReassignMicroWalletsReply::ReplyCode::DestinationDoesNotExist;
            }

            return iConnection->sendMessage(RPCMessageReassignMicroWalletsReply::create(lReplyCode,lRequest.transactionId(),QStringLiteral("Threshodl"),gApp->privateKeyPEM()));
        }
    }

    return false;
}
