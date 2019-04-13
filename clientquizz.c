/* cliTCPIt.c - QuizzGame

*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;

int main (int argc, char *argv[])
{
  int sd;			// descriptorul de socket
  struct sockaddr_in server;	// structura folosita pentru conectare 
  		// mesajul trimis
  int nr=0;
  char buf[1000];

  /* exista toate argumentele in linia de comanda? */
  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }

  /* stabilim portul */
  port = atoi (argv[2]);

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Eroare la socket().\n");
      return errno;
    }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(argv[1]);
  /* portul de conectare */
  server.sin_port = htons (port);
  
  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("[client]Eroare la connect().\n");
      return errno;
    }


// identificarea cu nume
char num[1001];
memset(num,0,1000);
  
  printf ("Introduceti un nume: ");
  fflush (stdout);
   if (read (0, num,sizeof(num)) < 0)
    {
      perror ("[client]Eroare la citirea numelui.\n");
      return errno;
    }
  
  int leng=strlen(num)-1;
  num[leng]='\0';
    strncpy(num,num,leng);


  /* trimiterea numelui la server */
  if (write (sd,num,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }

// citesc comanda
  memset(buf,0,sizeof(buf));
  
  printf ("Jucati : 1 \nAdaugati o intrebare : 2\n");
  fflush (stdout);

   if (read (0, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea comenzii.\n");
      return errno;
    }
  
   leng=strlen(buf)-1;
  buf[leng]='\0';
    strncpy(buf,buf,leng);

     if (write (sd,buf,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }

int com =atoi(buf);
if(com==1)
{
    printf("Punctaj = 0\n");
    fflush(stdout);

fd_set actfds;
int timpexp=0;

     /* citirea intrebarii dat de server 
     (apel blocant pina cind serverul raspunde) */
for(int cont=1;cont<=10&&timpexp==0;cont++)
{
  memset(buf,0,sizeof(buf));
  if (read (sd, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la read() de la server.\n");
      return errno;
    }

printf("\n%s",buf); //intrebarea
fflush(stdout);
memset(buf,0,sizeof(buf));

   printf ("[ %s ]Introduceti numarul raspunsului ales: ", num);
  fflush (stdout);

  if(read (0, buf, sizeof(buf))<0)
  {
      perror ("[client]Eroare la read() de la terminal.\n");
      return errno;
    }

leng=strlen(buf)-1;
buf[leng]='\0';
    strncpy(buf,buf,leng);

 /* trimiterea mesajului la server */
  if (write (sd,buf,sizeof(buf)) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }

memset(buf,0,sizeof(buf));
    //citirea punctajului curect

     if ( read (sd, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea punctajului.\n");
      return errno;
    }
  int r=atoi(buf);
      if(r==25)
    {
      printf("\n Timpul a expirat.Ne pare rau ai fost descalificat.\n");
       timpexp=1; break;
    }

    printf("Punctajul curent = %s \n", buf);
    fflush(stdout);

  }
  if(timpexp==0)
  { printf("Jocul s-a terminat \n");
  fflush(stdout);
  memset(buf,0,sizeof(buf));
   if ( read (sd, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea punctajului.\n");
      return errno;
    }
    printf("%s \n", buf);
  fflush(stdout);
}

}

else // ADAUGA INTREBAREA
      { printf("Introduceti intrebarea :");
    fflush(stdout);
    memset(buf,0,sizeof(buf));

   if (read (0, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea intrebarii.\n");
      return errno;
    }
  
   leng=strlen(buf)-1;
   buf[leng]='\0';
    strncpy(buf,buf,leng);

     if (write (sd,buf,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
    /////////////////////////

    printf("Introduceti raspunsul 1:");
    fflush(stdout);
        memset(buf,0,sizeof(buf));

   if (read (0, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea intrebarii.\n");
      return errno;
    }
  
   leng=strlen(buf)-1;
   buf[leng]='\0';
    strncpy(buf,buf,leng);

     if (write (sd,buf,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
    ////////////////////
     printf("Introduceti raspunsul 2:");
    fflush(stdout);
        memset(buf,0,sizeof(buf));

   if (read (0, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea intrebarii.\n");
      return errno;
    }
  
   leng=strlen(buf)-1;
   buf[leng]='\0';
    strncpy(buf,buf,leng);

     if (write (sd,buf,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
    /// 
    printf("Introduceti raspunsul 3:");
    fflush(stdout);
        memset(buf,0,sizeof(buf));

   if (read (0, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea intrebarii.\n");
      return errno;
    }
  
   leng=strlen(buf)-1;
   buf[leng]='\0';
    strncpy(buf,buf,leng);

     if (write (sd,buf,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
    ///////////
     printf("Introduceti raspunsul 4:");
    fflush(stdout);
        memset(buf,0,sizeof(buf));

   if (read (0, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea intrebarii.\n");
      return errno;
    }
  
   leng=strlen(buf)-1;
   buf[leng]='\0';
    strncpy(buf,buf,leng);

     if (write (sd,buf,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }
////////////
     printf("Introduceti numarului raspunsului corect(1-4):");
    fflush(stdout);
        memset(buf,0,sizeof(buf));

   if (read (0, buf,sizeof(buf)) < 0)
    {
      perror ("[client]Eroare la citirea intrebarii.\n");
      return errno;
    }
  
   leng=strlen(buf)-1;
   buf[leng]='\0';
    strncpy(buf,buf,leng);

     if (write (sd,buf,leng+1) <= 0)
    {
      perror ("[client]Eroare la write() spre server.\n");
      return errno;
    }

    printf("Multumim, %s!\n", num);

      }// else adauga intrebare


  /* inchidem conexiunea, am terminat */
  close (sd);
}