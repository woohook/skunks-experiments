/*
Copyright (C) 2005-2013 Victor Matei Petrescu

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


#define COLOR_BLACK     0
#define COLOR_BLUE      35
#define COLOR_RED       15
#define COLOR_MAGENTA   40
#define COLOR_GREEN     25
#define COLOR_CYAN      30
#define COLOR_YELLOW    20
#define COLOR_WHITE     10
#define COLOR_PAIR(i)   i
#define curs_set(i)
#define endwin()


SDL_Event eventglob;
SDL_Surface *screenglob;
Uint32 colglob[320];
int xcposglob,ycposglob,colorglob,bkgcolglob,bkgclerglob,  /*coordonate curente caracter pe x si y*/
    tabcolglob[20],tabcbkglob[20];  /*sir perechi culori (tip curses)*/


int gmodul(int x){if(x<0){x=-x;} return x;}


/*functie care converteste un numar real POZITIV (sau negativ) x cu p zecimale intr-un sir de caractere*/
void sirconv(double x,int p,char *s){
int i,j,k,cifra[30];
double x1;
static char cr[11]={'0','1','2','3','4','5','6','7','8','9'};
char cifcr[30],semn;

if(x<0){x=-x;semn='m';}else{semn='p';}

j=0;
 for(i=1;i<=p;i++){x=x*10;}x=floor(x);
 while(x>=1){
  x1=floor(x/10);cifra[++j]=(int)(x-x1*10);x=x1;
 }
   for(k=1;k<=j;k++){cifcr[k]=cr[cifra[j+1-k]];}
 if(j<=p){
 s[0]='0';s[1]='.';
 for(k=1;k<=(p-j);k++){s[1+k]='0';}
 for(k=p-j+1;k<=p;k++){s[1+k]=cifcr[k-p+j];}
 s[p+2]='\0';}else{
 for(k=1;k<=(j-p);k++){s[k-1]=cifcr[k];}
 s[j-p]='.';
 for(k=j-p+1;k<=j;k++){s[k]=cifcr[k];}
 s[j+1]='\0';} 

if(semn=='m'){k=0;while(s[k]){k++;}for(i=k;i>=0;i--){s[i+1]=s[i];}s[0]='-';}
}


/*functie care converteste un numar intreg POZITIV intr-un sir de caractere*/
void sircvint(int x,char *s){
int j,k,cifra[30],x1;
static char cr[11]={'0','1','2','3','4','5','6','7','8','9'};
j=0;
  while(x>=1){x1=x%10;x-=x1;x=x/10;cifra[++j]=x1;}
  for(k=1;k<=j;k++){s[k-1]=cr[cifra[j-k+1]];}
  s[j]='\0';
}


void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{   int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
    case 1:
        *p = pixel;
        break;
    case 2:
        *(Uint16 *)p = pixel;
        break;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32 *)p = pixel;
        break;}}


int grafic(int *tablou,int width, int height) /*elements of tablou: colors of pixels */
{int i,j;

/*Lock screen*/
  if ( SDL_MUSTLOCK(screenglob) ) {
        if ( SDL_LockSurface(screenglob) < 0 ) {
            printf("Can't lock screen: %s\n", SDL_GetError());SDL_Quit();return 0;}}

 for(i=0;i<=height-1;i++){
    for(j=0;j<=width-1;j++){
 putpixel(screenglob,j,i,colglob[tablou[i*width+j]]);

 }}

/*Unlock screen*/
 if ( SDL_MUSTLOCK(screenglob) ) {SDL_UnlockSurface(screenglob);}
/* Update display*/
    SDL_UpdateRect(screenglob, 0, 0, 0, 0);

return 1;}


/*the function below can do many things*/
int mdraw(int x,int y,int z,int col,char s)
{static int *tablou;
long int i,j,x2,y2,area;
float rap;
static int width,height,x1=0,y1=0,sem=0,sem1=0;

 if((!sem)&&(s!='i')){if(!sem1){printf("You forgot to initialize the screen\n");}
                 sem1=1;return 0;}
 if((sem==1)&&(s=='i')){printf("You tried to initialize the screen more than once\n");return 0;}

	switch(s){

	case 'i':
	width=x;height=y;area=(x+2)*(y+2);sem=1;
	if(!(tablou=(int *)malloc(area*sizeof(int)))){printf("Out of memory\n");return 0;}
	for(i=0;i<=area-2;i++){tablou[i]=col;}
	/*printf("Screen initialized: %dx%d\n",width,height);*/
	break;

	case 'p':
	x1=x;y1=y;tablou[(height-y1-1)*width+x1]=col; break;

	case 'l':
	x2=x1+x;y2=y1+y;
if((gmodul(x2-x1))||(gmodul(y2-y1))){
  if (gmodul(x1-x2)<gmodul(y1-y2))
   {rap=(float)(x2-x1)/(y2-y1);
    if(y2>y1)
    {for(i=y1;i<=y2;i++){j=(int)(x1+(i-y1)*rap);tablou[(height-i-1)*width+j]=col;}}else
    {for(i=y1;i>=y2;i--){j=(int)(x1+(i-y1)*rap);tablou[(height-i-1)*width+j]=col;}}
   }else
    {rap=(float)(y2-y1)/(x2-x1);
    if(x2>x1)
    {for(j=x1;j<=x2;j++){i=(int)(y1+(j-x1)*rap);tablou[(height-i-1)*width+j]=col;}}else
    {for(j=x1;j>=x2;j--){i=(int)(y1+(j-x1)*rap);tablou[(height-i-1)*width+j]=col;}}
    }
}else{tablou[(height-y1-1)*width+x1]=col;} x1=x2;y1=y2;break;

	case 'c':
	for(i=1;i<=12.5664*z;i++){
	x2=(int)(x+z*cos((float)i/6.2832));y2=(int)(y+z*sin((float)i/6.2832));
	tablou[(height-y2-1)*width+x2]=col;} break;	

/*display image with SDL*/
	case 'a':
	grafic(tablou,width,height);break;

	case 'f':
	free(tablou);break;

	}

return 1;}


int initgraph(int width,int height,int col)
{int i,j;
/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}
/*Initialize display*/
screenglob=SDL_SetVideoMode(width,height,16,SDL_SWSURFACE);
if(screenglob == NULL){printf("Couldn't set requested video mode: %s\n",SDL_GetError());SDL_Quit();return 0;}
printf("Set %dx%dx%d\n",(screenglob->pitch)/(screenglob->format->BytesPerPixel),height,screenglob->format->BitsPerPixel);
/*Set window title*/
SDL_WM_SetCaption("SDLgraph", "SDLgraph");
/* Enable Unicode translation */
SDL_EnableUNICODE(1);
/*SDL initialized*/

/* Define colors*/
/*col[0...10]: black...gray...white ; col[11...40]: all colors of rainbow, in order*/
  for(i=0;i<=10;i++){colglob[i]=SDL_MapRGB(screenglob->format,(int)(i*25.5),(int)(i*25.5),(int)(i*25.5));}
  for(i=11;i<=15;i++){
  /*
  colglob[i]=SDL_MapRGB(screenglob->format,255,i*51,0);
  colglob[i+5]=SDL_MapRGB(screenglob->format,255-i*51,255,0);
  colglob[i+10]=SDL_MapRGB(screenglob->format,0,255,i*51);
  colglob[i+15]=SDL_MapRGB(screenglob->format,0,255-i*51,255);
  colglob[i+20]=SDL_MapRGB(screenglob->format,i*51,0,255);
  colglob[i+25]=SDL_MapRGB(screenglob->format,255,0,255-i*51);
  */
  colglob[i]=SDL_MapRGB(screenglob->format,i*51,0,0); /*red 11-15*/
  colglob[i+5]=SDL_MapRGB(screenglob->format,i*51,i*51,0); /*yellow 16-20*/
  colglob[i+10]=SDL_MapRGB(screenglob->format,0,i*51,0); /*green 21-25*/
  colglob[i+15]=SDL_MapRGB(screenglob->format,0,i*51,i*51); /*cyan 26-30*/
  colglob[i+20]=SDL_MapRGB(screenglob->format,0,0,i*51); /*blue 31-35*/
  colglob[i+25]=SDL_MapRGB(screenglob->format,i*51,0,i*51); /*magenta 36-40*/
  }

/*Lock screen*/
  if ( SDL_MUSTLOCK(screenglob) ) {
        if ( SDL_LockSurface(screenglob) < 0 ) {
            printf("Can't lock screen: %s\n", SDL_GetError());SDL_Quit();return 0;}}

 for(i=0;i<=height-1;i++){
    for(j=0;j<=width-1;j++){
 putpixel(screenglob,j,i,colglob[col]);

 }}

/*Unlock screen*/
 if ( SDL_MUSTLOCK(screenglob) ) {SDL_UnlockSurface(screenglob);}
/* Update display*/
    SDL_UpdateRect(screenglob, 0, 0, 0, 0);

mdraw(width,height,0,col,'i'); /*initializare tablou cu pixeli (aia din memorie, nu de pe ecran)*/

return 1;
}


/*draws 1 character*/
void drawchar1(float *coord,int x,int y,int i,int h,int col,float caz)
{int j;
mdraw((int)(x+2*caz*h*coord[(i-1)*12+1]),(int)(y+h*coord[(i-1)*12+2]),0,col,'p');
for(j=3;j<=11;j=j+2){
	mdraw((int)(2*caz*h*coord[(i-1)*12+j]),(int)(h*coord[(i-1)*12+j+1]),0,col,'l');
	}
}


/*draws fragment of character*/
void drawchaux(int x,int y,char a,int h,int col,float caz)
{float crd[14]={0,0,0,0,0,0,0,0,0,0,0,0};
int j;
  switch(a){
    case 'i': crd[1]=0.25;crd[2]=0.75;crd[4]=0.1;break;
    case 'm': crd[1]=0.1;crd[2]=0.35;crd[4]=0.3;crd[5]=0.15;crd[8]=-0.3;break;
    case 'w': crd[1]=0.1;crd[2]=0.65;crd[4]=-0.3;crd[5]=0.15;crd[8]=0.3;break;
    case '3': crd[1]=0.15;crd[2]=0.9;crd[3]=0.2;crd[6]=-0.2;break;
    case '8': crd[1]=0.15;crd[2]=0.7;crd[4]=0.2;crd[5]=0.2;crd[8]=-0.2;break;
    case '0': crd[1]=0.22;crd[2]=0.6;crd[4]=0.1;break;
    default: break;
  }
mdraw((int)(x+2*caz*h*crd[1]),(int)(y+h*crd[2]),0,col,'p');
for(j=3;j<=11;j=j+2){
	mdraw((int)(2*caz*h*crd[j]),(int)(h*crd[j+1]),0,col,'l');
	}
}


/*draws 1 character; needs drawchar1() to work*/
void drawchar(int x,int y,int h,int col,char a,float caz)
{
/*define letters - coord[] is static so it is initialized only the first time that the function is called*/
static float coord[1000]={0, 0.1 , 0.65 ,0.3 , 0   , 0   ,-0.3 ,-0.3 , 0   , 0   , 0.15, 0.3 , 0   ,  /*a*/
                     0.1 , 0.65, 0.3 , 0   , 0   ,-0.3 ,-0.3 , 0   , 0   , 0.55, 0   , 0   ,  /*b*/
		     0.4 , 0.35,-0.3 , 0   , 0   , 0.3 , 0.3 , 0   , 0   , 0   , 0   , 0   ,  /*c*/
		     0.4 , 0.65,-0.3 , 0   , 0   ,-0.3 , 0.3 , 0   , 0   , 0.55, 0   , 0   ,  /*d*/
		     0.4 , 0.35,-0.3 , 0   , 0   , 0.3 , 0.3 , 0   , 0   ,-0.15,-0.3 , 0   ,  /*e*/
		     0.2 , 0.35, 0   , 0.3 , 0.15, 0   ,-0.15, 0   , 0   , 0.25, 0.15, 0   ,  /*f*/
		     0.4 , 0.35,-0.3 , 0   , 0   , 0.3 , 0.3 , 0   , 0   ,-0.55,-0.3 , 0   ,  /*g*/
		     0.4 , 0.35, 0   , 0.3 ,-0.3 , 0   , 0   ,-0.3 , 0   , 0.55, 0   , 0   ,  /*h*/
		     0.25, 0.35, 0   , 0.3 , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*i*/
		     0.25, 0.6 , 0   ,-0.45,-0.15, 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*j*/
		     0.4 , 0.35,-0.3 , 0.15, 0.3 , 0.15,-0.3 ,-0.15, 0   ,-0.15, 0   , 0.55,  /*k*/
		     0.4 , 0.35,-0.15, 0   , 0   , 0.55, 0   , 0   , 0   , 0   , 0   , 0   ,  /*l*/
		     0.1 , 0.35, 0   , 0.3 , 0.3 , 0   , 0   ,-0.3 , 0   , 0   , 0   , 0   ,  /*m*/
		     0.1 , 0.65, 0.1 , 0   , 0   ,-0.3 , 0   , 0.3 , 0.2 , 0   , 0   ,-0.3 ,  /*n*/
		     0.1 , 0.35, 0   , 0.3 , 0.3 , 0   , 0   ,-0.3 ,-0.3 , 0   , 0   , 0   ,  /*o*/
		     0.1 , 0.35, 0.3 , 0   , 0   , 0.3 ,-0.3 , 0   , 0   ,-0.55, 0   , 0   ,  /*p*/
		     0.4 , 0.35,-0.3 , 0   , 0   , 0.3 , 0.3 , 0   , 0   ,-0.55, 0   , 0   ,  /*q*/
		     0.1 , 0.35, 0   , 0.3 , 0.3 , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*r*/
		     0.1 , 0.35, 0.3 , 0   , 0   , 0.15,-0.3 , 0   , 0   , 0.15, 0.3 , 0   ,  /*s*/
		     0.2 , 0.9 , 0   ,-0.25, 0.15, 0   ,-0.15, 0   , 0   ,-0.3 , 0.15, 0   ,  /*t*/
		     0.1 , 0.65, 0   ,-0.3 , 0.3 , 0   , 0   , 0.3 , 0   , 0   , 0   , 0   ,  /*u*/
		     0.1 , 0.65, 0.15,-0.3 ,0.15 , 0.3 , 0   , 0   , 0   , 0   , 0   , 0   ,  /*v*/
		     0.1 , 0.65, 0   ,-0.3 , 0.3 , 0   , 0   , 0.3 , 0   , 0   , 0   , 0   ,  /*w*/
		     0.1 , 0.35, 0.15, 0.15,-0.15, 0.15, 0.3 ,-0.3 ,-0.15, 0.15, 0.15, 0.15,  /*x*/
		     0.1 , 0.65, 0.15,-0.3 , 0.15, 0.3 ,-0.15,-0.3 ,-0.1 ,-0.25, 0   , 0   ,  /*y*/
		     0.1 , 0.65, 0.3 , 0   ,-0.3 ,-0.3 , 0.3 , 0   , 0   , 0   , 0   , 0   ,  /*z*/

		     0.1 , 0.35, 0   , 0.55, 0.3 , 0   , 0   ,-0.55, 0   , 0.3 ,-0.3 , 0   ,  /*A*/
                     0.15, 0.35, 0.3 , 0   , 0   , 0.3 ,-0.3 , 0   , 0   ,-0.3 , 0   , 0.55,  /*B*/
		     0.4 , 0.35,-0.3 , 0   , 0   , 0.55, 0.3 , 0   , 0   , 0   , 0   , 0   ,  /*C*/
		     0.1 , 0.35, 0   , 0.55, 0.05, 0   , 0.25,-0.25, 0   ,-0.3 ,-0.3 , 0   ,  /*D*/
		     0.4 , 0.35,-0.3 , 0   , 0   , 0.55, 0.3 , 0   , 0   , 0   , 0   , 0   ,  /*E*/
		     0.1 , 0.35, 0   , 0.3 , 0.3 , 0   ,-0.3 , 0   , 0   , 0.25, 0.3 , 0   ,  /*F*/
		     0.4 , 0.9 ,-0.3 , 0   , 0   ,-0.55, 0.3 , 0   , 0   , 0.3 ,-0.1 , 0   ,  /*G*/
		     0.4 , 0.35, 0   , 0.55, 0   ,-0.25,-0.3 , 0   , 0   , 0.25, 0   ,-0.55,  /*H*/
		     0.25, 0.35, 0   , 0.55, 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*I*/
		     0.1 , 0.45, 0   ,-0.1 , 0.3 , 0   , 0   , 0.55, 0   , 0   , 0   , 0   ,  /*J*/
		     0.4 , 0.35,-0.3 , 0.3 , 0.3 , 0.25,-0.3 ,-0.25, 0   ,-0.3 , 0   , 0.55,  /*K*/
		     0.4 , 0.35,-0.3 , 0   , 0   , 0.55, 0   , 0   , 0   , 0   , 0   , 0   ,  /*L*/
		     0.1 , 0.35, 0   , 0.55, 0.15,-0.3 , 0.15, 0.3 , 0   ,-0.55, 0   , 0   ,  /*M*/
		     0.1 , 0.35, 0   , 0.55, 0.3 ,-0.55, 0   , 0.55, 0   , 0   , 0   , 0   ,  /*N*/
		     0.1 , 0.35, 0   , 0.55, 0.3 , 0   , 0   ,-0.55,-0.3 , 0   , 0   , 0   ,  /*O*/
		     0.1 , 0.35, 0   , 0.55, 0.3 , 0   , 0   ,-0.25,-0.3 , 0   , 0   , 0   ,  /*P*/
		     0.4 , 0.35, 0   , 0.55,-0.3 , 0   , 0   ,-0.55, 0.3 , 0   ,-0.1 , 0.1 ,  /*Q*/
		     0.1 , 0.35, 0   , 0.55, 0.3 , 0   , 0   ,-0.25,-0.3 , 0   , 0.3 ,-0.3 ,  /*R*/
		     0.1 , 0.35, 0.3 , 0   , 0   , 0.3 ,-0.3 , 0   , 0   , 0.25, 0.3 , 0   ,  /*S*/
		     0.25, 0.35, 0   , 0.55,-0.15, 0   , 0.3 , 0   , 0   , 0   , 0   , 0   ,  /*T*/
		     0.1 , 0.9 , 0   ,-0.55, 0.3 , 0   , 0   , 0.55, 0   , 0   , 0   , 0   ,  /*U*/
		     0.1 , 0.9 , 0.15,-0.55, 0.15, 0.55, 0   , 0   , 0   , 0   , 0   , 0   ,  /*V*/
		     0.1 , 0.9 , 0   ,-0.55, 0.15, 0.3 , 0.15,-0.3 , 0   , 0.55, 0   , 0   ,  /*W*/
		     0.1 , 0.35, 0.15, 0.3 ,-0.15, 0.3 , 0.3 ,-0.6 ,-0.15, 0.3 , 0.15, 0.3 ,  /*X*/
		     0.1 , 0.95, 0.15,-0.3 , 0.15, 0.3 ,-0.3 ,-0.6 , 0   , 0   , 0   , 0   ,  /*Y*/
		     0.1 , 0.9 , 0.3 , 0   ,-0.3 ,-0.55, 0.3 , 0   , 0   , 0   , 0   , 0   ,  /*Z*/

		     0.35, 0.35, 0   , 0.55,-0.15,-0.25, 0   , 0   , 0   , 0   , 0   , 0   ,  /*1*/
		     0.4 , 0.35,-0.3 , 0   , 0.3 , 0.3 , 0   , 0.25,-0.3 , 0   , 0   ,-0.1 ,  /*2*/
		     0.15, 0.35, 0.3 , 0   , 0   , 0.3 ,-0.3 , 0   , 0   , 0   , 0   , 0   ,  /*3*/
		     0.35, 0.35, 0   , 0.55,-0.2 ,-0.4 , 0.3 , 0   , 0   , 0   , 0   , 0   ,  /*4*/
		     0.1 , 0.35, 0.3 , 0   , 0   , 0.3 ,-0.3 , 0   , 0   , 0.25, 0.3 , 0   ,  /*5*/
		     0.4 , 0.9 ,-0.3 , 0   , 0   ,-0.55, 0.3 , 0   , 0   , 0.3 ,-0.3 , 0   ,  /*6*/
		     0.15, 0.9 , 0.35, 0   ,-0.25,-0.55, 0   , 0   , 0   , 0   , 0   , 0   ,  /*7*/
		     0.1 , 0.35, 0.3 , 0   , 0   , 0.3 ,-0.3 , 0   , 0   ,-0.3 , 0   , 0   ,  /*8*/
		     0.1 , 0.35, 0.3 , 0   , 0   , 0.55,-0.3 , 0   , 0   ,-0.3 , 0.3 , 0   ,  /*9*/
		     0.1 , 0.35, 0   , 0.55, 0.3 , 0   , 0   ,-0.55,-0.3 , 0   , 0   , 0   ,  /*0*/
		     0.35, 0.35, 0   , 0.15,-0.15, 0   , 0   ,-0.15, 0.15, 0   , 0   , 0   ,  /*.*/
		     0.4 , 0.55,-0.3 , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*-*/
		     0.05, 0.35, 0.35, 0.55, 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*slasz*/
		     0.25, 0.35,-0.1 ,-0.1 , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*,*/
		     0.15, 0.35, 0.15, 0.2 , 0   , 0.15,-0.15, 0.2 , 0   , 0   , 0   , 0   ,  /*)*/
		     0.35, 0.35,-0.15, 0.2 , 0   , 0.15, 0.15, 0.2 , 0   , 0   , 0   , 0   ,  /*(*/
		     0.25, 0.35, 0   , 0.1 , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*:*/
		     0.2 , 0.35, 0.3 , 0.2 ,-0.3 , 0.2 , 0   ,-0.4 , 0   , 0   , 0   , 0   ,  /*cursor(#)*/
		     0.25, 0.55, 0   , 0.1 , 0.1 , 0.1 , 0   , 0.1 ,-0.3 , 0   , 0   ,-0.1 ,  /*?*/
		     0.1 , 0.7 , 0   , 0.15,-0.15, 0   , 0   ,-0.15, 0.15, 0   , 0   , 0   ,  /*%*/
		     };

  switch(a){

  case 'a': drawchar1(coord,x,y,1,h,col,caz);break;
  case 'b': drawchar1(coord,x,y,2,h,col,caz);break;
  case 'c': drawchar1(coord,x,y,3,h,col,caz);break;
  case 'd': drawchar1(coord,x,y,4,h,col,caz);break;
  case 'e': drawchar1(coord,x,y,5,h,col,caz);break;
  case 'f': drawchar1(coord,x,y,6,h,col,caz);break;
  case 'g': drawchar1(coord,x,y,7,h,col,caz);break;
  case 'h': drawchar1(coord,x,y,8,h,col,caz);break;
  case 'i': drawchar1(coord,x,y,9,h,col,caz);drawchaux(x,y,'i',h,col,caz);break;
  case 'j': drawchar1(coord,x,y,10,h,col,caz);drawchaux(x,y,'i',h,col,caz);break;
  case 'k': drawchar1(coord,x,y,11,h,col,caz);break;
  case 'l': drawchar1(coord,x,y,12,h,col,caz);break;
  case 'm': drawchar1(coord,x,y,13,h,col,caz);drawchaux(x,y,'m',h,col,caz);break;
  case 'n': drawchar1(coord,x,y,14,h,col,caz);break;
  case 'o': drawchar1(coord,x,y,15,h,col,caz);break;
  case 'p': drawchar1(coord,x,y,16,h,col,caz);break;
  case 'q': drawchar1(coord,x,y,17,h,col,caz);break;
  case 'r': drawchar1(coord,x,y,18,h,col,caz);break;
  case 's': drawchar1(coord,x,y,19,h,col,caz);break;
  case 't': drawchar1(coord,x,y,20,h,col,caz);break;
  case 'u': drawchar1(coord,x,y,21,h,col,caz);break;
  case 'v': drawchar1(coord,x,y,22,h,col,caz);break;
  case 'w': drawchar1(coord,x,y,23,h,col,caz);drawchaux(x,y,'w',h,col,caz);break;
  case 'x': drawchar1(coord,x,y,24,h,col,caz);break;
  case 'y': drawchar1(coord,x,y,25,h,col,caz);break;
  case 'z': drawchar1(coord,x,y,26,h,col,caz);break;

  case 'A': drawchar1(coord,x,y,27,h,col,caz);break;
  case 'B': drawchar1(coord,x,y,28,h,col,caz);drawchaux(x,y,'3',h,col,caz);break;
  case 'C': drawchar1(coord,x,y,29,h,col,caz);break;
  case 'D': drawchar1(coord,x,y,30,h,col,caz);break;
  case 'E': drawchar1(coord,x,y,31,h,col,caz);drawchar1(coord,x,y,64,h,col,caz);break;
  case 'F': drawchar1(coord,x,y,32,h,col,caz);break;
  case 'G': drawchar1(coord,x,y,33,h,col,caz);break;
  case 'H': drawchar1(coord,x,y,34,h,col,caz);break;
  case 'I': drawchar1(coord,x,y,35,h,col,caz);break;
  case 'J': drawchar1(coord,x,y,36,h,col,caz);break;
  case 'K': drawchar1(coord,x,y,37,h,col,caz);break;
  case 'L': drawchar1(coord,x,y,38,h,col,caz);break;
  case 'M': drawchar1(coord,x,y,39,h,col,caz);break;
  case 'N': drawchar1(coord,x,y,40,h,col,caz);break;
  case 'O': drawchar1(coord,x,y,41,h,col,caz);break;
  case 'P': drawchar1(coord,x,y,42,h,col,caz);break;
  case 'Q': drawchar1(coord,x,y,43,h,col,caz);break;
  case 'R': drawchar1(coord,x,y,44,h,col,caz);break;
  case 'S': drawchar1(coord,x,y,45,h,col,caz);break;
  case 'T': drawchar1(coord,x,y,46,h,col,caz);break;
  case 'U': drawchar1(coord,x,y,47,h,col,caz);break;
  case 'V': drawchar1(coord,x,y,48,h,col,caz);break;
  case 'W': drawchar1(coord,x,y,49,h,col,caz);break;
  case 'X': drawchar1(coord,x,y,50,h,col,caz);break;
  case 'Y': drawchar1(coord,x,y,51,h,col,caz);break;
  case 'Z': drawchar1(coord,x,y,52,h,col,caz);break;

  case '1': drawchar1(coord,x,y,53,h,col,caz);break;
  case '2': drawchar1(coord,x,y,54,h,col,caz);break;
  case '3': drawchar1(coord,x,y,55,h,col,caz);drawchaux(x,y,'3',h,col,caz);break;
  case '4': drawchar1(coord,x,y,56,h,col,caz);break;
  case '5': drawchar1(coord,x,y,57,h,col,caz);break;
  case '6': drawchar1(coord,x,y,58,h,col,caz);break;
  case '7': drawchar1(coord,x,y,59,h,col,caz);break;
  case '8': drawchar1(coord,x,y,60,h,col,caz);drawchaux(x,y,'8',h,col,caz);break;
  case '9': drawchar1(coord,x,y,61,h,col,caz);break;
  case '0': drawchar1(coord,x,y,62,h,col,caz);drawchaux(x,y,'0',h,col,caz);break;
  case '.': drawchar1(coord,x,y,63,h,col,caz);break;
  case ':': drawchar1(coord,x,y,69,h,col,caz);drawchar1(coord,x,y+0.3*h,69,h,col,caz);break;
  case '-': drawchar1(coord,x,y,64,h,col,caz);break;
  case '=': drawchar1(coord,x,y-0.1*h,64,h,col,caz);drawchar1(coord,x,y+0.05*h,64,h,col,caz);break;
  case '/': drawchar1(coord,x,y,65,h,col,caz);break;
  case ',': drawchar1(coord,x,y,66,h,col,caz);break;
  case ';': drawchar1(coord,x,y,66,h,col,caz);drawchar1(coord,x,y+0.3*h,69,h,col,caz);break;
  case ')': drawchar1(coord,x,y,67,h,col,caz);break;
  case '(': drawchar1(coord,x,y,68,h,col,caz);break;
  case '#': drawchar1(coord,x,y,70,h,col,caz);break;
  case '?': drawchar1(coord,x,y,71,h,col,caz);drawchar1(coord,x,y,69,h,col,caz);break;
  case '%': drawchar1(coord,x,y,65,h,col,caz);drawchar1(coord,x,y,63,h,col,caz);drawchar1(coord,x,y,72,h,col,caz);break;
  case '\'': drawchar1(coord,x,y+0.4*h,69,h,col,caz);break;
  case '"': drawchar1(coord,x,y+0.4*h,69,h,col,caz);break;
  default: break;
 }

}


/*draws text*/
void dtext(char *s,float x,float y,float h,float col)
{
while(*s){drawchar((int)x,(int)y,(int)h,(int)col,*(s++),0.5);x=x+h*0.5;}
}


void move(int y,int x)
{xcposglob=x;
ycposglob=y;}  /*curses (needed b4 dtextcs)*/


/*draws text - curses*/
void dtextcs(char *s)
{float x,y;
int xi1,xi2,yi1,yi2,i,tmp;
  while(*s){
    switch(*s){
        case '\r': move(ycposglob,0);break;
	case '\n': move(ycposglob+1,0);break;
	default: x=xcposglob*10;y=459-ycposglob*20;
		   xi1=(int)x; yi1=(int)y; xi2=xi1+9; yi2=yi1+19;
		   tmp=xi2-xi1+1;
		   for(i=yi1;i<=yi2;i++)
		   {mdraw(xi1,i,0,bkgcolglob,'p');mdraw(tmp,0,0,bkgcolglob,'l');}
		 drawchar((int)x,(int)y,20,colorglob,*s,0.5);
		 move(ycposglob,xcposglob+1);
		 break;
    } s++;
  }
}


/*draws point*/
void dpoint(float x,float y,float col){mdraw((int)x,(int)y,0,(int)col,'p');}


/*draws line*/
void dline(float x,float y,float col){mdraw((int)x,(int)y,0,(int)col,'l');}


/*draws circle*/
void dcircle(float x,float y,float z,float col){mdraw((int)x,(int)y,(int)z,(int)col,'c');}


/*draws rectangle*/
void drectangle(float x1,float y1,float x2,float y2,float col)
{int xi1=(int)x1,xi2=(int)x2,yi1=(int)y1,yi2=(int)y2,coli=(int)col,tmp,i;
if(xi1>xi2){tmp=xi1;xi1=xi2;xi2=tmp;}
if(yi1>yi2){tmp=yi1;yi1=yi2;yi2=tmp;}
tmp=xi2-xi1;
for(i=yi1;i<=yi2;i++)
{mdraw(xi1,i,0,coli,'p');mdraw(tmp,0,0,coli,'l');}
}


void bkgd(int i){bkgclerglob=tabcbkglob[i];}


void erase(){drectangle(0,0,639,479,bkgclerglob);}


/*displays image with SDL*/
void displaysdl()
{int rez=0;

drectangle(0,461,40,479,30);dpoint(0,460,15);dline(42,0,15);dline(0,19,15);
dpoint(5,478,10);dline(0,-16,10);dline(-3,6,10);dline(3,-6,10);dline(3,6,10);
dtext("ESC",10,460,20,10); /*scris "ESC" in stanga sus*/

mdraw(0,0,0,0,'a');
while(!rez){
while(SDL_PollEvent(&eventglob)){
switch(eventglob.type){

case SDL_KEYDOWN:
	switch(eventglob.key.keysym.sym){
		case SDLK_ESCAPE: rez='1';break;
		case SDLK_x: rez='1';break;
		case SDLK_q: rez='1';break;
		case SDLK_RETURN: rez='1';break;
		case SDLK_KP_ENTER: rez='1';break;

		default: break;

	} break;
case SDL_KEYUP:
	switch(eventglob.key.keysym.sym){

		default:break;
	} break;
	
case SDL_QUIT: SDL_Quit();exit(1);

}}}
}


/*frees table with pixels created with initgraph()*/
void freegraph()
{mdraw(0,0,0,0,'f');SDL_Quit();}


/*displays image with SDL*/
void refresh(){mdraw(0,0,0,0,'a');}


void printw(const char *format, ...)
{char sir[256];
va_list argp;
va_start(argp,format);
vsprintf(sir,format,argp);
dtextcs(sir);
va_end(argp);
}


void initscr(){xcposglob=0;ycposglob=0;}


void start_color(){colorglob=10;bkgcolglob=0;bkgclerglob=0;}


void init_pair(int i,int col,int cbk)
{tabcolglob[i]=col;tabcbkglob[i]=cbk;}


void attron(int i)
{colorglob=tabcolglob[i];bkgcolglob=tabcbkglob[i];}


char getch_graph()
{char rez=0;
int act=0; /*0-scrie;1-nu scrie*/

printw("#");move(ycposglob,xcposglob-1);refresh();

while(!rez){ SDL_Delay(10);
while(SDL_PollEvent(&eventglob)){
switch(eventglob.type){

case SDL_KEYDOWN:
	switch(eventglob.key.keysym.sym){
		case SDLK_a: rez='a';break;
		case SDLK_b: rez='b';break;
		case SDLK_c: rez='c';break;
		case SDLK_d: rez='d';break;
		case SDLK_e: rez='e';break;
		case SDLK_f: rez='f';break;
		case SDLK_g: rez='g';break;
		case SDLK_h: rez='h';break;
		case SDLK_i: rez='i';break;
		case SDLK_j: rez='j';break;
		case SDLK_k: rez='k';break;
		case SDLK_l: rez='l';break;
		case SDLK_m: rez='m';break;
		case SDLK_n: rez='n';break;
		case SDLK_o: rez='o';break;
		case SDLK_p: rez='p';break;
		case SDLK_q: rez='q';break;
		case SDLK_r: rez='r';break;
		case SDLK_s: rez='s';break;
		case SDLK_t: rez='t';break;
		case SDLK_u: rez='u';break;
		case SDLK_v: rez='v';break;
		case SDLK_w: rez='w';break;
		case SDLK_x: rez='x';break;
		case SDLK_y: rez='y';break;
		case SDLK_z: rez='z';break;
		case SDLK_0: rez='0';break;
		case SDLK_1: rez='1';break;
		case SDLK_2: rez='2';break;
		case SDLK_3: rez='3';break;
		case SDLK_4: rez='4';break;
		case SDLK_5: rez='5';break;
		case SDLK_6: rez='6';break;
		case SDLK_7: rez='7';break;
		case SDLK_8: rez='8';break;
		case SDLK_9: rez='9';break;
		case SDLK_KP0: rez='0';break;
		case SDLK_KP1: rez='1';break;
		case SDLK_KP2: rez='2';break;
		case SDLK_KP3: rez='3';break;
		case SDLK_KP4: rez='4';break;
		case SDLK_KP5: rez='5';break;
		case SDLK_KP6: rez='6';break;
		case SDLK_KP7: rez='7';break;
		case SDLK_KP8: rez='8';break;
		case SDLK_KP9: rez='9';break;
		case SDLK_SPACE: rez=' ';break;
		case SDLK_PERIOD: rez='.';break;
		case SDLK_KP_PERIOD: rez='.';break;
		case SDLK_COMMA: rez='.';break;
		case SDLK_MINUS: rez='-';break;
		case SDLK_KP_MINUS: rez='-';break;
		case SDLK_BACKSPACE: rez='!';act=1;break;
		case SDLK_DELETE: rez='!';act=1;break;
		case SDLK_RETURN: rez='^';act=1;break;
		case SDLK_KP_ENTER: rez='^';act=1;break;

		default: act=0;break;

	} break;
case SDL_KEYUP:
	switch(eventglob.key.keysym.sym){

		default:break;
	} break;
	
case SDL_QUIT: SDL_Quit();exit(1);

}}}
   switch(act){
     case 0: printw("%c",rez);break;
     case 1: printw(" ");move(ycposglob,xcposglob-1);break;
     default: break;
   }
return rez;}


void scanw(const char *format, ...)
{va_list argp;
char sir[256];
int i=0,sem=0;
  while(!sem){
    sir[i]=getch_graph();
    switch(sir[i]){
      case '!': if(i>=1){i--;move(ycposglob,xcposglob-1);} break;
      case '^': sem=1;sir[i]='\0';printw(" "); break;
      default: i++; break;
    }
  }
va_start(argp,format);
vsscanf(sir,format,argp);
va_end(argp);
}
