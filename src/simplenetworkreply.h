#ifndef SIMPLENETWORKREPLY_H
#define SIMPLENETWORKREPLY_H
#include "inc.h"
//简化的网络回应数据结构，用于记录GET请求历史
class SimpleNetworkReply
{
public:
    QString requestApi;
    QByteArray replyData;

    SimpleNetworkReply();
    SimpleNetworkReply(const QString& requestApi,const QByteArray& replyData);
    void readFromJsonObject(const QJsonObject& jsonObject);
    QJsonObject toJsonObject() const;
};

#endif // SIMPLENETWORKREPLY_H
