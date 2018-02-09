#include<windows.h>
#include<bits/stdc++.h>
using namespace std;
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include "imageloader.h"
#include "text3d.h"
#include "vec3f.h"
/* GLUT callback Handlers */

float posL1=0.8f;
float posL2=0.8f;
float posL3=0.8f;
float R=0,G=0.4,B=1.0;
float skyAngle = 360;
float _angle = -30.0f;
float Rangle = 0.0f;
int night = 0;
int sun_moon_flag = 0;
int light_on = 0;
int fogflag=0;

GLuint loadTexture(Image* image)
{
    GLuint textureId;

    glGenTextures(1, &textureId); // 1 = # of texture we need

    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
                 0,                            //0 for now
                 GL_RGB,                       //Format OpenGL uses for image
                 image->width, image->height,  //Width and height
                 0,                            //The border of the image
                 GL_RGB, //GL_RGB, because pixels are stored in RGB format
                 GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
                 //as unsigned numbers
                 image->pixels);               //The actual pixel data
    return textureId; //Returns the id of the texture
}


float angle(float deg)
{
    float r = (deg / 180.0f) * 3.141592653589793f;
    return r;
}


//##################################################################################################################
//##################################################################################################################
//##################################################################################################################

GLuint _textureIdGrass;
GLuint _textureIdDay;
GLuint _textureIdNight;
GLuint _textureIdBrick;
GLuint _textureIdWall;
GLuint _textureIdTreeBody;
GLuint _textureIdPlatform;

// ------------------------------------------------------------------------------------------------------------------------
GLuint water_tex[10];
int water_tex_cnt, current_water_tex, water_tex_change_time;

GLuint leaf_tex[10];
int leaf_tex_cnt, current_leaf_tex, leaf_tex_change_time;

GLuint flag_tex[10];
int flag_tex_cnt, current_flag_tex, flag_tex_change_time;
// ------------------------------------------------------------------------------------------------------------------------

void draw_cylinder(float cylinder_radius,float cylinder_height,float slice_count)
{
    // Wheel
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdWall);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glColor3f(1,1,1);
    // angle
    float ttl_angle, angle_per_slice=360.0f/slice_count;
    // points
    float x, x_back=cylinder_radius;
    float y, y_back=0.0f;
    for(int i=1; i<=slice_count; i++)
    {
        // calculate coordinates
        ttl_angle = i*angle_per_slice;
        x = cylinder_radius * cos(angle(ttl_angle));
        y = cylinder_radius * sin(angle(ttl_angle));

        // front face
        // color
        //glColor3f(0.78f,0.78f,0.78f);

        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.5f+x*0.5f,0.5f+y*0.5f);
        glNormal3f(x,y,cylinder_height/2.0f);
        glVertex3f(x,y,cylinder_height/2.0f);

        glTexCoord2f(0.5f+x_back*0.5f,0.5f+y_back*0.5f);
        glNormal3f(x_back,y_back,cylinder_height/2.0f);
        glVertex3f(x_back,y_back,cylinder_height/2.0f);

        glTexCoord2f(0.5f+0.0f,0.5f+0.0f);
        glNormal3f(0.0f,0.0f,cylinder_height/2.0f);
        glVertex3f(0.0f,0.0f,cylinder_height/2.0f);
        glEnd();

        // filling
        // color
        //glColor3f(0.78f,0.78f,0.78f);
        // draw
        glBegin(GL_QUADS);
        glNormal3f(x,y,cylinder_height/2.0f);
        glVertex3f(x,y,cylinder_height/2.0f);
        glNormal3f(x_back,y_back,cylinder_height/2.0f);
        glVertex3f(x_back,y_back,cylinder_height/2.0f);
        glNormal3f(x_back,y_back,-cylinder_height/2.0f);
        glVertex3f(x_back,y_back,-cylinder_height/2.0f);
        glNormal3f(x,y,-cylinder_height/2.0f);
        glVertex3f(x,y,-cylinder_height/2.0f);
        glEnd();
        //glDisable(GL_TEXTURE_2D);
        // back face
        // color
        //glColor3f(1,1,1);

        //glEnable(GL_TEXTURE_2D);


        glBegin(GL_TRIANGLES);

        glTexCoord2f(0.5f+x*0.5f,0.5f+y*0.5f);
        glNormal3f(x,y,-cylinder_height/2.0f);
        glVertex3f(x,y,-cylinder_height/2.0f);

        glTexCoord2f(0.5f+x_back*0.5f,0.5f+y_back*0.5f);
        glNormal3f(x_back,y_back,-cylinder_height/2.0f);
        glVertex3f(x_back,y_back,-cylinder_height/2.0f);

        glTexCoord2f(0.5f+0.0f,0.5f+0.0f);
        glNormal3f(0.0f,0.0f,-cylinder_height/2.0f);
        glVertex3f(0.0f,0.0f,-cylinder_height/2.0f);
        glEnd();
        //glDisable(GL_TEXTURE_2D);
        // update coordinates
        x_back = x;
        y_back = y;

    }
    glPopMatrix();
}

void initrendering()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    t3dInit();

    Image* image = loadBMP("grass.bmp");
    _textureIdGrass = loadTexture(image);
    delete image;
    image = loadBMP("day.bmp");
    _textureIdDay = loadTexture(image);
    delete image;
    image = loadBMP("night.bmp");
    _textureIdNight = loadTexture(image);
    delete image;
    image = loadBMP("brick.bmp");
    _textureIdBrick = loadTexture(image);
    delete image;
    image = loadBMP("wall.bmp");
    _textureIdWall = loadTexture(image);
    delete image;
    image = loadBMP("tree_body.bmp");
    _textureIdTreeBody = loadTexture(image);
    delete image;
    image = loadBMP("platform.bmp");
    _textureIdPlatform = loadTexture(image);
    delete image;
    //##################################################################################################################
    //##################################################################################################################
    //##################################################################################################################
    image = loadBMP("1.bmp");
    water_tex[0] = loadTexture(image);
    delete image;
    image = loadBMP("2.bmp");
    water_tex[1] = loadTexture(image);
    delete image;
    image = loadBMP("3.bmp");
    water_tex[2] = loadTexture(image);
    delete image;
    image = loadBMP("4.bmp");
    water_tex[3] = loadTexture(image);
    delete image;
    image = loadBMP("5.bmp");
    water_tex[4] = loadTexture(image);
    delete image;
    image = loadBMP("6.bmp");
    water_tex[5] = loadTexture(image);
    delete image;
    image = loadBMP("7.bmp");
    water_tex[6] = loadTexture(image);
    delete image;
    image = loadBMP("8.bmp");
    water_tex[7] = loadTexture(image);
    delete image;
    image = loadBMP("9.bmp");
    water_tex[8] = loadTexture(image);
    delete image;
    image = loadBMP("10.bmp");
    water_tex[9] = loadTexture(image);
    delete image;
    water_tex_cnt = 10;
    current_water_tex = 0;
    water_tex_change_time = 200;
    //###############################################################################################################
    image = loadBMP("l1.bmp");
    leaf_tex[0] = loadTexture(image);
    delete image;
    image = loadBMP("l2.bmp");
    leaf_tex[1] = loadTexture(image);
    delete image;
    image = loadBMP("l3.bmp");
    leaf_tex[2] = loadTexture(image);
    delete image;
    image = loadBMP("l4.bmp");
    leaf_tex[3] = loadTexture(image);
    delete image;

    leaf_tex_cnt = 4;
    current_leaf_tex = 0;
    leaf_tex_change_time = 100;
    //###############################################################################################################
    image = loadBMP("f1.bmp");
    flag_tex[0] = loadTexture(image);
    delete image;
    image = loadBMP("f2.bmp");
    flag_tex[1] = loadTexture(image);
    delete image;
    image = loadBMP("f3.bmp");
    flag_tex[2] = loadTexture(image);
    delete image;
    image = loadBMP("f4.bmp");
    flag_tex[3] = loadTexture(image);
    delete image;
    image = loadBMP("f5.bmp");
    flag_tex[4] = loadTexture(image);
    delete image;
    image = loadBMP("f6.bmp");
    flag_tex[5] = loadTexture(image);
    delete image;
    image = loadBMP("f7.bmp");
    flag_tex[6] = loadTexture(image);
    delete image;
    image = loadBMP("f8.bmp");
    flag_tex[7] = loadTexture(image);
    delete image;
    image = loadBMP("f9.bmp");
    flag_tex[8] = loadTexture(image);
    delete image;

    flag_tex_cnt = 9;
    current_flag_tex = 0;
    flag_tex_change_time = 200;
}

void draw_flag()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, flag_tex[current_flag_tex]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glColor3f(1,1,1);

    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-2,4,0);
    glTexCoord2f(1,0);
    glVertex3f(-2,6,0);
    glTexCoord2f(1,1);
    glVertex3f(.7,6,0);
    glTexCoord2f(0,1);
    glVertex3f(.7,4,0);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(-2.25,3,0);
    glRotatef(90,1,0,0);
    draw_cylinder(.25,6.3,100);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-2.25,0,0);
    glRotatef(90,1,0,0);
    draw_cylinder(.5,1,100);
    glPopMatrix();
    glEnd();
}


void draw_fronts()
{
    glPushMatrix();
    glTranslatef(2.5,0,5);
    draw_flag();
    glPopMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdWall);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glColor3f(1,1,1);

    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-8,0,-3.6);
    glTexCoord2f(1,0);
    glVertex3f(-8,6,-3.6);
    glTexCoord2f(1,1);
    glVertex3f(-7,6,-2.9);
    glTexCoord2f(0,1);
    glVertex3f(-7,0,-2.9);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-6,0,-2.3);
    glTexCoord2f(1,0);
    glVertex3f(-6,6,-2.3);
    glTexCoord2f(1,1);
    glVertex3f(-5.5,6,-2);
    glTexCoord2f(0,1);
    glVertex3f(-5.5,0,-2);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-7,6,-2.9);
    glTexCoord2f(1,0);
    glVertex3f(-7,2.5,-2.9);
    glTexCoord2f(1,1);
    glVertex3f(-6.5,5.5,-2.5);
    glTexCoord2f(1,1);
    glVertex3f(-6,2.5,-2.3);
    glTexCoord2f(1,1);
    glVertex3f(-6,6,-2.3);
    glTexCoord2f(0,1);
    glVertex3f(-7,6,-2.9);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-7,2,-2.9);
    glTexCoord2f(1,0);
    glVertex3f(-7,2.5,-2.9);
    glTexCoord2f(1,1);
    glVertex3f(-6,2.5,-2.3);
    glTexCoord2f(0,1);
    glVertex3f(-6,2,-2.3);
    glEnd();

    // second
    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-10,0,-5);
    glTexCoord2f(1,0);
    glVertex3f(-10,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(-11,6,-5.5);
    glTexCoord2f(0,1);
    glVertex3f(-11,0,-5.5);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-9,0,-4.2);
    glTexCoord2f(1,0);
    glVertex3f(-9,6,-4.2);
    glTexCoord2f(1,1);
    glVertex3f(-8.5,6,-4);
    glTexCoord2f(0,1);
    glVertex3f(-8.5,0,-4);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-10,6,-5);
    glTexCoord2f(1,0);
    glVertex3f(-10,2.5,-5);
    glTexCoord2f(1,1);
    glVertex3f(-9.5,5.5,-4.5);
    glTexCoord2f(1,1);
    glVertex3f(-9,2.5,-4.2);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-4.2);
    glTexCoord2f(0,1);
    glVertex3f(-10,6,-5);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-10,2,-5);
    glTexCoord2f(1,0);
    glVertex3f(-10,2.5,-5);
    glTexCoord2f(1,1);
    glVertex3f(-9,2.5,-4.2);
    glTexCoord2f(0,1);
    glVertex3f(-9,2,-4.2);
    glEnd();

    // right side
    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(8,0,-3.6);
    glTexCoord2f(1,0);
    glVertex3f(8,6,-3.6);
    glTexCoord2f(1,1);
    glVertex3f(7,6,-2.9);
    glTexCoord2f(0,1);
    glVertex3f(7,0,-2.9);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(6,0,-2.3);
    glTexCoord2f(1,0);
    glVertex3f(6,6,-2.3);
    glTexCoord2f(1,1);
    glVertex3f(5.5,6,-2);
    glTexCoord2f(0,1);
    glVertex3f(5.5,0,-2);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(7,6,-2.9);
    glTexCoord2f(1,0);
    glVertex3f(7,2.5,-2.9);
    glTexCoord2f(1,1);
    glVertex3f(6.5,5.5,-2.5);
    glTexCoord2f(1,1);
    glVertex3f(6,2.5,-2.3);
    glTexCoord2f(1,1);
    glVertex3f(6,6,-2.3);
    glTexCoord2f(0,1);
    glVertex3f(7,6,-2.9);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(7,2,-2.9);
    glTexCoord2f(1,0);
    glVertex3f(7,2.5,-2.9);
    glTexCoord2f(1,1);
    glVertex3f(6,2.5,-2.3);
    glTexCoord2f(0,1);
    glVertex3f(6,2,-2.3);
    glEnd();

    // second
    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(10,0,-5);
    glTexCoord2f(1,0);
    glVertex3f(10,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(11,6,-5.5);
    glTexCoord2f(0,1);
    glVertex3f(11,0,-5.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(9,0,-4.2);
    glTexCoord2f(1,0);
    glVertex3f(9,6,-4.2);
    glTexCoord2f(1,1);
    glVertex3f(8.5,6,-4);
    glTexCoord2f(0,1);
    glVertex3f(8.5,0,-4);
    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(10,6,-5);
    glTexCoord2f(1,0);
    glVertex3f(10,2.5,-5);
    glTexCoord2f(1,1);
    glVertex3f(9.5,5.5,-4.5);
    glTexCoord2f(1,1);
    glVertex3f(9,2.5,-4.2);
    glTexCoord2f(1,1);
    glVertex3f(9,6,-4.2);
    glTexCoord2f(0,1);
    glVertex3f(10,6,-5);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(10,2,-5);
    glTexCoord2f(1,0);
    glVertex3f(10,2.5,-5);
    glTexCoord2f(1,1);
    glVertex3f(9,2.5,-4.2);
    glTexCoord2f(0,1);
    glVertex3f(9,2,-4.2);
    glEnd();

    // backside_shut -_-
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-11,0,-17);
    glTexCoord2f(1,0);
    glVertex3f(-11,6,-17);
    glTexCoord2f(1,1);
    glVertex3f(-5.5,6,-20.5);
    glTexCoord2f(0,1);
    glVertex3f(-5.5,0,-20.5);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(11,0,-17);
    glTexCoord2f(1,0);
    glVertex3f(11,6,-17);
    glTexCoord2f(1,1);
    glVertex3f(5.5,6,-20.5);
    glTexCoord2f(0,1);
    glVertex3f(5.5,0,-20.5);
    glEnd();

    // top cover
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(-11,6,-5.5);
    glTexCoord2f(1,0);
    glVertex3f(-9,6,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(-3.5,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(-5.5,6,-2);

    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(11,6,-5.5);
    glTexCoord2f(1,0);
    glVertex3f(9,6,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(3.5,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(5.5,6,-2);

    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(-11,6,-17);
    glTexCoord2f(1,0);
    glVertex3f(-5.5,6,-20.5);
    glTexCoord2f(1,1);
    glVertex3f(-3.5,6,-17.5);
    glTexCoord2f(0,1);
    glVertex3f(-9,6,-14);

    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(11,6,-17);
    glTexCoord2f(1,0);
    glVertex3f(5.5,6,-20.5);
    glTexCoord2f(1,1);
    glVertex3f(3.5,6,-17.5);
    glTexCoord2f(0,1);
    glVertex3f(9,6,-14);

    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(-3,6,1);
    glTexCoord2f(1,0);
    glVertex3f(-3,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(3,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(3,6,1);
    glEnd();

    // top cylindar
    glPushMatrix();
    glTranslatef(0,6,-11);
    //glColor3f(0,0,0);
    glRotatef(90,1,0,0);
    draw_cylinder(2,5,100);
    glPopMatrix();

    glColor3f(0,0,0);
    glPushMatrix();
    glTranslatef(0,6,-11);
    //glColor3f(0,0,0);
    glRotatef(90,1,0,0);
    draw_cylinder(6,1,100);
    glPopMatrix();

    // upper boxes
    glColor3f(.78,.78,.78);
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-6);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-6);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-6);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-6);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-8);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-8);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-8);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-8);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-6);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-6);
    glTexCoord2f(1,1);
    glVertex3f(-1,8,-8);
    glTexCoord2f(0,1);
    glVertex3f(-1,6,-8);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(1,6,-6);
    glTexCoord2f(1,0);
    glVertex3f(1,8,-6);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-8);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-8);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-14);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-14);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-14);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-14);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-16);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-16);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-16);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-16);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-14);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-14);
    glTexCoord2f(1,1);
    glVertex3f(-1,8,-16);
    glTexCoord2f(0,1);
    glVertex3f(-1,6,-16);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(1,6,-14);
    glTexCoord2f(1,0);
    glVertex3f(1,8,-14);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-16);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-16);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-5,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(-5,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-10);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-10);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-5,6,-12);
    glTexCoord2f(1,0);
    glVertex3f(-5,8,-12);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-12);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-12);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-5,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(-5,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(-5,8,-12);
    glTexCoord2f(0,1);
    glVertex3f(-5,6,-12);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-3,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(-3,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-12);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-12);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(5,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(5,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-10);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-10);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(5,6,-12);
    glTexCoord2f(1,0);
    glVertex3f(5,8,-12);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-12);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-12);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(5,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(5,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(5,8,-12);
    glTexCoord2f(0,1);
    glVertex3f(5,6,-12);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(3,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(3,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-12);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-12);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-4.5,6,-8.5);
    glTexCoord2f(1,0);
    glVertex3f(-4.5,8,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-6.5);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-6.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-4.5,6,-8.5);
    glTexCoord2f(1,0);
    glVertex3f(-4.5,8,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-10);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-10);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-3,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(-3,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(-1,8,-8);
    glTexCoord2f(0,1);
    glVertex3f(-1,6,-8);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-8);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-8);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-6.5);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-6.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(4.5,6,-8.5);
    glTexCoord2f(1,0);
    glVertex3f(4.5,8,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-6.5);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-6.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(4.5,6,-8.5);
    glTexCoord2f(1,0);
    glVertex3f(4.5,8,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-10);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-10);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(3,6,-10);
    glTexCoord2f(1,0);
    glVertex3f(3,8,-10);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-8);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-8);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(1,6,-8);
    glTexCoord2f(1,0);
    glVertex3f(1,8,-8);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-6.5);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-6.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-3,6,-12);
    glTexCoord2f(1,0);
    glVertex3f(-3,8,-12);
    glTexCoord2f(1,1);
    glVertex3f(-4.5,8,-13.5);
    glTexCoord2f(0,1);
    glVertex3f(-4.5,6,-13.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-4.5,6,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(-4.5,8,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-15);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-15);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-3,6,-12);
    glTexCoord2f(1,0);
    glVertex3f(-3,8,-12);
    glTexCoord2f(1,1);
    glVertex3f(-1,8,-13.5);
    glTexCoord2f(0,1);
    glVertex3f(-1,6,-13.5);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-1,6,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(-1,8,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(-3,8,-15);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,-15);


    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(3,6,-12);
    glTexCoord2f(1,0);
    glVertex3f(3,8,-12);
    glTexCoord2f(1,1);
    glVertex3f(4.5,8,-13.5);
    glTexCoord2f(0,1);
    glVertex3f(4.5,6,-13.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(4.5,6,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(4.5,8,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-15);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-15);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(3,6,-12);
    glTexCoord2f(1,0);
    glVertex3f(3,8,-12);
    glTexCoord2f(1,1);
    glVertex3f(1,8,-13.5);
    glTexCoord2f(0,1);
    glVertex3f(1,6,-13.5);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(1,6,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(1,8,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(3,8,-15);
    glTexCoord2f(0,1);
    glVertex3f(3,6,-15);

    glEnd();
}

void draw_box_shape()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdWall);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPushMatrix();
    glBegin(GL_QUADS);
    glColor3f(1,1,1);  // all white
    glNormal3f(0,0,-1); //front
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-3,0,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-3,4,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(3,4,1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(3,0,1);

    glNormal3f(0,0,-1); //back
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-3,0,-5);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-3,4,-5);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(3,4,-5);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(3,0,-5);

    glNormal3f(1,0,0); //left
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-3,0,-5);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-3,4,-5);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(-3,4,1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-3,0,1);


    glNormal3f(-1,0,0); // right
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(3,0,-5);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(3,4,-5);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(3,4,1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(3,0,1);

    glNormal3f(-1,0,0); // top
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-3,4,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(3,4,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(3,4,-5);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-3,4,-5);

    glEnd();
    glPopMatrix();

    if(light_on == 1)
        glColor3f(1,1,0);
    else
        glColor3f(1,1,1);
    glPushMatrix();
    glTranslated(0,5,-2);
    glutSolidSphere(1,10,10); // *** ???
    glPopMatrix();
}


void draw_tree(int base,int height)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdTreeBody);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glColor3f(1,1,1);

    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-base/2,0,base/2);
    glTexCoord2f(1,0);
    glVertex3f(-base/2,height-3.5,base/2);
    glTexCoord2f(1,1);
    glVertex3f(base/2,height-3.5,base/2);
    glTexCoord2f(0,1);
    glVertex3f(base/2,0,base/2);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-base/2,0,base/2);
    glTexCoord2f(1,0);
    glVertex3f(-base/2,height-3.5,base/2);
    glTexCoord2f(1,1);
    glVertex3f(-base/2,height-3.5,-base/2);
    glTexCoord2f(0,1);
    glVertex3f(-base/2,0,-base/2);


    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(base/2,0,base/2);
    glTexCoord2f(1,0);
    glVertex3f(base/2,height-3.5,base/2);
    glTexCoord2f(1,1);
    glVertex3f(base/2,height-3.5,-base/2);
    glTexCoord2f(0,1);
    glVertex3f(base/2,0,-base/2);


    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-base/2,0,-base/2);
    glTexCoord2f(1,0);
    glVertex3f(-base/2,height-3.5,-base/2);
    glTexCoord2f(1,1);
    glVertex3f(base/2,height-3.5,-base/2);
    glTexCoord2f(0,1);
    glVertex3f(base/2,0,-base/2);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, leaf_tex[current_leaf_tex]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glColor3f(1,1,1);


    glBegin(GL_TRIANGLES);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(0,height,0);
    glTexCoord2f(1,1);
    glVertex3f(-1.5*base,height-(height-3),1.5*base);
    glTexCoord2f(0,1);
    glVertex3f(1.5*base,height-(height-3),1.5*base);


    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(0,height,0);
    glTexCoord2f(1,1);
    glVertex3f(-1.5*base,height-(height-3),-1.5*base);
    glTexCoord2f(0,1);
    glVertex3f(1.5*base,height-(height-3),-1.5*base);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(0,height,0);
    glTexCoord2f(1,1);
    glVertex3f(-1.5*base,height-(height-3),-1.5*base);
    glTexCoord2f(0,1);
    glVertex3f(-1.5*base,height-(height-3),1.5*base);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(0,height,0);
    glTexCoord2f(1,1);
    glVertex3f(1.5*base,height-(height-3),-1.5*base);
    glTexCoord2f(0,1);
    glVertex3f(1.5*base,height-(height-3),1.5*base);
    glEnd();
    glDisable(GL_TEXTURE_2D);

}


void water_divider()
{
    // draw trees

    glPushMatrix();
    glTranslatef(-31,0,0);
    draw_tree(2,8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(31,0,0);
    draw_tree(2,12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(31,0,-41);
    draw_tree(2,12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-31,0,-41);
    draw_tree(3,15);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-3,0,-43);
    draw_tree(5,12);
    glPopMatrix();

    // stairs #########################################################
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdPlatform);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,1.5,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-7,1.5,2.1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,1.5,2.1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(7,1.5,1);

    glNormal3f(0,0,-1);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,1.5,2.1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(7,1.5,2.1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,1,2.1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-7,1,2.1);

    glNormal3f(0,1,0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,1,2.1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-7,1,2.5);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,1,2.5);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(7,1,2.1);

    glNormal3f(0,0,-1);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,1,2.5);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(7,1,2.5);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,.8,2.5);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-7,.8,2.5);

    glNormal3f(0,1,0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,.8,2.5);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-7,.8,2.8);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,.8,2.8);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(7,.8,2.5);

    glNormal3f(0,0,-1);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,.8,2.8);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(7,.8,2.8);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,.5,2.8);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-7,.5,2.8);

    glNormal3f(0,1,0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,.5,2.8);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-7,.5,3.1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,.5,3.1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(7,.5,2.8);

    glNormal3f(0,0,-1);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,.5,3.1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(7,.5,3.1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,0,3.1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-7,0,3.1);

    glNormal3f(0,1,0); //platform
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-7,.25,3.1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(7,.25,3.1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(7,.25,8.1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-7,.25,8.1);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    //###########################################################################################################
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdBrick);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1); // front left side
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-32,0,2);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-3,0,2);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(-3,1.5,2);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-32,1.5,2);

    glNormal3f(0,0,-1); // front right side
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(32,0,2);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(3,0,2);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(3,1.5,2);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(32,1.5,2);

    glNormal3f(0,0,-1); // front left side
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-30,0,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-3,0,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(-3,1.5,1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-30,1.5,1);

    glNormal3f(0,0,-1); // front right side
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(30,0,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(3,0,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(3,1.5,1);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(30,1.5,1);

    glNormal3f(1,0,0); //
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-30,0,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-30,1.5,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(-30,1.5,-42);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-30,0,-42);

    glNormal3f(1,0,0); //
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-32,0,2);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-32,1.5,2);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(-32,1.5,-42);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-32,0,-42);

    glNormal3f(-1,0,0); //
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(30,0,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(30,1.5,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(30,1.5,-42);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(30,0,-42);

    glNormal3f(-1,0,0); //
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(32,0,2);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(32,1.5,2);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(32,1.5,-42);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(32,0,-42);

    glNormal3f(0,0,-1); // back
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-32,0,-42);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-32,1.5,-42);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(32,1.5,-42);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(32,0,-42);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void draw_house()
{
    water_divider();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdGrass);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPushMatrix();
    glBegin(GL_QUADS);
    glColor3f(.4,.7,0);  // all bottom green
    glNormal3f(0,1,0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-100000,0,100000);
    glTexCoord2f(10000.0f,0.0f);
    glVertex3f(100000,0,100000);
    glTexCoord2f(10000.0f,10000.0f);
    glVertex3f(100000,0,-100000);
    glTexCoord2f(0.0f,10000.0f);
    glVertex3f(-100000,0,-100000);
    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //##################################################################################################################
    //##################################################################################################################
    //##################################################################################################################
    glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, _textureIdwater2);
    glBindTexture(GL_TEXTURE_2D, water_tex[current_water_tex]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    glPushMatrix();
    glBegin(GL_QUADS);
    glColor3f(1,1,1);  // all bottom white
    glNormal3f(0,1,0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-30,.1,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(30,.1,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(30,.1,-40);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-30,.1,-40);

    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,_textureIdWall);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glColor3f(1,1,1); // all gray
    // upper all

    // lower water remover
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(-11,0.2,-5.5);
    glTexCoord2f(0,0);
    glVertex3f(-9,0.2,-8.5);
    glTexCoord2f(0,0);
    glVertex3f(-3.5,0.2,-5);
    glTexCoord2f(0,0);
    glVertex3f(-5.5,0.2,-2);

    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(11,0.2,-5.5);
    glTexCoord2f(0,0);
    glVertex3f(9,0.2,-8.5);
    glTexCoord2f(0,0);
    glVertex3f(3.5,0.2,-5);
    glTexCoord2f(0,0);
    glVertex3f(5.5,0.2,-2);

    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(-3,0.2,-5);
    glTexCoord2f(0,0);
    glVertex3f(3,0.2,-5);
    glTexCoord2f(0,0);
    glVertex3f(3,0.2,1);
    glTexCoord2f(0,0);
    glVertex3f(-3,0.2,1);

    glEnd();

    glNormal3f(0,1,0);
    glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
    glVertex3f(-9,6,-14);
    glTexCoord2f(1,0);
    glVertex3f(-9,6,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(-3.5,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(3.5,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(9,6,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(9,6,-14);
    glTexCoord2f(1,1);
    glVertex3f(3.5,6,-17.5);
    glTexCoord2f(1,1);
    glVertex3f(-3.5,6,-17.5);
    glTexCoord2f(0,1);
    glVertex3f(-9,6,-14);


    glEnd();

    glBegin(GL_POLYGON);
    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(-13,6,-13);
    glTexCoord2f(1,0);
    glVertex3f(-10,6,-13);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-12);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-10);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-10);
    glTexCoord2f(1,1);
    glVertex3f(-10,6,-9.5);
    glTexCoord2f(1,1);
    glVertex3f(-13,6,-9.5);
    glTexCoord2f(0,1);
    glVertex3f(-13,6,-13);
    glEnd();

    glBegin(GL_QUADS);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-3,0,1);
    glTexCoord2f(1,0);
    glVertex3f(-3,6,1);
    glTexCoord2f(1,1);
    glVertex3f(-3,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(-3,0,-5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(3,0,1);
    glTexCoord2f(1,0);
    glVertex3f(3,0,-5);
    glTexCoord2f(1,1);
    glVertex3f(3,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(3,6,1);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-9,0,-14);
    glTexCoord2f(1,0);
    glVertex3f(-9,6,-14);
    glTexCoord2f(1,1);
    glVertex3f(-11,6,-17);
    glTexCoord2f(0,1);
    glVertex3f(-11,0,-17);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-6.5,0,-15.6);
    glTexCoord2f(1,0);
    glVertex3f(-6.5,6,-15.6);
    glTexCoord2f(1,1);
    glVertex3f(-8.5,6,-18);
    glTexCoord2f(0,1);
    glVertex3f(-8.5,0,-18);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-6,0,-16);
    glTexCoord2f(1,0);
    glVertex3f(-6,6,-16);
    glTexCoord2f(1,1);
    glVertex3f(-8,6,-18.2);
    glTexCoord2f(0,1);
    glVertex3f(-8,0,-18.2);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-3.5,0,-17.5);
    glTexCoord2f(1,0);
    glVertex3f(-3.5,6,-17.5);
    glTexCoord2f(1,1);
    glVertex3f(-5.5,6,-20.5);
    glTexCoord2f(0,1);
    glVertex3f(-5.5,0,-20.5);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-9,0,-8.5);
    glTexCoord2f(1,0);
    glVertex3f(-9,6,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-10);
    glTexCoord2f(0,1);
    glVertex3f(-9,0,-10);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(9,0,-8.5);
    glTexCoord2f(1,0);
    glVertex3f(9,6,-8.5);
    glTexCoord2f(1,1);
    glVertex3f(9,6,-10);
    glTexCoord2f(0,1);
    glVertex3f(9,0,-10);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-9,0,-14);
    glTexCoord2f(1,0);
    glVertex3f(-9,6,-14);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-10);
    glTexCoord2f(0,1);
    glVertex3f(-9,0,-10);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(9,0,-14);
    glTexCoord2f(1,0);
    glVertex3f(9,6,-14);
    glTexCoord2f(1,1);
    glVertex3f(9,6,-10);
    glTexCoord2f(0,1);
    glVertex3f(9,0,-10);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(8,0,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(8,6,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(8,6,-9);
    glTexCoord2f(0,1);
    glVertex3f(8,0,-9);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-5.5,0,-2);
    glTexCoord2f(1,0);
    glVertex3f(-5.5,6,-2);
    glTexCoord2f(1,1);
    glVertex3f(-3.5,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(-3.5,0,-5);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-11,0,-5.5);
    glTexCoord2f(1,0);
    glVertex3f(-11,6,-5.5);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-8.5);
    glTexCoord2f(0,1);
    glVertex3f(-9,0,-8.5);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(5.5,0,-2);
    glTexCoord2f(1,0);
    glVertex3f(5.5,6,-2);
    glTexCoord2f(1,1);
    glVertex3f(3.5,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(3.5,0,-5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(11,0,-5.5);
    glTexCoord2f(1,0);
    glVertex3f(11,6,-5.5);
    glTexCoord2f(1,1);
    glVertex3f(9,6,-8.5);
    glTexCoord2f(0,1);
    glVertex3f(9,0,-8.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(9,0,-14);
    glTexCoord2f(1,0);
    glVertex3f(9,6,-14);
    glTexCoord2f(1,1);
    glVertex3f(11,6,-17);
    glTexCoord2f(0,1);
    glVertex3f(11,0,-17);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(6.5,0,-15.6);
    glTexCoord2f(1,0);
    glVertex3f(6.5,6,-15.6);
    glTexCoord2f(1,1);
    glVertex3f(8.5,6,-18);
    glTexCoord2f(0,1);
    glVertex3f(8.5,0,-18);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(6,0,-16);
    glTexCoord2f(1,0);
    glVertex3f(6,6,-16);
    glTexCoord2f(1,1);
    glVertex3f(8,6,-18.2);
    glTexCoord2f(0,1);
    glVertex3f(8,0,-18.2);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(3.5,0,-17.5);
    glTexCoord2f(1,0);
    glVertex3f(3.5,6,-17.5);
    glTexCoord2f(1,1);
    glVertex3f(5.5,6,-20.5);
    glTexCoord2f(0,1);
    glVertex3f(5.5,0,-20.5);

    glNormal3f(0,0,1);
    glTexCoord2f(0,0);
    glVertex3f(8,0,-9);
    glTexCoord2f(1,0);
    glVertex3f(8,6,-9);
    glTexCoord2f(1,1);
    glVertex3f(11,6,-9);
    glTexCoord2f(0,1);
    glVertex3f(11,0,-9);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(8,0,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(8,6,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(11,6,-13.5);
    glTexCoord2f(0,1);
    glVertex3f(11,0,-13.5);

    glNormal3f(0,0,1);
    glTexCoord2f(0,0);
    glVertex3f(-13,0,-9.5);
    glTexCoord2f(1,0);
    glVertex3f(-13,6,-9.5);
    glTexCoord2f(1,1);
    glVertex3f(-9.5,6,-10);
    glTexCoord2f(0,1);
    glVertex3f(-9.5,0,-10);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-13,0,-13);
    glTexCoord2f(1,0);
    glVertex3f(-13,6,-13);
    glTexCoord2f(1,1);
    glVertex3f(-9.5,6,-12.5);
    glTexCoord2f(0,1);
    glVertex3f(-9.5,0,-12.5);
    //glColor3f(0,0,0);  // all back sides
    glColor3f(0,0,0);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-3,0,-5);
    glTexCoord2f(1,0);
    glVertex3f(-3,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(3,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(3,0,-5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(8,0,-3.6);
    glTexCoord2f(1,0);
    glVertex3f(8,6,-3.6);
    glTexCoord2f(1,1);
    glVertex3f(6,6,-6.5);
    glTexCoord2f(0,1);
    glVertex3f(6,0,-6.5);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(8.5,0,-4);
    glTexCoord2f(1,0);
    glVertex3f(8.5,6,-4);
    glTexCoord2f(1,1);
    glVertex3f(6.5,6,-6.9);
    glTexCoord2f(0,1);
    glVertex3f(6.5,0,-6.9);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-8,0,-3.6);
    glTexCoord2f(1,0);
    glVertex3f(-8,6,-3.6);
    glTexCoord2f(1,1);
    glVertex3f(-6,6,-6.5);
    glTexCoord2f(0,1);
    glVertex3f(-6,0,-6.5);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-8.5,0,-4);
    glTexCoord2f(1,0);
    glVertex3f(-8.5,6,-4);
    glTexCoord2f(1,1);
    glVertex3f(-6.5,6,-6.9);
    glTexCoord2f(0,1);
    glVertex3f(-6.5,0,-6.9);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-3.5,0,-5);
    glTexCoord2f(1,0);
    glVertex3f(-3.5,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(-9,6,-8.5);
    glTexCoord2f(0,1);
    glVertex3f(-9,0,-8.5);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(3.5,0,-5);
    glTexCoord2f(1,0);
    glVertex3f(3.5,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(9,6,-8.5);
    glTexCoord2f(0,1);
    glVertex3f(9,0,-8.5);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-9,0,-14);
    glTexCoord2f(1,0);
    glVertex3f(-9,6,-14);
    glTexCoord2f(1,1);
    glVertex3f(-3.5,6,-17.5);
    glTexCoord2f(0,1);
    glVertex3f(-3.5,0,-17.5);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(9,0,-14);
    glTexCoord2f(1,0);
    glVertex3f(9,6,-14);
    glTexCoord2f(1,1);
    glVertex3f(3.5,6,-17.5);
    glTexCoord2f(0,1);
    glVertex3f(3.5,0,-17.5);

    glNormal3f(0,0,1);
    glTexCoord2f(0,0);
    glVertex3f(-3.5,0,-5);
    glTexCoord2f(1,0);
    glVertex3f(-3.5,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(-3,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(-3,0,-5);

    glNormal3f(0,0,1);
    glTexCoord2f(0,0);
    glVertex3f(3.5,0,-5);
    glTexCoord2f(1,0);
    glVertex3f(3.5,6,-5);
    glTexCoord2f(1,1);
    glVertex3f(3,6,-5);
    glTexCoord2f(0,1);
    glVertex3f(3,0,-5);

    glColor3f(1,1,1);
    //glColor3f(0,1,0);  // all tops


    glNormal3f(0,1,0);
    glTexCoord2f(0,0);
    glVertex3f(8,6,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(11,6,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(11,6,-9);
    glTexCoord2f(0,1);
    glVertex3f(8,6,-9);
    //glColor3f(0,0,1); // all fronts
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(-3,0,1);
    glTexCoord2f(1,0);
    glVertex3f(-.5,0,1);
    glTexCoord2f(1,1);
    glVertex3f(-.5,6,1);
    glTexCoord2f(0,1);
    glVertex3f(-3,6,1);

    glNormal3f(0,0,-1);
    glTexCoord2f(0,0);
    glVertex3f(3,0,1);
    glTexCoord2f(1,0);
    glVertex3f(.5,0,1);
    glTexCoord2f(1,1);
    glVertex3f(.5,6,1);
    glTexCoord2f(0,1);
    glVertex3f(3,6,1);

    glNormal3f(-1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(11,0,-13.5);
    glTexCoord2f(1,0);
    glVertex3f(11,6,-13.5);
    glTexCoord2f(1,1);
    glVertex3f(11,6,-9);
    glTexCoord2f(0,1);
    glVertex3f(11,0,-9);

    glNormal3f(1,0,0);
    glTexCoord2f(0,0);
    glVertex3f(-13,0,-13);
    glTexCoord2f(1,0);
    glVertex3f(-13,6,-13);
    glTexCoord2f(1,1);
    glVertex3f(-13,6,-9.5);
    glTexCoord2f(0,1);
    glVertex3f(-13,0,-9.5);

    glEnd();

    // cylindar
    glPushMatrix();
    glTranslatef(0,3,-19.5);
    glRotatef(90,1,0,0);
    draw_cylinder(4,6,100);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(11,3,-11.2);
    glRotatef(90,1,0,0);
    draw_cylinder(1.5,6,100);
    glPopMatrix();
    glEnd();

    // outside


    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _textureIdBrick);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPushMatrix();
    glBegin(GL_QUADS);
    glColor3f(1,1,1);  // all bottom red
    glNormal3f(0,1,0);
    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-50,0.2,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(50,0.2,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(50,0.2,20);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-50,0.2,20);

    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-50,0.2,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(-30,0.2,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(-30,0.2,-40);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-50,0.2,-40);


    glTexCoord2f(0.0f,0.0f);
    glVertex3f(50,0.2,1);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(30,0.2,1);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(30,0.2,-40);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(50,0.2,-40);

    glTexCoord2f(0.0f,0.0f);
    glVertex3f(-50,0.2,-40);
    glTexCoord2f(1.0f,0.0f);
    glVertex3f(50,0.2,-40);
    glTexCoord2f(1.0f,1.0f);
    glVertex3f(50,0.2,-60);
    glTexCoord2f(0.0f,1.0f);
    glVertex3f(-50,0.2,-60);

    // ailes
    //aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    draw_fronts();

    glPushMatrix(); // left front
    glTranslatef(-47,0,18);
    draw_box_shape();
    glPopMatrix();

    glPushMatrix(); //right front
    glTranslatef(47,0,18);
    draw_box_shape();
    glPopMatrix();

    glPushMatrix(); // left back
    glTranslatef(-47,0,-48);
    draw_box_shape();
    glPopMatrix();

    glPushMatrix(); //right back
    glTranslatef(47,0,-48);
    draw_box_shape();
    glPopMatrix();

}

GLfloat trans[3] = {0.1,0.1,0.1};
GLfloat viewangle = 0,tipangle = 5;
float i1=.5,i2=.5,i3=.5;
float view_x=0,view_z=-5;


float computeScale(const char* strs[4])
{
    float maxWidth = 0;
    for(int i = 0; i < 4; i++)
    {
        float width = t3dDrawWidth(strs[i]);
        if (width > maxWidth)
        {
            maxWidth = width;
        }
    }

    return 2.6f / maxWidth;
}

float _scale;
const char* STRS[4] = {"National", "Parliament","of", "Bangladesh"};

void cleanup()
{
    t3dCleanup();
}
int text_timer=0;

void draw_text()
{
    glScalef(_scale,_scale,_scale);
    glTranslatef(-4,5,2);
    t3dDraw3D("National",-3,1,1.5f);
    glTranslatef(0,-1,0);
    t3dDraw3D("Parliament",-3,1,1.5f);
    glTranslatef(0,-1,0);
    t3dDraw3D("of",-3,1,1.5f);
    glTranslatef(0,-1,0);
    t3dDraw3D("Bangladesh",-3,1,1.5f);

}

void drawscene()
{
    GLUquadric *quadric;
    quadric = gluNewQuadric();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear before draw somethings
    glClearColor(R,G,B,1); // for background -- day night
    glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
    glLoadIdentity();

    glTranslatef(.5,0,-10);

    glPushMatrix();
    if(text_timer==1)
        draw_text();
    glPopMatrix();

    GLfloat ambientColor[] = {i1,i2,i3,1};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

    // Light 0
    GLfloat lightColor0[] = {posL1, posL2, posL3, 1.0f};
    GLfloat lightPos0[] = {0.0f, 8.0f, -4.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    if(fogflag==1)
    {
        GLfloat fogcolour[4]= {1.0,1.0,1.0,1.0};

        glFogfv(GL_FOG_COLOR,fogcolour);              /* Define the fog colour */
        glFogf(GL_FOG_DENSITY,0.08);                   /* How dense */
        glFogi(GL_FOG_MODE,GL_EXP);                   /* exponential decay */
        glFogf(GL_FOG_START,3.0);                   /* Where wwe start fogging */
        glFogf(GL_FOG_END,100.0);                       /* end */
        glHint(GL_FOG_HINT, GL_FASTEST);              /* compute per vertex */
        glEnable(GL_FOG);/* ENABLE */
    }
    if(fogflag==0)
    {
        glDisable(GL_FOG);
    }

    glRotatef (tipangle, 1,0,0);
    //glRotatef (viewangle, 0,1,0);
    glTranslatef(view_x,0,view_z);
    glRotatef(Rangle,0,1,0);

    glPushMatrix();
    glRotatef(0.0,0.0,0.0,1.0); // orbits the planet around the sun
    glTranslatef(1.40,0.0,0.0);        // sets the radius of the orbit
    glRotatef(skyAngle,0.0,1.0,1.0); // revolves the planet on its axis
    glColor3f(4.0, 4.0, 4.0);          // green

    glEnable(GL_TEXTURE_2D);
    if (night == 1)
    {
        glBindTexture(GL_TEXTURE_2D, _textureIdNight);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _textureIdDay);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluQuadricTexture(quadric, 20);
    //glutWireSphere(0.16,20,20);        // Earth
    gluSphere(quadric,75,40,40);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(trans[0],trans[1],trans[2]);
    glScalef(0.2,0.2,0.2);
    //glRotatef(-90,1,0,0);
    //draw_cylinder(3,6,100);
    draw_house();
    //draw_pillar();
    glPopMatrix();
    glEnd();

    if(night==0 && sun_moon_flag == 1)
    {
        ///sun
        glPushMatrix();
        glColor3f(1,1,.5);
        glTranslated(4.97,4.55,70.49);
        glutSolidSphere(0.5,10,10);
        glPopMatrix();
    }
    if(night==1 && sun_moon_flag == 1)
    {
        ///moon
        glPushMatrix();
        glColor3f(1,1,1);
        glTranslated(-4.97,4.55,70.49);
        glutSolidSphere(0.5,10,10);
        glPopMatrix();
    }
    glutSwapBuffers();
}
int light = 1;
void my_keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 't':
        if(text_timer==0)text_timer=1;
        else text_timer=0;
        break;
    case '1':
        if(light_on == 1)
            light_on = 0;
        else
            light_on = 1;
        break;
    case 'D':
    case 'd':
        tipangle-=5;
        if(tipangle<5)
            tipangle=5;
        break;
    case 'u':
    case 'U':
        tipangle+=5;
        if(tipangle>90)
            tipangle=90;
        break;
    case 'i':
        i1+=.25,i2+=.25,i3+=.25;
        if(i1>=1)
            i1=.2,i2=.2,i3=.2;
        break;
    case 'l':
    case 'L':
        if(light == 1)
            light = 0,glDisable(GL_LIGHTING);
        else
            light = 1, glEnable(GL_LIGHTING);
        break;
    case 'r':
    case 'R':
        Rangle += 2.5f;
        if (Rangle > 360)
        {
            Rangle -= 360;
        }
        break;
    case 'e':
    case 'E':
        Rangle -= 2.5f;
        if (Rangle < 360)
        {
            Rangle += 360;
        }
        break;
    case 'n':
    case 'N':
        if(night == 0)
        {
            R=0.0,G=0.0,B=0.0,posL1=0.2f,posL2=0.2f,posL3=0.2f,night=1;
        }
        else
        {
            R=0.0,G=0.4,B=1.0,posL1=0.8f,posL2=0.8f,posL3=0.8f,night=0;
        }
        break;
    case 'x' :
        trans[2] -= 0.1;
        break;
    case 'z' :
        trans[2] += 0.1;
        break;
    case 'a' :
        trans[0] = 0.1,trans[1] = 0.1,trans[2] = 0.1;
        viewangle = 0,tipangle = 5;
        view_x=0,view_z=-5;
        break;
    case 's':
    case 'S':
    case 'm':
    case 'M':
        if (sun_moon_flag == 0)
            sun_moon_flag = 1;
        else
            sun_moon_flag = 0;
        break;
    }
    glutPostRedisplay();
}

void my_mouse(int button, int state, int x, int y)
{
    switch (button)
    {
    case GLUT_RIGHT_BUTTON:
        fogflag=1;
        break;
    case GLUT_LEFT_BUTTON:
        fogflag=0;
        break;
    }
}

void special_key(int key,int x,int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT :
        view_x += .3;
        break;
    case GLUT_KEY_RIGHT:
        view_x -= .3;
        break;
    case GLUT_KEY_UP   :
        view_z += .3;
        break;
    case GLUT_KEY_DOWN :
        view_z -= .3;
        break;
    }
    glutPostRedisplay();
}

// --------------------------------------------------------------------------------------------------------------------------------
void update(int val)
{
    current_water_tex++;
    if(current_water_tex >= water_tex_cnt)
    {
        current_water_tex = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(water_tex_change_time,update,0);
}

void update2(int val)
{
    current_leaf_tex++;
    if(current_leaf_tex >= leaf_tex_cnt)
    {
        current_leaf_tex = 0;
    }

    glutPostRedisplay();
    glutTimerFunc(leaf_tex_change_time,update2,0);
}


void update3(int val)
{
    current_flag_tex++;
    if(current_flag_tex >= flag_tex_cnt)
    {
        current_flag_tex = 0;
    }
    glutPostRedisplay();
    glutTimerFunc(flag_tex_change_time,update3,0);
}

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(900,600);
    glutInitWindowPosition(100,200);

    glutCreateWindow("National Parliament Of Bangladesh");

    initrendering();
    _scale = computeScale(STRS);
    glutDisplayFunc(drawscene);
    glutKeyboardFunc(my_keyboard);
    glutMouseFunc(my_mouse);
    glutSpecialFunc(special_key);

    R = 0.0,G = 0.4,B = 1.0;
    glClearColor (0.1, 0.4, 0.6, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60,1.5,1,100);
    glMatrixMode(GL_MODELVIEW);
    glutTimerFunc(water_tex_change_time,update,0);
    glutTimerFunc(leaf_tex_change_time,update2,0);
    glutTimerFunc(flag_tex_change_time,update3,0);
    //glutPostRedisplay();
    glutMainLoop();

    return 0;
}
