// Loads and manages meshes
// Created by James Schaffer on 28/01/2026.

#include "mesh.h"

#include <signal.h>
#include <stdio.h>

void freeMesh(Mesh* mesh) {
	if (!mesh) return;

	puts("freeing mesh");

	free(mesh->vertices);
	free(mesh->faces);
	free(mesh->normals);

	mesh->vertices = NULL;
	mesh->faces = NULL;
	mesh->normals = NULL;

	free(mesh);

	puts("freed mesh");
}

// .obj parser which returns an array of meshes and sets meshCount to the number of meshes
Mesh* loadMeshFromOBJ(const char* fileName, int* meshCount) {
	char filePath[512];
	snprintf(filePath, sizeof(filePath), "%s%s", RESOURCES_MESHES_DIR, fileName);

	FILE* fptr = fopen(filePath, "r");

	if (fptr == NULL) {
		puts("Error opening file");
		return NULL;
	}

	Mesh* meshArr = NULL;
	int currentMeshIndex = -1;

	char lineBuffer[MESHLOADER_LINEBUFFER_SIZE];
	int lineNumb = 0;

	// For each line :
	while (fgets(lineBuffer, MESHLOADER_LINEBUFFER_SIZE, fptr)) {
	//	printf(" - %s", lineBuffer);

		lineNumb++;
		switch (lineBuffer[0]) {
			// Comment
			case '#':
				continue;

			// Material file reference
			case 'm':
				break;

			// Use material
			case 'u':
				break;

			// Faces
			case 'f':
				if (currentMeshIndex == -1) {
					printf("Error tried to add vertex data before mesh declaration in '%s' : line %i", fileName, lineNumb);
					raise(SIGTERM);
				}

				v3* newFaceArr = realloc(meshArr[currentMeshIndex].faces, (meshArr[currentMeshIndex].faceCount +1) * sizeof(Tri));
				if (!newFaceArr) {
					printf("Error resizing memory in '%s' : line %i", fileName, lineNumb);
					raise(SIGTERM);
				}

				meshArr[currentMeshIndex].faces = newFaceArr;

				Tri newFace = {0};
				int fCount = sscanf(lineBuffer, "f %i/%*i/%i %i/%*i/%*i %i/%*i/%*i", &newFace.v0, &newFace.n0, &newFace.v1, &newFace.v2);

				// Base 0
				newFace.v0--; newFace.v1--; newFace.v2--; newFace.n0--;

				if (fCount != 4) {
					printf("Error reading vertex data (only %i / 4) in '%s' : line %i", fCount, lineBuffer, lineNumb);
					raise(SIGTERM);
				}

				meshArr[currentMeshIndex].faceCount++;
				meshArr[currentMeshIndex].faces[meshArr[currentMeshIndex].faceCount -1] = newFace;
				break;

			// Line element
			case 'l':
				break;

			// Smooth shading
			case 's':
				break;

			// New object
			case 'o':
				// Allocate memory for new mesh and move pointer (meshCount) along
				currentMeshIndex++;

				Mesh* newMeshArr = realloc(meshArr, sizeof(Mesh) * (currentMeshIndex +1));
				if (!newMeshArr) {
					printf("Error resizing memory in '%s' : line %i", fileName, lineNumb);
					raise(SIGTERM);
				}

				meshArr = newMeshArr;
				meshArr[currentMeshIndex] = (Mesh){0};
				break;

			// Vertex / texture / normal / paremeter
			case 'v':
				if (currentMeshIndex == -1) {
					printf("Error tried to add vertex data before mesh declaration in '%s' : line %i", fileName, lineNumb);
					raise(SIGTERM);
				}

				switch (lineBuffer[1]) {
					// Vertex
					case ' ':
						v3* newVertexArr = realloc(meshArr[currentMeshIndex].vertices, (meshArr[currentMeshIndex].vertexCount +1) * sizeof(struct v3));
						if (!newVertexArr) {
							printf("Error resizing memory in '%s' : line %i", fileName, lineNumb);
							raise(SIGTERM);
						}

						meshArr[currentMeshIndex].vertices = newVertexArr;

						v3 newVertex = {0};
						int vCount = sscanf(lineBuffer, "v %lf %lf %lf", &newVertex.x, &newVertex.y, &newVertex.z);

						if (vCount != 3) {
							printf("Error reading vertex data (only %i / 3) in '%s' : line %i", vCount, lineBuffer, lineNumb);
							raise(SIGTERM);
						}

						meshArr[currentMeshIndex].vertexCount++;
						meshArr[currentMeshIndex].vertices[meshArr[currentMeshIndex].vertexCount -1] = newVertex;
						break;

					// Normal
					case 'n':
						v3* newNormalArr = realloc(meshArr[currentMeshIndex].normals, (meshArr[currentMeshIndex].normalCount +1) * sizeof(struct v3));
						if (!newNormalArr) {
							printf("Error resizing memory in '%s' : line %i", fileName, lineNumb);
							raise(SIGTERM);
						}

						meshArr[currentMeshIndex].normals = newNormalArr;

						v3 newNormal = {0};
						int nCount = sscanf(lineBuffer, "vn %lf %lf %lf", &newNormal.x, &newNormal.y, &newNormal.z);

						if (nCount != 3) {
							printf("Error reading normal data (only %i / 3) in '%s' : line %i", vCount, lineBuffer, lineNumb);
							raise(SIGTERM);
						}

						meshArr[currentMeshIndex].normalCount++;
						meshArr[currentMeshIndex].normals[meshArr[currentMeshIndex].normalCount -1] = newNormal;
						break;
					default:
						printf("Un-recognised (v) element -%s-in '%s' : line %i", &lineBuffer[0], fileName, lineNumb);
						break;
				}
				break;

			// Un-recognised, skip
			default:
				printf("Un-recognised element -%s-in '%s' : line %i", &lineBuffer[0], fileName, lineNumb);
				break;
		}
	}

	fclose(fptr);

	// Debug print
	// for (int i=0; i<currentMeshIndex+1; i++) {
	// 	printf("%i : \n", i);
	// 	for (int j=0; j<meshArr[i].vertexCount; j++) {
	// 		printf(" - v : %f %f %f\n", meshArr[i].vertices[j].x, meshArr[i].vertices[j].y, meshArr[i].vertices[j].z);
	// 	}
	// 	for (int j=0; j<meshArr[i].normalCount; j++) {
	// 		printf(" - n : %f %f %f\n", meshArr[i].normals[j].x, meshArr[i].normals[j].y, meshArr[i].normals[j].z);
	// 	}
	// 	for (int j=0; j<meshArr[i].faceCount; j++) {
	// 		printf(" - f : %i %i %i : %i\n", meshArr[i].faces[j].v0, meshArr[i].faces[j].v1, meshArr[i].faces[j].v2, meshArr[i].faces[j].n0);
	// 	}
	// }

	puts("HI");

	(*meshCount) = (currentMeshIndex+1);
	return meshArr;
}