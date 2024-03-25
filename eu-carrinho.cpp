#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#define TINYOBJLOADER_IMPLEMENTATION
#include "include/tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

const double PI = 3.14159265358979323846;
float posCameraX = -0.141169, posCameraY = 5.6, posCameraZ = -9.70667; // Posição inicial da câmera
GLfloat luz_pontual[] = { -10, 12.6, 51.1484, 1.0 }; // Posição inicial do sol

// Constantes para movimento do carrinho
float posCarrinhoX = 0.0, posCarrinhoZ = 0.0; // Posição inicial do carrinho
float alturaCarrinho = 0.0; // Altura inicial do carrinho
float anguloCarrinho = 0.0; // Ângulo inicial do carrinho
float velocidadeCarrinhoX = 0.0f, velocidadeCarrinhoZ = 0.0f; // Velocidade inicial do carrinho


// Variaveis debug camera
float upX = 0.0;
float upY = 1;
float upZ = 0.0;
float cameraX = 0.0;
float cameraY = 3;
float cameraZ = 1.2;

const float distanciaAtras = 7.5f; // Distância atrás do carrinho
const float alturaCamera = 3.5f;  // Altura da câmera em relação ao carrinho


std::vector<std::vector<int>> altitudes;

void lerPGM(const std::string& arquivo, std::vector<std::vector<int>>& altitudes) {
    std::ifstream in(arquivo); // Abre o arquivo no modo padrão, que é texto
    std::string header;
    int largura, altura, maxval;
    in >> header;
    if (header != "P2") {
        throw std::runtime_error("Formato de arquivo não suportado ou arquivo não é P2 PGM.");
    }
    in >> largura >> altura >> maxval;
    altitudes.resize(altura, std::vector<int>(largura));

    for (int i = 0; i < altura; ++i) {
        for (int j = 0; j < largura; ++j) {
            int valorPixel;
            in >> valorPixel; // Lê cada valor de pixel como um inteiro
            altitudes[i][j] = valorPixel;
        }
    }
}

void desenhar_terreno(const std::vector<std::vector<int>>& altitudes) {
    int altura = altitudes.size();
    int largura = altitudes[0].size();
    float fatorEscala = 10.0f; // Ajuste o fator de escala conforme necessário

    // Calcula o ponto central do mapa para centralizar o terreno
    float centroX = largura / 2.0f;
    float centroZ = altura / 2.0f;

    for (int i = 0; i < altura - 1; ++i) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < largura; ++j) {
            // Ajusta as coordenadas para centralizar o terreno
            float x = i - centroX;
            float z = j - centroZ;

            glNormal3f(0.0, 1.0, 0.0); // Define a normal do terreno
            glVertex3f(x, altitudes[i][j] / 255.0 * fatorEscala, z);
            glVertex3f(x + 1, altitudes[i + 1][j] / 255.0 * fatorEscala, z);

        }
        glEnd();
    }
}



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
    glPushMatrix();
    glTranslatef(luz_pontual[0], luz_pontual[1], luz_pontual[2]); 

    // Desativa a iluminação para desenhar o sol
    glDisable(GL_LIGHTING);

    // Configurando a cor amarela para a luz pontual
    GLfloat cor_luz[] = {1.0, 1.0, 0.3, 1.0}; // Cor amarela
    GLfloat posicao_luz[] = {luz_pontual[0], luz_pontual[1], luz_pontual[2], 1.0}; // Posição do sol

    // Configurando a luz pontual
    glLightfv(GL_LIGHT1, GL_DIFFUSE, cor_luz);
    glLightfv(GL_LIGHT1, GL_POSITION, posicao_luz);
    glEnable(GL_LIGHT1);

    // Desenha o sol como uma esfera amarela
    glColor3f(1.0, 1.0, 0.0); 
    glutSolidSphere(3.0, 50, 50); 
    glEnable(GL_LIGHTING);
    glPopMatrix();
}


void desenhar_nuvem(float x, float y, float z, float raio) {
    glColor3f(1.0, 1.0, 1.0); // Cor branca para as nuvens
    glPushMatrix();
    glTranslatef(x, y, z);

    // Desativa a iluminação para desenhar as nuvens
    glDisable(GL_LIGHTING);

    // Desenhar círculos para formar a nuvem
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glRotatef(45.0f * i, 0.0f, 1.0f, 0.0f);
        glTranslatef(raio * 0.5f, 0.0f, 0.0f);
        glutSolidSphere(raio, 30, 30);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
    glPopMatrix();
}


// Textura
GLuint texturaID; 
GLuint carregarTextura(const char* arquivo) {
    GLuint texturaID;
    glGenTextures(1, &texturaID);
    glBindTexture(GL_TEXTURE_2D, texturaID);

    // Configura parâmetros da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int largura, altura, nrCanais;
    unsigned char* data = stbi_load(arquivo, &largura, &altura, &nrCanais, 0);
    if (data) {
        GLenum formato = GL_RGB;
        if (nrCanais == 4)
            formato = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, formato, largura, altura, 0, formato, GL_UNSIGNED_BYTE, data);
        
    } else {
        std::cerr << "Falha ao carregar a textura" << std::endl;
    }
    stbi_image_free(data);

    return texturaID;
}

tinyobj::attrib_t attrib;
std::vector<tinyobj::shape_t> g_shapes;


bool carregarModelo(const std::string& inputfile) {
    std::vector<tinyobj::material_t> materials;
    std::string err;

    // Carrega o modelo
    bool ret = tinyobj::LoadObj(&attrib, &g_shapes, &materials, &err, inputfile.c_str(), NULL, true);
    
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    return ret;
}

void desenharModelo() {
    glEnable(GL_TEXTURE_2D); // Habilita o uso de texturas
    glBindTexture(GL_TEXTURE_2D, texturaID); // Vincula a textura carregada
    glEnable(GL_COLOR_MATERIAL); // Permite que o material do objeto receba cor
    
    glPushMatrix();
    
    glTranslatef(posCarrinhoX,alturaCarrinho, posCarrinhoZ); // Posiciona o modelo usado para a movimentação do carrinho

    // Rotaciona o carrinho em torno do eixo Y baseado em seu ângulo atual
    glRotatef(anguloCarrinho, 0.0f, 1.0f, 0.0f);

    glScalef(0.1f, 0.1f, 0.1f); // Escala o modelo (ajuste conforme necessário)

    

    // Itera sobre cada shape no modelo
    for (size_t s = 0; s < g_shapes.size(); s++) {
        glBegin(GL_TRIANGLES);
        // Itera sobre cada face
        for (size_t f = 0; f < g_shapes[s].mesh.indices.size() / 3; f++) {
            // Itera sobre cada vértice
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = g_shapes[s].mesh.indices[3*f+v];
                
                // Extrai e aplica a normal para o vértice, se disponível
                if (idx.normal_index >= 0) {
                    float nx = attrib.normals[3 * idx.normal_index + 0];
                    float ny = attrib.normals[3 * idx.normal_index + 1];
                    float nz = attrib.normals[3 * idx.normal_index + 2];
                    glNormal3f(nx, ny, nz);
                }

                // Aplica coordenadas de textura, se disponíveis
                if (idx.texcoord_index >= 0) {
                    float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
                    float ty = attrib.texcoords[2 * idx.texcoord_index + 1];
                    glTexCoord2f(tx, 1-ty); // OpenGL inverte a direção Y da textura
                }

                // Extrai e aplica a posição dos vértices
                float vx = attrib.vertices[3 * idx.vertex_index + 0];
                float vy = attrib.vertices[3 * idx.vertex_index + 1];
                float vz = attrib.vertices[3 * idx.vertex_index + 2];
                glVertex3f(vx, vy, vz);
            }
        }
        glEnd();
    }

    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
    glBindTexture(GL_TEXTURE_2D, 0); // Desvincula a textura
    glDisable(GL_TEXTURE_2D); // Desabilita o uso de texturas
}

float aceleracaoCarrinho = 0.2f; // Taxa de aceleração do carrinho
const float desaceleracao = 0.05f; // Taxa de desaceleração do carrinho
const float velocidadeMaxima = 5.0f; // Velocidade máxima do carrinho
bool acelerando = false; // Indica se o carrinho está acelerando
bool desacelerando = false; // Indica se o carrinho está desacelerando

void atualizarPosicaoCarrinho(float deltaTime) {
    // Verifica se está acelerando ou desacelerando e aplica a aceleração correspondente
    if (acelerando) {
        velocidadeCarrinhoX += sin(anguloCarrinho * PI / 180.0f) * aceleracaoCarrinho * deltaTime;
        velocidadeCarrinhoZ += cos(anguloCarrinho * PI / 180.0f) * aceleracaoCarrinho * deltaTime;
    } else if (desacelerando) {
        // Para dar ré, a aceleração negativa é aplicada na direção oposta
        velocidadeCarrinhoX -= sin(anguloCarrinho * PI / 180.0f) * aceleracaoCarrinho * deltaTime;
        velocidadeCarrinhoZ -= cos(anguloCarrinho * PI / 180.0f) * aceleracaoCarrinho * deltaTime;
    }
    
    // Calcula a velocidade atual sem usar a raiz quadrada para manter o sinal
    float velocidadeAtualX = velocidadeCarrinhoX;
    float velocidadeAtualZ = velocidadeCarrinhoZ;

    // Aplica desaceleração natural se não estiver acelerando ou desacelerando
    if (!acelerando && !desacelerando) {
        float desacel = desaceleracao * deltaTime;
        if (abs(velocidadeAtualX) > desacel) {
            velocidadeCarrinhoX -= desacel * (velocidadeAtualX > 0 ? 1 : -1);
        } else {
            velocidadeCarrinhoX = 0;
        }
        if (abs(velocidadeAtualZ) > desacel) {
            velocidadeCarrinhoZ -= desacel * (velocidadeAtualZ > 0 ? 1 : -1);
        } else {
            velocidadeCarrinhoZ = 0;
        }
    }

    // Limita a velocidade para não exceder a velocidade máxima, considerando a direção
    if (abs(velocidadeCarrinhoX) > velocidadeMaxima) {
        velocidadeCarrinhoX = velocidadeMaxima * (velocidadeCarrinhoX > 0 ? 1 : -1);
    }
    if (abs(velocidadeCarrinhoZ) > velocidadeMaxima) {
        velocidadeCarrinhoZ = velocidadeMaxima * (velocidadeCarrinhoZ > 0 ? 1 : -1);
    }

    // Atualiza posição
    posCarrinhoX += velocidadeCarrinhoX * deltaTime;
    posCarrinhoZ += velocidadeCarrinhoZ * deltaTime;
}


// Funções movimento da câmera
void atualizaCamera(float deltaTime) {
    float fatorInterpolacao = 0.7f; // Ajuste para mais ou menos suavidade
    float destinoCameraX = posCarrinhoX - distanciaAtras * sin(anguloCarrinho * PI / 180.0f);
    float destinoCameraZ = posCarrinhoZ - distanciaAtras * cos(anguloCarrinho * PI / 180.0f);
    float destinoCameraY = alturaCarrinho + alturaCamera;

    // Interpola suavemente a posição da câmera em direção ao destino
    posCameraX += (destinoCameraX - posCameraX) * fatorInterpolacao * deltaTime;
    posCameraZ += (destinoCameraZ - posCameraZ) * fatorInterpolacao * deltaTime;
    posCameraY += (destinoCameraY - posCameraY) * fatorInterpolacao * deltaTime;

    // Atualiza a direção para onde a câmera está olhando
    cameraX = posCarrinhoX;
    cameraZ = posCarrinhoZ;
    cameraY = alturaCarrinho;
}


void init(void) {
    glClearColor(0.7, 0.9, 1.0, 1.0); // Cor de fundo azul claro

    // Carrega a textura
    texturaID = carregarTextura("textures/maquina-misterio.png");

    // Configura iluminação
    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    // Configura posição da luz pontual do sol
    glLightfv(GL_LIGHT1, GL_POSITION, luz_pontual);
    // Diminui a atenuação da luz, para que a luz do sol seja constante
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.7);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);


    // Configura a cor do ambiente para branco
    GLfloat luz_ambiente[] = { 0.5, 0.5, 0.5, 0.5 }; // Luz ambiente branca
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luz_ambiente);

    // Ativa teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}


void display(void) {
    
    // calculo deltaTime
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Tempo em segundos
    lastTime = currentTime;
    
    // Atualiza a posição do carrinho
    atualizarPosicaoCarrinho(deltaTime);
    // Atualiza a posição da câmera
    atualizaCamera(deltaTime);

    // Limpeza do z-buffer deve ser feita a cada desenho da tela
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Define a posição da câmera
    gluLookAt(posCameraX, posCameraY, posCameraZ, cameraX, cameraY, cameraZ, upX, upY, upZ);
    // Parametros look at
    // eyeX, eyeY, eyeZ: posição da câmera
    // centerX, centerY, centerZ: ponto para onde a câmera está olhando
    // upX, upY, upZ: vetor que indica a direção para cima
    // Debug Camera

    std::cout << "posCameraX: " << posCameraX << " posCameraY: " << posCameraY << " posCameraZ: " << posCameraZ << std::endl;
    std::cout << "cameraX: " << cameraX << " cameraY: " << cameraY << " cameraZ: " << cameraZ << std::endl;
    std::cout << "upX: " << upX << " upY: " << upY << " upZ: " << upZ << std::endl;

    // Debug carrinho
    std::cout << "posCarrinhoX: " << posCarrinhoX << " posCarrinhoZ: " << posCarrinhoZ << std::endl;
    std::cout << "velocidadeCarrinhoX: " << velocidadeCarrinhoX << " velocidadeCarrinhoZ: " << velocidadeCarrinhoZ << std::endl;
    std::cout << "anguloCarrinho: " << anguloCarrinho << std::endl;
    std::cout << "aceleracaoCarrinho: " << aceleracaoCarrinho << std::endl;

    


    // Desenha o sol
    desenhar_sol();

    // Desenha o plano verde
    //desenhar_plano();

    // Desenha nuvens
    desenhar_nuvem(-8, 10, 40, 2.0);
    desenhar_nuvem(-12, 11, 45, 1.5);
    desenhar_nuvem(10, 18, 50, 3.5);
    desenhar_nuvem(25, 10, 80, 4.0);
    desenhar_nuvem(-40, 15, 30, 4.0);
    desenhar_nuvem(35, 15, 35, 3.5);
    desenhar_nuvem(-20, 15, -20, 3.0);
    desenhar_nuvem(25, 15, -25, 3.5);
    desenhar_nuvem(-30, 20, 50, 3.5);
    desenhar_nuvem(30, 20, 45, 3.0);

    // Desenha o modelo
    desenharModelo();   

    // Colorir o terreno
    GLfloat cor_terreno[] = {0.0, 1.0, 0.0, 1.0}; // Cor verde
    glMaterialfv(GL_FRONT, GL_AMBIENT, cor_terreno);


    // Desenha o terreno
    desenhar_terreno(altitudes);


    // Troca o buffer da tela
    glutSwapBuffers();
}


void reshape(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0, (GLfloat) w / (GLfloat) h, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, w, h);
}

void specialKeysUp(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            acelerando = false;
            break;
        case GLUT_KEY_DOWN:
            desacelerando = false;
            break;
    }
}

void specialKeys(int key, int x, int y) {
    const float velocidadeRotacao = 2.0f; // Velocidade de rotação do carrinho
    float velocidadeAtual = sqrt(velocidadeCarrinhoX * velocidadeCarrinhoX + velocidadeCarrinhoZ * velocidadeCarrinhoZ); // Magnitude da velocidade atual
    float velocidadeBase = 1.0f; // Velocidade base do carrinho

    switch (key) {

    // controles antigos da camera
    //     case GLUT_KEY_LEFT : 
    //         posCameraX =  posCameraX*cos(-angulo) + posCameraZ*sin(-angulo);
    //         posCameraZ = -posCameraX*sin(-angulo) + posCameraZ*cos(-angulo);
    //         break;
    //    case GLUT_KEY_RIGHT : 
    //         posCameraX =  posCameraX*cos(angulo) + posCameraZ*sin(angulo);
    //         posCameraZ = -posCameraX*sin(angulo) + posCameraZ*cos(angulo);                      
    //         break;         
    //     case GLUT_KEY_UP :
    //         posCameraZ -= 0.1;
    //         break;
    //     case GLUT_KEY_DOWN :
    //         posCameraZ += 0.1;
    //         break;

    // Controles do carrinho
        // Controles do carrinho
        case GLUT_KEY_LEFT: 
            anguloCarrinho += velocidadeRotacao; // Gira para a esquerda
            break;
        case GLUT_KEY_RIGHT:
            anguloCarrinho -= velocidadeRotacao; // Gira para a direita
            break;
        case GLUT_KEY_UP:
            acelerando = true;
            break;
        case GLUT_KEY_DOWN:
            desacelerando = true;
            break;
        case GLUT_KEY_PAGE_UP :
            posCameraY += 0.1;
            break;
        case GLUT_KEY_PAGE_DOWN :
            posCameraY -= 0.1;
            break;
        case GLUT_KEY_F1 :
            cameraX += 0.1;
            break;
        case GLUT_KEY_F2 :
            cameraY -= 0.1;
            break;
        case GLUT_KEY_F3 :
            cameraZ += 0.1;
            break;
    }

    velocidadeCarrinhoX = sin(anguloCarrinho * PI / 180.0f) * velocidadeAtual;
    velocidadeCarrinhoZ = cos(anguloCarrinho * PI / 180.0f) * velocidadeAtual;

    // Assegura que o ângulo permaneça dentro do intervalo [0,360)
    if (anguloCarrinho >= 360.0f) anguloCarrinho -= 360.0f;
    if (anguloCarrinho < 0.0f) anguloCarrinho += 360.0f;

    glutPostRedisplay();
}

// Função para atualizar o carrinho e a camera mesmo quando não há eventos
void atualizaOcioso(void) {

    // calculo deltaTime
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0f; // Tempo em segundos
    lastTime = currentTime;

    atualizarPosicaoCarrinho(deltaTime);
    atualizaCamera(deltaTime);

    // Força a tela a ser redesenhada
    glutPostRedisplay();
}


int main(int argc, char** argv) {

    carregarModelo("maquina-misterio.obj");
    
    lerPGM("terrain/mapa.pgm", altitudes);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    init();

    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(reshape);
    glutIdleFunc(atualizaOcioso);
    glutSpecialUpFunc(specialKeysUp);

    glutMainLoop();

    return 0;
}
