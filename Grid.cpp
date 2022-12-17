#include <FL/gl.h>
#include <FL/glut.h>
#include "Grid.h"
#include "iostream"

Grid::Grid() {}

Grid::~Grid() {}

void Grid::update() {
	//std::cout << " render grid " << std::endl;
	if (speed >= 2.0f) speed = 1.0f;
	speed *= 1.001;
}

void Grid::render() {
	draw_grid(speed);
}


void Grid::draw_rec(float size) {
	// glDisable(GL_LIGHTING);
	if (size >= 0.2) {

		glPushMatrix();

			glBegin(GL_LINES);
			glColor4f(0.95f, 0.95f, 0.98f, 0.8f);

			float fix = 0.2;
			float x = 0.7f * fix;
			float y = 0.7f * fix;
			float z = 0.2f * fix; 
			float distance = 30.0f;
			float xd, yd, zd;

			// fixed
			if (size != 1) {
				glVertex3f(-x, y, z); 
				glVertex3f(x, y, z); 

				glVertex3f(x, y, z); 
				glVertex3f(x, -y, -z); 

				glVertex3f(x, -y, -z); 
				glVertex3f(-x, -y, -z); 

				glVertex3f(-x, -y, -z); 
				glVertex3f(-x, y, z); 
			}

			// lines
			xd = x * distance; 
			yd = y * distance; 
			zd = z * distance;
			
			glVertex3f(-x, y, z); 
			glVertex3f(-xd, yd, zd); 

			glVertex3f(x, y, z); 
			glVertex3f(xd, yd, zd); 

			glVertex3f(x, -y, -z); 
			glVertex3f(xd, -yd, -zd); 

			glVertex3f(-x, -y, -z); 
			glVertex3f(-xd, -yd, -zd); 

			// moving
			x = 0.7f * size;
			y = 0.7f * size;
			z = 0.2f * size;

			// rectangles
			glVertex3f(-x, y, z); 
			glVertex3f(x, y, z); 

			glVertex3f(x, y, z); 
			glVertex3f(x, -y, -z); 

			glVertex3f(x, -y, -z); 
			glVertex3f(-x, -y, -z); 

			glVertex3f(-x, -y, -z); 
			glVertex3f(-x, y, z); 

			glEnd();

		glPopMatrix();
	}
	// glEnable(GL_LIGHTING);
}

void Grid::draw_grid(float speed) { 
	draw_rec(0.1f * speed);
	draw_rec(0.15f * speed);
	draw_rec(0.2f * speed);
	draw_rec(0.3f * speed);
	draw_rec(0.4f * speed);
	draw_rec(0.6f * speed);
	draw_rec(0.8f * speed);
	draw_rec(1.2f * speed);
	draw_rec(1.6f * speed);
	draw_rec(2.0f * speed);
	draw_rec(2.8f * speed);
	draw_rec(3.6f * speed);
}
