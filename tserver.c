#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>

// gcc with -lpthread -lncurses

struct message {
	int length;
	char* msg;
};

struct message net_msg;

void show_ncurs();
int open_socket();
void* accept_client();
void* listener(void*sock);
void *unser(struct message*, char* );


int main(int argc, char*argv[]){
	//signal(SIGPIPE,SIG_IGN);

	int ch;
	show_ncurs();
	int sock=open_socket();
	if(sock==-1){
		printf("error occured\n");
		return -1;
	}
	pthread_t listen;
	if ( pthread_create(&listen, NULL, listener, (void*)&sock) < 0 ){
			perror("Thread problem\n");
			return 1;
		}


	while(1){
		ch = getch();
		if(ch=='x'){
			printw("The pressed key is:");
			addch(ch | A_BOLD);
			refresh();
			break;
		}else{
			printw("Press 'x' to quit.\n");
		}
	
	}
	printw("Press any key to quit.");
	getch();
	endwin();

	endwin();
	exit(0);
}

void show_ncurs(){
int row,col;
	initscr();
	raw();
	keypad(stdscr, true);
	noecho();
	
	getmaxyx(stdscr,row,col);

}

int open_socket(){
	struct sockaddr_in saddr;
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(123);
	saddr.sin_addr.s_addr=INADDR_ANY;
	int s;
	if((s=socket(PF_INET,SOCK_STREAM,0))==-1){
		printw("Something wrong with socket!\n");
		refresh();
	}
	if(bind(s,(struct sockaddr*)&saddr,(socklen_t)(sizeof(saddr)))<0){
		printw("Bind error!\n");
		refresh();
		return -1;
	}
	printw("Bind ok.\n");
	refresh();
	if(listen(s,3)==-1){
		printw("Listen failed\n");
		refresh();
	}
	return s;
}

void *accept_client(void* ns){
	char*msg="Hi! I'm listening.\n";
	char buf[255];
	int num,lol=1;
	int my_sock=*(int*)ns;
	while(1){
		send(my_sock,msg,strlen(msg),0);
		num=recv(my_sock,buf,255,0);
		if(num==0){
			break;
		}

		unser(&net_msg,buf);		

		printw("%d\n%s\n",net_msg.length,net_msg.msg);//%s buf
		refresh();
		buf[0]='\0';
	}
	close(my_sock);
}

void *unser(struct message *m, char* buf){
	memcpy(&m->length, buf, sizeof(int));
	m->msg=malloc(m->length);
	memcpy(m->msg, buf+sizeof(int), m->length);
}

void *listener(void*sock){
	struct sockaddr_in client;
	int m_sock=*(int*)sock;
	int new_sock;
	int c=sizeof(struct sockaddr_in);
		
	

	while ( new_sock = accept(m_sock,(struct sockaddr*)&client,(socklen_t*)&c) ){
		
		pthread_t client_thread;

		printw("Connection accepted.\n");
		if ( pthread_create(&client_thread, NULL, accept_client, (void*)&new_sock) < 0 ){
			perror("Thread problem\n");
		}
		printw("Waiting for more connections...\n");
		refresh();
	}
	if(new_sock<0){printw("Shit new socket.");refresh();}
}
