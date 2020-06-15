/*server.c*/
#include "servicethread.h"

int listen_fd, conn_fd; 	//监听socket，连接socket
struct sockaddr_in server, cli_addr;			//服务器地址信息，客户端地址信息
int ret, buffer_index, i;
socklen_t cli_len;
unsigned long ip_addr;
int flag=1;
pthread_t listentid,servicetid;						//监听线程ID,服务线程ID；

static  GtkWidget 	*app;						/*程序主窗口*/
static  GtkWidget	*frame, *vbox, *box2 ,*table;	/*box2用来封装文本构件与垂直滚动条*/
static  GtkWidget	*view  ;	                //服务器文本信息输出窗口，用来输出相关提示信息。

static  GtkTextIter iter;
/*监听线程*/
void * listen_thread(void *p)
{
	char msg[512];
	while(1) {
		/*接受远端的TCP连接请求*/
		cli_len=sizeof(cli_addr);
		conn_fd=accept(listen_fd,(struct sockaddr *)&cli_addr, &cli_len);
		if(conn_fd<0)
			continue;

		ip_addr=ntohl(cli_addr.sin_addr.s_addr);
		/*检测重复连接*/
		//check_connection(ip_addr);
		/*分配线程缓冲区*/
		buffer_index=get_free_buff();
		if(buffer_index<0) {
			sprintf(msg,"没用空闲的线程缓冲区。\n");
			add_info(msg);
			close(conn_fd);
			continue;
		}
		/*填写服务线程需要的信息*/
		pthread_mutex_lock(&buff_mutex);
		thread_buff[buffer_index].buff_index=buffer_index;
		thread_buff[buffer_index].ip_addr=ip_addr;
		thread_buff[buffer_index].conn_fd=conn_fd;
		thread_buff[buffer_index].buff_status=BUFF_OCCUPIED;
		pthread_mutex_unlock(&buff_mutex);

		/*创建新的服务线程*/
		ret=pthread_create(&servicetid, NULL, service_thread, &thread_buff[buffer_index]);
		if(ret==-1) {
			sprintf(msg,"创建服务线程出错！\n");
			add_info(msg);
			close(conn_fd);
			/*释放线程缓冲区*/
			pthread_mutex_lock(&buff_mutex);
			thread_buff[buffer_index].tid=0;
			thread_buff[buffer_index].buff_status=BUFF_FREED;
			pthread_mutex_unlock(&buff_mutex);
		}
	}
}

int isserveropened=FALSE;		//服务器端是否开启标志位

void startserver(GtkWidget *widget, gpointer data);
void stopserver(GtkWidget *widget, gpointer data);
void inquireone();
void inquireall();
void displaycontents(GtkWidget *widget, gpointer data);
void about(GtkWidget *widget, gpointer data);

/*生成服务器操作菜单项*/
GnomeUIInfo server_operation_menu[]={
	GNOMEUIINFO_ITEM_NONE("开启服务器" ,"开启服务器", startserver),
	GNOMEUIINFO_ITEM_NONE("关闭服务器" ,"关闭服务器", stopserver),
	GNOMEUIINFO_ITEM_NONE("退出" ,"退出程序", gtk_main_quit),
	GNOMEUIINFO_END
};

/*生成航班查询菜单项*/
GnomeUIInfo inquire_menu[]={
	GNOMEUIINFO_ITEM_NONE("特定航班查询" ,"查询某一特定航班机票信息", inquireone),
	GNOMEUIINFO_ITEM_NONE("所有航班查询" ,"查询所有航班机票信息", inquireall),
	GNOMEUIINFO_END
};

/*生成帮助菜单项*/
GnomeUIInfo help_menu[]={
	GNOMEUIINFO_ITEM_NONE("显示内容" ,"显示帮助内容", displaycontents),
	GNOMEUIINFO_ITEM_NONE("关于" ,"关于此程序说明", about),
	GNOMEUIINFO_END
};

/*生成顶层菜单项*/
GnomeUIInfo menubar[]= {
	GNOMEUIINFO_SUBTREE("服务器操作(_S)", server_operation_menu),
	GNOMEUIINFO_SUBTREE("机票查询(_Q)", inquire_menu),
	GNOMEUIINFO_SUBTREE("帮助(_H)", help_menu),
	GNOMEUIINFO_END
};

GnomeUIInfo toolbar[]= {
	GNOMEUIINFO_ITEM_STOCK("开启服务器","开启服务器", startserver, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_ITEM_STOCK("关闭服务器","关闭服务器", stopserver, GNOME_STOCK_PIXMAP_PREFERENCES),
	GNOMEUIINFO_ITEM_STOCK("特定航班查询","查询某一特定航班机票信息", inquireone, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("所有航班查询","查询所有航班机票信息", inquireall, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("退出", "退出程序", gtk_main_quit, GNOME_STOCK_PIXMAP_EXIT),
	GNOMEUIINFO_END
};

/*消息内容输出函数*/
void display_info(char *msg, GtkWidget *view)
{
	GtkTextBuffer *buffer;              //服务器提示信息缓冲区；
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);
	gtk_text_buffer_insert(buffer, &iter, msg, -1);
}

/*开启服务器操作*/
void startserver(GtkWidget *widget, gpointer data)
{
	char msg[512];		//提示信息
	GtkWidget *isenable;

	/*初始化数据结构*/
	init_thread_buff();
	init_ticket_list();

	if(!isserveropened)
	{
		/*创建套接字*/
		listen_fd=socket(AF_INET,SOCK_STREAM,0);
		if(listen_fd<0) {
			sprintf(msg,"创建监听套接字出错！ \n");
			display_info(msg,view);
			return;
		}

		/*填写服务器的地址信息*/
		server.sin_family=AF_INET;
		server.sin_addr.s_addr=htonl(INADDR_ANY);
		server.sin_port=htons(SERVER_PORT_NO);

		setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,(void *)&flag,sizeof(int));

		/*绑定端口*/
		ret=bind(listen_fd,(struct sockaddr*)&server, sizeof(server));
		if(ret<0) {
			sprintf(msg,"绑定TCP端口：%d 出错！ \n",SERVER_PORT_NO);
			display_info(msg,view);
			close(listen_fd);
			return;
		}

		/*转化成倾听套接字*/
		listen(listen_fd,5);
		ret=pthread_create(&listentid, NULL, listen_thread, NULL);
		if(ret==-1) {
			sprintf(msg,"创建监听线程出错！ \n");
			display_info(msg,view);
			close(listen_fd);
			return;
		}

		//成功后输出提示信息
		sprintf(msg,"服务器端开启成功！服务器在端口：%d 准备接受连接！ \n",SERVER_PORT_NO);
		display_info(msg,view);
		isserveropened=TRUE;

		/*开启服务器菜单项和工具条灰化*/
		isenable=toolbar[0].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
		isenable=server_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable,FALSE);

		/*关闭服务器菜单项和工具条使能*/
		isenable=toolbar[1].widget;
		gtk_widget_set_sensitive(isenable,TRUE);
		isenable=server_operation_menu[1].widget;
		gtk_widget_set_sensitive(isenable,TRUE);

	}
}

/*关闭服务器操作*/
void stopserver(GtkWidget *widget, gpointer data)
{
	GtkWidget	*isenable;
	char msg[512];
	if(isserveropened)
	{

		pthread_mutex_lock(&buff_mutex);
		for(i=0;i<THREAD_BUFF_NUM;i++) {
			if(thread_buff[i].buff_status!=BUFF_FREED) {
				/*退出服务线程*/
				pthread_cancel(thread_buff[i].tid);
				pthread_join(thread_buff[i].tid,NULL);
				/*退出的线程不释放它的缓冲区，释放工作由主线程来处理*/
				thread_buff[i].tid=0;
				thread_buff[i].buff_status=BUFF_FREED;
				close(thread_buff[i].conn_fd);
			}
		}

		pthread_mutex_unlock(&buff_mutex);
		pthread_cancel(listentid);
		pthread_join(listentid,NULL);
		close(listen_fd);
		sprintf(msg,"服务器端成功关闭！ \n");
		display_info(msg,view);
		isserveropened=FALSE;

		/*开启服务器菜单项和工具条使能*/
		isenable=toolbar[0].widget;
		gtk_widget_set_sensitive(isenable,TRUE);
		isenable=server_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable,TRUE);

		/*关闭服务器菜单项和工具条灰化*/
		isenable=toolbar[1].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
		isenable=server_operation_menu[1].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
	}
}
/*查询特定航班对话框的“确定”“取消”按钮的回调函数*/
int button_inquireone(GnomeDialog *dialog, gint id,gpointer data)
{
	GtkWidget *flight=data;
	GtkWidget *mbox;
	char msg[512];
	int	flight_ID;
	if(id==0) {				//如果“确定”按钮被点击
		sprintf(msg,gtk_entry_get_text(GTK_ENTRY(flight)));
		flight_ID=atoi(msg);
		if(flight_ID<=0 || flight_ID>10) {	//判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
			mbox = gnome_message_box_new ("输入的航班号错误！请重新输入！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(flight)," ");
			return -1;
		}
		for(i=0;i<FLIGHT_NUM;i++) {
			pthread_mutex_lock(&ticket_list[i].ticket_mutex);
			if(ticket_list[i].flight_ID==flight_ID) {
				sprintf(msg,"你查询的航班号是：%d, 剩余票数：%d,票价：%d\n",ticket_list[i].flight_ID,ticket_list[i].ticket_num,ticket_list[i].ticket_price);
				display_info(msg,view);
				pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
				break;
			}
			pthread_mutex_unlock(&ticket_list[i].ticket_mutex);
		}
		gnome_dialog_close(dialog);
	}
	else
		gnome_dialog_close(dialog);
}

/*查询某一特定航班*/
void inquireone()
{
	GtkWidget *dialog;
	GtkWidget *label,*flight_entry;
	dialog = gnome_dialog_new(_("查询特定航班机票信息"),GNOME_STOCK_BUTTON_OK ,GNOME_STOCK_BUTTON_CANCEL ,NULL) ;

	label=gtk_label_new("请输入要查询的航班号(1-10)：");
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),label,TRUE,TRUE ,0 ) ;
	gtk_widget_show (label);

	flight_entry=gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(flight_entry),TRUE);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),flight_entry, FALSE, FALSE,0);
	gtk_widget_show (flight_entry);

	gnome_dialog_set_default(GNOME_DIALOG(dialog),0);		//设定确定作为缺省按钮

	g_signal_connect(G_OBJECT(dialog),"clicked" ,G_CALLBACK(button_inquireone) ,flight_entry) ; //设定“确定”“取消”按钮的回调函数
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	gtk_widget_show (dialog);
	gnome_dialog_set_parent(GNOME_DIALOG( dialog),GTK_WINDOW(app));	//设定对话框的父窗口为主程序窗口
}

void inquireall()
{

	int i;
	char msg[512];
	for(i=0;i<FLIGHT_NUM;i++) {
		sprintf(msg,"航班号：%d, 剩余票数：%d, 票价：%d\n",ticket_list[i].flight_ID,ticket_list[i].ticket_num, ticket_list[i].ticket_price);
		display_info(_(msg),view);
	}
}

/*帮助对话框和关于对话框的确定按钮处理函数*/
void dialog_ok(GnomeDialog *dialog, gint id,gpointer data)
{
	gnome_dialog_close(dialog);
}

/*帮助－－－显示内容*/
void displaycontents(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	char msg[4][512]={"开启服务器：开启服务器，准备接受客户端连接","关闭服务器：关闭服务器端","特定航班查询：查询某一特定航班机票信息","所有航班查询：查询所有航班机票信息"};
	dialog = gnome_dialog_new(_("帮助"),_ ( "确定" ) ,NULL ,NULL) ;
	for(i=0;i<4;i++) {
		label=gtk_label_new(msg[i]);
		gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),label,TRUE,TRUE ,0 ) ;
		gtk_widget_show (label);
	}

	g_signal_connect(G_OBJECT(dialog),"clicked" ,G_CALLBACK(dialog_ok) ,&dialog) ;
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	gtk_widget_show (dialog);
	gnome_dialog_set_parent(GNOME_DIALOG( dialog),GTK_WINDOW(app));
}


/*帮助－－－关于*/
void about(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "网络售票模拟系统服务器端");
   gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.9");

   gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
                                                                    "本程序仅用于教学，请勿用于商业目的。");
     gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
                                                                    "版权所有(Copyright) 刘学勇 \n 2011.11");
  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);
}

/*定时器回调函数，从界面输出信息缓冲区中取出要显示的信息输出到屏幕上*/
gint info_print()
{
	int i;
	pthread_mutex_lock(&info_mutex);
	for(i=0;i<INFO_NUM;i++)
		if(info[i].status==INFO_OCCUPIED) {
			display_info(info[i].msg,view);
			info[i].status=INFO_FREED;
		}
	pthread_mutex_unlock(&info_mutex);
	return TRUE;
}

int main(int argc, char *argv[])
{
	GtkWidget    *isenable;
	GtkWidget   *scrolled;

	init_ticket_list();
	//界面初始化部分
	gnome_init("example", "0.1", argc, argv);
	app=gnome_app_new("example", "网络售票模拟系统服务器端");
	g_signal_connect(G_OBJECT(app), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	/*创建菜单和工具栏*/
	gnome_app_create_menus(GNOME_APP(app),menubar);
	gnome_app_create_toolbar(GNOME_APP(app),toolbar);

	/*启动程序时使关闭服务器工具条和菜单项灰化*/
	if(!isserveropened) {
		isenable=toolbar[1].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
		isenable=server_operation_menu[1].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
	}

	gtk_window_set_default_size((GtkWindow *)app, 800, 600);
	vbox=gtk_vbox_new(FALSE,0);
	gnome_app_set_contents(GNOME_APP(app),vbox);


	frame=gtk_frame_new(NULL);
	gtk_frame_set_label(GTK_FRAME(frame),"服务器信息");
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);

	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE,TRUE,10);	/*最后一个参数控制间隔*/
	gtk_widget_show(vbox);

	box2 = gtk_vbox_new (FALSE, 10);
	gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
	gtk_container_add(GTK_CONTAINER(frame),box2);
	gtk_widget_show (box2);

	table = gtk_table_new (2, 2, FALSE);
	gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
	gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
	gtk_box_pack_start (GTK_BOX (box2), table, TRUE, TRUE, 0);
	gtk_widget_show (table);

	/*创建服务器输出信息*/
	view=gtk_text_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (view), FALSE);	//文本信息不可编辑
    scrolled=gtk_scrolled_window_new(NULL,NULL);                        /*创建滚动窗口构件*/
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),view);/*将文本视图构件加入滚动窗口*/

	gtk_table_attach (GTK_TABLE (table), scrolled, 0, 1, 0, 1, GTK_EXPAND | GTK_SHRINK | GTK_FILL,
	GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

	gtk_widget_show_all(app);
	g_timeout_add(500,info_print,NULL);	//定时更新界面输出信息
	gtk_main();

	return 0;
}
