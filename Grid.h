#ifndef EYE_H
#define EYE_H

class Grid{
	public:
		float speed = 1.0f;
		float deltaSpeed = 0.001f;
		Grid();
		~Grid();
		void update();
		void render();
		void draw_grid(float speed);

private:
	void draw_circle(float radius);
	void draw_rec(float size);
};

#endif