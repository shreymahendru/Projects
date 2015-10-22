/*
 * socket demonstrations:
 * This is the server side of an "internet domain" socket connection, for
 * communicating over the network.
 *
 * In this case we are willing to wait either for chatter from the client
 * _or_ for a new connection.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef PORT
	#define PORT 42001
#endif

struct client {
	int fd;
	char name[265];
	//name = &buff
	struct in_addr ipaddr;
	struct client *next;
	int in_game;
	int initial;
	int wait;
	int last_opt_fd;
	int opt;
	int hp;
	int pm;
	int attck;
	int speak;
};


static struct client *addclient(struct client *top, int fd, struct in_addr addr);
static struct client *removeclient(struct client *top, int fd);
static void broadcast(struct client *top, char *s, int size);
static int handleclient(struct client *p, struct client *top);
static void start_fight(struct client *list, int client1, int client2);
static void match(struct client *head, int clientfd);
static int initialize(struct client *top, int fd);
static void battle(struct client *list, int client1, int client2, char *buf);
static void update(struct client *list, int client1, int client2);
static void move(struct client *top, int client); 


static void move(struct client *top, int client)
{
    struct client *T= top;
    struct client *node;
    if (T->fd== client)
    {   
        node = T;
        top= T->next;  
    } 
    else 
    {
        while (T->next != NULL)
        {

            if (T->next->fd == client)
            {
                node= T->next;
                T->next = T->next->next;
            }
            T= T->next;
        }
    }
    T->next= node;
    node->next= NULL;

}









int bindandlisten(void);

void printll(struct client *head)
 {
        	//struct client *hlp=head;
        	//printf("%s FACK", hlp->name);
        	//printf("%lu\n", sizeof(hlp->name));
        	//printf("_%s_\n", hlp->name);
        	printf("%lu\n", sizeof(head));
        	while (head !=NULL)
        	{
         	//	printf("%i FUCK\n", hlp->in_game);
           	// printf("1\n");
            	printf("%s  %i   %s   sjadopajs\n",head->name,head->fd, inet_ntoa(head->ipaddr));
            	head=head->next;
        	}
      	//  printf("2\n");
 }
int main(void) {
	int clientfd, maxfd, nready;
	struct client *p;
	struct client *head = NULL;
	socklen_t len;
	struct sockaddr_in q;
    
	fd_set allset;
	fd_set rset;

	int i;


	int listenfd = bindandlisten();
	// initialize allset and add listenfd to the
	// set of file descriptors passed into select
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	// maxfd identifies how far into the set to search
	maxfd = listenfd;

	while (1) {
    	// make a copy of the set before we pass it into select
    	rset = allset;
  	 
    	nready = select(maxfd + 1, &rset, NULL, NULL,NULL);
   	 
    	if (nready == -1) {
        	perror("select");
        	continue;
    	}

    	if (FD_ISSET(listenfd, &rset)){
        	printf("a new client is connecting\n");
        	len = sizeof(q);
        	if ((clientfd = accept(listenfd, (struct sockaddr *)&q, &len)) < 0) {
            	perror("accept");
            	exit(1);
        	}
        	FD_SET(clientfd, &allset);
        	if (clientfd > maxfd) {
            	maxfd = clientfd;
        	}
        	printf("connection from %s\n", inet_ntoa(q.sin_addr));
        	head = addclient(head, clientfd, q.sin_addr);
        	printll(head);
    	}

    	for(i = 0; i <= maxfd; i++) {
        	if (FD_ISSET(i, &rset)) {
            	for (p = head; p != NULL; p = p->next) {
                	if (p->fd == i && p->wait==0) {
                    	int result = handleclient(p, head);
                    	if (result == -1) {
                        	int tmp_fd = p->fd;
                        	head = removeclient(head, p->fd);
                            match(head, p->opt);
                        	FD_CLR(tmp_fd, &allset);
                        	close(tmp_fd);
                    	}
                    	break;
                	}
            	}
        	}
    	}
	}
	return 0;
}

int handleclient(struct client *p, struct client *top) {
	char buf[256];
	char outbuf[512];
	if (p->initial==0)
	{
    	int r=initialize(top, p->fd);
    	printf("got name\n");
    	match(top, p->fd);
    	return r;
	}
	int len = read(p->fd, buf, sizeof(buf) - 1);
	if (len > 0) {
    	buf[len] = '\0';
    	printf("Received %d bytes: %s", len, buf);
    	sprintf(outbuf, "%s says: %s", inet_ntoa(p->ipaddr), buf);

    	if (p->speak==1)
    	{   
        	char msg[256];
        	sprintf(msg,"%s takes a break to tell you:\n%s", p->name, buf);
        	printf("%s\n",msg);
        	int Message = write(p->opt, &msg, strlen(msg));
        	update(top, p->opt, p->fd);
        	p->speak=0;
        	if (Message == -1){
           	perror("write");
           	exit(1);
       	}
       	return 0;
    	}
    	printf("I dont go here niigah!\n");
    	battle(top, p->fd, p->opt, buf);
    	return 0;
	} else if (len == 0) {
    	// socket is closed
    	printf("Disconnect from %s\n", inet_ntoa(p->ipaddr));
    	sprintf(outbuf, "**%s leaves**\n", p->name);
    	char win[256];
    	sprintf(win, "-- %s dropped. You win!\n  Awaiting net opponent!!!", p->name);
   	 
    	broadcast(top, outbuf, strlen(outbuf));
   	    move(top, p->opt);
        int Win_Msg = write(p->opt, &win, strlen(win));
    	if (Win_Msg == -1){
           	perror("write");
           	exit(1);
    	}
    	return -1;
	} else { // shouldn't happen
    	perror("read");
    	return -1;
	}
}

 /* bind and listen, abort on error
  * returns FD of listening socket
  */
int bindandlisten(void) {
	struct sockaddr_in r;
	int listenfd;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	perror("socket");
    	exit(1);
	}
	int yes = 1;
	if ((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1) {
    	perror("setsockopt");
	}
	memset(&r, '\0', sizeof(r));
	r.sin_family = AF_INET;
	r.sin_addr.s_addr = INADDR_ANY;
	r.sin_port = htons(PORT);

	if (bind(listenfd, (struct sockaddr *)&r, sizeof r)) {
    	perror("bind");
    	exit(1);
	}

	if (listen(listenfd, 5)) {
    	perror("listen");
    	exit(1);
	}
	return listenfd;
}

 struct client *addclient(struct client *top, int fd, struct in_addr addr) {
	struct client *p = malloc(sizeof(struct client));
	if (!p) {
    	perror("malloc");
    	exit(1);
	}
	char *buf_name= "What is your name?\n";
    
	int k= write(fd, buf_name, strlen(buf_name));
	if (k== -1)
	{
    	perror("write");
    	exit(1);
	}

	printf("Adding client %s\n", inet_ntoa(addr));

	p->fd = fd;
	p->initial=0;
	p->wait=0;
	p->in_game=0;
	p->ipaddr = addr;
	if (top== NULL)
	{
    	top=p;
    	top->next=NULL;
	}
	else
	{
    	p->next = top;
     	top = p;
	}
	return top;
}


static int initialize(struct client *top, int fd)
{  	 
	char buff[265];

int len = read(fd, &buff, sizeof(buff) - 1);
	if (len > 0) {
 	buff[len-1] = '\0';

	char bc_msg[256];
	sprintf(bc_msg, "**%s enters the arena**\n", buff);
	printf("%s\n",bc_msg);

	broadcast(top, bc_msg, strlen(bc_msg));

	char welcome_msg[265];
	sprintf(welcome_msg, "Welcome, %s! Awaiting opponent...\n", buff);  
	printf("%s\n",welcome_msg);
    
	int l= write(fd, welcome_msg, strlen(welcome_msg));
	if (l== -1)
	{
    	perror("write");
    	exit(1);
	}
	struct client *p= top;
 	while (p!= NULL)
 	{
    	if (p->fd == fd)
    	{
        	strcpy(p->name, buff);
        	p->initial=1;
    	}
    	p=p->next;
 	}
	return 0;
	}
 	else if (len == 0)
 	{
    	// socket is closed
    	return -1;
	}
	else
	{ // shouldn't happen
    	perror("read");
    	return -1;
	}
}


static struct client *removeclient(struct client *top, int fd) {
	struct client **p;

	for (p = &top; *p && (*p)->fd != fd; p = &(*p)->next);
	// Now, p points to (1) top, or (2) a pointer to another client
	// This avoids a special case for removing the head of the list
	if (*p) {
    	struct client *t = (*p)->next;
    	printf("Removing client %d %s\n", fd, (*p)->name);
    	free(*p);
    	*p = t;
	} else {
    	fprintf(stderr, "Trying to remove fd %d, but I don't know about it\n",fd);
	}
	return top;
}


static void match(struct client *head, int clientfd)
	{
    	struct client *p;
    	for (p= head; p != NULL; p= p->next)
    	{
        	if(p->fd != clientfd)
        	{
            	if(p->in_game==0)
            	{
                	if(p->last_opt_fd != clientfd)
                	{
                    	if (p->initial == 1){
                        	start_fight( head, clientfd, p->fd); //new one starts
                    	}
                    	else if (p->initial == 0){
                    	}          	 
                	}
            	}
        	}
    	}
	}  
static void start_fight(struct client *list, int client1, int client2) // client1 is the nigger
{ //client2 got shat on
	int hitpoint1, powermoves1;
	int hitpoint2, powermoves2;  
	time_t t;
	srand((unsigned) time(&t));

	// Randomize ///////////////////////
	hitpoint1 = (rand() % 10) + 20;
	powermoves1 = (rand() % 2) + 1;
	//damage = (rand() % 4) + 2;
	hitpoint2 = (rand() % 10) + 20;
	powermoves2 = (rand() % 2) + 1;
    int first_move=(rand() % 100);

	struct client *temp= list;
	// Variables
	printf("PRINT 1\n");

	

    struct client *c1;
    struct client *c2;
    printf("fuck you                      LOL");
	while (temp != NULL)
	{
    	if(temp->fd== client1)
    	{   
        	//temp->wait=1;
            c1=temp;
        	c1->opt=client2;
        	c1->hp=hitpoint1;
        	c1->pm=powermoves1;
        	//c1_name= temp->name;
		printf("fuck you"); //it sometimes (not evry time)segfaults here randomly but sometimes it don't
        	temp->in_game=1;   // but it's fucking stupid cause in_game is an int and i am assigning 0 to it
		printf("fuck you here");
        	//cntr++;
    	}
    	if (temp->fd== client2)
    	{
        	//temp->wait=0;
            c2=temp;
        	//c2_nme=temp->name;
        	c2->opt=client1;
        	c2->hp=hitpoint2;
        	c2->pm=powermoves2;
	printf("fuck you               l"); // same for here 	  
        	temp->in_game=1; //same for here
        	//ctr++;
		printf("fuck you here           l");
    	}
    	temp=temp->next;
	}

	char c1_msg[256];
	sprintf(c1_msg, "You engage %s!\nYour hitpoints: %i \nYour powermoves: %i \n\n%s's hitpoints:  %i\n\n", c2->name, hitpoint1,powermoves1,c1->name, hitpoint2);
	int l1= write(client1, c1_msg, strlen(c1_msg));

	if (l1== -1)
	{
    	perror("write");
    	exit(1);
	}

	char c2_msg[256];
	sprintf(c2_msg, "You engage %s!\nYour hitpoints: %i \nYour powermoves: %i \n\n%s's hitpoints:  %i\n\n", c1->name, hitpoint2,powermoves2,c1->name, hitpoint1);
	int l2 = write(client2, c2_msg, strlen(c2_msg));

	if (l2== -1)
	{
    	perror("write");
    	exit(1);
	}

	char moves[256];
	sprintf(moves, "(a)ttack\n(p)owermove\n(s)peak something\n");

	if (first_move>=50)
	{
        c1->wait=0;
    	int l3= write(client1, moves, strlen(moves));
    	if (l3== -1)
    	{
        	perror("write");
        	exit(1);
    	}
    	char wait_msg[256];
        c2->wait = 1;
    	sprintf(wait_msg, "Waiting for %s to stike...\n", c1->name);
    	int l4= write(client2, wait_msg, strlen(wait_msg));
    	if (l4== -1)
    	{
        	perror("write");
        	exit(1);
    	}
	}
	else {
        c2->wait= 0;
        c1->wait= 1;
    	int l3= write(client2, moves, strlen(moves));
    	if (l3== -1)
    	{
        	perror("write");
        	exit(1);
    	}
    	char wait_msg[256];
    	sprintf(wait_msg, "Waiting for %s to stike...\n", c2->name);
    	int l4= write(client1, wait_msg, strlen(wait_msg));
    	if (l4== -1)
    	{
        	perror("write");
        	exit(1);
    	}
	}
  }


	static void broadcast(struct client *top, char *s, int size) {
    	struct client *p;
    	for (p = top; p; p = p->next) {
        	write(p->fd, s, size);
    	}
    	/* should probably check write() return value and perhaps remove client */
	}


	static void battle(struct client *list, int client1, int client2, char *buf){ //client 1 is the nigger
	// client2 is getting shat on
    	struct client *player = list;
    	struct client *player1;
    	struct client *player2;
    	
    	while(player != NULL){

        	if(player->fd == client1){
            	player1=player;
           	 
        	}
        	if(player->fd == client2){
            	player2 = player;
            	}
        	player = player->next;
    	}

    	int attack = (rand() % 4) + 2;
    	int chance = (rand() % 100);
    
    	char missed[256];
    	sprintf(missed, "You missed!\n"); // fix this to the proper statement

    	char fuckyea[256];
    	sprintf(fuckyea, "%s missed you!\n", player1->name);

    	char p_hit[256];
    	sprintf(p_hit, "You hit %s for %i damage!", player2->name, (attack * 3));

    	char got_hit[256];
    	sprintf(got_hit, "%s's powermoves you for %i damage!", player1->name, (attack * 3));

    	//player 1
    	if (buf[0] == 'a') {
        	player2->hp = player2->hp - attack;
            player1->wait=1;
            player2->wait=0;
    	}
    	if (buf[0] == 'p' && player1->pm > 0){
            player1->wait=1;
            player2->wait=0;
        	player1->pm = player1->pm - 1;
        	if (chance >= 50){
            	int p_attack = write(player1->fd, p_hit, strlen(p_hit));

            	if (p_attack == -1){
                	perror("write");
                	exit(1);
            	}

            	int got_attacked = write(player2->fd, got_hit, strlen(got_hit));

            	if (got_attacked == -1){
                	perror("write");
                	exit(1);
            	}

            	player2->hp = player2->hp - (attack * 3);

        	}
        	else if (chance < 50) {
            	int miss = write(player1->fd, missed, strlen(missed));

            	if (miss == -1){
                	perror("write");
                	exit(1);
            	}

            	int k = write(player2->fd, fuckyea, strlen(fuckyea));
           	 
            	if (k == -1)
            	{
                	perror("write");
            	}
        	}
    	}
    	if (buf[0] == 's')
    	{
        	player1->speak=1;
        //	s=1;
        	char msg[265];
        	sprintf(msg, "Speak: ");
        	int T = write(player1->fd, msg, strlen(msg));
   	 
    	if (T== -1){
        	perror("write");
           	exit(1);
    	}
    	}  

    	else
    	{  
            if (player2->hp>0)
            {
        	update(list, player1->fd, player2->fd);
            }
            else if (player2->hp <= 0 )
            {
                char win[256];
                char lose[256];
                sprintf(win, "%s is no match for you. You win!\n", player2->name);
                sprintf(lose, "You are no match for %s. You lose!\n", player1->name);
                int lol=write(player1->fd, win, strlen (win));
                int lol2=write(player2->fd, lose, strlen(lose  ));

                //int T = write(player1->fd, msg, strlen(msg));
                // move it to the back of the lists 
                player1->wait=1;
                player2->wait=1;
                player1->in_game=0;
                player2->in_game=0;
                player2->opt=0;
                player1->opt=0;
                player1->last_opt_fd=player2->fd;
                player2->last_opt_fd=player1->fd;
                move(list, player1->fd);
                move(list, player2->fd);
                
                char msg[265];
                sprintf(msg, "Awaiting next opponent...\n");  
                printf("%s\n",msg);
    
                int l= write(player1->fd, msg, strlen(msg));
                int l2= write(player2->fd, msg, strlen(msg));
                if (l== -1 || l2== -1)
                {
                   perror("write");
                      exit(1);
                  }



                match(list, player2->fd);
                match(list, player1->fd);
                if (lol == -1 || lol2 == -1){
                 perror("write");
                    exit(1);
                 }
            }
    	}
	}

    
 	static void update(struct client *list, int client1, int client2) {
    	struct client *player = list;
    	struct client *player1;
    	struct client *player2;
    	while(player != NULL){
        	if(player->fd == client1){
            	player1=player;
        	}
        	if(player->fd == client2){
            	player2 = player;
        	}
        	player = player->next;
    	}

    	char stat2[256];
    	sprintf(stat2, "\nYour hitpoints: %i\n Your powermoves: %i\n\n %s's hitpoints: %i \n\n", player2->hp, player2->pm, player1->name, player1->hp);
   	 
    	char stat1[256];
    	sprintf(stat1, "\nYour hitpoints: %i\n Your powermoves: %i \n\n %s's hitpoints: %i \n\n", player1->hp, player1->pm, player2->name, player2->hp);
   	 
    	int status1 = write(player2->fd, stat2, strlen(stat1));
   	 
    	if (status1 == -1){
        	perror("write");
           	exit(1);
    	}

    	int status2 = write(player1->fd, stat1, strlen(stat2));
   	 
    	if (status2 == -1){
        	perror("write");
           	exit(1);
    	}

    	char wait_msg[256];
    	sprintf(wait_msg, "\nWaiting for %s to strike...\n", player2->name);

    	int waiting_msg = write(player1->fd, wait_msg, strlen(wait_msg));
   	 
    	if (waiting_msg == -1){
        	perror("write");
        	exit(1);
    	}

    	char option[256];
    	sprintf(option, "(a)ttack\n(p)owermove\n(s)peak something\n");

    	char pMove[256];
    	sprintf(pMove, "(a)ttack\n(s)peak something\n");

    	if (player2->pm == 0){
        	int no_pmove = write(player2->fd, pMove, strlen(pMove));
        	if (no_pmove == -1){
            	perror("write");
            	exit(1);
        	}
    	}

    	else if (player2->pm > 0){
        	int pmove = write(player2->fd, option, strlen(option));
        	if (pmove == -1){
            	perror("write");
            	exit(1);
        	}
    	}
	}



