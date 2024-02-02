#include "customnetworkreply.h"

CustomNetworkReply::CustomNetworkReply()
{

}

CustomNetworkReply::CustomNetworkReply(const QString &arg_requestApi, const QByteArray &arg_replyData)
    : requestApi(arg_requestApi) , replyData(arg_replyData)
{

}

void CustomNetworkReply::readFromJsonObject(const QJsonObject &jsonObject)
{
    requestApi = jsonObject["requestApi"].toString();
    replyData = jsonObject["replyData"].toString().toUtf8();
}

QJsonObject CustomNetworkReply::toJsonObject() const
{
    QJsonObject obj;
    obj.insert("requestApi",requestApi);
    obj.insert("replyData",QString::fromUtf8(replyData));
    return obj;
}

