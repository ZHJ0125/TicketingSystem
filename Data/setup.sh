#!/bin/sh
# To Setup TicketingSystem program
# ZhangHoujin Created on 2020.07.16

echo "+-------------------------------------------------------+"
echo "|   Welcome to TicketingSystem's Setup program          |"
echo "|  ---------------------------------------------------  |"
echo "|   What to do:                                         |"
echo "|       1. Initializing the LAMP architecture           |"
echo "|       2. Create MySQL database                        |"
echo "|       3. database named 'linux'                       |"
echo "|       4. data table named 'tickets'                   |"
echo "|       5. data table password '666588'                 |"
echo "|  ---------------------------------------------------  |"
echo "|   Author: ZhangHoujin                                 |"
echo "|   Date: 2020.07.16                                    |"
echo "+-------------------------------------------------------+"
#----------------------------------------------------------------------
echo "Updating software source ..."
sudo apt-get update
sudo apt-get install gcc
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install libgl1-mesa-dev
echo "Software source update complete."
#----------------------------------------------------------------------
echo "Initializing the LAMP architecture ..."
sudo apt-get install apache2
sudo apt-get install php
sudo apt-get install mysql-client mysql-server
sudo apt-get install libmysqlclient-dev
sudo apt-get install phpmyadmin
sudo ln -s /usr/share/phpmyadmin /var/www/html/phpmyadmin
echo "LAMP architecture has been installed."
#----------------------------------------------------------------------
echo "Creating MySQL database ..."
sudo mysql -u root <<EOF
drop user 'zhj'@'localhost';
FLUSH PRIVILEGES;
CREATE USER 'zhj'@'localhost' IDENTIFIED BY '666588';
GRANT ALL PRIVILEGES ON *.* TO 'zhj'@'localhost' WITH GRANT OPTION;
FLUSH PRIVILEGES;
CREATE DATABASE linux;
SHOW databases;
QUIT;
EOF
echo "MySQL database has been created."
#----------------------------------------------------------------------
echo "Initializing MySQL database content ..."
gcc mysql_setup.c -o mysql_setup -lmysqlclient
./mysql_setup
sleep 1
rm mysql_setup
echo "The contents of the database have been initialized."
#----------------------------------------------------------------------
echo "+-------------------------------------------------------+"
echo "|   Welcome to TicketingSystem's Setup program          |"
echo "|  ---------------------------------------------------  |"
echo "|   What this script has done:                          |"
echo "|     ✔ 1. The LAMP architecture has been initialized.  |"
echo "|     ✔ 2. MySQL database has been created.             |"
echo "|     ✔ 3. database named 'linux'                       |"
echo "|     ✔ 4. data table named 'tickets'                   |"
echo "|     ✔ 5. data table password '666588'                 |"
echo "|  ---------------------------------------------------  |"
echo "|   Author: ZhangHoujin                                 |"
echo "|   Date: 2020.07.16                                    |"
echo "+-------------------------------------------------------+"
#-----------------------    End of File   -----------------------------
