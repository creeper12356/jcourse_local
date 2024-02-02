#ifndef NETWORKREPLYHISTORY_H
#define NETWORKREPLYHISTORY_H
#include "circularqueue.h"
#include "customnetworkreply.h"
#include "inc.h"

/*!
 * \brief 封装CircularQueue<CustomNetworkReply>的类
 */
class NetworkReplyHistory : public CircularQueue<CustomNetworkReply>
{
public:
    NetworkReplyHistory();
    bool readFromFile(const QString& fileName);
    void writeToFile(const QString& fileName) const;

    void addNetworkReplyHistory(const QString& requestApi , const QByteArray& replyData);
    CustomNetworkReply* findNetworkReply(const QString& requestApi);
};

#endif // NETWORKREPLYHISTORY_H
