<h1>QuizzGame </h1>
<b>
Bianca Chirica<br>
December 2018<br>
</b>
<b>1 Introduction <b> <br>
<p>Proiectul QuizzGame reprezinta un server multithreading care suporta clientii
care raspund la un set de intrebari pe rand, in ordinea in care s-au inregistrat.
Fiecarui client i se pune o intrebare si are un numar n de secunde pentru a
raspunde la intrebare. Serverul verica raspunsul dat de client si daca este
corect va retine punctajul pentru acel client. La nal, serverul aseaza clientul
cu punctajul cel mai mare.<br>
  </p>
  
<b>2 Tehnologii utilizate</b><br>
<p>Serverul va implementa protocolul TCP deoarece ofera siguranta si asigura
transmiterea in ordine a datelor. Informatia transmisa intre server si client este
important sa ajunga completa, altfel, informatia lipsa poate rezulta in descal-
ificarea unui jucator. Pentru crearea bazei de date care va stoca intrebarile
impreuna cu raspunsurile voi folosi SQLite3.
  </p> <br>
  
<b>3 Arhitectura aplicatiei</b>
<br>
<img src="arhitectura.jpg" alt="Schema-Arhitectura aplicatiei">
<br>

4 Detalii de implementare
 Serverul va comunica cu clientii prin intermediul unui socket :
2
 Socketului ii va  atasat ip-ul si portul la care se vor conecteze clentii :
 Dupa acceptarea clientilor server-ul va forma cate un thread pentru ecare
client cu functia pthread create:
 In interiorul thread-ului voi folosi functia select pentru a astepta raspunsul
clientului timp de n secunde specicat ca parametrul functiei, iar daca
respectivul client nu va respunde la timp, acesta va  descalicat :
 Functia void pthread exit(void* status); va  folosita pentru terminarea
thread-ului.
 Pentru determinarea castigatorului se va apela o functie int max(punctaj)
care va alege punctajul maxim.
Caz 1: (ideal) Se initializeaza structurile de date din server. Se stabileste
conexiunea cu un ecare client venit. Pentru ecare client se creaza un thread,
prin intermediul caruia ecare client primeste intrebarile cu variantele de raspuns.
Dupa ecare intrebare, raspunsul clientului daca este primit in mai putin de n
secunde, este vericat si i se stocheaza punctajul.Daca mai vine un jucator
acesta se va conecta si va incepe jocul lui. Dupa ce jucatori au raspuns la 10
intrebari, acesti vor astepta sa termine toti clientii apoi se va alege un casti-
gator,iar punctajele se vor reseta pentru a astepta alti clienti. Thread-urile se
inchid si serverul continua sa astepte o noua runde de jucatori, in functia accept.
3
Jucatorii pot, de aemenea, sa introduca intrebari in baza de date, acesti nu vor
participa la joc.
Caz 2: In sitauatia in care un client nu raspunde la intrebarea primita in cele
n secunde, acesta va  descalicat astefel : thread-ul lui va  inchis, punctajul
va deveni 0, iar ceilalti jucatori vor continua fara ca jocul sa e afectat.
5 Conclusion
In concluzie, serverul poate accepta oricat de multi clienti care vor cuncura intre
ei, apoi dupa ce toti termina intrebarile ecare primeste numele castigatorului,
iar punctajele se reseteaza pentru a astepta alti jucatori.
O imbunatarire implementata este optiunea clientului de a adauga intrebari noi
la baza de date .
6 Bibliograe
 profs.info.uaic.ro/ computernetworks
 www.draw.io
 www.sqlite.org/index.html
4
