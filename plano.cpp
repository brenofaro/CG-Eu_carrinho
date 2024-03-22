#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <string>
#include<iostream>
#include<fstream>
float posCameraX = 0.0, posCameraY = 2.0, posCameraZ = 5.0; // Posição inicial da câmera
GLfloat luz_pontual[] = { -5.0, 5.0, -5.0, 1.0 }; // Posição inicial do sol


void desenhar_plano() {
    // Define cor verde para o plano
    GLfloat cor_plano[] = {0.0, 1.0, 0.0, 1.0}; // Cor verde
    glMaterialfv(GL_FRONT, GL_AMBIENT, cor_plano);

    // Desenha o plano como um quadrilátero
    glBegin(GL_QUADS);
        glNormal3f(0.0, 1.0, 0.0); // Define a normal do plano
        glVertex3f(-10.0, 0.0, -10.0);
        glVertex3f(10.0, 0.0, -10.0);
        glVertex3f(10.0, 0.0, 10.0);
        glVertex3f(-10.0, 0.0, 10.0);
    glEnd();
}

void desenhar_sol(){
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(luz_pontual[0], luz_pontual[1], luz_pontual[2]); 

    // Configurando a cor amarela para a luz pontual
    GLfloat cor_luz[] = {1.0, 1.0, 0.0, 1.0}; // Cor amarela
    GLfloat posicao_luz[] = {luz_pontual[0], luz_pontual[1], luz_pontual[2], 1.0}; // Posição do sol

    // Configurando a luz pontual
    glLightfv(GL_LIGHT1, GL_DIFFUSE, cor_luz);
    glLightfv(GL_LIGHT1, GL_POSITION, posicao_luz);
    glEnable(GL_LIGHT1);

    // Desenha o sol como uma esfera amarela
    glColor3f(1.0, 1.0, 0.0); 
    glutSolidSphere(1.0, 50, 50); 
    glEnable(GL_LIGHTING);
    glPopMatrix();
}


void desenhar_nuvem(float x, float y, float z, float raio) {
    glDisable(GL_LIGHTING); // Desativa a iluminação para desenhar as nuvens
    glColor3f(1.0, 1.0, 1.0); // Cor branca para as nuvens
    glPushMatrix();
    glTranslatef(x, y, z);

    // Desenhar círculos para formar a nuvem
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glRotatef(45.0f * i, 0.0f, 1.0f, 0.0f);
        glTranslatef(raio * 0.5f, 0.0f, 0.0f);
        glutSolidSphere(raio, 30, 30);
        glPopMatrix();
    }

    glPopMatrix();
    glEnable(GL_LIGHTING); // Reativa a iluminação após desenhar as nuvens
}


    



void init(void) {
    glClearColor(0.7, 0.9, 1.0, 1.0); // Cor de fundo azul claro

    // Configura iluminação
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // Configura posição da luz pontual
    glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);

    // Configura a cor do ambiente para branco
    GLfloat luz_ambiente[] = { 1.0, 1.0, 1.0, 1.0 }; // Luz ambiente branca
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luz_ambiente);

    // Ativa teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

void display(void) {
    // Limpeza do z-buffer deve ser feita a cada desenho da tela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Define a posição da câmera
    gluLookAt(posCameraX, posCameraY, posCameraZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Desenha o sol
    desenhar_sol();

    // Desenha o plano verde
    desenhar_plano();

    // Desenha nuvens
    desenhar_nuvem(-2.0, 2.0, 1.0, 0.5);  
    desenhar_nuvem(3.0, 2.5, 2.0, 0.6);    
    desenhar_nuvem(-4.0, 3.0, 0.5, 0.7);   
    desenhar_nuvem(1.0, 1.0, 3.0, 0.4);    
    desenhar_nuvem(5.0, 2.5, 1.5, 0.8);    
    desenhar_nuvem(-5.0, 3.0, 2.0, 0.6);   
    glutSwapBuffers();
}


void reshape(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat) w / (GLfloat) h, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, w, h);
}

void specialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT :
            posCameraX -= 0.1;
            break;
        case GLUT_KEY_RIGHT :
            posCameraX += 0.1;
            break;
        case GLUT_KEY_UP :
            posCameraZ -= 0.1;
            break;
        case GLUT_KEY_DOWN :
            posCameraZ += 0.1;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);

    glutMainLoop();

    return 0;
}
