#ifndef WAVEFRONT_H_H
#define WAVEFRONT_H
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <stdlib.h>

#include <my_math/vector.h>

typedef struct {
    unsigned int VAO;
    unsigned int num_indices;
} WaveFront;

typedef Vector2D TexCoord;


typedef struct {
    unsigned int vertex, texture, normal;
} FacePoint;

typedef FacePoint FaceVertex[3];
typedef struct {
    Vector3D *vertices;
    TexCoord *texCoords;
    FaceVertex *faces;
    int numVertices, numTexCoords, numFaces;
} ObjModel;


WaveFront load_wave(const char* filename)
{
    ObjModel model;
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int numFaceProperties = 0;

    char line[256];
    int numVertices = 0, numTexCoords = 0, numFaces = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            numVertices++;
        } else if (line[0] == 'v' && line[1] == 't') {
            numTexCoords++;
        } else if (line[0] == 'f') {
            numFaces++;

            if(!numFaceProperties) {
                int v;

                int vertex = sscanf(line, "f %d %d %d", &v, &v, &v);
                int vertex_texture = sscanf(line, "f %d/%d %d/%d %d/%d", &v, &v, &v, &v, &v, &v);
                int vertex_texture_normal = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v, &v, &v, &v, &v, &v, &v, &v, &v);

                if (vertex == 3) {
                    numFaceProperties = 1;
                } else if (vertex_texture == 6) {
                    numFaceProperties = 2;
                } else if (vertex_texture_normal == 12) {
                    numFaceProperties = 3;
                }
            }
        }
    }

    model.vertices = (Vector3D *)malloc(numVertices * sizeof(Vector3D));
    model.texCoords = (TexCoord *)malloc(numTexCoords * sizeof(TexCoord));
    model.faces = (FaceVertex *)malloc(numFaces * sizeof(FaceVertex));
    model.numVertices = numVertices;
    model.numTexCoords = numTexCoords;
    model.numFaces = numFaces;

    fseek(file, 0, SEEK_SET);

    int vertexIndex = 0, texCoordIndex = 0, faceIndex = 0;
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            sscanf(line, "v %f %f %f", &model.vertices[vertexIndex].x, &model.vertices[vertexIndex].y, &model.vertices[vertexIndex].z);
            vertexIndex++;
        } else if (line[0] == 'v' && line[1] == 't') {
            sscanf(line, "vt %f %f", &model.texCoords[texCoordIndex].x, &model.texCoords[texCoordIndex].y);
            texCoordIndex++;
        } else if (line[0] == 'f') {

            if(numFaceProperties == 1) {
                sscanf(line, "f %d %d %d",
                       &model.faces[faceIndex][0].vertex,
                       &model.faces[faceIndex][1].vertex,
                       &model.faces[faceIndex][2].vertex);

                       model.faces[faceIndex][0].texture = -1;
            }
            else if (numFaceProperties == 2)
            {
                sscanf(line, "f %d/%d %d/%d %d/%d",
                       &model.faces[faceIndex][0].vertex, &model.faces[faceIndex][0].texture,
                       &model.faces[faceIndex][1].vertex, &model.faces[faceIndex][1].texture,
                       &model.faces[faceIndex][2].vertex, &model.faces[faceIndex][2].texture);
            }
            else if (numFaceProperties == 3)
            {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &model.faces[faceIndex][0].vertex, &model.faces[faceIndex][0].texture, &model.faces[faceIndex][0].normal,
                       &model.faces[faceIndex][1].vertex, &model.faces[faceIndex][1].texture, &model.faces[faceIndex][1].normal,
                       &model.faces[faceIndex][2].vertex, &model.faces[faceIndex][2].texture, &model.faces[faceIndex][2].normal);
                
            }
            faceIndex++;
        }
    }

    /*
    printObjModel(&model);
    */

    fclose(file);

    int numIndices = model.numFaces * 3;
    unsigned int* indices = (unsigned int*)malloc(numIndices * sizeof(unsigned int));

    int i;
    for (i = 0; i < model.numFaces; ++i)
    {
        indices[i * 3] = model.faces[i][0].vertex - 1;
        indices[i * 3 + 1] = model.faces[i][1].vertex - 1;
        indices[i * 3 + 2] = model.faces[i][2].vertex - 1;
    }

    for (i = 0; i < numIndices; ++i)
    {
        /* printf("indice[%d]:%d\n", i, indices[i]); */
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
     
    GLuint vertexVBO;
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, model.numVertices * sizeof(Vector3D), model.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3D), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint texCoordVBO;
    glGenBuffers(1, &texCoordVBO);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, model.numTexCoords * sizeof(TexCoord), model.texCoords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexCoord), (void*)0);
    glEnableVertexAttribArray(1);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    WaveFront wavefront;
    wavefront.VAO = vao;
    wavefront.num_indices = numIndices;

    return wavefront;

}

WaveFront wavefront_load(const char* filename) 
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERROR::WAVEFRONT: Failed to open file: %s\n", filename);
        return;
    }

    char line[128];

    int num_vertices = 0;
    int num_texcoords = 0;
    int num_faces = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            num_vertices++;
        } else if (line[0] == 'v' && line[1] == 't') {
            num_texcoords++;
        } else if (line[0] == 'f') {
            num_faces++;
        }
    }

    printf("num_vertices: %d\n", num_vertices);
    printf("num_texcoords: %d\n", num_texcoords);
    printf("num_faces: %d\n", num_faces);

    Vector3D* vertices = (Vector3D*)malloc(num_vertices * sizeof(Vector3D));
    TexCoord* texcoords = (TexCoord*)malloc(num_texcoords * sizeof(TexCoord));

    unsigned int num_indices = num_faces * 3;
    unsigned int* indices = (unsigned int*)malloc(num_indices * sizeof(unsigned int));


    int vertex_count = 0;
    int tex_count = 0;
    int indice_count = 0;

    rewind(file);

    while (fgets(line, sizeof(line), file)) 
    {
        if (line[0] == 'v' && line[1] == ' ') 
        {
            Vector3D vertex;
            sscanf(line, "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            vertices[vertex_count] = vertex;
            vertex_count++;
        } 
        else if (line[0] == 'v' && line[1] == 't') 
        {
            TexCoord texCoord;
            sscanf(line, "vt %f %f\n", &texCoord.x, &texCoord.y);
            texcoords[tex_count] = texCoord;
            tex_count++;
        } 
        else if (line[0] == 'f') 
        {
            unsigned int indice1, indice2, indice3;
            sscanf(line, "f %d/%*d %d/%*d %d/%*d\n", &indice1, &indice2, &indice3);

            indices[indice_count] = indice1;
            indices[indice_count + 1] = indice2;
            indices[indice_count + 2] = indice3;

            indice_count += 3;
        }
    }


    int i;
    for(i = 0; i < num_vertices; ++i)
    {
        printf("Vertex[%d]: %f %f %f\n", i, vertices[i].x, vertices[i].y, vertices[i].z);
    }
    for(i = 0; i < num_texcoords; ++i)
    {
        printf("TexCoord[%d]: %f %f\n", i, texcoords[i].x, texcoords[i].y);
    }
    printf("Indices: \n");
    for(i = 0; i < num_indices; i++)
    {
         printf("%d \n", indices[i]);
         /*
        printf("%d %d %d\n", indices[i], indices[i + 1], indices[i + 2]);
        i+=3;
        */
    }


    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vboVertices;
    glGenBuffers(1, &vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vector3D), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3D), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint vboTexCoords;
    glGenBuffers(1, &vboTexCoords);
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords);
    glBufferData(GL_ARRAY_BUFFER, num_texcoords * sizeof(TexCoord), texcoords, GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexCoord), (void*)0);
    glEnableVertexAttribArray(1);

    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    fclose(file);

    free(vertices);
    free(texcoords);
    free(indices);

    printf("num_indices: %d\n", num_indices);

    WaveFront wavefront = { vao, num_indices };

    return wavefront;
}


void wavefront_draw(unsigned int shader_id, WaveFront wavefront)
{
    glUseProgram(shader_id);
     
    glBindVertexArray(wavefront.VAO);
    glDrawElements(GL_TRIANGLES, wavefront.num_indices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

#endif