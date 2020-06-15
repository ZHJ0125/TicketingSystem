/*client.c*/

#include "globalapi.h"

int socket_fd; 	//连接socket
struct sockaddr_in server;			//服务器地址信息，客户端地址信息
int ret,i;
int flag=1;

static	GtkWidget 	*app;		/*程序主窗口*/
static GtkWidget	*frame, *vbox, *box2 ,*table;	/*box2用来封装文本构件与垂直滚动条*/
static	GtkWidget	*clientwindow ;	//客户端窗口，用来输出相关提示信息。
static  GtkTextIter iter;

int isconnected=FALSE;		//是否已连接服务器


// 以下函数声明应该是与GTK界面显示有关，同时也实现了不同的功能
void connectserver(GtkWidget *widget, gpointer data);
void disconnect(GtkWidget *widget, gpointer data);
void buyticket(GtkWidget *widget,gpointer data);
void inquireone();
void inquireall();
void displaycontents(GtkWidget *widget, gpointer data);
void about(GtkWidget *widget, gpointer data);

// 下面这几个函数，用来对客户端界面和回调函数进行绑定
/*生成客户端操作菜单项（下拉菜单）*/
GnomeUIInfo client_operation_menu[]={
	GNOMEUIINFO_ITEM_NONE("连接服务器" ,"与远程服务器建立连接", connectserver),
	GNOMEUIINFO_ITEM_NONE("断开连接" ,"断开与远程服务器的连接", disconnect),
	GNOMEUIINFO_ITEM_NONE("购买机票" ,"购买机票", buyticket),
	GNOMEUIINFO_ITEM_NONE("退出" ,"退出程序", gtk_main_quit),
	GNOMEUIINFO_END
};
/*生成航班查询菜单项（下拉菜单）*/
GnomeUIInfo inquire_menu[]={
	GNOMEUIINFO_ITEM_NONE("查询特定航班" ,"查询某一特定航班机票信息", inquireone),
	GNOMEUIINFO_ITEM_NONE("查询所有航班" ,"查询所有航班机票信息", inquireall),
	GNOMEUIINFO_END
};
/*生成帮助菜单项（下拉菜单）*/
GnomeUIInfo help_menu[]={
	GNOMEUIINFO_ITEM_NONE("显示内容" ,"显示帮助内容", displaycontents),
	GNOMEUIINFO_ITEM_NONE("关于" ,"关于此程序说明", about),
	GNOMEUIINFO_END
};
/*生成顶层菜单项*/
GnomeUIInfo menubar[]= {
	GNOMEUIINFO_SUBTREE("客户端操作(_S)", client_operation_menu),
	GNOMEUIINFO_SUBTREE("机票查询(_Q)", inquire_menu),
	GNOMEUIINFO_SUBTREE("帮助(_H)", help_menu),
	GNOMEUIINFO_END
};
/*生成工具栏*/
GnomeUIInfo toolbar[]= {
	GNOMEUIINFO_ITEM_STOCK("连接服务器","与远程服务器建立连接", connectserver, GNOME_STOCK_PIXMAP_PRINT),
	GNOMEUIINFO_ITEM_STOCK("断开连接","断开与远程服务器的连接", disconnect, GNOME_STOCK_PIXMAP_CUT),
	GNOMEUIINFO_ITEM_STOCK("购买机票","购买机票", buyticket, GNOME_STOCK_PIXMAP_OPEN),
	GNOMEUIINFO_ITEM_STOCK("查询特定航班","查询某一特定航班机票信息", inquireone, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("查询所有航班","查询所有航班机票信息", inquireall, GNOME_STOCK_PIXMAP_SAVE),
	GNOMEUIINFO_ITEM_STOCK("退出", "退出程序", gtk_main_quit, GNOME_STOCK_PIXMAP_EXIT),
	GNOMEUIINFO_END
};

/*消息内容输出函数*/
// 该函数将msg信息输出到GTK布局界面上
void display_info(char *msg, GtkWidget *view)
{
	GtkTextBuffer *buffer;              //客户端提示信息缓冲区
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	gtk_text_buffer_get_iter_at_offset(buffer, &iter, -1);
	gtk_text_buffer_insert(buffer, &iter, msg, -1);
}

/*连接服务器操作*/
void connectserver(GtkWidget *widget, gpointer data)
{
	char msg[512];		//提示信息
	GtkWidget *isenable;
	int i;

	if(!isconnected)
	{
		/*创建套接字*/
		socket_fd=socket(AF_INET,SOCK_STREAM,0);
		if(socket_fd<0) {
			sprintf(msg,"创建套接字出错！ \n");
			display_info(msg,clientwindow);
			return;
		}
		/*设置接收、发送超时值*/
		struct timeval time_out;
		time_out.tv_sec=5;
		time_out.tv_usec=0;
		setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &time_out, sizeof(time_out));

		/*填写服务器的地址信息*/
		server.sin_family=AF_INET;
		server.sin_addr.s_addr=inet_addr("127.0.0.1");//htonl(INADDR_ANY);
		server.sin_port=htons(SERVER_PORT_NO);

		/*连接服务器*/
		ret=connect(socket_fd,(struct sockaddr*)&server, sizeof(server));
		if(ret<0) {
			sprintf(msg,"连接服务器出错！%d\n",SERVER_PORT_NO);		// 这里改了一下，添加了格式控制符%d
			display_info(msg,clientwindow);
			close(socket_fd);
			return;
		}

		//成功后输出提示信息
		sprintf(msg,"连接服务器成功！\n");
		display_info(msg,clientwindow);
		isconnected=TRUE;

		/*连接服务器菜单项和工具条灰化*/
		isenable=toolbar[0].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
		isenable=client_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable,FALSE);

		/*其他菜单项和工具条使能*/
		for(i=1;i<=4;i++) {
			isenable=toolbar[i].widget;
			gtk_widget_set_sensitive(isenable,TRUE);
		}
		for(i=1;i<=2;i++) {
			isenable=client_operation_menu[i].widget;
			gtk_widget_set_sensitive(isenable,TRUE);
		}
		isenable=menubar[1].widget;
		gtk_widget_set_sensitive(isenable,TRUE);

	}
}

/*断开连接操作*/
void disconnect(GtkWidget *widget, gpointer data)
{
	GtkWidget	*isenable;
	char msg[512];
	if(isconnected)
	{
		close(socket_fd);
		sprintf(msg,"断开连接成功！ \n");
		display_info(msg,clientwindow);
		isconnected=FALSE;

		/*连接服务器菜单项和工具条使能*/
		isenable=toolbar[0].widget;
		gtk_widget_set_sensitive(isenable,TRUE);
		isenable=client_operation_menu[0].widget;
		gtk_widget_set_sensitive(isenable,TRUE);

		/*其他菜单项和工具条灰化*/
		for(i=1;i<=4;i++) {
			isenable=toolbar[i].widget;
			gtk_widget_set_sensitive(isenable,FALSE);
		}
		for(i=1;i<=2;i++) {
			isenable=client_operation_menu[i].widget;
			gtk_widget_set_sensitive(isenable,FALSE);
		}
		isenable=menubar[1].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
	}
}

/*购买机票对话框的文本输入框，用来获取输入的航班号和票数*/
struct flight_entry_t {
	GtkWidget *flight_ID;
	GtkWidget *ticket_num;
} st_flight;

/*购买机票的按钮回调函数*/
int button_buyticket(GnomeDialog *dialog, gint id,gpointer data)
{
	char msg[512];
	char send_buf[512],recv_buf[512];
	GtkWidget *mbox;
	if(id==0) {				//如果“确定”按钮被点击
		struct flight_entry_t *p=(struct flight_entry_t *)data;
		GtkWidget *flight_ID_entry=p->flight_ID;
		GtkWidget *ticket_num_entry=p->ticket_num;
		/*获取输入的航班号*/
		char *str=gtk_entry_get_text(GTK_ENTRY(flight_ID_entry));
		int flight_ID=atoi(str);
		if(flight_ID<=0 || flight_ID>10) {	//判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
			mbox = gnome_message_box_new ("输入的航班号错误！请重新输入！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(flight_ID_entry)," ");
			return ;
		}
		/*获取输入的机票数*/
		str=gtk_entry_get_text(GTK_ENTRY(ticket_num_entry));
		int ticket_num=atoi(str);
		if(ticket_num<=0) {	//判断输入的票数是否正确，不正确的话，给出提示信息，重新输入。
			mbox = gnome_message_box_new ("输入的票数错误！请重新输入！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(ticket_num_entry)," ");
			return ;
		}
		/*购买机票*/

		init_message();
		message.msg_type=BUY_TICKET;
		message.flight_ID=flight_ID;
		message.ticket_num=ticket_num;
		memcpy(send_buf,&message,sizeof(message));
		int ret=send(socket_fd, send_buf,sizeof(message),0);
		/*发送出错*/
		if(ret==-1) {
			mbox = gnome_message_box_new ("发送失败！请重新发送！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			return ;
		}
		ret=recv(socket_fd,recv_buf,sizeof(message),0);
		if(ret==-1) {
			mbox = gnome_message_box_new ("接收失败！请重新发送！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			return ;
		}
		memcpy(&message,recv_buf,sizeof(message));
		if(message.msg_type==BUY_SUCCEED)
			sprintf(msg,"购买成功！航班号：%d, 票数：%d, 总票价：%d\n",message.flight_ID,message.ticket_num, message.ticket_total_price);
		else
			sprintf(msg,"购买失败！航班号：%d, 剩余票数：%d, 请求票数：%d\n",message.flight_ID,message.ticket_num,ticket_num);
		display_info(msg,clientwindow);
	}

		gnome_dialog_close(dialog);
}

/*购买机票回调函数*/
void buyticket(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label,*flight_entry;
	dialog = gnome_dialog_new(_("购买机票"),GNOME_STOCK_BUTTON_OK ,GNOME_STOCK_BUTTON_CANCEL ,NULL) ;

	label=gtk_label_new("请输入要购买的航班号(1-10)：");
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),label,TRUE,TRUE ,0 ) ;
	gtk_widget_show (label);

	flight_entry=gtk_entry_new();
	st_flight.flight_ID=flight_entry;
	gtk_entry_set_visibility(GTK_ENTRY(flight_entry),TRUE);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),flight_entry, FALSE, FALSE,0);
	gtk_widget_show (flight_entry);

	label=gtk_label_new("请输入要购买的机票数：");
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),label,TRUE,TRUE ,0 ) ;
	gtk_widget_show (label);

	flight_entry=gtk_entry_new();
	st_flight.ticket_num=flight_entry;
	gtk_entry_set_visibility(GTK_ENTRY(flight_entry),TRUE);
	gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),flight_entry, FALSE, FALSE,0);
	gtk_widget_show (flight_entry);
	gnome_dialog_set_default(GNOME_DIALOG(dialog),0);		//设定确定作为缺省按钮

	g_signal_connect(G_OBJECT(dialog),"clicked" ,G_CALLBACK(button_buyticket) ,&st_flight) ; //设定“确定”“取消”按钮的回调函数
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	gtk_widget_show (dialog);
	gnome_dialog_set_parent(GNOME_DIALOG( dialog),GTK_WINDOW(app));	//设定对话框的父窗口为主程序窗口

}
/*查询特定航班对话框的“确定”“取消”按钮的回调函数*/
int button_inquireone(GnomeDialog *dialog, gint id,gpointer data)
{
	GtkWidget *flight=data;
	GtkWidget *mbox;
	char msg[512];
	char send_buf[512],recv_buf[512];
	int	flight_ID;
	if(id==0) {				//如果“确定”按钮被点击
		sprintf(msg,gtk_entry_get_text(GTK_ENTRY(flight)));
		flight_ID=atoi(msg);
		if(flight_ID<=0 || flight_ID>10) {	//判断输入的航班号是否正确，不正确的话，给出提示信息，重新输入。
			//gnome_dialog_close(dialog);
			mbox = gnome_message_box_new ("输入的航班号错误！请重新输入！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			gtk_entry_set_text(GTK_ENTRY(flight)," ");
			return ;
		}
		init_message();
		message.msg_type=INQUIRE_ONE;
		message.flight_ID=flight_ID;
		memcpy(send_buf,&message,sizeof(message));
		int ret=send(socket_fd, send_buf,sizeof(message),0);
		/*发送出错*/
		if(ret==-1) {
			mbox = gnome_message_box_new ("发送失败！请重新发送！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			return ;
		}
		ret=recv(socket_fd,recv_buf,sizeof(message),0);
		if(ret==-1) {
			mbox = gnome_message_box_new ("接收失败！请重新连接服务器！\n",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
			gtk_widget_show (mbox);
			gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
			gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(dialog));
			return ;
		}
		memcpy(&message,recv_buf,sizeof(message));
		if(message.msg_type==INQUIRE_SUCCEED)
			sprintf(msg,"查询成功！航班号：%d, 剩余票数：%d, 票价：%d\n",message.flight_ID,message.ticket_num, message.ticket_total_price);
		else
			sprintf(msg,"查询失败！航班号：%d, 剩余票数：未知\n",message.flight_ID);
		display_info(msg,clientwindow);
		gnome_dialog_close(dialog);
	}
	else			//取消按钮被按下
		gnome_dialog_close(dialog);
}

/*查询某一特定航班*/
void inquireone()
{
	/**/
	//int flight=toolbar[2].
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
	int i,pos;
	char msg[512];
	char send_buf[512], recv_buf[512];
	GtkWidget *mbox;
	init_message();
	message.msg_type=INQUIRE_ALL;
	memcpy(send_buf,&message,sizeof(message));
	int ret=send(socket_fd, send_buf,sizeof(message),0);
	/*发送出错*/
	if(ret==-1) {
		mbox = gnome_message_box_new ("发送失败！请重新发送！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
		gtk_widget_show (mbox);
		gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
		gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(app));
		return ;
	}
	ret=recv(socket_fd,recv_buf,sizeof(recv_buf),0);
	if(ret==-1) {
		mbox = gnome_message_box_new ("接收失败！请重新发送！",GNOME_MESSAGE_BOX_INFO,GNOME_STOCK_BUTTON_OK,NULL ) ;
		gtk_widget_show (mbox);
		gtk_window_set_modal (GTK_WINDOW (mbox), TRUE);
		gnome_dialog_set_parent(GNOME_DIALOG( mbox),GTK_WINDOW(app));
		return ;
	}
	pos=0;
	sprintf(msg,"查询所有航班结果：\n");
	display_info(msg,clientwindow);
	for (i=0;i<ret;i=i+sizeof(message)) {
		memcpy(&message,recv_buf+pos,sizeof(message));
		if(message.msg_type==INQUIRE_SUCCEED)
			sprintf(msg,"查询成功！航班号：%d, 剩余票数：%d, 票价：%d\n",message.flight_ID,message.ticket_num, message.ticket_total_price);
		else
			sprintf(msg,"查询失败！航班号：%d, 剩余票数：未知\n",message.flight_ID);
		display_info(msg,clientwindow);
		pos+=sizeof(message);
	}
}

/*帮助对话框和关于对话框的确定按钮处理函数*/
void dialog_ok(GnomeDialog *dialog, gint id,gpointer data)
{
	gnome_dialog_close(dialog);
}

/*帮助-显示内容*/
void displaycontents(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog;
	GtkWidget *label;
	char msg[5][512]={"连接服务器：与远程服务器建立连接","断开连接：断开与远程服务器的连接","购买机票：购买机票","特定航班查询：查询某一特定航班机票信息","所有航班查询：查询所有航班机票信息"};
	dialog = gnome_dialog_new(_("帮助"),GNOME_STOCK_BUTTON_OK  ,NULL ,NULL) ;
	for(i=0;i<5;i++) {
		label=gtk_label_new(msg[i]);
		gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(dialog)->vbox),label,TRUE,TRUE ,0 ) ;
		gtk_widget_show (label);
	}

	g_signal_connect(G_OBJECT(dialog),"clicked" ,G_CALLBACK(dialog_ok) ,&dialog) ;
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	gtk_widget_show (dialog);
	gnome_dialog_set_parent(GNOME_DIALOG( dialog),GTK_WINDOW(app));
}


/*帮助-关于*/
void about(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "网络售票模拟系统客户端");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.9");

	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),"本程序仅用于教学，请勿用于商业目的。");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"版权所有(Copyright) 刘学勇 \n 2011.11");

	gtk_dialog_run(GTK_DIALOG (dialog));
	gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[])
{
	GtkWidget     *isenable;
	GtkWidget     *scrolled;
	int i;

	//界面初始化部分
	gnome_init("example", "0.1", argc, argv);
	app=gnome_app_new("example", "网络售票模拟系统客户端");
	g_signal_connect(G_OBJECT(app), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	//添加菜单和工具栏
	gnome_app_create_menus(GNOME_APP(app),menubar);
	gnome_app_create_toolbar(GNOME_APP(app),toolbar);

	/*启动程序时使工具条上断开连接、机票购买、机票查询以及相应的菜单项灰化*/
	if(!isconnected) {
		for(i=1;i<=4;i++) {
			isenable=toolbar[i].widget;
			gtk_widget_set_sensitive(isenable,FALSE);
		}
		for(i=1;i<=2;i++) {
			isenable=client_operation_menu[i].widget;
			gtk_widget_set_sensitive(isenable,FALSE);
		}
		isenable=menubar[1].widget;
		gtk_widget_set_sensitive(isenable,FALSE);
	}

	gtk_window_set_default_size((GtkWindow *)app, 800, 600);
	vbox=gtk_vbox_new(FALSE,0);
	gnome_app_set_contents(GNOME_APP(app),vbox);

	frame=gtk_frame_new(NULL);
	gtk_frame_set_label(GTK_FRAME(frame),"客户端信息");
	gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_OUT);

	gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE,TRUE,10);	/*最后一个参数控制间隔*/

	box2 = gtk_vbox_new (FALSE, 10);
	gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
	gtk_container_add(GTK_CONTAINER(frame),box2);

	table = gtk_table_new (2, 2, FALSE);
	gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
	gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
	gtk_box_pack_start (GTK_BOX (box2), table, TRUE, TRUE, 0);

	/*创建客户端显示信息区*/
	clientwindow=gtk_text_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (clientwindow), FALSE);	//文本信息不可编辑
	scrolled=gtk_scrolled_window_new(NULL,NULL); /*创建滚动窗口构件*/
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),clientwindow);/*将文本视图构件加入滚动窗口*/

	gtk_table_attach (GTK_TABLE (table), scrolled, 0, 1, 0, 1, GTK_EXPAND | GTK_SHRINK | GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

	gtk_widget_show_all(app);
	gtk_main();

	return 0;
}
