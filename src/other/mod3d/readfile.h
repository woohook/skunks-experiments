/*
Copyright (C) 2007-2012 Victor Matei Petrescu

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#define MAXWLG 51
/*maximum word length*/

#define REALN float
/*don't change*/


/*functie care returneaza 1 daca i se transmite un caracter de delimitare si 0 daca nu*/
int verdel(char s)
{char a[6]={' ','\r','\n','\t',':'};
int i,sem=0;
  for(i=0;i<=4;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care returneaza 1 daca i se transmite o litera mica si 0 daca nu*/
int vercrl(char s)
{char a[28]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int i,sem=0;
  for(i=0;i<=25;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care returneaza 1 daca i se transmite o cifra sau '.' si 0 daca nu*/
int vercrcf(char s)
{char a[13]={'0','1','2','3','4','5','6','7','8','9','.'};
int i,sem=0;
  for(i=0;i<=10;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care se uita sa vada ce e un sir de caractere: intreg(0), cuvant(1), real(2) sau altceva(3) ...*/
int detsir(char *s)
{int i=1,sem=3,crp=0; /*crp - variabila pentru numarat punctele*/
  if(vercrl(s[0])){sem=1; /*adica daca primul caracter e o litera*/
    while((!verdel(s[i]))&&s[i]){if(!vercrl(s[i])){sem=3;return sem;} i++;}
  return sem;}
    if((vercrcf(s[0]))||(s[0]=='-')){sem=2; /*adica daca primul caracter e o cifra*/
	if(s[0]=='.'){crp++;}
      while((!verdel(s[i]))&&s[i]){
	if(s[i]=='.'){crp++;}
	if(!vercrcf(s[i])){sem=3;return sem;} i++;
      }if((crp>=2)||((crp==1)&&(i==1))){sem=3;return sem;}
    if((!crp)&&(s[0]!='-')){sem=0;}
    return sem;}
return sem;}


/*functie care citeste un cuvant s din fisierul fis; lincr - linia curenta*/
int fisgetw(FILE *fis,char *s,int *lincr)
{int i,sem=0;
  static int sem2=0;if(sem2){(*lincr)++;sem2=0;}
s[0]=getc(fis);if(s[0]=='\n'){(*lincr)++;}
  while((!(feof(fis)))&&(verdel(s[0]))){s[0]=getc(fis);if(s[0]=='\n'){(*lincr)++;}}
  /*sarit peste caracterele de dinaintea cuvantului*/
i=1;
  while((!(feof(fis)))&&(i<MAXWLG)&&(!verdel(s[i-1])))
  {s[i]=getc(fis);i++;} if(!verdel(s[i-1])){sem=1;}
  if(s[i-1]=='\n'){sem2=1;} s[i-1]='\0';
return sem;} /*1 daca s-a depasit lungimea maxima a cuvantului sau sfarsitul fisierului si 0 daca nu*/


/*functie care gaseste numarul de ordine al comenzii*/
int identcom(char *s)
{if(strcmp(s,"objtypes")==0){return 1;} /*pt. declarat nr. de tipuri de obiecte*/
 if(strcmp(s,"objects")==0){return 2;} /*pt. declarat nr. de obiecte*/
 return 0;}


/*functie care afiseaza un mesaj de eroare si iese din program daca e cazul*/
void afermex(char *numefis,int lincr,char *s,int cdvr)
{/*cdvr - conditie de verificare (0-intreg,1-cuvant,2-real)*/
if((cdvr==0)&&(detsir(s)!=0)){printf("Error: '%s' line %d - integer expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==1)&&(detsir(s)!=1)){printf("Error: '%s' line %d - word expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==2)&&(detsir(s)!=2)&&(detsir(s)!=0)){printf("Error: '%s' line %d - number expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
}
