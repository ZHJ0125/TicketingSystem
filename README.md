# TicketingSystem

Linux C 机票网络售票系统

本项目中的航班信息存储使用了MySQL数据库，数据库的前端显示使用了LAMP架构，运行程序前需要进行相关配置

## 一、环境配置

环境配置可以选择使用**自动部署脚本**或者**自行手动配置**，手动配置脚本请[点击此链接](Data/manual_config.md)

自动部署方式直接运行Data/setup.sh脚本即可，例子如下

```bash
~/TicketingSystem$ cd Data/
~/TicketingSystem/Data$ ls
images  mysql_setup.c  Report  setup.sh
~/TicketingSystem/Data$ bash setup.sh 
```

自动部署方式会执行LAMP安装及MySQL的相关配置脚本，执行完成后的终端界面如下所示

```shell
+---------------------------------------------------------------+
|         Welcome to TicketingSystem's Setup Program            |
|                        Version: V0.3                          |
|  -----------------------------------------------------------  |
|     What this script has done:                                |
|      ✔ 1. The LAMP architecture has been initialized.         |
|      ✔ 2. MySQL database has been created.                    |
|      ✔ 3. database named 'linux'                              |
|      ✔ 4. data table named 'tickets'                          |
|      ✔ 5. data table password '666588'                        |
|  -----------------------------------------------------------  |
|     Then you should use the 'ifconfig' statement to           |
|     obtain a local IP address.                                |
|     Enter the {youripaddr}/phpmyadmin in browser.             |
|     e.g. '192.168.96.131/phpmyadmin'                          |
|     Enter the username zhj in phpmyadmin login interface.     |
|     The password is 666588                                    |
|  -----------------------------------------------------------  |
|                                         Author: ZhangHoujin   |
|                                           Date: 2020.07.16    |
+---------------------------------------------------------------+
```

之后您可以根据上述提示，登录PHPMyAdmin界面查看具体的数据库内容

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
