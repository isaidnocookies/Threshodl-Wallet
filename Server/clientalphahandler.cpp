#include "clientalphahandler.h"
#include "rpcmessagepingrequest.h"
#include "rpcmessagepingreply.h"
#include "rpcmessagecreateaccountrequest.h"
#include "rpcmessagecreateaccountreply.h"

#include "app.h"
#include "digest.h"

#include <QDebug>

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
    }

    return false;
}
