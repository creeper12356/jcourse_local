#include "client.h"
int main(int argc, char *argv[])
{
    /* comment this line when building release */
    QDir::setCurrent("..");

    QApplication a(argc, argv);
    Client client(&a);
    return a.exec();
}
