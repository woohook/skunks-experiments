/*
Copyright (C) 2007-2011 Victor Matei Petrescu

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


int SCREENWIDTH=800;
/*screen width (pixels)*/

int SCREENHEIGHT=600;
/*screen height (pixels)*/


#if CLBITS==16

/*draw points and lines*/
void mdraw(SDL_Surface *screen,int x,int y,char s)
{long int i,j,x2,y2;
float rap;
static int x1=0,y1=0;
Uint16 *ptr,bright;
int width,height,red=255,green=255,blue=255;

bright=SDL_MapRGB(screen->format,red,green,blue);
width=SCREENWIDTH; height=SCREENHEIGHT;

switch(s){
  case 'p': x1=x;y1=y;ptr=(Uint16 *)((Uint8 *)screen->pixels + (height-y1)*screen->pitch + x1*2); *ptr=bright;
            break;

  case 'l': x2=x1+x;y2=y1+y;
            if((fabs(x2-x1))||(fabs(y2-y1))){
              if (fabs(x1-x2)<fabs(y1-y2)){
                rap=(float)(x2-x1)/(y2-y1);
                if(y2>y1){
                  for(i=y1;i<=y2;i++){j=(int)(x1+(i-y1)*rap);
                    ptr=(Uint16 *)((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*2); *ptr=bright;}
                }else{
                  for(i=y1;i>=y2;i--){j=(int)(x1+(i-y1)*rap);
                    ptr=(Uint16 *)((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*2); *ptr=bright;}}
              }else{
                rap=(float)(y2-y1)/(x2-x1);
                if(x2>x1){
                  for(j=x1;j<=x2;j++){i=(int)(y1+(j-x1)*rap);
                    ptr=(Uint16 *)((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*2); *ptr=bright;}
                }else{
                  for(j=x1;j>=x2;j--){i=(int)(y1+(j-x1)*rap);
                    ptr=(Uint16 *)((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*2); *ptr=bright;}}
              }
            }else{ptr=(Uint16 *)((Uint8 *)screen->pixels + (height-y1)*screen->pitch + x1*2); *ptr=bright;}
            x1=x2;y1=y2;
            break;

  default: break;
}

}

#else

/*draw points and lines*/
void mdraw(SDL_Surface *screen,int x,int y,char s)
{long int i,j,x2,y2;
float rap;
static int x1=0,y1=0;
Uint8 *ptr;
int width,height,bitd;

bitd=CLBITS/8;

width=SCREENWIDTH; height=SCREENHEIGHT;

switch(s){
  case 'p': x1=x;y1=y;ptr=((Uint8 *)screen->pixels + (height-y1)*screen->pitch + x1*bitd);
            ptr[0]=ptr[1]=ptr[2]=255;
            break;

  case 'l': x2=x1+x;y2=y1+y;
            if((fabs(x2-x1))||(fabs(y2-y1))){
              if (fabs(x1-x2)<fabs(y1-y2)){
                rap=(float)(x2-x1)/(y2-y1);
                if(y2>y1){
                  for(i=y1;i<=y2;i++){j=(int)(x1+(i-y1)*rap);
                    ptr=((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*bitd);
                    ptr[0]=ptr[1]=ptr[2]=255;}
                }else{
                  for(i=y1;i>=y2;i--){j=(int)(x1+(i-y1)*rap);
                    ptr=((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*bitd);
                    ptr[0]=ptr[1]=ptr[2]=255;}}
              }else{
                rap=(float)(y2-y1)/(x2-x1);
                if(x2>x1){
                  for(j=x1;j<=x2;j++){i=(int)(y1+(j-x1)*rap);
                    ptr=((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*bitd);
                    ptr[0]=ptr[1]=ptr[2]=255;}
                }else{
                  for(j=x1;j>=x2;j--){i=(int)(y1+(j-x1)*rap);
                    ptr=((Uint8 *)screen->pixels + (height-i)*screen->pitch + j*bitd);
                    ptr[0]=ptr[1]=ptr[2]=255;}}
              }
            }else{ptr=((Uint8 *)screen->pixels + (height-y1)*screen->pitch + x1*bitd);
                  ptr[0]=ptr[1]=ptr[2]=255;}
            x1=x2;y1=y2;
            break;

  default: break;
}

}

#endif


/*draws 1 character*/
void drawchar1(SDL_Surface *screen,float *coord,int x,int y,int i,int h,float caz)
{int j;
mdraw(screen,(int)(x+2*caz*h*coord[(i-1)*12+1]),(int)(y+h*coord[(i-1)*12+2]),'p');
for(j=3;j<=11;j=j+2){
  mdraw(screen,(int)(2*caz*h*coord[(i-1)*12+j]),(int)(h*coord[(i-1)*12+j+1]),'l');
}
}


/*draws fragment of character*/
void drawchaux(SDL_Surface *screen,int x,int y,char a,int h,float caz)
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
mdraw(screen,(int)(x+2*caz*h*crd[1]),(int)(y+h*crd[2]),'p');
for(j=3;j<=11;j=j+2){
  mdraw(screen,(int)(2*caz*h*crd[j]),(int)(h*crd[j+1]),'l');
}
}


/*draws 1 character; needs drawchar1() to work*/
void drawchar(SDL_Surface *screen,int x,int y,int h,char a,float caz)
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
		     0.1, 0.35, 0.35, 0.55, 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*slasz*/
		     0.25, 0.35,-0.1 ,-0.1 , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*,*/
		     0.15, 0.35, 0.15, 0.2 , 0   , 0.15,-0.15, 0.2 , 0   , 0   , 0   , 0   ,  /*)*/
		     0.35, 0.35,-0.15, 0.2 , 0   , 0.15, 0.15, 0.2 , 0   , 0   , 0   , 0   ,  /*(*/
		     0.25, 0.35, 0   , 0.1 , 0   , 0   , 0   , 0   , 0   , 0   , 0   , 0   ,  /*:*/
		     0.2 , 0.35, 0.3 , 0.2 ,-0.3 , 0.2 , 0   ,-0.4 , 0   , 0   , 0   , 0   ,  /*cursor(#)*/
		     0.25, 0.55, 0   , 0.1 , 0.1 , 0.1 , 0   , 0.1 ,-0.3 , 0   , 0   ,-0.1 ,  /*?*/
		     0.1 , 0.7 , 0   , 0.15,-0.15, 0   , 0   ,-0.15, 0.15, 0   , 0   , 0   ,  /*%*/
		     0.2 , 0.35, 0.3 , 0.2 ,-0.3 , 0.2 , 0   ,0    , 0   , 0   , 0   , 0   ,  /*>*/
		     };

  switch(a){

  case 'a': drawchar1(screen,coord,x,y,1,h,caz);break;
  case 'b': drawchar1(screen,coord,x,y,2,h,caz);break;
  case 'c': drawchar1(screen,coord,x,y,3,h,caz);break;
  case 'd': drawchar1(screen,coord,x,y,4,h,caz);break;
  case 'e': drawchar1(screen,coord,x,y,5,h,caz);break;
  case 'f': drawchar1(screen,coord,x,y,6,h,caz);break;
  case 'g': drawchar1(screen,coord,x,y,7,h,caz);break;
  case 'h': drawchar1(screen,coord,x,y,8,h,caz);break;
  case 'i': drawchar1(screen,coord,x,y,9,h,caz);drawchaux(screen,x,y,'i',h,caz);break;
  case 'j': drawchar1(screen,coord,x,y,10,h,caz);drawchaux(screen,x,y,'i',h,caz);break;
  case 'k': drawchar1(screen,coord,x,y,11,h,caz);break;
  case 'l': drawchar1(screen,coord,x,y,12,h,caz);break;
  case 'm': drawchar1(screen,coord,x,y,13,h,caz);drawchaux(screen,x,y,'m',h,caz);break;
  case 'n': drawchar1(screen,coord,x,y,14,h,caz);break;
  case 'o': drawchar1(screen,coord,x,y,15,h,caz);break;
  case 'p': drawchar1(screen,coord,x,y,16,h,caz);break;
  case 'q': drawchar1(screen,coord,x,y,17,h,caz);break;
  case 'r': drawchar1(screen,coord,x,y,18,h,caz);break;
  case 's': drawchar1(screen,coord,x,y,19,h,caz);break;
  case 't': drawchar1(screen,coord,x,y,20,h,caz);break;
  case 'u': drawchar1(screen,coord,x,y,21,h,caz);break;
  case 'v': drawchar1(screen,coord,x,y,22,h,caz);break;
  case 'w': drawchar1(screen,coord,x,y,23,h,caz);drawchaux(screen,x,y,'w',h,caz);break;
  case 'x': drawchar1(screen,coord,x,y,24,h,caz);break;
  case 'y': drawchar1(screen,coord,x,y,25,h,caz);break;
  case 'z': drawchar1(screen,coord,x,y,26,h,caz);break;

  case 'A': drawchar1(screen,coord,x,y,27,h,caz);break;
  case 'B': drawchar1(screen,coord,x,y,28,h,caz);drawchaux(screen,x,y,'3',h,caz);break;
  case 'C': drawchar1(screen,coord,x,y,29,h,caz);break;
  case 'D': drawchar1(screen,coord,x,y,30,h,caz);break;
  case 'E': drawchar1(screen,coord,x,y,31,h,caz);drawchar1(screen,coord,x,y,64,h,caz);break;
  case 'F': drawchar1(screen,coord,x,y,32,h,caz);break;
  case 'G': drawchar1(screen,coord,x,y,33,h,caz);break;
  case 'H': drawchar1(screen,coord,x,y,34,h,caz);break;
  case 'I': drawchar1(screen,coord,x,y,35,h,caz);break;
  case 'J': drawchar1(screen,coord,x,y,36,h,caz);break;
  case 'K': drawchar1(screen,coord,x,y,37,h,caz);break;
  case 'L': drawchar1(screen,coord,x,y,38,h,caz);break;
  case 'M': drawchar1(screen,coord,x,y,39,h,caz);break;
  case 'N': drawchar1(screen,coord,x,y,40,h,caz);break;
  case 'O': drawchar1(screen,coord,x,y,41,h,caz);break;
  case 'P': drawchar1(screen,coord,x,y,42,h,caz);break;
  case 'Q': drawchar1(screen,coord,x,y,43,h,caz);break;
  case 'R': drawchar1(screen,coord,x,y,44,h,caz);break;
  case 'S': drawchar1(screen,coord,x,y,45,h,caz);break;
  case 'T': drawchar1(screen,coord,x,y,46,h,caz);break;
  case 'U': drawchar1(screen,coord,x,y,47,h,caz);break;
  case 'V': drawchar1(screen,coord,x,y,48,h,caz);break;
  case 'W': drawchar1(screen,coord,x,y,49,h,caz);break;
  case 'X': drawchar1(screen,coord,x,y,50,h,caz);break;
  case 'Y': drawchar1(screen,coord,x,y,51,h,caz);break;
  case 'Z': drawchar1(screen,coord,x,y,52,h,caz);break;
  
  case '1': drawchar1(screen,coord,x,y,53,h,caz);break;
  case '2': drawchar1(screen,coord,x,y,54,h,caz);break;
  case '3': drawchar1(screen,coord,x,y,55,h,caz);drawchaux(screen,x,y,'3',h,caz);break;
  case '4': drawchar1(screen,coord,x,y,56,h,caz);break;
  case '5': drawchar1(screen,coord,x,y,57,h,caz);break;
  case '6': drawchar1(screen,coord,x,y,58,h,caz);break;
  case '7': drawchar1(screen,coord,x,y,59,h,caz);break;
  case '8': drawchar1(screen,coord,x,y,60,h,caz);drawchaux(screen,x,y,'8',h,caz);break;
  case '9': drawchar1(screen,coord,x,y,61,h,caz);break;
  case '0': drawchar1(screen,coord,x,y,62,h,caz);drawchaux(screen,x,y,'0',h,caz);break;
  case '.': drawchar1(screen,coord,x,y,63,h,caz);break;
  case ':': drawchar1(screen,coord,x,y,69,h,caz);drawchar1(screen,coord,x,y+(int)(0.3*h),69,h,caz);break;
  case '-': drawchar1(screen,coord,x,y,64,h,caz);break;
  case '=': drawchar1(screen,coord,x,y-(int)(0.1*h),64,h,caz);drawchar1(screen,coord,x,y+(int)(0.05*h),64,h,caz);break;
  case '/': drawchar1(screen,coord,x,y,65,h,caz);break;
  case ',': drawchar1(screen,coord,x,y,66,h,caz);break;
  case ';': drawchar1(screen,coord,x,y,66,h,caz);drawchar1(screen,coord,x,y+(int)(0.3*h),69,h,caz);break;
  case ')': drawchar1(screen,coord,x,y,67,h,caz);break;
  case '(': drawchar1(screen,coord,x,y,68,h,caz);break;
  case '#': drawchar1(screen,coord,x,y,70,h,caz);break;
  case '?': drawchar1(screen,coord,x,y,71,h,caz);drawchar1(screen,coord,x,y,69,h,caz);break;
  case '%': drawchar1(screen,coord,x,y,65,h,caz);drawchar1(screen,coord,x,y,63,h,caz);drawchar1(screen,coord,x,y,72,h,caz);break;
  case '\'': drawchar1(screen,coord,x,y+(int)(0.4*h),69,h,caz);break;
  case '"': drawchar1(screen,coord,x,y+(int)(0.4*h),69,h,caz);break;
  case '>': drawchar1(screen,coord,x,y,73,h,caz);break;
  default: break;
 }

}


/*draws text*/
void dtext(SDL_Surface *screen,char *s,float x,float y,float h)
{
while(*s){drawchar(screen,(int)x,(int)y,(int)h,*(s++),0.5);x=x+h*0.5;}
}
