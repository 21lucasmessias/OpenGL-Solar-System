#ifndef OBJECTS_H
#define OBJECTS_H

#include "main.hpp"

#define INDEX_SPHERE 0
#define INDEX_RING 1
#define INDEX_PIRAMID 2
#define INDEX_CUBE 3

using namespace std;
using namespace glm;

struct ObjectProperties {
	vector<GLfloat> object;
	vector<GLushort> indexs;
};

struct RotationObject {
	vec3 vec;
	GLfloat radians;
};

struct ObjectsToRender {
	GLushort index;
	vec3 translation;
	RotationObject rotation;
	vec3 scaling;
	GLushort index_texture;
};

extern vector<ObjectProperties> objects;
extern vector<ObjectsToRender> objectsToRender;

#endif