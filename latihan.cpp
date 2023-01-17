#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <MMSystem.h>
#include <math.h >
#include <bits/stdc++.h>

using namespace std;
const int width = 1000;
const int height = 1000;

// VIEW KAMERA START
GLfloat eyeX = -15; // Kanan-Kiri
GLfloat eyeY = 5;   // Atas-Bawah
GLfloat eyeZ = -50; // Depan-Belakang

// ++
GLfloat centerX = 15;
GLfloat centerY = 5.0f;
GLfloat centerZ = 200;
double angle = 0;
bool l_on1 = true;
bool l_on2 = true;
bool l_on3 = true;
bool l_on4 = false;
float rot = -12;
int stop = 1;
float door_angle = .5;
float l_height = .5;
float spt_cutoff = 30;
unsigned int ID;

vector<int> v;
static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2 - x1;
    Uy = y2 - y1;
    Uz = z2 - z1;

    Vx = x3 - x1;
    Vy = y3 - y1;
    Vz = z3 - z1;

    Nx = Uy * Vz - Uz * Vy;
    Ny = Uz * Vx - Ux * Vz;
    Nz = Ux * Vy - Uy * Vx;

    glNormal3f(Nx, Ny, Nz);
}

static GLfloat v_cube[8][3] =
    {
        {0, 0, 0},
        {0, 0, 1},
        {0, 1, 0},
        {0, 1, 1},

        {1, 0, 0},
        {1, 0, 1},
        {1, 1, 0},
        {1, 1, 1}};

static GLubyte c_ind[6][4] =
    {
        {3, 1, 5, 7}, // front
        {6, 4, 0, 2}, // back
        {2, 3, 7, 6}, // top
        {1, 0, 4, 5}, // bottom
        {7, 5, 4, 6}, // right
        {2, 0, 1, 3}  // left
};

// PEWARNAAN
void cube(float R = 0.5, float G = 0.5, float B = 0.5, int type = 0, float val = 1)
{
    GLfloat m_no[] = {0, 0, 0, 1.0};
    GLfloat m_amb[] = {R, G, B, 1};
    GLfloat m_diff[] = {R, G, B, 1};
    GLfloat m_spec[] = {1, 1, 1, 1};
    GLfloat m_sh[] = {30};

    GLfloat m_em[] = {1, 1, 1, 1};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);
    if (type == 1)
    {
        if (l_on1)
            glMaterialfv(GL_FRONT, GL_EMISSION, m_em);
        else
            glMaterialfv(GL_FRONT, GL_EMISSION, m_no);
    }
    else if (type == 2)
    {
        if (l_on3)
            glMaterialfv(GL_FRONT, GL_EMISSION, m_em);
        else
            glMaterialfv(GL_FRONT, GL_EMISSION, m_no);
    }
    else
        glMaterialfv(GL_FRONT, GL_EMISSION, m_no);

    glBegin(GL_QUADS);
    for (GLint i = 0; i < 6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);
        glTexCoord2f(0, val);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0, 0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(val, 0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(val, val);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);
    }
    glEnd();
}

class BmpLoader
{
public:
    unsigned char *textureData;
    int iWidth, iHeight;

    BmpLoader(const char *);
    ~BmpLoader();

private:
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
};

BmpLoader::BmpLoader(const char *filename)
{
    FILE *file;
    file = fopen(filename, "rb");
    if (!file)
        cout << "File not found" << endl;
    fread(&bfh, sizeof(BITMAPFILEHEADER), 1, file);
    if (bfh.bfType != 0x4D42)
        cout << "Not a valid bitmap" << endl;
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);
    if (bih.biSizeImage == 0)
        bih.biSizeImage = bih.biHeight * bih.biWidth * 3;
    textureData = new unsigned char[bih.biSizeImage];
    fseek(file, bfh.bfOffBits, SEEK_SET);
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for (int i = 0; i < bih.biSizeImage; i += 3)
    {
        temp = textureData[i];
        textureData[i] = textureData[i + 2];
        textureData[i + 2] = temp;
    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader()
{
    delete[] textureData;
}

void LoadTexture(const char *filename)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData);
}

static void res(int width, int height)
{
    glViewport(0, 0, width, height);
}

// lampu bentuk 'C1'
void cse()
{
    glPushMatrix();
    glScalef(.3, .5, 1);
    glTranslatef(45, 15, 1);
    glPushMatrix();

    // C_r1
    glScalef(.4, 1.5, 2.9);

    glTranslatef(45, 3, 2);
    float length = 3.4;
    float width = .4;
    // bawah
    float a = 1, b = 0, c = .5;
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(3 * length, width, width);
    glTranslatef(-.75, -0.5, -0.5);
    cube(a, b, c);
    glPopMatrix();

    // tengah
    glPushMatrix();
    glPushMatrix();
    glTranslatef(1, length / 2, 1);
    ;
    glScalef(width + 3, length + .5, width);
    glTranslatef(0, -0.2, -0.5);
    cube(a, b, c);
    glPopMatrix();
    // atas
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(length * 3, width, width);
    glTranslatef(-.75, 8.7, -0.5);
    cube(a, b, c);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    // 1

    glPushMatrix();

    glScalef(.4, 1.5, 2.9);

    glTranslatef(15, 3, 2);

    // bawah
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(3 * length, width, width);
    glTranslatef(0.75, -0.5, -0.5);
    cube(a, b, c); // kaki bawah
    glPopMatrix();

    // tengah

    glPushMatrix();
    glPushMatrix();
    glTranslatef(-2, length / 2 + .1, 1);
    ;
    glScalef(width + 3, length + .5, width);
    glTranslatef(4, -0.2, -0.5);
    cube(a, b, c); // 1
    glPopMatrix();

    // atas
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(length, width, width);
    glTranslatef(3.75, 8.7, -0.5);
    cube(a, b, c);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}

// lampu bentuk 'C2'
void cse_r2()
{
    glPushMatrix();
    glScalef(.3, .5, 1);
    glTranslatef(-25, 15, 1);
    glPushMatrix();

    // C
    glScalef(.4, 1.5, 2.9);

    glTranslatef(-25, 3, 2);
    float length = 3.4;
    float width = .4;
    // bawah
    float a = 1, b = 0, c = .5;
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(3 * length, width, width);
    glTranslatef(-24.05, -0.5, -0.5);
    cube(a, b, c);
    glPopMatrix();

    // tengah

    glPushMatrix();
    glPushMatrix();
    glTranslatef(1, length / 2, 1);
    ;
    glScalef(width + 3, length + .5, width);
    glTranslatef(-69.9, -0.2, -0.5);
    cube(a, b, c);
    glPopMatrix();

    // atas
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(length * 3, width, width);
    glTranslatef(-24.05, 8.7, -0.5);
    cube(a, b, c);
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    // 2

    glPushMatrix();

    glScalef(.4, 1.5, 2.9);
    glTranslatef(-40, 3, 2);

    // bawah
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(3 * length, width, width);
    glTranslatef(-24.05, -0.5, -0.5);
    cube(a, b, c);
    glPopMatrix();

    //  | tengah kanan
    glPushMatrix();
    glPushMatrix();
    glTranslatef(1, length / 2, 1);
    ;
    glScalef(width + 3, length / 2, width);
    glTranslatef(-72, 0.7, -0.5);
    cube(a, b, c);
    glPopMatrix();

    //  | tengah kiri
    glPushMatrix();
    glTranslatef(1, length / 2, 1);
    ;
    glScalef(width + 3, length / 2 + .1, width);
    glTranslatef(-70, -0.5, -0.5);
    cube(a, b, c);
    glPopMatrix();

    // atas
    glPushMatrix();
    glTranslatef(length / 2, 1, 1);
    ;
    glScalef(length * 3, width, width);
    glTranslatef(-24.05, 8.7, -0.5);
    cube(a, b, c);
    glPopMatrix();

    // - tengah
    glPushMatrix();
    glTranslatef(length, 1, 1);
    ;
    glScalef(3 * length, width, width);
    glTranslatef(-24.23, 4, -0.5);
    cube(a, b, c);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

// lantai kelas 1 dan 2
void flr()
{

    for (int i = -10; i <= 36; i++)
    {
        for (float j = -45; j <= 14; j += 2)
        {
            glPushMatrix();
            //    glTranslatef(0,-0.5,0);

            glScalef(2, .5, 1);
            glTranslatef(j, -.999, -i);
            if (i == 36)
                cube(.71, .75, .58); // batas lantai
            else if (i % 2 == 0)
                cube(1, 1, 1); // putih
            else
                cube(1, .93, .6); // kuning
            glPopMatrix();
        }
    }

    for (int i = -10; i <= 36; i++)
    {
        for (float j = -44; j <= 14; j += 2)
        {
            glPushMatrix();

            glScalef(2, .5, 1);
            glTranslatef(j, -.999, -i);
            if (i == 36)
                cube(.71, .75, .58);
            else if (i % 2 != 0)
                cube(1, 1, 1); // putih
            else
                cube(.71, .75, .58); // ijo
            glPopMatrix();
        }
    }
}

// lantai kelas 3 dan 4 (kelas belakang)
void flr_blkg()
{
    for (int i = -70; i <= -30; i++) // 46
    {
        for (float j = -45; j <= 14; j += 2)
        {
            glPushMatrix();

            glScalef(2, .5, 1);
            glTranslatef(j, -.999, -i);
            if (i == -30)
                cube(.71, .75, .58);
            else if (i % 2 == 0)
                cube(1, 1, 1);
            else
                cube(1, .93, .6);
            glPopMatrix();
        }
    }

    for (int i = -70; i <= -30; i++)
    {
        for (float j = -44; j <= 14; j += 2)
        {
            glPushMatrix();

            glScalef(2, .5, 1);
            glTranslatef(j, -.999, -i);
            if (i == -30)
                cube(.71, .75, .58);
            else if (i % 2 != 0)
                cube(1, 1, 1);
            else
                cube(.71, .75, .58);
            glPopMatrix();
        }
    }
}

// dinding tengah (pembatas kelas 1 dan 2)
void wall1()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);

    glPushMatrix();

    glScalef(1, 20, 41);
    glTranslatef(-30, 0, -0.75);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// dinding tengah (pembatas kelas 3 dan 4)
void wall1_belakang()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);

    glPushMatrix();

    glScalef(1, 20, 41);
    glTranslatef(-30, 0, 0.715);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Dinding Kiri kelas 1
void wall2()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(1, 20, 41);
    glTranslatef(29, 0, -0.75);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Dinding Kiri kelas 3
void wall2_belakang()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);
    glScalef(1, 20, 41);
    glTranslatef(29, 0, 0.715);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Dinding Kiri Pagar
void wall_luar_kiri()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
    glPushMatrix();
    glScalef(1, 20, 140);
    glTranslatef(45, 0, -.470);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Dinding Kanan Pagar
void wall_luar_kanan()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
    glPushMatrix();
    glScalef(1, 20, 140);
    glTranslatef(-106, 0, -.470);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Dinding Depan Pagar
void wall_luar_depan()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
    glPushMatrix();
    glScalef(150, 20, 1);
    glTranslatef(-.70, 0, -66);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Dinding Belakang Pagar
void wall_luar_belakang()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[5]);
    glPushMatrix();
    glScalef(150, 20, 1);
    glTranslatef(-.70, 0, 71);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// Dinding kanan kelas 2
void wall1_r2()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(1, 20, 41);
    glTranslatef(-90, 0, -0.75);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// dinding kanan kelas 4
void wall1_r2_belakang()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);
    glScalef(1, 20, 41);
    glTranslatef(-90, 0, 0.715);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// dinding papan tulis kelas 1 dan 2
void wall3()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(120, 20, 1);
    glTranslatef(-.75, 0, 10);
    cube(0.741, 0.718, 0.420, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// dinding papan tulis kelas 3 dan 4
void wall3_belakang()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPushMatrix();
    glScalef(120, 20, 1);
    glTranslatef(-.75, 0, 70);
    cube(0.741, 0.718, 0.420, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// dinding pintu kelas 1
void door()
{
    glPushMatrix();
    glScalef(50, 20, 1);
    glTranslatef(-.6, 0, -30);
    cube(.71, .75, .58);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20, 10, -30);
    glScalef(10, 10, 1);
    cube(.71, .75, .58);
    glPopMatrix();
}

// dinding pintu kelas 3
void door_belakang()
{
    glPushMatrix();
    glScalef(50, 20, 1);
    glTranslatef(-.6, 0, 30);
    cube(.71, .75, .58);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20, 10, 30);
    glScalef(10, 10, 1);
    cube(.71, .75, .58);
    glPopMatrix();
}

// dinding pintu kelas 2
void door_r2()
{
    glPushMatrix();
    glScalef(50, 20, 1);
    glTranslatef(-1.8, 0, -30);
    cube(.71, .75, .58);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-40, 10, -30);
    glScalef(10, 10, 1);
    cube(.71, .75, .58);
    glPopMatrix();
}

// dinding pintu kelas 4
void door_r2_belakang()
{
    glPushMatrix();
    //    glTraneslatef(0,-0.5,0);
    glScalef(50, 20, 1);
    glTranslatef(-1.8, 0, 30);
    cube(.71, .75, .58);
    glPopMatrix();

    glPushMatrix();
    //    glTranslatef(0,-0.5,0);
    glTranslatef(-40, 10, 30);
    glScalef(10, 10, 1);
    cube(.71, .75, .58);
    glPopMatrix();
}

// foto presiden
void foto_presiden()
{
    // kelas 3
    glPushMatrix();
    glTranslatef(14.75, 9.75, 9.8 + 60);
    glScalef(4.5, 6.5, .2);
    cube(0, 0, 0); // Warna bingkai
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[6]);

    glPushMatrix();
    glTranslatef(15, 10, 9.7 + 60);
    glScalef(4, 6, .2);
    cube(1, 1, 1); // warna foto
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // kelas 4
    glPushMatrix();
    glTranslatef(-44, 9.75, 9.8 + 60);
    glScalef(4.5, 6.5, .2);
    cube(0, 0, 0); // Warna bingkai
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[6]);

    glPushMatrix();
    glTranslatef(-43.75, 10, 9.7 + 60);
    glScalef(4, 6, .2);
    cube(1, 1, 1); // warna foto
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

// papan tulis kelas 1
void board()
{
    glPushMatrix();
    glScalef(30, 10, 1);
    glTranslatef(-0.6, .4, 9);
    cube(0, 0, 0); // Warna dasar papan tulis
    glPopMatrix();

    glPushMatrix();
    glScalef(31.5, 11, 1);
    glTranslatef(-0.601, .32, 9.5);
    cube(.6, .54, .24); // Warna bingkai papan tulis
    glPopMatrix();
}

// papan tulis kelas 3
void board_belakang()
{
    glPushMatrix();
    glScalef(30, 10, 1);
    glTranslatef(-0.6, .4, 69);
    cube(0, 0, 0); // Warna dasar papan tulis
    glPopMatrix();

    glPushMatrix();
    glScalef(31.5, 11, 1);
    glTranslatef(-0.601, .32, 69.5);
    cube(.6, .54, .24); // Warna bingkai papan tulis
    glPopMatrix();
}

// papan tulis kelas 2
void board_r2()
{

    glPushMatrix();
    glScalef(30, 10, 1);
    glTranslatef(-2.6, .4, 9);
    cube(.9, .9, .9); // Warna dasar papan tulis
    glPopMatrix();

    glPushMatrix();
    glScalef(31.5, 11, 1);
    glTranslatef(-2.501, .32, 9.5);
    cube(1, .89, .40); // Warna bingkai papan tulis
    glPopMatrix();
}

// papan tulis kelas 4
void board_r2_belakang()
{
    glPushMatrix();
    glScalef(30, 10, 1);
    glTranslatef(-2.6, .4, 69);
    cube(1, 1, 1); // Warna dasar papan tulis
    glPopMatrix();

    glPushMatrix();
    glScalef(31.5, 11, 1);
    glTranslatef(-2.501, .32, 69.5);
    cube(.9, .9, .9); // Warna bingkai papan tulis
    glPopMatrix();
}

// jendela kelas 1
void window()
{
    glPushMatrix();
    glPushMatrix();
    glScalef(1, 8, 15);
    glTranslatef(27.9, .5, -1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[1]);
    cube(1, 1, 1, 0, 1);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPopMatrix();
    glPushMatrix();
    glScalef(1, 8, .2);
    glTranslatef(27.9, .5, -76);
    cube(1, .59, .0);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glRotatef(door_angle, 0, 1, 0);

    glPushMatrix();
    glScalef(1, .2, 15);
    glTranslatef(27.9, 50, -1);
    cube(0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glScalef(1, .2, 15);
    glTranslatef(27.9, 30, -1);
    cube(0, 0, 0);
    glPopMatrix();
    for (int i = 0; i >= -75; i -= 2)
    {
        glPushMatrix();
        glScalef(1, 8, .2);
        glTranslatef(27.9, .5, i);
        cube(0, 0, 0);
        glPopMatrix();
    }

    glPopMatrix();
}

// jendela kelas 3
void window_belakang()
{
    glPushMatrix();
    glPushMatrix();
    glScalef(1, 8, 15);
    glTranslatef(28.2, .5, 3);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[1]);
    cube(1, 1, 1, 0, 1);
    glBindTexture(GL_TEXTURE_2D, v[0]);
    glPopMatrix();
    glPushMatrix();
    glScalef(1, 8, .2);
    glTranslatef(28.5, .5, -76);
    cube(1, .59, .0);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glRotatef(door_angle, 0, -1, 0);

    glPushMatrix();
    glScalef(1, .2, 15);
    glTranslatef(28.5, 50, 3);
    cube(0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glScalef(1, .2, 15);
    glTranslatef(28.5, 30, 3);
    cube(0, 0, 0);
    glPopMatrix();
    for (int i = 298; i >= 223; i -= 2)
    {
        glPushMatrix();
        glScalef(1, 8, .2);
        glTranslatef(28.2, .5, i);
        cube(0, 0, 0);
        glPopMatrix();
    }

    glPopMatrix();
}

// jendela kelas 2
void window_r2()
{
    /* bagian dalam */
    // Jendela kiri
    glPushMatrix();
    glTranslatef(-88.9, 5, -7);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-89, 4.9, -7.1);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Senderan Kiri
    glPushMatrix();
    glTranslatef(-89, 4.9, -7.1);
    glScalef(1, .5, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kiri
    glPushMatrix();
    glTranslatef(-88.7, 5, -4.75);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-88.7, 8.9, -7.1);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Jendela kanan
    glPushMatrix();
    glTranslatef(-88.9, 5, -22);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-89, 4.9, -22.1);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Sandaran Tangan Kanan
    glPushMatrix();
    glTranslatef(-89, 4.9, -22.1);
    glScalef(1, .5, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kanan
    glPushMatrix();
    glTranslatef(-88.7, 5, -19.75);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-88.7, 8.9, -22.1);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    /* BAGIAN LUAR */
    // Jendela kiri
    glPushMatrix();
    glTranslatef(-90.2, 5, -7);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.1, 4.9, -7.1);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kiri
    glPushMatrix();
    glTranslatef(-90.5, 5, -4.75);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.5, 8.9, -7.1);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Jendela kanan
    glPushMatrix();
    glTranslatef(-90.2, 5, -22);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.1, 4.9, -22.1);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kiri
    glPushMatrix();
    glTranslatef(-90.5, 5, -19.75);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.5, 8.9, -22.1);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();
}

// jendela kelas 4
void window_r2_belakang()
{
    /* bagian dalam */
    // Jendela kiri
    glPushMatrix();
    glTranslatef(-88.9, 5, 53);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-89, 4.9, 52.9);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Senderan Kiri
    glPushMatrix();
    glTranslatef(-89, 4.9, 52.9);
    glScalef(1, .5, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kiri
    glPushMatrix();
    glTranslatef(-88.7, 5, 55.25);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-88.7, 8.9, 52.9);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Jendela kanan
    glPushMatrix();
    glTranslatef(-88.9, 5, 38);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-89, 4.9, 37.9);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Sandaran Tangan Kanan
    glPushMatrix();
    glTranslatef(-89, 4.9, 37.9);
    glScalef(1, .5, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kanan
    glPushMatrix();
    glTranslatef(-88.7, 5, 40.25);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-88.7, 8.9, 37.9);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    /* BAGIAN LUAR */
    // Jendela kiri
    glPushMatrix();
    glTranslatef(-90.2, 5, 53);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.1, 4.9, 52.9);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kiri
    glPushMatrix();
    glTranslatef(-90.5, 5, 55.25);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.5, 8.9, 52.9);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Jendela kanan
    glPushMatrix();
    glTranslatef(-90.2, 5, 38);
    glScalef(.2, 8, 5);
    cube(0.54, 0.80, 0.93, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.1, 4.9, 37.9);
    glScalef(.2, 8.2, 5.2);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    // Garis Tengah Kiri
    glPushMatrix();
    glTranslatef(-90.5, 5, 40.25);
    glScalef(.1, 8, .01);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-90.5, 8.9, 37.9);
    glScalef(.1, .01, 5);
    cube(0, 0, 0, 0, 1);
    glPopMatrix();
}

// atap kelas 1 dan 2
void headwall()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
    glPushMatrix();

    glScalef(120, 1, 41);
    glTranslatef(-0.75, 19, -0.75);
    cube(0.690, 0.769, 0.871, 0, 4);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

// atap kelas 3 dan 4
void headwall_belakang()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[2]);
    glPushMatrix();

    glScalef(120, 1, 41);
    glTranslatef(-0.75, 19, 0.715);
    cube(0.690, 0.769, 0.871, 0, 4);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

float k = 0;
float angle1 = 0;
float kibar = 0, sudut = 0.2;
int batas = 0;
bool markdoor = 0;
double window_val = 0;

void spin()
{
    // kipas
    angle = (angle + k) * stop;
    k += .001;
    if (angle >= 360)
        angle = 0;

    // bendera
    angle1 += .2;

    if (kibar >= 10)
    {
        batas = 1;
    }

    if (kibar <= -10)
    {
        batas = 0;
    }

    if (batas == 0)
    {
        sudut = .2;
    }
    else
    {
        sudut = -.2;
    }

    kibar += sudut;

    if (angle1 >= 360)
        angle1 = 0;

    // jendela
    bool ok = 0;
    bool ok1 = 0;
    if (markdoor && door_angle > -.8)
    {
        door_angle -= .001, ok = 1;
        if (door_angle < .4)
            window_val += .00085;
        window_val = min(window_val, 1.00);
        l_on4 = 1;
    }
    if (!markdoor && door_angle < .5)
    {
        door_angle += .001, ok = 1;
        window_val -= .001;
        window_val = max(window_val, 0.00);
    }
    if (!ok)
        l_on4 = markdoor;
}

double a = 2;
double b = 1;
double c = 10;

// kipas
void fan()
{
    float width = 0.3;
    glPushMatrix();
    glTranslatef(1, 15, -4);
    glScalef(width, 5, width);
    glTranslatef(-20, 0, 20);
    cube(1, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glScalef(1, .1, 1);
    glTranslatef(-5, 150, 2);
    cube(1, 1, 1);
    glRotatef(angle, 0, 0, 1);
    glRecti(-a, -a, a, a);
    cube(1, 0, 0);
    glRecti(-b, a, b, c);
    glRecti(-c, -b, -a, b);
    glRecti(-b, -c, b, -a);
    glRecti(a, -b, c, b);
    glPopMatrix();
}

// kursi
void Chair12()
{
    float height = 2;
    float width = 4;
    float length = 2;

    float base_height = .5;
    float leg_height = height - base_height;
    float leg_width = .4;

    // whole table
    glPushMatrix();
    glTranslatef(0, leg_height, 0);

    // base
    glPushMatrix();
    glScalef(width, base_height, length);
    glTranslatef(-0.5, 0, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    // legs
    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(0.4f, 0.302f, 0.0f);
    glPopMatrix();

    // upper put / Senderan
    glPushMatrix();
    glScalef(3, 2, .5);
    glTranslatef(-0.5, .5, -2);
    cube(0.4f, 0.302f, 0.0f); // WARNA SENDERAN KURSI
    glPopMatrix();

    // middle part 1
    glPushMatrix();
    glScalef(.2, 1, .2);
    glTranslatef(3, 0, -4);
    cube(0, 0, 0);
    glPopMatrix();

    glPushMatrix();
    glScalef(.2, 1, .2);
    glTranslatef(-4, 0, -4);
    cube(0, 0, 0);
    glPopMatrix();

    glPopMatrix();
}

// meja
void Table12(float height = 2, float width = 4, float length = 2, float base_height = .5, float leg_width = .2)
{
    float leg_height = height - base_height;

    // whole table
    glPushMatrix();
    glTranslatef(0, leg_height, 0);

    // base
    glPushMatrix();
    glScalef(width, base_height, length);
    glTranslatef(-0.5, 0, -0.5);
    cube(.6, .2, 0);
    glPopMatrix();

    // legs
    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef((width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, (length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(width / 2 - leg_width / 2), 0, -(length / 2 - leg_width / 2));
    glScalef(leg_width, leg_height, leg_width);
    glTranslatef(-0.5, -1, -0.5);
    cube(.8f, .6f, 0.0f);
    glPopMatrix();
    glPopMatrix();
}

void bangku_taman()
{
    // kelas 1
    glPushMatrix();
    glTranslatef(10.0, 0, -31);
    glScalef(2.0f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    // kelas 2
    glPushMatrix();
    glTranslatef(-45.0, 0, -31);
    glScalef(2.0f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    // kelas 3
    glPushMatrix();
    glTranslatef(10.0, 0, 29);
    glScalef(2.0f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    // kelas 4
    glPushMatrix();
    glTranslatef(-45.0, 0, 29);
    glScalef(2.0f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();
}

// kursi meja di kelas 1 dan 2
void kursi_meja()
{
    // kursi guru di kelas 1
    glPushMatrix();
    glTranslatef(10.0, 1, -14 + 9 * 2.0 + 2.2f);
    glScalef(0.8f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();
    // meja guru kelas 1
    glPushMatrix();
    glTranslatef(10.0, 1, -16 + 9 * 2.0 + 2.2f);
    glScalef(1.2f, 1.3f, 0.8f);
    Table12(2, 4, 2, .6, .3);
    glPopMatrix();

    // kursi guru kelas 2
    glPushMatrix();
    glTranslatef(-50.0, 1, -14 + 9 * 2.0 + 2.2f);
    glScalef(0.8f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    // meja guru kelas 2
    glPushMatrix();
    glTranslatef(-50.0, 1, -16 + 9 * 2.0 + 2.2f);
    glScalef(1.2f, 1.3f, 0.8f);
    Table12(2, 4, 2, .6, .3);
    glPopMatrix();

    // perulangan kursi di kelas 1
    for (int i = -8; i <= 10; i += 2)
    {
        for (int j = -4; j <= 6; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0, 0.6f, -16 + j * 2.0 + 2.2f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(0.0, 0.0, 1.0, 0.0);
            Chair12();
            glPopMatrix();
        }
    }

    // perulangan kursi di kelas 2
    for (int i = -39; i <= -21; i += 2)
    {
        for (int j = -4; j <= 6; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0, 0.6f, -16 + j * 2.0 + 2.2f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(0.0, 0.0, 1.0, 0.0);
            Chair12();
            glPopMatrix();
        }
    }

    // perulangan meja di kelas 1
    for (int i = -8; i <= 10; i += 2)
    {
        for (int j = -4; j <= 6; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0 + 0.3f, 1.2f, -14.2 + j * 2.0 + 1.2f);
            glScalef(0.75f, 0.7f, 0.6f);
            Table12();
            glPopMatrix();
        }
    }

    // perulangan meja di kelas 2
    for (int i = -39; i <= -21; i += 2)
    {
        for (int j = -4; j <= 6; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0 + 0.3f, 1.2f, -14.2 + j * 2.0 + 1.2f);
            glScalef(0.75f, 0.7f, 0.6f);
            Table12();
            glPopMatrix();
        }
    }
}

// kursi meja di kelas 3 dan 4
void kursi_meja_belakang()
{
    // kursi guru di kelas 3
    glPushMatrix();
    glTranslatef(10.0, 1, 46 + 9 * 2.0 + 2.2f);
    glScalef(0.8f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    // meja guru di kelas 3
    glPushMatrix();
    glTranslatef(10.0, 1, 44 + 9 * 2.0 + 2.2f);
    glScalef(1.2f, 1.3f, 0.8f);
    Table12(2, 4, 2, .6, .3);
    glPopMatrix();

    // kursi guru di kelas 4
    glPushMatrix();
    glTranslatef(-50.0, 1, 46 + 9 * 2.0 + 2.2f);
    glScalef(0.8f, 0.8f, 0.8f);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    Chair12();
    glPopMatrix();

    // meja guru di kelas 4
    glPushMatrix();
    glTranslatef(-50.0, 1, 44 + 9 * 2.0 + 2.2f);
    glScalef(1.2f, 1.3f, 0.8f);
    Table12(2, 4, 2, .6, .3);
    glPopMatrix();

    // perulangan kursi di kelas 3
    for (int i = -8; i <= 10; i += 2) // -16 s/d +20 // ada 10 bangku
    {
        for (int j = 26; j <= 36; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0, 0.6f, -16 + j * 2.0 + 2.2f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(0.0, 0.0, 1.0, 0.0);
            Chair12();
            glPopMatrix();
        }
    }

    // perulangan kursi di kelas 4
    for (int i = -39; i <= -21; i += 2)
    {
        for (int j = 26; j <= 36; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0, 0.6f, -16 + j * 2.0 + 2.2f);
            glScalef(0.5f, 0.5f, 0.5f);
            glRotatef(0.0, 0.0, 1.0, 0.0);
            Chair12();
            glPopMatrix();
        }
    }

    // perulangan meja di kelas 3
    for (int i = -8; i <= 10; i += 2)
    {
        for (int j = 26; j <= 36; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0 + 0.3f, 1.2f, -14.2 + j * 2.0 + 1.2f);
            glScalef(0.75f, 0.7f, 0.6f);
            Table12();
            glPopMatrix();
        }
    }

    // perulangan meja di kelas 4
    for (int i = -39; i <= -21; i += 2)
    {
        for (int j = 26; j <= 36; j += 2)
        {
            glPushMatrix();
            glTranslatef(i * 2.0 + 0.3f, 1.2f, -14.2 + j * 2.0 + 1.2f);
            glScalef(0.75f, 0.7f, 0.6f);
            Table12();
            glPopMatrix();
        }
    }
}

void drop()
{
    glPushMatrix();
    // wear drop front
    cube(113 / 255.0, 74 / 255.0, 44 / 255.0);
    // glColor3ub (113,74,44);
    glBegin(GL_POLYGON);
    glVertex3f(460, 660, 0);
    glVertex3f(470, 670, 0);
    glVertex3f(330, 670, 0);
    glVertex3f(340, 660, 0);
    glVertex3f(340, 320, 0);
    glVertex3f(460, 320, 0);
    glEnd();

    // wear drop front-drawer 3 - outer

    glPopMatrix();

    glPushMatrix();

    glTranslatef(0, 0, 0);
    glScalef(105, 95, 10);

    glPushMatrix();
    glTranslatef(3.32, 4.6, -.49);
    cube(62 / 255.0, 43 / 255.0, 45 / 255.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.32, 5.7, -.49);
    cube(62 / 255.0, 43 / 255.0, 45 / 255.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.32, 3.5, -.49);
    cube(62 / 255.0, 43 / 255.0, 45 / 255.0);
    glPopMatrix();
    glPopMatrix();

    // wear drop front-drawer 1 - handel

    glPushMatrix();
    glTranslatef(1, 0, 0);
    glScalef(20, 10, 10);
    glPushMatrix();
    glTranslatef(19.5, 38, -.5);
    cube(1, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5, 58, -.5);
    cube(1, 1, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(19.5, 48, -.5);
    cube(1, 1, 1);
    glPopMatrix();

    glPopMatrix();

    // wear drop side

    cube(62 / 255.0, 43 / 255.0, 35 / 255.0);
    glColor3ub(62, 43, 35);
    glBegin(GL_POLYGON);
    glVertex3f(470, 670, 0);
    glVertex3f(500, 667, 0);
    glVertex3f(492, 657, 0);
    glVertex3f(492, 330, 0);
    glVertex3f(460, 320, 0);
    glVertex3f(460, 660, 0);
    glEnd();

    glPopMatrix();
}
void weardrop()
{
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(9, -7, 8);
    // glRotatef(rot,0,1,1);
    glScalef(.04, .022, .4);
    drop();
    glPopMatrix();
}

void weardrop_belakang()
{
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(9, -7, 68);
    // glRotatef(rot,0,1,1);
    glScalef(.04, .022, .4);
    drop();
    glPopMatrix();
}

void weardrop_r2()
{
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(-50, -7, 8);
    // glRotatef(rot,0,1,1);
    glScalef(.04, .022, .4);
    drop();
    glPopMatrix();
}

void weardrop_r2_belakang()
{
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(-50, -7, 68);
    // glRotatef(rot,0,1,1);
    glScalef(.04, .022, .4);
    drop();
    glPopMatrix();
}
void tiangbendera()
{
    // Base
    glPushMatrix();
    glTranslatef(-20, 0, -40);
    glScalef(3, 0.2, 2);
    cube(0.3, 0.3, 0.3);
    glPopMatrix();

    // Kotak tengah
    glPushMatrix();
    glTranslatef(-19, 0.2, -39.25);
    glScalef(1, 2, 0.5);
    cube(0.1, 0.1, 0.1);
    glPopMatrix();

    // tiang
    glPushMatrix();
    glTranslatef(-18.5, 2.2, -39.1);
    glScalef(0.1, 14, 0.1);
    cube(1, 1, 1);
    glPopMatrix();
}

bool gerakBendera = 0, atas = 1, bawah = 0;
double tmerah = 15, tputih = 14;
void bendera()
{
    // Bendera Merah
    if (gerakBendera == 1)
    {
        if (atas == 1)
        {
            if (tmerah - 1 != 4 && tputih - 1 != 3)
            {
                tmerah = tmerah - 1;
                tputih = tputih - 1;
            }
            else
            {
                atas = 0;
                bawah = 1;
            }
            gerakBendera = 0;
        }

        if (bawah == 1)
        {
            if (tmerah + 1 != 16 && tputih + 1 != 15)
            {
                tmerah = tmerah + 1;
                tputih = tputih + 1;
            }
            else
            {
                atas = 1;
                bawah = 0;
            }
            gerakBendera = 0;
        }
    }
    glPushMatrix();
    glTranslatef(-18.5, tmerah, -39.1);
    glRotatef(kibar, 0, 1, 0);
    glScalef(-3, 1, 0.1);
    cube(1, 0, 0);
    glPopMatrix();

    // Bendera Putih
    glPushMatrix();
    glTranslatef(-18.5, tputih, -39.1);
    glRotatef(kibar, 0, 1, 0);
    glScalef(-3, 1, 0.1);
    cube(1, 1, 1);
    glPopMatrix();
}

void clock1()
{
    glPushMatrix();
    // clock outer
    glPushMatrix();
    glScalef(1.5, 1.3, 1);
    glTranslatef(-220, -200, 1);
    cube(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex3f(680, 800, 0);
    glVertex3f(630, 800, 0);
    glVertex3f(630, 900, 0);
    glVertex3f(680, 900, 0);
    glEnd();

    // clock inner
    glPushMatrix();
    glTranslatef(0, 0, -.98);
    cube(0.847, 0.749, 0.847);
    glBegin(GL_QUADS);
    glVertex3f(675, 805, 0);
    glVertex3f(635, 805, 0);
    glVertex3f(635, 895, 0);
    glVertex3f(675, 895, 0);
    glEnd();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -2);
    ////////////////////////////
    glPushMatrix();
    glTranslatef(653, 847, 1);
    glRotatef(angle1, 0, 0, 1);

    cube(1, 0, 0);
    glRecti(-b, a, b, 3 * c);
    glPopMatrix();
    /////////////////////////////
    // clock second
    cube(0, 0, 0);

    glBegin(GL_LINES);
    glVertex3f(655, 850, 0);
    glVertex3f(655, 870, 0);
    glEnd();

    // clock hour
    cube(0, 0, 0);
    glBegin(GL_LINES);

    // glBegin(GL_POINT_SIZE);
    glVertex3f(655, 850, 0);
    glVertex3f(645, 810, 0);
    glEnd();
    glPopMatrix();
    glPopMatrix();
}
void clock2()
{

    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(-75, -30, 9);
    // glRotatef(rot,0,1,1);
    glScalef(.08, .05, .05);
    clock1();
    glPopMatrix();
}
void clock2_belakang()
{

    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(-75, -30, 69);
    // glRotatef(rot,0,1,1);
    glScalef(.08, .05, .05);
    clock1();
    glPopMatrix();
}
void clock2_r2()
{

    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(-135, -30, 9);
    // glRotatef(rot,0,1,1);
    glScalef(.08, .05, .05);
    clock1();
    glPopMatrix();
}

void clock2_r2_belakang()
{

    glPushMatrix();
    //    glTranslatef(0,-0.5,0);

    glTranslatef(-135, -30, 69);
    // glRotatef(rot,0,1,1);
    glScalef(.08, .05, .05);
    clock1();
    glPopMatrix();
}

void fan1()
{
    glPushMatrix();
    glTranslatef(1, -.5, -15);
    fan();
    glPopMatrix();
}

void fan1_r2()
{
    glPushMatrix();
    glTranslatef(-60, -.5, -15);
    fan();
    glPopMatrix();
}

void fan2_r2()
{
    glPushMatrix();
    glTranslatef(-60, -.5, -2);
    fan();
    glPopMatrix();
}

void fan_belakang()
{
    glPushMatrix();
    glTranslatef(1, -.5, 58);
    fan();
    glPopMatrix();
}

void fan1_belakang()
{
    glPushMatrix();
    glTranslatef(1, -.5, 45);
    fan();
    glPopMatrix();
}

void fan1_r2_belakang()
{
    glPushMatrix();
    glTranslatef(-60, -.5, 58);
    fan();
    glPopMatrix();
}

void fan2_r2_belakang()
{
    glPushMatrix();
    glTranslatef(-60, -.5, 45);
    fan();
    glPopMatrix();
}

void lapang()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[3]);
    glPushMatrix();
    // glTranslatef(0,-0.5,0);
    glScalef(150, .5, 30);
    glTranslatef(-0.70, -.998, -2.2);
    cube(0.2, 1, 0.2, 0, 1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void lorong_kanan() // Dinding Kanan Ruang Kedua
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[4]);
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);
    glScalef(15, .5, 110);
    glTranslatef(-7, -.998, -.330);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void lorong_kiri() // Dinding Kanan Ruang Kedua
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[4]);
    glPushMatrix();
    //    glTranslatef(0,-0.5,0);
    glScalef(15, .5, 110);
    glTranslatef(2, -.998, -.330);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void lorong()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, v[4]);
    glPushMatrix();
    // glTranslatef(0,-0.5,0);
    glScalef(120, .5, 20);
    glTranslatef(-0.75, -.998, .50);
    cube(1, 1, 1, 0, 2);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

float al1, dl1, sl1;
float al2, dl2, sl2;
float al3, dl3, sl3;

void light1(float a, float b, float c)
{
    glEnable(GL_LIGHT0);

    // light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.4 + al1, 0.4 + al1, 0.4 + al1, 1.0};
    GLfloat l_dif[] = {.1 + dl1, .1 + dl1, .1 + dl1, 1};
    GLfloat l_spec[] = {.1 + sl1, .1 + sl1, .1 + sl1, 1};
    GLfloat l_pos[] = {a, b, c, 1.0};

    if (l_on1)
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_amb);
    else
        glLightfv(GL_LIGHT0, GL_AMBIENT, l_no);
    if (l_on1)
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_dif);
    else
        glLightfv(GL_LIGHT0, GL_DIFFUSE, l_no);
    if (l_on1)
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_spec);
    else
        glLightfv(GL_LIGHT0, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT0, GL_POSITION, l_pos);
}

void light2(float a, float b, float c)
{
    glEnable(GL_LIGHT2);
    // light
    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {0.3 + al2, 0.3 + al2, 0.3 + al2, 1.0};
    GLfloat l_dif[] = {0.1 + dl2, 0.1 + dl2, 0.1 + dl2, 1};
    GLfloat l_spec[] = {0 + sl2, 0 + sl2, 0 + sl2, 1};
    GLfloat l_pos[] = {a, b, c, 1.0};

    if (l_on3)
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_amb);
    else
        glLightfv(GL_LIGHT2, GL_AMBIENT, l_no);
    if (l_on3)
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_dif);
    else
        glLightfv(GL_LIGHT2, GL_DIFFUSE, l_no);
    if (l_on3)
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_spec);
    else
        glLightfv(GL_LIGHT2, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT2, GL_POSITION, l_pos);
}

void spot_light(float a, float b, float c)
{
    // light
    glEnable(GL_LIGHT1);

    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {1 + al3, 0 + al3, 0 + al3, 1.0};
    GLfloat l_dif[] = {1 + dl3, 1 + dl3, 1 + dl3, 1};
    GLfloat l_spec[] = {1 + sl3, 1 + sl3, 1 + sl3, 1};
    GLfloat l_pos[] = {a, b, c, 1.0};

    if (l_on2)
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_amb);
    else
        glLightfv(GL_LIGHT1, GL_AMBIENT, l_no);
    if (l_on2)
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_dif);
    else
        glLightfv(GL_LIGHT1, GL_DIFFUSE, l_no);
    if (l_on2)
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_spec);
    else
        glLightfv(GL_LIGHT1, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT1, GL_POSITION, l_pos);
    GLfloat l_spt[] = {0, -1, 0, 1};
    GLfloat spt_ct[] = {30};
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, l_spt);
    glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, spt_ct);
}

void window_light(float a, float b, float c)
{
    // light
    glEnable(GL_LIGHT3);

    GLfloat l_no[] = {0, 0, 0, 1.0};
    GLfloat l_amb[] = {.5 * window_val, .5 * window_val, .5 * window_val, 1.0};
    GLfloat l_dif[] = {1 * window_val, 1 * window_val, 1 * window_val, 1};
    GLfloat l_spec[] = {1 * window_val, 1 * window_val, 1 * window_val, 1};
    GLfloat l_pos[] = {a, b, c, 1.0};

    if (l_on4)
        glLightfv(GL_LIGHT3, GL_AMBIENT, l_amb);
    else
        glLightfv(GL_LIGHT3, GL_AMBIENT, l_no);
    if (l_on4)
        glLightfv(GL_LIGHT3, GL_DIFFUSE, l_dif);
    else
        glLightfv(GL_LIGHT3, GL_DIFFUSE, l_no);
    if (l_on4)
        glLightfv(GL_LIGHT3, GL_SPECULAR, l_spec);
    else
        glLightfv(GL_LIGHT3, GL_SPECULAR, l_no);

    glLightfv(GL_LIGHT3, GL_POSITION, l_pos);
    GLfloat l_spt[] = {0, -1, 0, 1};
    GLfloat spt_ct[] = {84};
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, l_spt);
    glLightfv(GL_LIGHT3, GL_SPOT_CUTOFF, spt_ct);
}

void light()
{
    // light 1

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    float a = -5, b = 16, c = 9;
    light1(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1, 1);
    glPopMatrix();
    // cout<<l_height<<" "<<spt_cutoff<<endl;

    // light2

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    a = -5, b = 17, c = -15;
    light2(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1, 2);
    glPopMatrix();

    // spot light
    glPushMatrix();
    glPushMatrix();
    glRotatef(-95, 0, 1, 0);
    a = 15, b = 30, c = -15;
    spot_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    // cube(1,0,0,true);
    glPopMatrix();

    // window light
    glPushMatrix();
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    a = 35, b = 15, c = -10;
    window_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    glPopMatrix();
}

void light_belakang()
{
    // light 1

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    float a = -5, b = 16, c = 69;
    light1(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1, 1);
    glPopMatrix();
    // cout<<l_height<<" "<<spt_cutoff<<endl;

    // light2

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    a = -5, b = 17, c = 45;
    light2(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1, 2);
    glPopMatrix();

    // spot light
    glPushMatrix();
    glPushMatrix();
    glRotatef(-95, 0, 1, 0);
    a = 15, b = 30, c = 45;
    // spot_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    // cube(1,0,0,true);
    glPopMatrix();

    // window light
    glPushMatrix();
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    a = 35, b = 15, c = 50;
    window_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    // cube(1,0,0,true);
    glPopMatrix();
    // cout<<sl2<<endl;
    //  cout<<window_val<<endl;
    // cout<<l_height<<" "<<spt_cutoff<<endl;
}

void light_r2()
{

    // light 1

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    float a = -5, b = 16, c = 9;
    light1(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-4.5, -0.5, -0.5);
    cube(1, 1, 1, 1);
    glPopMatrix();
    // cout<<l_height<<" "<<spt_cutoff<<endl;

    // light2

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    a = 15, b = 17, c = -15;
    light2(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-5.75, -0.5, -0.5);
    cube(1, 1, 1, 2);
    glPopMatrix();

    // spot light // nah disini ni pusing
    glPushMatrix();
    glPushMatrix();
    glRotatef(-95, 0, 1, 0);
    a = 15, b = 30, c = -15;
    spot_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-4.5, -0.5, -0.5);
    // cube(1,0,0,true);
    glPopMatrix();

    // window light
    glPushMatrix();
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    a = 35, b = 15, c = -10;
    window_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-4.5, -0.5, -0.5);

    // cube(1,0,0,true);
    glPopMatrix();
}

void light_r2_belakang()
{

    // light 1

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    float a = -5, b = 16, c = 69;
    light1(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-4.5, -0.5, -0.5);
    cube(1, 1, 1, 1);
    glPopMatrix();
    // cout<<l_height<<" "<<spt_cutoff<<endl;

    // light2

    glPushMatrix();
    glPushMatrix();

    glRotatef(200, 0, 1, 0);
    a = 15, b = 17, c = 45;
    light2(a, b, c);
    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-5.75, -0.5, -0.5);
    cube(1, 1, 1, 2);
    glPopMatrix();

    // spot light // nah disini ni pusing
    glPushMatrix();
    glPushMatrix();
    glRotatef(-95, 0, 1, 0);
    a = 15, b = 30, c = -15;
    spot_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-4.5, -0.5, -0.5);
    // cube(1,0,0,true);
    glPopMatrix();

    // window light
    glPushMatrix();
    glPushMatrix();
    glRotatef(0, 0, 1, 0);
    a = 35, b = 15, c = 50;
    window_light(a, b, c);

    glPopMatrix();
    glTranslatef(a, b + 1, c);
    glScalef(15, 1, 1);
    glTranslatef(-4.5, -0.5, -0.5);

    // cube(1,0,0,true);
    glPopMatrix();
}

// angle of rotation for the camera direction
float angle_baru = 0.0;
// actual vector representing the camera's direction
float lx = 0.0f, ly = 0.0f, lz = 1.0f;
// XZ position of the camera
float x = eyeX, y = eyeY, z = eyeZ;

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-3, 3, -3, 3, 2.0, 200.0);
    gluPerspective(0, 0, 0, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z,
              x + lx, y + ly, z + lz,
              0.0f, 1.0f, 0.0f);
    // gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, 0, 1, 0);
    /*eyeX, eyeY, eyeZ Specifies the position of the eye point.
    centerX, centerY, centerZ Specifies the position of the reference point.
    upX, upY, upZ Specifies the direction of the up vector.
    */
    //  glViewport(0, 0, width, height);

    glRotatef(rot, 0, 1, 0);
    cse();
    cse_r2();
    flr();
    flr_blkg();
    wall1();
    wall1_belakang();
    wall2();
    wall2_belakang();
    wall1_r2();
    wall1_r2_belakang();
    wall3();
    wall3_belakang();
    wall_luar_kiri();
    wall_luar_kanan();
    wall_luar_depan();
    wall_luar_belakang();
    door();
    door_belakang();
    door_r2();
    door_r2_belakang();
    board();
    board_r2();
    board_belakang();
    board_r2_belakang();
    headwall();
    headwall_belakang();
    fan();
    kursi_meja();
    kursi_meja_belakang();
    bangku_taman();
    window();
    window_belakang();
    window_r2();
    window_r2_belakang();
    clock2_belakang();
    clock2();
    clock2_r2_belakang();
    clock2_r2();
    weardrop();
    weardrop_r2();
    weardrop_belakang();
    weardrop_r2_belakang();
    foto_presiden();
    tiangbendera();
    bendera();
    fan1();
    fan1_r2();
    fan2_r2();
    fan_belakang();
    fan1_belakang();
    fan1_r2_belakang();
    fan2_r2_belakang();
    light();
    light_r2();
    light_belakang();
    light_r2_belakang();
    lapang();
    lorong();
    lorong_kanan();
    lorong_kiri();
    glutSwapBuffers();
}

void processSpecialKeys(int key, int xx, int yy)
{

    float fraction = 1.0f;

    switch (key)
    {
    case GLUT_KEY_LEFT: // Muterin kamera ke kiri
        angle_baru -= 0.05f;
        lx = sin(angle_baru);
        lz = -cos(angle_baru);
        break;
    case GLUT_KEY_RIGHT: // Muterin kamera ke kanan
        angle_baru += 0.05f;
        lx = sin(angle_baru);
        lz = -cos(angle_baru);
        break;
    case GLUT_KEY_UP: // Maju sesuai arah kamera
        x += lx * fraction;
        z += lz * fraction;
        break;
    case GLUT_KEY_DOWN: // Mundur sesuai arah kamera
        x -= lx * fraction;
        z -= lz * fraction;
        break;
    case GLUT_KEY_F12: // Terbang ke langit
        y += lz * fraction;
        break;
    case GLUT_KEY_F11: // Jatuh ke dasar
        y -= lz * fraction;
        break;
    case GLUT_KEY_F5: // Mendongak ke atas
        angle_baru += 0.05f;
        ly = sin(angle_baru);
        break;
    case GLUT_KEY_F4: // Nunduk ke bawah
        angle_baru -= 0.05f;
        ly = sin(angle_baru);
        break;
    }
}

static void key(unsigned char key, int x, int y);

float l_val = .1;

static void light14(unsigned char light1, int x, int y)
{

    switch (light1)
    {
    case 27:
        glutKeyboardFunc(key);
        break;
    case '1':
        al1 += l_val;
        break;
    case '2':
        al1 -= l_val;
        break;
    case '3':
        dl1 += l_val;
        break;
    case '4':
        dl1 -= l_val;
        break;
    case '5':
        sl1 += l_val;
        break;
    case '6':
        sl1 -= l_val;
        break;
    }
read:
    return;
}

static void light24(unsigned char light2, int x, int y)
{
    switch (light2)
    {

    case 27:
        glutKeyboardFunc(key);

        break;
    case '1':
        al2 += l_val;
        break;
    case '2':
        al2 -= l_val;
        break;
    case '3':
        dl2 += l_val;
        break;
    case '4':
        dl2 -= l_val;
        break;
    case '5':
        sl2 += l_val;
        break;
    case '6':
        sl2 -= l_val;
        break;
    }
}

static void spot_light14(unsigned char spot_light, int x, int y)
{
    switch (spot_light)
    {

    case 27:

        glutKeyboardFunc(key);
        break;
    case '1':
        al3 += l_val;
        break;
    case '2':
        al3 -= l_val;
        break;
    case '3':
        dl3 += l_val;
        break;
    case '4':
        dl3 -= l_val;
        break;
    case '5':
        sl3 += l_val;
        break;
    case '6':
        sl3 -= l_val;
        break;
    }
}
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {

    case 27:
        exit(0);
        break;

    case 'i':
        // centerX++;
        centerY += 0.5f; // Dongak ke atas
        //  centerZ++;
        break;
    case 'k':
        //  centerX--;
        centerY--; // Nunduk ke bawah
        //  centerZ--;
        break;

    case ',':
        rot++;
        break;
    case '.':
        rot--;
        break;

    case '*':
        stop = 0;
        break;

    case ';':
        angle = 0, stop = 1, k = 0;
        break;

    case '1':
        l_height += .1;
        break;
    case '2':
        l_height -= .1;
        break;

    case '3':
        spt_cutoff++;
        break;
    case '4':
        spt_cutoff--;
        break;
    case 't':
        l_on1 = 1 - l_on1; // Lampu di atas papan tulis
        break;

    case 'u':
        l_on2 = 1 - l_on2; // Lampu CSE
        break;

    case 'y': // Lampu paling luar
        l_on3 = 1 - l_on3;
        break;
    case 'v':
        markdoor = 1 - markdoor;
        break;
    case 'f': // Naik Turun Bendera
        gerakBendera = 1 - gerakBendera;
    case 'b':
        glutKeyboardFunc(light14);
        break;
    case 'n':
        glutKeyboardFunc(light24);
        break;
    case 'm':
        glutKeyboardFunc(spot_light14);
        break;
    case 'x':
        sndPlaySound("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\libur-telah-tiba.wav", SND_ASYNC);
        break;
    case 'z':
        sndPlaySound(0, 0);
        break;
    case 'h':
        glClearColor(0.54f, 0.80f, 0.93f, 1.0f); // siang
        break;
    case 'g':
        glClearColor(0.0f, 0.05f, 0.3f, 0.0f); // malam
        break;
    case 'j':
        glClearColor(1.0f, 0.76f, 0.3f, 0.0f); // sore
        break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
    spin();
}

void texture_image()
{
    LoadTexture("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\wall1.bmp");
    v.push_back(ID);
    LoadTexture("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\head.bmp");
    v.push_back(ID);
    LoadTexture("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\top.bmp");
    v.push_back(ID);
    LoadTexture("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\lapangan.bmp");
    v.push_back(ID);
    LoadTexture("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\piso.bmp");
    v.push_back(ID);
    LoadTexture("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\marble.bmp");
    v.push_back(ID);
    LoadTexture("E:\\SEMESTER 3\\GRAFIKOM\\CODEBLOCK\\TUBES\\figures\\bintang.bmp");
    v.push_back(ID);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 50);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Sekolah Sederhana - Tugas Besar Kelompok 7 Ilmu Komputer C1 2021");

    glClearColor(0.54f, 0.80f, 0.93f, 1.0f);
    glutReshapeFunc(res);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(processSpecialKeys);
    glutIdleFunc(idle);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);

    glEnable(GL_LIGHTING);
    GLfloat globalAmbient[] = {0.0, 0.0, 0.0, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    texture_image();
    printf("PERINGATAN!!! Mohon matikan tombol capslock dan gunakan tombol shift untuk menggunakan perintah tanda bintang (*).\n");
    printf("1.  Tekan 't' untuk mematikan lampu depan.\n");
    printf("2.  Tekan 'y' untuk mematikan lampu belakang.\n");
    printf("3.  Tekan 'u' untuk mematikan lampu simbol kelas.\n");
    printf("4.  Tekan '*' untuk mematikan kipas angin.\n");
    printf("5.  Tekan ';' untuk menyalakan kipas angin.\n");
    printf("6.  Tekan 'v' untuk membuka/menutup jendela.\n");
    printf("7.  Tekan 'f' untuk menaik/menurunkan bendera. (lalu tekan ESC untuk keluar dari opsi) \n");
    printf("8.  Tekan '->' untuk ke kanan.\n9.  Tekan '<-' untuk ke kiri.\n");
    printf("10. Tekan 'PgUp' untuk maju.\n11. Tekan 'PgDn' untuk mundur.\n");
    printf("12. Tekan 'f11' untuk jatuh.\n13. Tekan 'f12' untuk terbang.\n");
    printf("14. Tekan 'f4' untuk menunduk.\n15. Tekan 'f5' untuk mendongak.\n");
    printf("15.  Tekan 'g' untuk mode malam.\n");
    printf("16.  Tekan 'h' untuk mode pagi.\n");
    printf("17.  Tekan 'j' untuk mode sore.\n");
    printf("18.  Tekan 'x' untuk menyalakan musik.\n");
    printf("19.  Tekan 'z' untuk mematikan musik.\n");
    printf("20. Tekan ESC untuk keluar dari program\n");

    // sndPlaySound("C:\\KULIAH\\SEMESTER 3\\GRAFIKOM\\test\\test\\audio\\libur-telah-tiba.wav", SND_ASYNC);
    glutMainLoop();

    return EXIT_SUCCESS;
}
