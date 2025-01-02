#include "commonscpimethods.h"
#include "netmessages.pb.h"
#include "zscpi_response_definitions.h"
#include <scpicommand.h>
#include <QDataStream>

void CommonScpiMethods::sendProtoAnswer(QTcpSocket *telnetSocket,
                                        XiQNetWrapper *protobufWrapper,
                                        cProtonetCommand *protoCmd)
{
    if(protoCmd->m_pPeer == 0) {
        // we worked on a command comming from scpi socket connection
        QString answer = protoCmd->m_sOutput+"\n";
        QByteArray ba = answer.toLatin1();
        telnetSocket->write(ba);
        qInfo("External SCPI response: %s", qPrintable(answer));
    }
    else {
        if(protoCmd->m_bhasClientId) {
            ProtobufMessage::NetMessage protobufAnswer;
            ProtobufMessage::NetMessage::NetReply *netReply = protobufAnswer.mutable_reply();
            // dependent on rtype caller can see ack, nak, error
            // in case of error the body has to be analyzed for details
            QString output = protoCmd->m_sOutput;
            if (output.contains(ZSCPI::scpiAnswer[ZSCPI::ack]))
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::nak]))
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_NACK);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::busy]))
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::erraut]))
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errval]))
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errxml]))
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errpath])) // for zdspd only -> remove
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else if (output.contains(ZSCPI::scpiAnswer[ZSCPI::errexec]))
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ERROR);
            else
                netReply->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

            netReply->set_body(output.toStdString()); // in any case we set the body
            protobufAnswer.set_clientid(protoCmd->m_clientId, protoCmd->m_clientId.count());
            protobufAnswer.set_messagenr(protoCmd->m_nmessageNr);
            protoCmd->m_pPeer->sendMessage(protobufWrapper->protobufToByteArray(protobufAnswer));
        }
        else {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Qt_4_0);
            out << (qint32)0;

            out << protoCmd->m_sOutput.toUtf8();
            out.device()->seek(0);
            out << (qint32)(block.size() - sizeof(qint32));

            protoCmd->m_pPeer->writeRaw(block);
        }
    }
    delete protoCmd;
}

QString CommonScpiMethods::handleScpiInterfaceRead(std::shared_ptr<cSCPI> scpiInterface,
                                                   const QString &scpiInput)
{
    cSCPICommand cmd = scpiInput;
    if (cmd.isQuery()) {
        QString s;
        scpiInterface->exportSCPIModelXML(s);
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}
