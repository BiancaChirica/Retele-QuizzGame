/* servTCPConcTh2.c - Exemplu de server TCP concurent care deserveste clientii
   prin crearea unui thread pentru fiecare client.
	Intoarce corect identificatorul din program al thread-ului.
*/

//Chirica Bianca , II B5 FII

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string.h>

/* portul folosit */
#define PORT 2908

/* codul de eroare returnat de anumite apeluri */
extern int errno;

typedef struct thData{
	int idThread; //id-ul thread-ului tinut in evidenta de acest program
	int cl; //descriptorul intors de accept
}thData;

struct nume {
	char nume[1000];
	int punctaj;
}clienti[1000];

int terminat[1000];
int i=0;
int write_castig[1000];
int rasp[100];
int nrintrebari=11;


static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);
void adauga(void *);
int intrebarea(int j,char s[1000]);
int inserare_baza();
int maxim(char castigator[100]);
int terminat_toti();
void reseteaza_punctaje();
int write_toti();

int main ()
{


if( inserare_baza()==1) // populez baza de date cu cele 10 intrebari
{
	 perror ("[server]Eroare la popularea bazei de date().\n");
      return errno;

}

//raspunsurile la intrebari
rasp[6]=rasp[8]=rasp[10]=1;
rasp[2]=rasp[3]=rasp[1]=rasp[4]=2;
rasp[5]=rasp[9]=4;
rasp[7]=3;

  struct sockaddr_in server;	// structura folosita de server
  struct sockaddr_in from;	
  int nr;		//mesajul primit de trimis la client 
  int sd;		//descriptorul de socket 
  int pid;
  pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
	
  

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }
  /* utilizarea optiunii SO_REUSEADDR */
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  
  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));
  
  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;	
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);
  
  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
  /* servim in mod concurent clientii...folosind thread-uri */
  while (1)
    {
      int client;
      thData * td; //parametru functia executata de thread     
      int length = sizeof (from);

      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);

      // client= malloc(sizeof(int));
      /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
      if ( (client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
	{
	  perror ("[server]Eroare la accept().\n");
	  continue;
	}
	
        /* s-a realizat conexiunea, se astepta mesajul */

	// int idThread; //id-ul threadului
	// int cl; //descriptorul intors de accept

	td=(struct thData*)malloc(sizeof(struct thData));	
	td->idThread=i++;
	td->cl=client;

// struct id thread

	pthread_create(&th[i], NULL, &treat, td);	      
				
	}//while    

};				

static void *treat(void * arg)
{		
		struct thData tdL; 
		tdL= *((struct thData*)arg);
		pthread_detach(pthread_self());	

			// se primeste numele clientului, login
	if (read (tdL.cl, clienti[tdL.idThread].nume ,sizeof(clienti[tdL.idThread].nume)) <= 0)
			{
				terminat[tdL.idThread]=1;
				write_castig[tdL.idThread]=1;
			  printf("[Thread %d]\n",tdL.idThread);
			  perror ("Eroare la citirea numelui de la client.\n");	
			}
	else {	
			//comanda adauga intrebare
			char buf[100];
			memset(buf,0,sizeof(buf));
				if (read (tdL.cl,buf ,sizeof(buf)) <= 0)
					{
				  terminat[tdL.idThread]=1;
				  write_castig[tdL.idThread]=1;
				  printf("[Thread %d]\n",tdL.idThread);
				  perror ("Eroare la comenzii numelui de la client.\n");	
					}			     
			else {
				int com=atoi(buf);
				if(com==1)
				{ 
					clienti[tdL.idThread].punctaj=0;
					printf ("[thread]- %d - Incepem jocul pentru %s.\n", tdL.idThread,clienti[tdL.idThread].nume);
					fflush (stdout);			
					raspunde((struct thData*)arg);
				}
				else  //adauga intrebare
				{
					terminat[tdL.idThread]=1;
				    write_castig[tdL.idThread]=1;
				    adauga((struct thData*)arg);
				    //adauga
				}
			}//else citire comanda

		} //primul else nume

		if(write_toti()==1)
		{
			reseteaza_punctaje();	}

		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return(NULL);	
  		
}

void raspunde(void *arg)
{
	fd_set actfds;		/* multimea descriptorilor activi */
  struct timeval tv;	

  		int timpexp=0; // daca un client nu raspunde la timp =1
        int  i=0,nrrasp=0;
        char nrbuf[1000]={0};
      

	struct thData tdL; 
	tdL= *((struct thData*)arg);

  	

	for(int j=1;j<=10 && timpexp==0;j++)
    { 
    	FD_ZERO (&actfds);		/* initial, multimea este vida */
    	FD_SET (tdL.cl, &actfds);

    	char s[1000];
    	memset(s,0,sizeof(s));

    	intrebarea(j,s); // pun in s cate o intrebare
    	if(write (tdL.cl, s, sizeof(s))<=0)
    	{
				
			  printf("[Thread %d]\n",tdL.idThread);
			  perror ("Eroare la scrierea intrebarii.\n");
			  break;
    	}

 tv.tv_sec = 20;		/* se va astepta un timp de 10 sec. */
  tv.tv_usec = 0;

int rez=select (tdL.cl+1, &actfds, NULL, NULL, &tv);

if ( rez< 0)
	{
	printf("Eroare la select");
	  perror ("[server] Eroare la select().\n");
	  
	}
if(rez>0)
 {
 	memset(nrbuf,0,sizeof(nrbuf));
 	int rez2=read(tdL.cl,nrbuf,sizeof(nrbuf)); 
	nrrasp=atoi(nrbuf);
   		
	if(rez2<0)
	{
		terminat[tdL.idThread]=1;
				write_castig[tdL.idThread]=1;
			
				timpexp=1;
		  printf("[Thread %d]\n",tdL.idThread);
		  perror ("Eroare la read() de la client.\n");
		  	break;		
	}
	if(rez2==0)
		{
			terminat[tdL.idThread]=1;
			write_castig[tdL.idThread]=1;
			clienti[tdL.idThread].punctaj=0;
			printf("Clientul %s a parasit jocul \n",clienti[tdL.idThread].nume);
			fflush(stdout);
			timpexp=1;
			break;
		}
	if(rez2>0)
	{
		printf ("[Thread %d]Mesajul a fost receptionat...%d\n",tdL.idThread, nrrasp);
	fflush(stdout);

	//verific raspuns

	if(nrrasp==rasp[j])
				{
					printf("corect\n");
					fflush(stdout);
					clienti[tdL.idThread].punctaj++;
				}
	else 
		{printf("raspuns gresit\n");fflush(stdout);	}

memset(nrbuf,0,sizeof(nrbuf));

int l=sprintf(nrbuf,"%d",clienti[tdL.idThread].punctaj);

	if(write (tdL.cl,nrbuf , l )<=0)
    	{
			  printf("[Thread %d]\n",tdL.idThread);
			  perror ("Eroare la scrierea punctajului.\n");
    	}	
usleep(10);
	}	
}
else if(rez==0)
	{printf("Timpul a expirat pentru intrebarea %i. CLientul %s a fost descalificat.\n",j,clienti[tdL.idThread].nume);
	fflush(stdout);
	terminat[tdL.idThread]=1;
	write_castig[tdL.idThread]=1;
	clienti[tdL.idThread].punctaj=0;
	memset(clienti[tdL.idThread].nume,0,sizeof(clienti[tdL.idThread].nume));
timpexp=1;

shutdown(tdL.cl,SHUT_RD);
  // trimit 25  semnal ca a fost descalificat
strcpy(nrbuf,"25");
if(write (tdL.cl,nrbuf ,sizeof(nrbuf) )<=0)
    	{
			  printf("[Thread %d]\n",tdL.idThread);
			  perror ("Eroare la scrierea intrebarii.\n");
    	}

break;
}

}
if(timpexp==0)
	{
		printf("Clientul %s a terminat jocul.\n",clienti[tdL.idThread].nume);
	fflush(stdout); 
	terminat[tdL.idThread]=1;

/// asteptam pana termina toti si alegem punctajul maxim care a fost realizat in cel mai scurt timp
while(terminat_toti()==0)
	sleep(1);

if(terminat_toti()==1)
 { char castig[100]={0};
 	int punctaj=0;
 	punctaj=maxim(castig);
memset(nrbuf,0,sizeof(nrbuf));
strcpy(nrbuf,"Castigatorul este : ");
strcat(nrbuf,castig);
strcat(nrbuf,"\nCu punctajul de : ");
memset(castig,0,sizeof(castig));
sprintf(castig,"%d",punctaj);
strcat(nrbuf,castig);

 	if(write (tdL.cl,nrbuf ,sizeof(nrbuf) )<=0)
    	{
			  printf("[Thread %d]\n",tdL.idThread);
			  perror ("Eroare la scrierea punctajului.\n");
    	}
    	write_castig[tdL.idThread]=1;

}

}

}

void reseteaza_punctaje()
{
	int k=0;
	for(k=0;k<i;k++)
		clienti[k].punctaj=0;
}

int maxim(char str[100])
{
	int j;
	char castigator[100];
	int punctajmax=-1;
	for(j=i-1;j>=0;j--)
		if(clienti[j].punctaj>punctajmax)
		{ punctajmax=clienti[j].punctaj;
			memset(castigator,0,sizeof(castigator));
			strcpy(castigator,clienti[j].nume);}

strcpy(str,castigator);
return punctajmax;
}

int terminat_toti()
{
	int j;
	for(j=0;j<i;j++)
		if(terminat[j]!=1)
			return 0;
	return 1;
}

int write_toti()
{
	int j;
	for(j=0;j<i;j++)
		if(write_castig[j]!=1)
			return 0;
	return 1;
}

void adauga(void *arg)
{
	int ok=0;
	struct thData tdL; 
	tdL= *((struct thData*)arg);
	char intr[1000],rasp1[1000],rasp2[1000],rasp3[1000],rasp4[1000],rasp_corect[10];
	memset(intr,0,sizeof(intr));
	memset(rasp1,0,sizeof(rasp1));
	memset(rasp2,0,sizeof(rasp2));
	memset(rasp3,0,sizeof(rasp3));
	memset(rasp4,0,sizeof(rasp4));

int ok1=read (tdL.cl, intr,sizeof(intr));
	if ( ok1< 0)
    { 
    	printf("[Thread %d]\n",tdL.idThread);
      perror ("Eroare la citirea intrebarii.\n");
    }
    else if(ok1==0)
    {
    	printf("Clientul a parasit jocul\n");
    	ok=1;
    }

if(ok==0)
{ int ok1=read (tdL.cl, rasp1,sizeof(rasp1));
    if ( ok1< 0)
    {
    	printf("[Thread %d]\n",tdL.idThread);
      perror ("Eroare la citirea intrebarii.\n");
    }
    else if(ok1==0)
    {
    	printf("Clientul a parasit jocul\n");
    	ok=1;
    }
}
if(ok==0)
{ int ok1=read (tdL.cl, rasp2,sizeof(rasp2));
     if (ok1 < 0)
    {
    	printf("[Thread %d]\n",tdL.idThread);
      perror ("Eroare la citirea intrebarii.\n");
    }
    else if(ok1==0)
    {
    	printf("Clientul a parasit jocul\n");
    	ok=1;
    }
}
 if(ok==0)
{ int ok1=read (tdL.cl, rasp3,sizeof(rasp3));
     if (ok1 < 0)
    {
    	printf("[Thread %d]\n",tdL.idThread);
      perror ("Eroare la citirea intrebarii.\n");
    }
    else if(ok1==0)
    {
    	printf("Clientul a parasit jocul\n");
    	ok=1;
    }
}
if(ok==0)
{ int ok1=read (tdL.cl, rasp4,sizeof(rasp4));
     if (ok1 < 0)
    {
    	printf("[Thread %d]\n",tdL.idThread);
      perror ("Eroare la citirea intrebarii.\n");
    }
    else if(ok1==0)
    {
    	printf("Clientul a parasit jocul\n");
    	ok=1;
    }
}
if(ok==0)
{ int ok1=read (tdL.cl, rasp_corect,sizeof(rasp_corect));
 if (ok1 < 0)
    {
    	printf("[Thread %d]\n",tdL.idThread);
      perror ("Eroare la citirea intrebarii.\n");
    }
    else if(ok1==0)
    {
    	printf("Clientul %s a parasit jocul\n",clienti[tdL.idThread].nume);
    	ok=1;
    }
}
if(ok==0)
{
     int tt=atoi(rasp_corect);
 rasp[nrintrebari]=tt;


sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database to insert informations: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
    }
    
    char sqlbuf[10000];
    memset(sqlbuf,0,sizeof(sqlbuf));
    sprintf(sqlbuf,"INSERT INTO bazadate VALUES(%d,'%s','%s','%s','%s','%s');",nrintrebari++,intr,rasp1,rasp2,rasp3,rasp4); 
    char *sql=sqlbuf;

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
    } 
     sqlite3_close(db);
   }
}


////
int intrebarea(int j,char s[1000])
{
	 sqlite3 *db;
    char *err_msg = 0;
    sqlite3_stmt *res;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    char *sql = "SELECT * FROM bazadate WHERE Id = ?";
        
     rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
    }

    	  sqlite3_bind_int(res, 1, j);
    
    int step = sqlite3_step(res);
    char c[2]={0};
    
    if (step == SQLITE_ROW) {
    	strcat(s,"Intrebare: ");
    	strcat(s, sqlite3_column_text(res, 1) );
    	strcat(s,"\n");
    	for(int k=2;k<=5;k++)
    		{
		strcat(s,"Raspuns ");
		k--;
		c[0]=(char)(k)+48;
		c[1]='\0';
		strcat(s,c);
		strcat (s,": ");
		k++;
    	strcat(s, sqlite3_column_text(res, k) );
        strcat(s,"\n");
        
            } 
        }
    sqlite3_finalize(res);
    sqlite3_close(db);
    
    return 0;
}
		

int inserare_baza()
{
		 sqlite3 *db;
    char *err_msg = 0;
    
    int rc = sqlite3_open("test.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database to insert informations: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    char *sql = "DROP TABLE IF EXISTS bazadate;" 
                "CREATE TABLE bazadate(Id INT PRIMARY KEY, Intrebare TEXT, Rasp1 TEXT,Rasp2 TEXT,Rasp3 TEXT,Rasp4 TEXT);" 
                "INSERT INTO bazadate VALUES(1, 'In ce an s-a nascut Mihai Eminescu ?', '18 ianuarie 1850','15 ianuarie 1850','23 martie 1850','06 februarie 1858');" 
                "INSERT INTO bazadate VALUES(2, 'Cati ani are un deceniu ?', '1','10','100','12');" 
                "INSERT INTO bazadate VALUES(3, 'Cati ani a domnit Stefan cel Mare ?', '10','47','34','50');"  
                "INSERT INTO bazadate VALUES(4, 'Care este capitala Romaniei ?', 'Iasi','Bucuresti','Cluj','Constanta');"
                "INSERT INTO bazadate VALUES(5, 'Prin cate tari trece Dunarea ?', '7','8','5','10');"
                "INSERT INTO bazadate VALUES(6, 'Ce acont detine cele mai multe like-uri la o poza pe instagram ?', 'world_record_egg','kyliejenner','selenagomez','elonmusknews');"
                "INSERT INTO bazadate VALUES(7, 'Cine detine cele mai multe trofee Oscar ?', 'Meryl Streep','Johnny Depp','Walt Disney','John Ford');"
                "INSERT INTO bazadate VALUES(8, 'Care este cel mai mare parc national din Statele Unite ?', 'Yellowstone','Sequoia','Yosemite','Zion');"
                "INSERT INTO bazadate VALUES(9, 'Care este capitala Germaniei ?', 'Viena','Paris','Budapesta','Berlin');"
                "INSERT INTO bazadate VALUES(10, 'Care este primarul Iasului ?', 'Chirica','Basescu','Iohannis','Firea');"
             ;

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
     sqlite3_close(db);
     return 0;
    
}