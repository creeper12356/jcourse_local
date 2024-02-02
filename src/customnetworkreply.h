#ifndef SIMPLENETWORKREPLY_H
#define SIMPLENETWORKREPLY_H
#include "inc.h"
/*!
 * \brief 自定义的网络回应数据结构，主要用于记录请求历史
 */
class CustomNetworkReply
{
public:
    QString requestApi;
    QByteArray replyData;

    CustomNetworkReply();
    CustomNetworkReply(const QString& requestApi,const QByteArray& replyData);
    void readFromJsonObject(const QJsonObject& jsonObject);
    QJsonObject toJsonObject() const;
};

#endif // SIMPLENETWORKREPLY_H
