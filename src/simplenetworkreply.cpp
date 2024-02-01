#include "simplenetworkreply.h"

SimpleNetworkReply::SimpleNetworkReply()
{

}

SimpleNetworkReply::SimpleNetworkReply(const QString &arg_requestApi, const QByteArray &arg_replyData)
    : requestApi(arg_requestApi) , replyData(arg_replyData)
{

}

void SimpleNetworkReply::readFromJsonObject(const QJsonObject &jsonObject)
{
    requestApi = jsonObject["requestApi"].toString();
    replyData = jsonObject["replyData"].toString().toUtf8();
}

QJsonObject SimpleNetworkReply::toJsonObject() const
{
    QJsonObject obj;
    obj.insert("requestApi",requestApi);
    obj.insert("replyData",QString::fromUtf8(replyData));
    return obj;
}

