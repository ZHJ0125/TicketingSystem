#include <stdio.h>
#include <mysql/mysql.h>
void printResult(MYSQL *mysql);
int main()
{
    int i;
    MYSQL mysql;
    MYSQL_RES * result;
    mysql_init(&mysql);
    mysql_real_connect(&mysql, "localhost", "zhj", "666588", "linux", 0, NULL, 0);
    mysql_query(&mysql, "create table tickets(flight_ID int AUTO_INCREMENT PRIMARY KEY, ticket_num int, ticket_price int)");
    mysql_query(&mysql, "insert into tickets values(1, 100, 300),(2, 100, 300),(3, 100, 300),(4, 100, 300),(5, 100, 300),(6, 100, 300),(7, 100, 300),(8, 100, 300),(9, 100, 300),(10, 100, 300)");
    mysql_query(&mysql, "select * from tickets");
    printf("--增加数据测试--\n");
    printResult(&mysql);

    // mysql_query(&mysql, "delete from tickets where flight_ID = 1");
    // mysql_query(&mysql, "select * from tickets");
    // printf("--删除数据测试--\n");
    // printResult(&mysql);

    // mysql_query(&mysql, "update tickets set ticket_price = 12345 where flight_ID = 2");
    // mysql_query(&mysql, "select * from tickets");
    // printf("--更新数据测试--\n");
    // printResult(&mysql);

    // mysql_query(&mysql, "TRUNCATE tickets");
    // mysql_query(&mysql, "select * from tickets");
    // printf("--清空数据测试--\n");
    // printResult(&mysql);
    // mysql_query(&mysql, "drop table tickets");    
    // mysql_query(&mysql, "drop database tickets");
    mysql_close(&mysql);
}
void printResult(MYSQL *mysqlPrint)//打印结果集(此处传入指针，而非内容)
{
    MYSQL_RES * result;
    int numFields = 0;
    int numRows = 0;
    MYSQL_FIELD * field;
    MYSQL_ROW row;
    int i = 0;
    result = mysql_store_result(mysqlPrint);//将查询的全部结果读取到客户端
    numFields = mysql_num_fields(result);//统计结果集中的字段数
    numRows = mysql_num_rows(result);//统计结果集的行数
    while(field = mysql_fetch_field(result))//返回结果集中的列信息(字段)
        printf("%s\t", field->name);
    printf("\n");
    if(result)
    {
        while(row = mysql_fetch_row(result))//返回结果集中行的记录
        {
            for(i = 0; i < numFields; i++)
            {
                printf("%s\t", row[i]);
            }
            printf("\n");
        }
    }
    mysql_free_result(result);//释放result空间，避免内存泄漏
}