* 身份认证

    选课社区使用OAuth身份验证
    向服务器发送**选课社区**登录邮箱（不包含后缀）和密码，获得cookies
    ```
    type:POST 
    url:'https://course.sjtu.plus/oauth/email/login/'
    data:{
        'account': @youraccount,
        'password': @yourpassword
    }
    ```
    cookies位于```respond.headers['set-cookie']```中，初始为字符串类型，转成字典后结构如下:
    ```python
    {
        'csrftoken' : '@something',
        'sessionid' : '@something',
        """other key-value pairs"""
    }
    ```
    只需保留'csrftoken'和'sessionid'字段，向服务器请求信息时请求头加上该cookies即可。
    ```
    type:GET
    url:'https://course.sjtu.plus/whatever/data/you/request'
    cookies:{
        'csfrtoken': @something,
        'sessionid': @something
    }
    ```
https://course.sjtu.plus/api/course/16333/review/?&page=1&size=20

* 使用信号槽的问题

    在Qt中使用信号槽需要直接或间接继承于QObject , 并添加Q_OBJECT宏， 如果构建失败，删除构建目录重新构建即可解决

* Win10部署时运行报错：qt.network.ssl: QSslSocket::connectToHostEncrypted: TLS initialization failed
    原因：缺少Openssl动态链接库。
    解决：通过 QSslSocket::sslLibraryBuildVersionString()查看 Openssl版本信息，到https://slproweb.com/products/Win32OpenSSL.html下载对应版本，安装在bin目录，安装后，将libcrypto-1_1-x64.dll , libssl-1_1-x64.dll两个文件拷贝到可执行文件目录，即可正常运行。（详见https://blog.csdn.net/no_say_you_know/article/details/124045847）
    
