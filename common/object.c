#include "object.h"

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   bool light,
                   struct Object* O) {
    /* set surface properties */
    O->light = light;
    for (int i = 0; i < 3; i++)
        O->color[i] = color[i];
    
    /* parse vertices and vertex indices for faces */
    int indices[MAX_TRIANGLES][3];
    O->tric = 0;
    O->vertc = 0;
    if (!load_obj_file(obj_file_path, &O->vertc, O->verts, &O->tric, indices)) {
        return false;
    }

    O->verts_rendered = malloc(O->vertc*sizeof(bool));
    
    /* move vertices */
    for (int i = 0; i < O->vertc; i++)
        for (int j = 0; j < 3; j++)
            O->verts[i][j] += pos[j];
    
    /* create triangles with new vertices */
    O->tris = (struct Triangle*)malloc(sizeof(struct Triangle)*O->tric);
    for (int i = 0; i < O->tric; i++) {
        triangle_create(O->verts, indices[i], O, &O->tris[i]);
    }

    return true; /* object was created successfully */
}

void object_clear_2d_array(struct Object* O) {
    for (int i = 0; i < O->vertc; i++) {
        O->verts_rendered[i] = false;
    }
}

bool load_obj_file(const char* file_path, 
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
    else { 
        fprintf(stderr, "renderer: file at '%s' could not be loaded\n", file_path);
        return false;
    }
    free(line_buffer);
    return (*vertexc > 0 && indexc > 0);
}

void parse_line(char* line,
                int* vertexc, double vertices[][3],
                int* indexc, int indices[][3]) {
    /* face must be a triangle
     * use strtok to tokenize / segment line
     * strtok modifies original string
     * strtok replaces separator string with null character
     * hidden global variable keeps track of start for next token
     * "v 1.0 2.0 3.0"    | token_start = "v"
     * "v\01.0 2.0 3.0"   | token_1 = "1.0"
     * "v\01.0\02.0 3.0"  | token_2 = "2.0"
     * "v\01.0\02.0\03.0" | token_3 = "3.0" */

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
