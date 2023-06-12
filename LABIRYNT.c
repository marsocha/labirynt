#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "postscript.h"
#include "queue.h"
int WYS;
int fn, en;
#define dwa(i,j) ((i) + WYS * (j)) /* makro potrzebne do używanie tablicy jednowymiarowej jako jednowymiarowej */
#define MAXQUEUE 1000
#define zakodowany "labirynt3.txt" /*skąd mamy wziąć labirynt */
char nazwa_pliku[] = "labirynt.ps"; /*jak się będzie nazywał plik z narysowanym labiryntem */

typedef struct {
	int x, y, jest;
	bool gora, dol, lewo, prawo;
	wspolrzedne poprz;
	char znak;
} pole;

wspolrzedne wymiary_pliku() {

	char pom;
	wspolrzedne wyn;
	FILE* tablica;
	int i,j, maxi;
	i = j = maxi = 0;
	if ((tablica = fopen(zakodowany, "r")) == NULL) {
		printf("Nie da się odczytać pliku \n");
		exit(1);
	}

	
	pom = getc(tablica);
	
	while (pom != EOF) {

		while (pom != '\n' && pom != EOF) {
			j++;
			pom = getc(tablica);
		}
		maxi = (j > maxi ? j : maxi);	
		j = 0;
		i++;
		if (pom != EOF) {
			pom = getc(tablica);
		}
	}

	fclose(tablica);
	wyn.x = maxi; /* szerokosc*/
	wyn.y = i; /*wysokosci */
	return wyn;
}
/*znajduje wysokość oraz szerokość labiryntu z pliku nawet jeżeli podany plik */
/* nie jest poprawny ze względu na wygląd labiryntu */



void zczytuj_labirynt (pole tab[]) {
	
	FILE* tablica;
	int i, j;
	char pom;
	i = j = 0;


	if ((tablica = fopen(zakodowany, "r")) == NULL) {
		printf("Nie da się odczytać pliku \n");
		exit(1);
	}

	pom = getc(tablica);
	
	while (pom != EOF) {

		while (pom != '\n' && pom != EOF) {

			tab[dwa(i,j)].znak = pom;
			j++;
			pom = getc(tablica);
		}
			
		j = 0;
		i++;
		
		
		if (pom != EOF) {
			pom = getc(tablica);
		}	}

	fclose(tablica);
}
/* zczytujemy labirynt z pliku */

bool czy_prawo (pole tab[], wspolrzedne p, int SZER) {
	return (p.x + 1 < SZER) ? ((tab[dwa(p.y,p.x + 1)].znak != '|') ? true : false) : false;
}

bool czy_lewo (pole tab[], wspolrzedne p) {
	return (p.x > 0) ? ( (tab[dwa(p.y,p.x - 1)].znak != '|') ? true : false ) : false;
}

bool czy_dol (pole tab[], wspolrzedne p, int WYS) {
	return (p.y + 1 < WYS) ? ( (tab[dwa(p.y + 1,p.x)].znak != '-') ? true : false ) : false;
}

bool czy_gora (pole tab[], wspolrzedne p) {
	return (p.y > 0) ? ( (tab[dwa(p.y - 1,p.x)].znak != '-') ? true : false ) : false;
}
/*powyższe funkcje sprawdzają, czy pole w konkretną stronę jest dostępne czy też */
/*między nimi jest ściana */

void uzupelnianie_labiryntu(pole labirynt[], int WYS, int SZER) {
	
	int i, j;
	wspolrzedne tmp_wsp;
	
	for (i = 0; i < WYS; i += 2) {
		for (j = 0; j < SZER; j += 2) {
			tmp_wsp.y = i;
			tmp_wsp.x = j;
			labirynt[dwa(i,j)].gora = czy_gora(labirynt, tmp_wsp);
			labirynt[dwa(i,j)].dol = czy_dol(labirynt, tmp_wsp, WYS);
			labirynt[dwa(i,j)].lewo = czy_lewo(labirynt, tmp_wsp);
			labirynt[dwa(i,j)].prawo = czy_prawo(labirynt, tmp_wsp, SZER);
			labirynt[dwa(i,j)].jest = 0;
			labirynt[dwa(i,j)].poprz.x = -1; /*chcemy wiedzieć, że nie ma poprzednika */
			labirynt[dwa(i,j)].poprz.y = -1; /* ponieważ nie ma pola o takich współrzędnych */
										/* w tablicy dwuwymiarowej */
		}
	}
} /* uzupełniamy labirynt o podstawowe dane potrzebne przy dalszym */
	/*działaniu programu */

bool porownuj_wspolrzedne(wspolrzedne a, wspolrzedne b) {
	return (a.x == b.x && a.y == b.y) ? true : false;
} /* sprawdza czy podane dwa pola są takie same */

void usun_kolejke() {
	wspolrzedne tmp;
	while (!QueueEmpty()) Dequeue(&tmp); 
} /*usuwa całą kolejkę, tak aby była pusta */

wspolrzedne zmiana_na_programowe(wspolrzedne a) {
	a.x = (a.x - 1) * 2;
	a.y = (a.y - 1) * 2;
	return a;
} /* intuicyjne wspolrzedne są inne niż te wykorzystywane w programie */

wspolrzedne zmiana_na_intuicyjne(wspolrzedne a) {
	a.y = (a.y / 2) + 1;
	a.x = (a.x / 2) + 1;
	return a;
} /* intuicyjne wspolrzedne są inne niż te wykorzystywane w programie */

void wypisz_wspolrzedne(wspolrzedne a) {
	printf("%d, %d\n", a.x, a.y );
} /* wypisuje współrzędne, pomocne przy sprawdzaniu poprawności */

void rysuj_bez_sciezki (pole tab[], int dl1, int WYS, int SZER) {
	int i, j;
	int dl2;
	int jp, ip;
	
	dl2 = dl1 * 2;
	/* rysujemy teraz obwód */
	PSDrawLine(0, 0, 0, (WYS + 1) * dl1); /*lewa sciana */
	PSDrawLine((SZER + 1) * dl1, 0, (SZER + 1) * dl1, (WYS + 1) * dl1); /*prawa ściana */
	PSDrawLine(0, 0, (SZER + 1) * dl1, 0); /* dolna ściana */
	PSDrawLine(0, (WYS + 1) * dl1, (SZER + 1) * dl1, (WYS + 1) * dl1); /* gorna sciana */

	for (i = 0; i < WYS; ++i)
	{
		for (j = 0; j < SZER; ++j)
		{
			jp = (j + 1) / 2;
			ip = (i + 1) / 2;
			switch(tab[dwa(i, j)].znak) {
				case '|' :
					PSDrawLine((jp) * dl2, (WYS - ip) * dl2 - (WYS - 1) * dl1 , (jp) * dl2, (WYS - ip - 1) * dl2 - (WYS - 1) * dl1 ); /* pionowe */
					break;
				case '-' :
					PSDrawLine((jp) * dl2, (WYS - ip) * dl2 - (WYS - 1) * dl1 , (jp + 1) * dl2, ( WYS - ip) * dl2 - (WYS - 1) * dl1 ); /* poziome */
					break;
			}
			
		}
	}

} /* rysuje labirynt, jednakże bęz ściezki */

void rysuj_sciezke (pole tab[], wspolrzedne b, int dl1, int WYS) {
	wspolrzedne c;
	printf("wejdz w plik labirynt.ps\n");
	
	PSSetRGB (255, 0 , 20);

	
	if (tab[dwa(b.y,b.x)].poprz.x == -1) {
		PSDrawCircle( (b.x + 1)* dl1 , (WYS - (b.y + 1)) * dl1 + dl1, dl1 / 2);
	}

	c.x = tab[dwa(b.y,b.x)].poprz.x;
	c.y = tab[dwa(b.y,b.x)].poprz.y;
	while (tab[dwa(b.y, b.x)].poprz.x != -1) {

		
		PSDrawLine((b.x + 1) * dl1, (WYS - (b.y + 1) )* dl1 + (dl1 / 2), (c.x + 1) * dl1 , (WYS -  (c.y + 1)) * dl1 + (dl1 / 2) ); 
		
		b = c;
		c.x = tab[dwa(b.y,b.x)].poprz.x;
		c.y = tab[dwa(b.y,b.x)].poprz.y;
	}
} /* rysuje najkrótszą ścieżkę */


void rysuj_labirynt(pole tab[], wspolrzedne b, int WYS, int SZER) {
	
	int dl1 = 200;

	rysuj_bez_sciezki(tab, dl1, WYS, SZER);

	rysuj_sciezke(tab, b, dl1, WYS);
} /* rysuje labirynt wraz ze ścieżką */

bool szukanie_sciezki ( pole tab[], wspolrzedne a, wspolrzedne b, int WYS, int SZER) {
	
	wspolrzedne tmp, tmp2;

	if (porownuj_wspolrzedne(a, b)) {
		rysuj_labirynt(tab, b, WYS, SZER);
		return true;	
	}

	tab[dwa(a.y,a.x)].jest = 1;
	Enqueue(a);
	while ( !QueueEmpty() ) {

		Dequeue( &tmp );
		tab[dwa(tmp.y,tmp.x)].jest = 1;
		if (tab[dwa(tmp.y,tmp.x)].gora) {
			tmp2.y = tmp.y - 2;
			tmp2.x = tmp.x;
			if (tab[dwa(tmp2.y,tmp2.x)].jest == 0) {
				if (porownuj_wspolrzedne(tmp2, b)) {
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					rysuj_labirynt(tab, b, WYS, SZER);
					usun_kolejke();
					return true;
				} else {
					tab[dwa(tmp2.y,tmp2.x)].jest = 1;
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					Enqueue(tmp2);
				}
			}
		}
		if (tab[dwa(tmp.y,tmp.x)].dol) {
			tmp2.y = tmp.y + 2;
			tmp2.x = tmp.x;
			if (tab[dwa(tmp2.y,tmp2.x)].jest == 0) {
				if (porownuj_wspolrzedne(tmp2, b)) {
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					rysuj_labirynt(tab, b, WYS, SZER);
					usun_kolejke();
					return true;
				} else {
					tab[dwa(tmp2.y,tmp2.x)].jest = 1;
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					Enqueue(tmp2);
				}
			}
		}
		if (tab[dwa(tmp.y,tmp.x)].lewo) {
			tmp2.y = tmp.y;
			tmp2.x = tmp.x - 2;
			if (tab[dwa(tmp2.y,tmp2.x)].jest == 0) {
				if (porownuj_wspolrzedne(tmp2, b)) {
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					rysuj_labirynt(tab, b, WYS, SZER);
					usun_kolejke();
					return true;
				} else {
					tab[dwa(tmp2.y,tmp2.x)].jest = 1;
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					Enqueue(tmp2);
				}
			}
		}
		if (tab[dwa(tmp.y,tmp.x)].prawo) {
			tmp2.y = tmp.y;
			tmp2.x = tmp.x + 2;
			if (tab[dwa(tmp2.y,tmp2.x)].jest == 0) {
				if (porownuj_wspolrzedne(tmp2, b)) {
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					rysuj_labirynt(tab, b, WYS, SZER);
					usun_kolejke();
					return true;
				} else {
					tab[dwa(tmp2.y,tmp2.x)].jest = 1;
					tab[dwa(tmp2.y,tmp2.x)].poprz.x = tmp.x;
					tab[dwa(tmp2.y,tmp2.x)].poprz.y = tmp.y;
					Enqueue(tmp2);
				}
			}
		}


	}

	return false;
} /* jeżeli istnieje ścieżka to wówczas znajdzie najkrótszą z nich oraz wynikiem będzie prawda, w przeciwnym przypadku */
	/* wynikiem funkcji będzie fałsz */

bool sprawdz_dane(wspolrzedne a, wspolrzedne b, int WYS, int SZER) {
	return ((a.x < 0 || a.y < 0 || b.x < 0 
		   || b.y < 0 || a.y >= WYS || b.y >= WYS ||
		   a.x >= SZER || b.x >= SZER)
		   ? false : true);
} /* sprawdza czy podane wspołrzędne przez użytkownika "mieszczą się" w labiryncie */


int main()
{
	int SZER;
	wspolrzedne wys_szer, a, b;
	pole* labirynt;

	wys_szer = wymiary_pliku();
	WYS = wys_szer.y;
	SZER = wys_szer.x;	
	labirynt = (pole*)malloc(WYS * SZER * sizeof(pole));
	
	InitQueue();
	zczytuj_labirynt(labirynt); 
	uzupelnianie_labiryntu(labirynt, (WYS - 1) / 2, (SZER - 1) / 2);

	PSOpenFile(nazwa_pliku, 600);
	scanf("%d%d%d%d", &a.x, &a.y, &b.x, &b.y);
	a = zmiana_na_programowe(a);
	b = zmiana_na_programowe(b);
	
	if (sprawdz_dane(a,b, WYS, SZER)) {
		if (!szukanie_sciezki(labirynt, a,b, WYS, SZER)) printf("niestety nie ma ścieżki pomiędzy podanymi punktami\n");
	} else {
		printf("podano nieprawidłowe współrzędne\n");
	}
	PSCloseFile();
	usun_kolejke();
	free (labirynt);
	return 0;

}