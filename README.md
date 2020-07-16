# TicketingSystem

Linux C 机票网络售票系统

本项目中的航班信息存储使用了MySQL数据库，数据库的前端显示使用了LAMP架构，运行程序前需要进行相关配置

## 一、环境配置

环境配置可以选择使用**自动部署脚本**或者进行**手动配置**，手动配置请跳转至[此链接](Data/manual_config.md)

自动部署方式直接运行`Data/setup.sh`脚本即可，例子如下

![Setup](Data/images/setup.png)

自动部署方式会实现LAMP安装并完成MySQL的相关配置，执行完成后的终端界面如下所示

![autoconfig](Data/images/autoconfig.png)

之后您可以根据上述提示，登录PHPMyAdmin界面查看具体的数据库内容

执行完上述内容后，数据库和LAMP的部分就已经配置好了

接下来您可以打开`Qt for Linux`软件，运行`TicketingSystem_Server`、`TicketingSystem_Client`和`TicketingSystem_Sell`三个文件夹中的QT程序进行测试

---

## 二、项目展示

👉 [航班网络售票模拟系统展示视频 - BiliBili](https://www.bilibili.com/video/BV14T4y1J7bt/) 👈

> 由于众所周知的原因，GitHub仓库里的图片可能加载不出来。您可以前往[本项目的Gitee仓库](https://gitee.com/zhj0125/TicketingSystem)，查看相关图片内容。

### 2.1 服务端

![server](Data/images/server.png)

### 2.2 客户购票端

* 客户购票端欢迎界面

![welcome](Data/images/welcome.png)

* 客户购票端

![client](Data/images/client.png)

### 2.3 管理员售票端

* 管理员售票端登录界面

![login](Data/images/login.png)

* 管理员售票端

![admin](Data/images/admin.png)

### 2.4 数据库界面

![mysql](Data/images/mysql.png)

### 2.5 关于我们

![info](Data/images/info.png)

---

## 三、项目代码说明

Gitee 仓库地址（推荐）： [https://gitee.com/zhj0125/TicketingSystem](https://gitee.com/zhj0125/TicketingSystem)

GitHub 仓库地址： [https://github.com/ZHJ0125/TicketingSystem](https://github.com/ZHJ0125/TicketingSystem)

该代码是在《LINUX C编程从入门到精通》（刘学勇编著 ISBN:978-7-121-17415-5）书中例题的基础上修改的。如果您对代码有任何疑问或修改意见，欢迎提出[issue](https://gitee.com/zhj0125/TicketingSystem/issues) 或者直接私信我们，谢谢。
