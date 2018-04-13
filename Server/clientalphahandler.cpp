#include "clientalphahandler.h"
#include "rpcmessagepingrequest.h"
#include "rpcmessagepingreply.h"
#include "rpcmessagecreateaccountrequest.h"
#include "rpcmessagecreateaccountreply.h"
#include "rpcmessagereassignmicrowalletsrequest.h"
#include "rpcmessagereassignmicrowalletsreply.h"
#include "rpcmessagecreatemicrowalletpackagerequest.h"
#include "rpcmessagecreatemicrowalletpackagereply.h"

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
#warning THIS IS INCOMPLETE
    // Missing BTC transaction stuff

    if( authenticateMessage(iMessage) ) {
        auto lDBI = gApp->databaseInterface();

        if( lDBI ) {
            RPCMessageCreateMicroWalletPackageRequest   lRequest{iMessage};

            if( lRequest.cryptoTypeShortName() == QStringLiteral("btc") ) {
                std::vector<BValue> *   lValuesPtr = WalletGrinderAlpha::getBreaks(lRequest.cryptoValue());
                std::vector<BValue>     lValues;
                QStringList             lWalletValues;
                QList<BitcoinWallet>    lBTCWallets;
                QList<QByteArray>       lMicroWalletsData;


                if( lValuesPtr ) {
                    lValues = *lValuesPtr;

                    for( size_t lIndex = 0; lIndex < lValues.size(); lIndex++ ) {
                        lWalletValues << QString::number(WalletGrinderAlpha::getValueFromBValue(lValues[lIndex]));
                        lBTCWallets << BitcoinWallet::createNewBitcoinWallet();
                    }

                    delete lValuesPtr;

                    // Now create micro-wallets from the wallets
                    int         lIndex = 0;
                    QByteArray  lPrivKey;
                    QByteArray  lPrivKeyRight;
                    QByteArray  lPrivKeyLeft;
                    int         lPrivKeyLen;
                    int         lPrivKeyMid;
                    for( BitcoinWallet lMW : lBTCWallets ) {
                        lMW.setValue(lWalletValues[lIndex]);
                        lMW.setIsMicroWallet(true);

                        lPrivKey = lMW.privateKey();
                        lPrivKeyLen = lPrivKey.size();
                        lPrivKeyMid = lPrivKeyLen / 2;
                        lPrivKeyLeft = lPrivKey.left(lPrivKeyMid-1);
                        lPrivKeyRight = lPrivKey.mid(lPrivKeyMid);
                        lMW.setPrivateKey(lPrivKeyLeft);
                        lMW.setWif(QByteArray()); // Clear the Wif.

                        // Store PrivKeyRight
                    }

                }else{
                    qWarning() << "Unable to grind up crypto currency into Micro-Wallets.";
                }
            }else{
                qWarning() << "Unhandled crypto type for creating Micro-Wallets.";
            }
        }else{
            qWarning() << "Unable to connect to database!";
        }
    }else{
        qWarning() << "Unable to authenticate message.";
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
