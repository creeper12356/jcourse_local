#include "networkreplyhistory.h"

NetworkReplyHistory::NetworkReplyHistory()
    : CircularQueue<CustomNetworkReply> (HISTORY_SIZE)
{

}

bool NetworkReplyHistory::readFromFile(const QString &fileName)
{
    QFile replyHistoryReader(fileName);
    if(!replyHistoryReader.open(QIODevice::ReadOnly)) {
        return false;
    }
    QJsonArray replyHistoryJsonArray = QJsonDocument::fromJson(replyHistoryReader.readAll()).array();
    replyHistoryReader.close();
    for(auto it = replyHistoryJsonArray.begin();it != replyHistoryJsonArray.end();++it) {
        //TODO : 优化
        CustomNetworkReply reply;
        reply.readFromJsonObject((*it).toObject());
        this->push(reply);
    }

    return true;
}

void NetworkReplyHistory::writeToFile(const QString &fileName) const
{
    QJsonArray replyHistoryJsonArray;
    for(auto it = this->begin();it != this->end();++it) {
        replyHistoryJsonArray.append((*it).toJsonObject());
    }
    QFile replyHistoryWriter(fileName);
    replyHistoryWriter.open(QIODevice::WriteOnly);
    replyHistoryWriter.write(QJsonDocument(replyHistoryJsonArray).toJson());
    replyHistoryWriter.close();
}

void NetworkReplyHistory::addNetworkReplyHistory(const QString &requestApi, const QByteArray &replyData)
{
    data[back].requestApi = requestApi;
    data[back].replyData = replyData;

    back = (back + 1) % capacity;
    if(back == front){
        //队列已满
        front = (front + 1) % capacity;
    }
}

CustomNetworkReply *NetworkReplyHistory::findNetworkReply(const QString &requestApi)
{
    for(int i = front;i != back;i = (i + 1) % capacity) {
        if(requestApi == data[i].requestApi) {
            return data + i;
        }
    }
    return nullptr;
}

