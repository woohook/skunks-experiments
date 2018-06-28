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


/*set camera*/
void setcamg(sgob *camera,vhc *car,int flag)
{
float pos[3];
float rot[12];
REALN x0,y0,z0,a,h,d;

physics_getBodyPosition(car->parts[1], &pos[0], &pos[1], &pos[2]);
physics_getBodyRotation(car->parts[1], &rot[0], &rot[1], &rot[2], &rot[3]
                                     , &rot[4], &rot[5], &rot[6], &rot[7]
                                     , &rot[8], &rot[9], &rot[10], &rot[11]);

switch(flag){
  case 1: a=0.57;
          x0=pos[0]+12.0; y0=pos[1]; z0=pos[2]-12.0;
          camera->transform.vx[0]=x0; camera->transform.vy[0]=y0; camera->transform.vz[0]=z0;
          camera->transform.vx[1]=x0+cos(a); camera->transform.vy[1]=y0; camera->transform.vz[1]=z0+sin(a);
          camera->transform.vx[2]=x0; camera->transform.vy[2]=y0+1; camera->transform.vz[2]=z0;
          camera->transform.vx[3]=x0-sin(a); camera->transform.vy[3]=y0; camera->transform.vz[3]=z0+cos(a);
          break;

  case 2: h=car->camh; d=car->camd;
          x0=pos[0]; y0=pos[1]; z0=pos[2];
          camera->transform.vx[0]=x0; camera->transform.vy[0]=y0; camera->transform.vz[0]=z0;
          camera->transform.vx[1]=x0+rot[0]; camera->transform.vy[1]=y0+rot[4]; camera->transform.vz[1]=z0+rot[8];
          camera->transform.vx[2]=x0+rot[1]; camera->transform.vy[2]=y0+rot[5]; camera->transform.vz[2]=z0+rot[9];
          camera->transform.vx[3]=x0+rot[2]; camera->transform.vy[3]=y0+rot[6]; camera->transform.vz[3]=z0+rot[10];
          translat(camera,h*rot[0]+d*rot[2],h*rot[4]+d*rot[6],h*rot[8]+d*rot[10]);
          break;

  case 3: x0=pos[0]+7.0; y0=pos[1]; z0=pos[2]-8.0;
          camera->transform.vx[0]=x0; camera->transform.vy[0]=y0; camera->transform.vz[0]=z0;
          camera->transform.vx[1]=x0+1; camera->transform.vy[1]=y0; camera->transform.vz[1]=z0;
          camera->transform.vx[2]=x0; camera->transform.vy[2]=y0+1; camera->transform.vz[2]=z0;
          camera->transform.vx[3]=x0; camera->transform.vy[3]=y0; camera->transform.vz[3]=z0+1;
          y0=rot[6]; z0=rot[10]; a=sqrt(y0*y0+z0*z0);
          if(fabs(a)>1e-5){y0/=a; z0/=a;}else{y0=0; z0=1;}
          h=camera->transform.vy[3]-camera->transform.vy[0]; d=camera->transform.vz[3]-camera->transform.vz[0];
          a=y0*h+z0*d;
          if(a>1){a=1;}else{if(a<-1){a=-1;}}
          if(y0>0){a=acos(a);}else{a=-acos(a);}
          rotatx(camera,pos[1],pos[2],-a);
          rotab(camera,camera->transform.vx[0],camera->transform.vy[0],camera->transform.vz[0],camera->transform.vx[2],camera->transform.vy[2],camera->transform.vz[2],-0.57);
          break;

  default: break;
}
}


#if SOUND==1

REALN sintr(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi/2){y=x/(pi/2);return y;}
if((x>pi/2)&&(x<=3*pi/2)){y=1-(x-pi/2)/(pi/2);return y;}
if(x>3*pi/2){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinst(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi){y=x/(pi/2);return y;}
if(x>pi){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinrd()
{REALN x;
x=rand();
x=(REALN)(((int)x)%100)/50-1;
return x;
}


/*audio callback function for SDL*/
void my_audio_callback(void *userdata, Uint8 *stream, int len)
{static REALD cr=0;
static int sem=0;
int freqtr,i,j1,j2,n=40;
REALN *volum,a,c,tmp;
Uint16 format;
Uint8 channels;
REALN coef;

static REALN freq[41]; /*frequencies of all sounds*/
REALN freqdo=0.3*261.6255653; /*first frequency*/



  if(sem==0){
    sem=1;
    tmp=1.06;
    freq[1]=freqdo;
    for(i=2;i<=40;i++){
      freq[i]=freq[i-1]*tmp;
    }
  } /*set frequencies*/

volum=(REALN *)userdata;

format=(Uint16)volum[2];
channels=(Uint8)volum[3];
freqtr=(int)volum[4];

switch(format){
	case AUDIO_U8: coef=1; break;
	case AUDIO_U16: coef=0.5; break;
	case AUDIO_S8: coef=1; break;
	case AUDIO_S16: coef=0.5; break;
	default: coef=0.5; break;
}
switch(channels){
	case 1: break;
	case 2: coef*=0.5;break;
	default: printf("Unsupported number of channels\r\n");exit(1);
}
coef=coef/freqtr;

/*SDL_Delay(10);*/

a=volum[1]/200; j1=(int)floor(a*(n-2)+1); j2=(int)floor(a*(n-2)+2);
a=a*(n-2)+1-j1;

  for(i=0;i<=len-1;i++){
    stream[i]=63;
    cr++;if(cr>1e6){cr=0;}
      c=0.85+0.15*sin(0.005*cr);
      stream[i]+=(Uint8)(10.0*c*(1.0-a)*sinst((freq[j1])*6.283185307*cr*coef));
      stream[i]+=(Uint8)(10.0*c*a*sinst((freq[j2])*6.283185307*cr*coef));
      if(volum[5]>20){
        stream[i]+=(Uint8)(40.0*c*a*sinrd());
      }

  }

}

#endif
