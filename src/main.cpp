#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client client(&a);
    if(client.initialize()){
        return a.exec();
    }
    else{
        return 0;
    }
    //使用本行会造成栈溢出
//    return client.initialize() ? a.exec() : 0;
}
