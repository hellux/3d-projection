#include "object.h"

static void triangle_create(double vertices[][3],
                            int indices[],
                            struct Triangle* T) {
    for (int i = 0; i < 3; i++) {
        T->P1[i] = vertices[indices[0]][i];
        T->P2[i] = vertices[indices[1]][i];
        T->P3[i] = vertices[indices[2]][i];
    } 

    vector_subtract(T->P2, T->P1, T->Q1);
    vector_subtract(T->P3, T->P1, T->Q2);

    cross_product(T->Q1, T->Q2, T->N);

    double mat[][2] = {{ dot_product(T->Q1, T->Q1), dot_product(T->Q1, T->Q2) },
                       { dot_product(T->Q1, T->Q2), dot_product(T->Q2, T->Q2) }};
    matrix_inverse(mat, T->M);

    T->D = -dot_product(T->N, T->P1);
}

void object_create(char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   double brightness,
                   double reflectiveness,
                   struct Object* O) {
    /* set surface properties */
    O->brightness = brightness;
    O->reflectiveness = reflectiveness;
    for (int i = 0; i < 3; i++)
        O->color[i] = color[i];
    
    /* parse vertices and vertex indices for faces */
    double vertices[MAX_VERTICES][3];
    int indices[MAX_TRIANGLES][3];
    int vertexc = 0, indexc = 0;
    load_obj_file(obj_file_path, &vertexc, vertices, &indexc, indices);
    O->tric = indexc;
    
    /* move vertices */
    for (int i = 0; i < vertexc; i++)
        for (int j = 0; j < 3; j++)
            vertices[i][j] += pos[j];
    
    /* create triangles with new vertices */
    O->tris = (struct Triangle*)malloc(sizeof(struct Triangle)*O->tric);
    for (int i = 0; i < O->tric; i++) {
        triangle_create(vertices, indices[i], &O->tris[i]);
    }
}

static void load_obj_file(char* file_path, 
                          int* vertexc, double vertices[][3], 
                          int* indexc, int indices[][3]) {
    /* add vertices and faces from obj file
     * if provided with pos/rot, add position and rotation */
    int c, p;
    char* line_buffer = (char*)malloc(OBJ_LINE_BUFFER_SIZE);
    FILE* f = fopen(file_path, "r");
    if (f) {
        while (1) {
            p = 0;
            do {
                c = fgetc(f);
                if (c != EOF) line_buffer[p++] = (char)c;
            } while (c != EOF && c != '\n');
            if (c == EOF) break;
            line_buffer[p] = 0;
            parse_line(line_buffer, vertexc, vertices, indexc, indices);
        }
        fclose(f);
        printf("%d vertices and %d triangles loaded from '%s'.\n",
        *vertexc, *indexc, file_path);
    }
    else { printf("File at %s not loaded", file_path); }
    free(line_buffer);
}

static void parse_line(char* line,
                       int* vertexc, double vertices[][3],
                       int* indexc, int indices[][3]) {
    /* use strtok to tokenize / segment line
     * strtok modifies original string
     * strtok replaces separator string with null character
     * "v 1.0 2.0 3.0" | token_start = "v"
     * "v\01.0 2.0 3.0" | token_1 = "1.0"
     * "v\01.0\02.0 3.0" | token_2 = "2.0"
     * "v\01.0\02.0\03.0" | token_3 = "3.0"
     * hidden global variable keeps track of start for next token*/
    char* separators[4];
    char* token_start = strtok(line, " ");
    char* tokens[3];
    for (int i = 0; i < 3; i++) tokens[i] = strtok(NULL, " ");
    if (token_start[1] == 0) {
        if (token_start[0] == 'v') {
            /* parse vertex coordinates */
            for (int i = 0; i < 3; i++) {
                /* string to double */
                vertices[*vertexc][i] = atof(tokens[i]);
            }
            *vertexc = *vertexc + 1;
        }
        else if (token_start[0] == 'f') {
            /* parse face/triangle indices */
            for (int i = 0; i < 3; i++) {
                char* index_str = strtok(tokens[i], "/");
                /* string to int */
                indices[*indexc][i] = atoi(index_str) - 1;
            }
            *indexc = *indexc + 1;
        }
    }
}
