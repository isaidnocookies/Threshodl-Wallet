#include "wcpclient.h"
#include "wcpserverhandler.h"
#include "wcpmessages.h"
#include "digest.h"
#include "app.h"

void WCPClient::_getBTCFees(BitcoinWallet::ChainType iChain, QStringMath &oBaseFee, QStringMath &oInputFee, QStringMath &oOutputFee)
{
    QStringList                     lFeeGroup;
    QMap< QString, QStringMath >    lFeeResult;

    switch( iChain ) {
    case BitcoinWallet::ChainType::TestNet:
        lFeeGroup = QStringList()
               << mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::TestNetBaseFee)
               << mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::TestNetInputFee)
               << mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::TestNetOutputFee);
        lFeeResult = mServerV1->feeEstimator()->getFees(QStringLiteral("BTC"), lFeeGroup);
        oBaseFee = lFeeResult[mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::TestNetBaseFee)];
        oInputFee = lFeeResult[mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::TestNetInputFee)];
        oOutputFee = lFeeResult[mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::TestNetOutputFee)];
        break;

    case BitcoinWallet::ChainType::Main:
    default:
        lFeeGroup = QStringList()
               << mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::MainNetBaseFee)
               << mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::MainNetInputFee)
               << mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::MainNetOutputFee);
        lFeeResult = mServerV1->feeEstimator()->getFees(QStringLiteral("BTC"), lFeeGroup);
        oBaseFee = lFeeResult[mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::MainNetBaseFee)];
        oInputFee = lFeeResult[mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::MainNetInputFee)];
        oOutputFee = lFeeResult[mServerV1->feeEstimator()->commonTypeToString(CommonFeeType::MainNetOutputFee)];
        break;
    }
}

bool WCPClient::_authenticateMessage(const WCPMessage &iMessage)
{
    QByteArray      lPubKey     = mServerV1->database()->publicKeyForAddress(iMessage.username());
    if( ! lPubKey.isEmpty() && iMessage.signatureKeyEncoding() == WCPMessage::KeyEncoding::SHA512 ) {
        return Digest::verify(lPubKey, iMessage.dataForSignature(), iMessage.signature(), Digest::HashTypes::SHA512);
    }
    return false;
}

void WCPClient::_createMicroWallets(const WCPMessage &iMessage)
{
    WCPMessageCreateMicroWalletPackageRequest   lReq{iMessage};

    if( lReq.cryptoTypeShortName().toLower() == QStringLiteral("btc") ) {
        _createMicroWalletsBTC(iMessage);
        return;
    }

    sendMessage(WCPMessageCreateMicroWalletPackageReply::create(
                    WCPMessageCreateMicroWalletPackageReply::ReplyCode::UnhandledCryptoType,
                    QList<QByteArray>(),
                    lReq.transactionId(),
                    mServerV1->app()->productFQDN(),
                    mServerV1->app()->serverPrivateKeyPEM()
                    ));
}

void WCPClient::_createMicroWalletsBTC(const WCPMessage &iMessage)
{
    WCPMessageCreateMicroWalletPackageReply::ReplyCode  lReplyCode          = WCPMessageCreateMicroWalletPackageReply::ReplyCode::Unauthorized;
    WCPMessageCreateMicroWalletPackageRequest           lRequest{iMessage};
    QList<QByteArray>                                   lMicroWalletsData;
    QStringMath                                         lEstimatedFees;
    QStringList                                         lGrinderList;
    QStringMath                                         lBaseFee;
    QStringMath                                         lInputFee;
    QStringMath                                         lOutputFee;
    QStringMath                                         lPostFeeValue;
    QList<BitcoinWallet>                                lBTCWallets;
    QString                                             lWalletIdPrefix;
    quint64                                             lWalletIdPrefixValue;
    qint64                                              lNow;
    QString                                             lWalletId;
    QMap< QString, QByteArray >                         lStoreInDB;

    if( _authenticateMessage(iMessage) ) {
        lReplyCode      = WCPMessageCreateMicroWalletPackageReply::ReplyCode::UnknownFailure;
        lGrinderList    = mServerV1->grinder()->grindValues(lRequest.cryptoValue(),QStringLiteral("BTC"));
        if( ! lGrinderList.isEmpty() ) {
            // Now we have a rough idea of how many wallets we would need, we need to estimate the fees with the given inputs and outputs
            _getBTCFees(lRequest.chainType(), lBaseFee, lInputFee, lOutputFee);
            lInputFee       = lInputFee.multiply(lRequest.inputCount());
            lOutputFee      = lOutputFee.multiply(lRequest.outputCount() + lGrinderList.size());
            lEstimatedFees  = lBaseFee.add(lInputFee).add(lOutputFee);
            lEstimatedFees  = QStringMath::roundUpToNearest0001(lEstimatedFees.toString());
            lPostFeeValue   = QStringMath(lRequest.cryptoValue());
            lPostFeeValue   = lPostFeeValue.subtract(lEstimatedFees);

            // Now grind that up
            lGrinderList    = mServerV1->grinder()->grindValues(lPostFeeValue.toString(),QStringLiteral("BTC"));

            if( mServerV1->database()->microWalletAcquireFreeWalletIdPrefixBlock( static_cast<unsigned int>(lGrinderList.size()), lWalletIdPrefix ) ) {
                lWalletIdPrefixValue = lWalletIdPrefix.toULongLong();

                // Now create the wallets
                for( int lIndex = 0; lIndex < lGrinderList.size(); lIndex++ )
                { lBTCWallets << BitcoinWallet::createNewBitcoinWallet(lRequest.chainType()); }

                lNow = QDateTime::currentSecsSinceEpoch();

                int         lIndex = 0;
                QByteArray  lPrivKey;
                QByteArray  lPrivKeyRight;
                QByteArray  lPrivKeyLeft;
                int         lPrivKeyLen;
                int         lPrivKeyMid;

                for( BitcoinWallet & lMW : lBTCWallets ) {
                    lMW.setValue(lGrinderList[lIndex]);
                    lMW.setIsMicroWallet(true);

                    lPrivKey        = lMW.privateKey();
                    lPrivKeyLen     = lPrivKey.size();
                    lPrivKeyMid     = lPrivKeyLen / 2;
                    lPrivKeyLeft    = lPrivKey.left(lPrivKeyMid);
                    lPrivKeyRight   = lPrivKey.mid(lPrivKeyMid);

                    lMW.setPrivateKey(lPrivKeyLeft);
                    lMW.setWif(QByteArray()); // Clear the Wif.

                    // Generate the WalletId
                    lWalletId = QStringLiteral("%1.%2").arg(lWalletIdPrefixValue,16,16,QChar{'0'}).arg(lNow);
                    lMW.setWalletId(lWalletId);
                    lStoreInDB[lWalletId] = lPrivKeyRight;
                    lMicroWalletsData << lMW.toData();
                    lWalletIdPrefixValue++;
                    lIndex++;
                }

                // Store the wallet data in the db
                if( mServerV1->database()->microWalletScratchCreates(lStoreInDB,lRequest.username(),lNow) ) {
                    lReplyCode = WCPMessageCreateMicroWalletPackageReply::ReplyCode::Success;
                }else{
                    lReplyCode = WCPMessageCreateMicroWalletPackageReply::ReplyCode::InternalServerError3;
                }
            }else{
                lReplyCode = WCPMessageCreateMicroWalletPackageReply::ReplyCode::InternalServerError2;
            }
        }else{
            lReplyCode = WCPMessageCreateMicroWalletPackageReply::ReplyCode::UnableToGrindUpCryptoCurrency;
        }
    }else{
        lReplyCode = WCPMessageCreateMicroWalletPackageReply::ReplyCode::InternalServerError1;
    }

    sendMessage(WCPMessageCreateMicroWalletPackageReply::createBtc(
                    lReplyCode,
                    lMicroWalletsData,
                    lRequest.transactionId(),
                    lEstimatedFees.toString(),
                    mServerV1->app()->productFQDN(),
                    mServerV1->app()->serverPrivateKeyPEM()
                    ));
}

void WCPClient::_claimNewMicroWallets(const WCPMessage &iMessage)
{
    WCPMessageClaimNewMicroWalletsRequest           lRequest{iMessage};
    WCPMessageClaimNewMicroWalletsReply::ReplyCode  lReplyCode              = WCPMessageClaimNewMicroWalletsReply::ReplyCode::Unauthorized;
    QStringList                                     lClaimedWalletIds;

    if( _authenticateMessage(iMessage) ) {
        lReplyCode = WCPMessageClaimNewMicroWalletsReply::ReplyCode::InternalServerError1;
        if( mServerV1->database()->microWalletMoveFromScratch(lRequest.walletIds(),lRequest.username()) ) {
            lReplyCode = WCPMessageClaimNewMicroWalletsReply::ReplyCode::Success;
            lClaimedWalletIds = lRequest.walletIds();
        }else{
            lReplyCode = WCPMessageClaimNewMicroWalletsReply::ReplyCode::OneOrMoreMicroWalletsDoNotExist;
        }
    }

    sendMessage(WCPMessageClaimNewMicroWalletsReply::create(
                    lReplyCode,
                    lClaimedWalletIds,
                    lRequest.transactionId(),
                    mServerV1->app()->productFQDN(),
                    mServerV1->app()->serverPrivateKeyPEM()
                    ));
}

void WCPClient::_checkOwnershipOfMicroWallets(const WCPMessage &iMessage)
{
    WCPMessageCheckOwnershipOfMicroWalletsRequest           lRequest{iMessage};
    WCPMessageCheckOwnershipOfMicroWalletsReply::ReplyCode  lReplyCode              = WCPMessageCheckOwnershipOfMicroWalletsReply::ReplyCode::UnknownFailure;

    if( mServerV1->database()->microWalletsExists(lRequest.walletIds()) ) {
        if( mServerV1->database()->microWalletsOwnershipCheck(lRequest.walletIds(),lRequest.owner()) ) {
            lReplyCode = WCPMessageCheckOwnershipOfMicroWalletsReply::ReplyCode::Success;
        }else{
            lReplyCode = WCPMessageCheckOwnershipOfMicroWalletsReply::ReplyCode::OneOrMoreUnauthorized;
        }
    }else{
        lReplyCode = WCPMessageCheckOwnershipOfMicroWalletsReply::ReplyCode::OneOrMoreMicroWalletsDoNotExist;
    }

    sendMessage(WCPMessageCheckOwnershipOfMicroWalletsReply::create(
                    lReplyCode,
                    lRequest.transactionId(),
                    mServerV1->app()->productFQDN(),
                    mServerV1->app()->serverPrivateKeyPEM()
                    ));
}

void WCPClient::_completeMicroWallets(const WCPMessage &iMessage)
{
    WCPMessageCompleteMicroWalletsRequest           lRequest{iMessage};
    WCPMessageCompleteMicroWalletsReply::ReplyCode  lReplyCode              = WCPMessageCompleteMicroWalletsReply::ReplyCode::UnknownFailure;
    QMap< QString, QByteArray >                     lWalletPartialKeys;
    QMap< QString, QByteArray >                     lWalletPartialKeysTemp;

    if( _authenticateMessage(iMessage) ) {
        lWalletPartialKeysTemp = mServerV1->database()->microWalletsCopyPayload(lRequest.walletIds(),lRequest.username());
        if( mServerV1->database()->microWalletsExists(lRequest.walletIds()) ) {
            if( mServerV1->database()->microWalletsOwnershipCheck(lRequest.walletIds(),lRequest.username()) ) {
                if( mServerV1->database()->microWalletsDelete(lRequest.walletIds(),lRequest.username()) ) {
                    if( lWalletPartialKeysTemp.size() == lRequest.walletIds().size() ) {
                        lWalletPartialKeys = lWalletPartialKeysTemp;
                        lReplyCode = WCPMessageCompleteMicroWalletsReply::ReplyCode::Success;
                    }else{
                        lReplyCode = WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError1;
                    }
                }else{
                    lReplyCode = WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError2;
                }
            }else{
                lReplyCode = WCPMessageCompleteMicroWalletsReply::ReplyCode::OneOrMoreUnauthorized;
            }
        }else{
            lReplyCode = WCPMessageCompleteMicroWalletsReply::ReplyCode::OneOrMoreMicroWalletsDoNotExist;
        }
    }

    sendMessage(WCPMessageCompleteMicroWalletsReply::create(
                    lReplyCode,
                    lWalletPartialKeys,
                    lRequest.transactionId(),
                    mServerV1->app()->productFQDN(),
                    mServerV1->app()->serverPrivateKeyPEM()
                    ));
}

void WCPClient::_createAccount(const WCPMessage &iMessage)
{
    WCPMessageCreateAccountRequest          lRequest{iMessage};
    WCPMessageCreateAccountReply::ReplyCode lReplyCode              = WCPMessageCreateAccountReply::ReplyCode::UnknownFailure;
    QByteArray                              lPublicKey              = lRequest.publicKey();
    QString                                 lReplyUsername          = mServerV1->database()->sanatizedUsername(lRequest.username());

    if( ! mServerV1->database()->addressExists(lReplyUsername) ) {
        if( lRequest.signatureKeyEncoding() == WCPMessage::KeyEncoding::SHA512 ) {
            if( Digest::verify(lPublicKey,lRequest.dataForSignature(),lRequest.signature(),Digest::HashTypes::SHA512) ) {
                if( mServerV1->database()->addressCreate(lReplyUsername,lPublicKey) ) {
                    lReplyCode = WCPMessageCreateAccountReply::ReplyCode::Success;
                }else{
                    lReplyCode = WCPMessageCreateAccountReply::ReplyCode::UsernameTaken;
                }
            }else{
                lReplyCode = WCPMessageCreateAccountReply::ReplyCode::KeyInvalid;
            }
        }else{
            lReplyCode = WCPMessageCreateAccountReply::ReplyCode::KeyTypeInvalid;
        }
    }else{
        lReplyCode = WCPMessageCreateAccountReply::ReplyCode::UsernameTaken;
    }

    sendMessage(WCPMessageCreateAccountReply::create(
                    lReplyCode,
                    lReplyUsername,
                    mServerV1->app()->productFQDN(),
                    mServerV1->app()->serverPrivateKeyPEM()
                    ));
}

void WCPClient::_reassignMicroWallets(const WCPMessage &iMessage)
{
    WCPMessageReassignMicroWalletsRequest           lRequest{iMessage};
    WCPMessageReassignMicroWalletsReply::ReplyCode  lReplyCode          = WCPMessageReassignMicroWalletsReply::ReplyCode::UnknownFailure;

    if( _authenticateMessage(iMessage) ) {
        if( mServerV1->database()->microWalletsChangeOwnership(lRequest.microWalletIds(),lRequest.username(),lRequest.destination()) ) {
            lReplyCode = WCPMessageReassignMicroWalletsReply::ReplyCode::Success;
        }else{
            lReplyCode = WCPMessageReassignMicroWalletsReply::ReplyCode::Failure;
        }
    }

    sendMessage(WCPMessageReassignMicroWalletsReply::create(
                    lReplyCode,
                    lRequest.transactionId(),
                    mServerV1->app()->productFQDN(),
                    mServerV1->app()->serverPrivateKeyPEM()
                    ));
}

void WCPClient::_ping(const WCPMessage &iMessage)
{
    WCPMessagePingRequest   lRequest{iMessage};
    sendMessage(WCPMessagePingReply::create(lRequest.payload(),mServerV1->app()->productFQDN()));
}

WCPClient::WCPClient(WCPConnection *iConnection, WCPServerHandler *iServer, QObject *iParent)
    : WCPClientInterface(iConnection,iServer,iParent), mServerV1(iServer)
{ }

void WCPClient::processMessage(const QString iWCPVersion, const QString iCommand, const WCPMessage &iMessage)
{
    if( iWCPVersion != kWCPVersionV2 ) {
        mConnection->close();
        return;
    }

    if( iCommand == WCPMessageCreateMicroWalletPackageRequest::commandValue() ) {
        _createMicroWallets(iMessage);
    } else if( iCommand == WCPMessageClaimNewMicroWalletsRequest::commandValue() ) {
        _claimNewMicroWallets(iMessage);
    } else if( iCommand == WCPMessageCheckOwnershipOfMicroWalletsRequest::commandValue() ) {
        _checkOwnershipOfMicroWallets(iMessage);
    } else if( iCommand == WCPMessageCompleteMicroWalletsRequest::commandValue() ) {
        _completeMicroWallets(iMessage);
    } else if( iCommand == WCPMessageCreateAccountRequest::commandValue() ) {
        _createAccount(iMessage);
    } else if( iCommand == WCPMessageReassignMicroWalletsRequest::commandValue() ) {
        _reassignMicroWallets(iMessage);
    } else if( iCommand == WCPMessagePingRequest::commandValue() ) {
        _ping(iMessage);
    } else {
        mConnection->close();
    }
}
