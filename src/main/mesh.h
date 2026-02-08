// Loads and manages meshes
// Created by James Schaffer on 28/01/2026.

#ifndef CUBERENDER_MESHLOADER_H
#define CUBERENDER_MESHLOADER_H

// Allows for fopen() which is considered deprecated by MSVC
#define _CRT_SECURE_NO_DEPRECATE

#define RESOURCES_MESHES_DIR "resources/meshes/"
#define MESHLOADER_LINEBUFFER_SIZE 512

#include <SDL3/SDL_pixels.h>
#include "vector.h"

typedef struct {
	int v0, v1, v2; // vertex array index
	int n0; // normal array index
	//int t0, t1, t2; // texcoord indices
} Tri;

typedef struct  {
	v3* vertices;
	v3* normals;
	Tri* faces;

	SDL_FColor color;

	size_t vertexCount, normalCount, faceCount;
} Mesh;

Mesh newMesh();
void freeMesh(Mesh* mesh);

Mesh* loadMeshFromOBJ(const char* fileName, int* meshCount);

#endif //CUBERENDER_MESHLOADER_H