#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <ncurses.h>

//gcc with lncurses

void show_ncurs();

struct message {
	int length;
	char* msg;
};
void ser(struct message*, char*);

int main(int argc, char*argv[]){
	struct message msg_net;
	char msg[255];
	char buf[255];
	show_ncurs();
	struct sockaddr_in saddr;
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(123);
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	int sock;
	if((sock=socket(PF_INET,SOCK_STREAM,0))==-1){
		printw("Something wrong with socket!\n");
		refresh();
		endwin();
		return 1;
	}


	int c=sizeof(saddr);
	int status = connect(sock,(struct sockaddr*)&saddr,(socklen_t)c);
	if(status<0){printw("Connection error.\n");refresh();endwin();exit(1);}
	printw("Connetcion established.\n");
	refresh();

	int num;
	while(1){
		num=recvfrom(sock,buf,255,0,(struct sockaddr*)&saddr,(socklen_t*)&c);
		buf[num]='\0';
		printw("%s",buf);
		refresh();
		buf[0]='\0';

		getstr(msg);
		if(msg[0]=='!'){
			switch (msg[1]){

				case 'q' :
				close(sock);
				printw("Bye!\n");
				return 0;

				default :
				printw("Type !q for quit.\n");
			}
		}
		msg_net.msg=msg;
		msg_net.length=sizeof(char)*(strlen(msg)+1);
		ser(&msg_net, buf);
		send(sock,buf,sizeof(buf),0);
	}
	close(sock);
	endwin();
	return 0;
}

void ser(struct message *m, char* buf){

	memcpy(buf, &m->length, sizeof(int));
	memcpy(buf+sizeof(int), m->msg, m->length);

}

void show_ncurs(){
int row,col;
	initscr();
	raw();
	keypad(stdscr, true);
	
	getmaxyx(stdscr,row,col);

}
