### TODO List
* 实现拼音搜索
* 处理同名教师的情形
* 适配不同大小的窗口


### 问题和解决

* 使用信号槽的问题

    在Qt中使用信号槽需要直接或间接继承于QObject , 并添加Q_OBJECT宏， 如果构建失败，删除构建目录重新构建即可解决

* Win10部署时运行报错：qt.network.ssl: QSslSocket::connectToHostEncrypted: TLS initialization failed
    原因：缺少Openssl动态链接库。
    解决：通过 QSslSocket::sslLibraryBuildVersionString()查看 Openssl版本信息，到https://slproweb.com/products/Win32OpenSSL.html下载对应版本，安装在bin目录，安装后，将libcrypto-1_1-x64.dll , libssl-1_1-x64.dll两个文件拷贝到可执行文件目录，即可正常运行。（详见https://blog.csdn.net/no_say_you_know/article/details/124045847）
    
