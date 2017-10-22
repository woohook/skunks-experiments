/*
Copyright (C) 2007-2010 Victor Matei Petrescu

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

typedef struct _pixcol
{Uint8 red; Uint8 green; Uint8 blue; /*culoarea pixelului*/
GLfloat glred; GLfloat glgreen; GLfloat glblue;
} pixcol;

typedef struct _tria
{GLfloat x1; GLfloat y1;GLfloat z1;
GLfloat x2; GLfloat y2;GLfloat z2;
GLfloat x3; GLfloat y3;GLfloat z3;
int red; int green; int blue; /*culoarea triunghiului*/
REALN light; /*intensitatea iluminarii*/
unsigned char cull; /*1-cull; 0-no cull*/
} tria;

typedef struct _lightpr
{REALN ambient;
REALN headlight;
REALN directional;
REALN dx;
REALN dy;
REALN dz;
} lightpr; /*light parameters*/

/*functie care elimina triunghiurile care sunt in plus*/
int fclip(tria *face,int nrfaces,REALN zmin,tria *facedisp,REALN zmax,REALN tgh,REALN tgv)
{int i,j,k,l,kmin,invs;
REALN x[4],y[4],z[4],tmp,tmp2,min;
j=0; /*variabila pt. numarat triunghiurile afisate*/
for(i=1;i<=nrfaces;i++){

  if((face[i].z1<=zmin)&&(face[i].z2<=zmin)&&(face[i].z3<=zmin)){goto NEXT;}
  if((face[i].z1>=zmax)&&(face[i].z2>=zmax)&&(face[i].z3>=zmax)){goto NEXT;}
    if((face[i].x1>tgv*face[i].z1)&&(face[i].x2>tgv*face[i].z2)&&(face[i].x3>tgv*face[i].z3)){goto NEXT;}
    if((face[i].y1>tgh*face[i].z1)&&(face[i].y2>tgh*face[i].z2)&&(face[i].y3>tgh*face[i].z3)){goto NEXT;}
  if((face[i].x1<-tgv*face[i].z1)&&(face[i].x2<-tgv*face[i].z2)&&(face[i].x3<-tgv*face[i].z3)){goto NEXT;}
  if((face[i].y1<-tgh*face[i].z1)&&(face[i].y2<-tgh*face[i].z2)&&(face[i].y3<-tgh*face[i].z3)){goto NEXT;}

  if((face[i].z1>zmin)&&(face[i].z2>zmin)&&(face[i].z3>zmin)){j++;facedisp[j]=face[i];}
  else{
    x[1]=face[i].x1;x[2]=face[i].x2;x[3]=face[i].x3;
    y[1]=face[i].y1;y[2]=face[i].y2;y[3]=face[i].y3;
    z[1]=face[i].z1;z[2]=face[i].z2;z[3]=face[i].z3;

    invs=1;

    for(k=1;k<=2;k++){min=z[k];kmin=k;
      for(l=k+1;l<=3;l++){
	if(z[l]<z[kmin]){kmin=l;}
      }if(kmin!=k){ invs*=-1;
	tmp=x[k];x[k]=x[kmin];x[kmin]=tmp;
	tmp=y[k];y[k]=y[kmin];y[kmin]=tmp;
	tmp=z[k];z[k]=z[kmin];z[kmin]=tmp;
      }
    } /*ordonat varfuri dupa z*/

  if((z[1]<=zmin)&&(z[2]<=zmin)){
    j++;
      tmp=(zmin-z[1])/(z[3]-z[1]);
        facedisp[j].x1=tmp*(x[3]-x[1])+x[1];
        facedisp[j].y1=tmp*(y[3]-y[1])+y[1];
      tmp=(zmin-z[2])/(z[3]-z[2]);
        facedisp[j].x2=tmp*(x[3]-x[2])+x[2];
        facedisp[j].y2=tmp*(y[3]-y[2])+y[2];
	  facedisp[j].x3=x[3];
	  facedisp[j].y3=y[3];
	    facedisp[j].z1=facedisp[j].z2=zmin;
	    facedisp[j].z3=z[3];
	      facedisp[j].red=face[i].red;facedisp[j].green=face[i].green;facedisp[j].blue=face[i].blue;

	      facedisp[j].cull=face[i].cull;
	      if(invs==(-1)){
	        tmp2=facedisp[j].x1; facedisp[j].x1=facedisp[j].x2; facedisp[j].x2=tmp2;
                tmp2=facedisp[j].y1; facedisp[j].y1=facedisp[j].y2; facedisp[j].y2=tmp2;
                tmp2=facedisp[j].z1; facedisp[j].z1=facedisp[j].z2; facedisp[j].z2=tmp2;
	      }
  }else{
    j++;
      tmp=(zmin-z[1])/(z[2]-z[1]);
        facedisp[j].x1=tmp*(x[2]-x[1])+x[1];
	facedisp[j].y1=tmp*(y[2]-y[1])+y[1];
	  facedisp[j].x2=x[2];
	  facedisp[j].y2=y[2];
	    facedisp[j].x3=x[3];
	    facedisp[j].y3=y[3];
	      facedisp[j].z1=zmin;
	      facedisp[j].z2=z[2];
	      facedisp[j].z3=z[3];
	        facedisp[j].red=face[i].red;facedisp[j].green=face[i].green;facedisp[j].blue=face[i].blue;

	        facedisp[j].cull=face[i].cull;
	        if(invs==(-1)){
	          tmp2=facedisp[j].x1; facedisp[j].x1=facedisp[j].x2; facedisp[j].x2=tmp2;
                  tmp2=facedisp[j].y1; facedisp[j].y1=facedisp[j].y2; facedisp[j].y2=tmp2;
                  tmp2=facedisp[j].z1; facedisp[j].z1=facedisp[j].z2; facedisp[j].z2=tmp2;
	        }
    j++;
        facedisp[j].x1=tmp*(x[2]-x[1])+x[1];
	facedisp[j].y1=tmp*(y[2]-y[1])+y[1];
      tmp=(zmin-z[1])/(z[3]-z[1]);
        facedisp[j].x2=tmp*(x[3]-x[1])+x[1];
	facedisp[j].y2=tmp*(y[3]-y[1])+y[1];
	  facedisp[j].x3=x[3];
	  facedisp[j].y3=y[3];
	    facedisp[j].z1=facedisp[j].z2=zmin;
	    facedisp[j].z3=z[3];
	      facedisp[j].red=face[i].red;facedisp[j].green=face[i].green;facedisp[j].blue=face[i].blue;

	      facedisp[j].cull=face[i].cull;
	      if(invs==(1)){
	        tmp2=facedisp[j].x1; facedisp[j].x1=facedisp[j].x2; facedisp[j].x2=tmp2;
                tmp2=facedisp[j].y1; facedisp[j].y1=facedisp[j].y2; facedisp[j].y2=tmp2;
                tmp2=facedisp[j].z1; facedisp[j].z1=facedisp[j].z2; facedisp[j].z2=tmp2;
	      }
  }
  }

NEXT: ;}

return j;}


/*functie care verifica daca punctul M(x,y,z) se afla in interiorul triunghiului ABC
A(x1,y1,z1);B(x2,y2,z2);c(x3,y3,z3)
(punctul M apartine planului ABC)
nu se mai foloseste functia asta*/
int vertri(REALN x1,REALN y1,REALN x2,REALN y2,REALN x3,REALN y3,REALN x,REALN y)
{REALN l1,l2,m1,m2,p; /*p - produs vectorial*/
char s1,s2,s3; /*semnele celor 3 produse vectoriale care vor fi calculate*/

  l1=x2-x1;l2=x-x1;
  m1=y2-y1;m2=y-y1;
  p=l1*m2-l2*m1; if(p>0){s1=1;}else{s1=2;}

  l1=x3-x2;l2=x-x2;
  m1=y3-y2;m2=y-y2;
  p=l1*m2-l2*m1; if(p>0){s2=1;}else{s2=2;}

  l1=x1-x3;l2=x-x3;
  m1=y1-y3;m2=y-y3;
  p=l1*m2-l2*m1; if(p>0){s3=1;}else{s3=2;}

  if((s1==s2)&&(s1==s3)){return 1;}

return 0;}


/*functie care determina ecuatia unui plan care trece prin 3 puncte
A(x1,y1,z1);B(x2,y2,z2);c(x3,y3,z3)
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nu se mai foloseste functia asta ci findplan()*/
REALN ccplan(REALN x1,REALN y1,REALN z1,REALN x2,REALN y2,REALN z2,REALN x3,REALN y3,REALN z3,REALN xf,REALN yf,REALN zf)
{REALN a1,b1,c1,a2,b2,c2,a,b,c,d;
REALN xi,yi,zi;
REALN dist; /*patratul distantei de la origine pana la intersectia dreptei cu planul*/

  a1=x1-x2; b1=y1-y2; c1=z1-z2;
  a2=x1-x3; b2=y1-y3; c2=z1-z3;

    a=b1*c2-b2*c1; b=a2*c1-a1*c2; c=a1*b2-a2*b1;

    d=a*x1+b*y1+c*z1;
    /*ecuatia planului este ax+by+cz=d*/

   zi=d/(a*xf/zf+b*yf/zf+c); yi=zi*yf/zf; xi=zi*xf/zf;
   /*xi,yi si zi sunt punctele de intersectie ale dreptei cu planul*/

   if(zi<0){return 0;}

   dist=xi*xi+yi*yi+zi*zi;

return dist;}


/*functie care determina ecuatia planului cu numarul de ordine 'i'
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nrfaces - numar total de triunghiuri*/
void findplan(tria *face,int i,REALN *a,REALN *b,REALN *c,REALN *d)
{REALN a1,b1,c1,a2,b2,c2;

	a1=face[i].x1-face[i].x2; b1=face[i].y1-face[i].y2; c1=face[i].z1-face[i].z2;
	a2=face[i].x1-face[i].x3; b2=face[i].y1-face[i].y3; c2=face[i].z1-face[i].z3;

	*a=b1*c2-b2*c1; *b=a2*c1-a1*c2; *c=a1*b2-a2*b1;
	*d=(*a)*face[i].x1+(*b)*face[i].y1+(*c)*face[i].z1;
	/*ecuatia planului este ax+by+cz=d*/
}


void displaysdl(SDL_Surface *screen,tria *face,int nrfaces,REALN *distmin,unsigned int width,unsigned int height,REALN focal,pixcol backcol,REALN zfog,REALN zmax,lightpr *light)
{int i,red,green,blue;
GLfloat fcol[4],cf1[4],cf2[4],ratio,bright;
REALN a,b,c,d,a1,dist,tmp;
pixcol bkc;

bkc.glred=(GLfloat)(backcol.red-50)*0.5/255;
bkc.glgreen=(GLfloat)(backcol.green-50)*0.5/255;
bkc.glblue=(GLfloat)(backcol.blue-50)*0.5/255; /*background color for OpenGL, as set in "sk-gl.c"*/

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glLoadIdentity();

fcol[3]=1.0f;

glBegin(GL_TRIANGLES);

for(i=1;i<=nrfaces;i++){

findplan(face,i,&a,&b,&c,&d);

/*start lighting and backface culling*/
tmp=a*face[i].x1+b*face[i].y1+c*face[i].z1;
/*dot product used for lighting and backface culling*/
if((face[i].cull)&1){ /*backface culling*/
  if(tmp<0){continue;}
}

if((face[i].cull)&2){ /*fullbright*/
  a1=1;
}else{
  dist=sqrt(a*a+b*b+c*c);
  a1=(light->dx*a+light->dy*b+light->dz*c)/dist;
  if(tmp>=0){
    if(a1<0){a1=0;}
  }else{
    if(a1<0){a1=-a1;}else{a1=0;}
  }
  a1=light->ambient+fabs(light->headlight*c/dist)+light->directional*a1;
}

bright=a1; if(bright<0){bright=0;}

red=(int)(bright*face[i].red); green=(int)(bright*face[i].green); blue=(int)(bright*face[i].blue);
if(red>255){red=255;}
if(green>255){green=255;}
if(blue>255){blue=255;}
/*finished lighting and backface culling*/

    fcol[0]=bright*(GLfloat)face[i].red/255;
    fcol[1]=bright*(GLfloat)face[i].green/255;
    fcol[2]=bright*(GLfloat)face[i].blue/255;

    if(face[i].z1<zfog){
      cf1[0]=0; cf2[0]=1;
      cf1[1]=0; cf2[1]=1;
      cf1[2]=0; cf2[2]=1;
    }else{
      if(face[i].z1>=zmax){
        cf1[0]=bkc.glred; cf2[0]=0;
        cf1[1]=bkc.glgreen; cf2[1]=0;
        cf1[2]=bkc.glblue; cf2[2]=0;
      }else{
        ratio=(face[i].z1-zfog)/(zmax-zfog);
        cf1[0]=ratio*bkc.glred; cf2[0]=1-ratio;
        cf1[1]=ratio*bkc.glgreen; cf2[1]=1-ratio;
        cf1[2]=ratio*bkc.glblue; cf2[2]=1-ratio;
      }
    }
    glColor3f(cf1[0]+cf2[0]*fcol[0],cf1[1]+cf2[1]*fcol[1],cf1[2]+cf2[2]*fcol[2]);
    glVertex3f(-face[i].y1,face[i].x1,-face[i].z1);

      if(face[i].z2<zfog){
      cf1[0]=0; cf2[0]=1;
      cf1[1]=0; cf2[1]=1;
      cf1[2]=0; cf2[2]=1;
    }else{
      if(face[i].z2>=zmax){
        cf1[0]=bkc.glred; cf2[0]=0;
        cf1[1]=bkc.glgreen; cf2[1]=0;
        cf1[2]=bkc.glblue; cf2[2]=0;
      }else{
        ratio=(face[i].z2-zfog)/(zmax-zfog);
        cf1[0]=ratio*bkc.glred; cf2[0]=1-ratio;
        cf1[1]=ratio*bkc.glgreen; cf2[1]=1-ratio;
        cf1[2]=ratio*bkc.glblue; cf2[2]=1-ratio;
      }
    }
      glColor3f(cf1[0]+cf2[0]*fcol[0],cf1[1]+cf2[1]*fcol[1],cf1[2]+cf2[2]*fcol[2]);
      glVertex3f(-face[i].y2,face[i].x2,-face[i].z2);

        if(face[i].z3<zfog){
      cf1[0]=0; cf2[0]=1;
      cf1[1]=0; cf2[1]=1;
      cf1[2]=0; cf2[2]=1;
    }else{
      if(face[i].z3>=zmax){
        cf1[0]=bkc.glred; cf2[0]=0;
        cf1[1]=bkc.glgreen; cf2[1]=0;
        cf1[2]=bkc.glblue; cf2[2]=0;
      }else{
        ratio=(face[i].z3-zfog)/(zmax-zfog);
        cf1[0]=ratio*bkc.glred; cf2[0]=1-ratio;
        cf1[1]=ratio*bkc.glgreen; cf2[1]=1-ratio;
        cf1[2]=ratio*bkc.glblue; cf2[2]=1-ratio;
      }
    }
        glColor3f(cf1[0]+cf2[0]*fcol[0],cf1[1]+cf2[1]*fcol[1],cf1[2]+cf2[2]*fcol[2]);
        glVertex3f(-face[i].y3,face[i].x3,-face[i].z3);
}

  glColor3f(1.0,0.0,0.0);
  glVertex3f(0.1,-0.34,-0.5);
    glColor3f(1.0,1.0,0.0);
    glVertex3f(0.105,-0.36,-0.5);
      glColor3f(1.0,1.0,0.0);
      glVertex3f(0.095,-0.36,-0.5);

  glColor3f(1.0,0.0,0.0);
  glVertex3f(0.0,-0.34,-0.5);
    glColor3f(1.0,1.0,0.0);
    glVertex3f(0.005,-0.36,-0.5);
      glColor3f(1.0,1.0,0.0);
      glVertex3f(-0.005,-0.36,-0.5);

  glColor3f(1.0,0.0,0.0);
  glVertex3f(-0.1,-0.34,-0.5);
    glColor3f(1.0,1.0,0.0);
    glVertex3f(-0.105,-0.36,-0.5);
      glColor3f(1.0,1.0,0.0);
      glVertex3f(-0.095,-0.36,-0.5);

if(backcol.glred>0.5){
  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.1-0.0036*backcol.glgreen,-0.31,-0.5);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.11-0.0036*backcol.glgreen,-0.33,-0.5);
      glColor3f(1.0,1.0,1.0);
      glVertex3f(0.09-0.0036*backcol.glgreen,-0.33,-0.5);
}else{
glColor3f(1.0,0.5,0.0);
  glVertex3f(0.1-0.0036*backcol.glgreen,-0.33,-0.5);
    glColor3f(1.0,0.5,0.0);
    glVertex3f(0.11-0.0036*backcol.glgreen,-0.31,-0.5);
      glColor3f(1.0,0.5,0.0);
      glVertex3f(0.09-0.0036*backcol.glgreen,-0.31,-0.5);
}

glEnd();

/*
glBegin(GL_LINES);

  fcol[0]=fcol[1]=fcol[2]=1;
  glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,fcol);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.2,-0.6,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.2,-0.6,-1.0);

  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.19,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.19,-0.53,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.19,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.21,-0.53,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.21,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.21,-0.58,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.21,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.19,-0.58,-1.0);

  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.16,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.18,-0.53,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.18,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.18,-0.55,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.18,-0.55,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.16,-0.55,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.16,-0.55,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.16,-0.58,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.16,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.18,-0.58,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.18,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.18,-0.57,-1.0);

  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.22,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.24,-0.58,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.24,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.24,-0.53,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.24,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.22,-0.53,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.22,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.22,-0.58,-1.0);

  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.19,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.19,-0.53,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.19,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.21,-0.53,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.21,-0.53,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.21,-0.58,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.21,-0.58,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.19,-0.58,-1.0);

  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.2,-0.6,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.2,-0.62,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(0.0,-0.6,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(0.0,-0.62,-1.0);
  glColor3f(1.0,1.0,1.0);
  glVertex3f(-0.2,-0.6,-1.0);
    glColor3f(1.0,1.0,1.0);
    glVertex3f(-0.2,-0.62,-1.0);

glEnd();
*/

SDL_GL_SwapBuffers();
}



