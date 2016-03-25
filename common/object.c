#include "object.h"

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   struct Object* O) {
    object_set_color(O, color);
    int indices[MAX_TRIANGLES][3];
    if (!object_parse(O, obj_file_path, indices)) return false;
    object_adjust_position(O, pos);
    object_create_triangles(O, indices);

    return true;
}

void object_set_color(struct Object* O, uint8_t color[]) {
    for (int i = 0; i < 3; i++) {
        O->color[i] = color[i];
    }
}

void object_adjust_position(struct Object* O, double pos[]) {
    for (int i = 0; i < O->vertc; i++)
        for (int j = 0; j < 3; j++)
            O->verts[i][j] += pos[j];
}

bool object_parse(struct Object* O, const char* file_path, int indices[][3]) {
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
            object_parse_line(line_buffer,
                              &O->vertc, O->verts,
                              &O->tric, indices);
        }
        fclose(f);
        printf("%d vertices and %d triangles loaded from '%s'.\n",
               O->vertc, O->tric, file_path);
    }
    else { 
        fprintf(stderr, "renderer: file at '%s' could not be loaded\n", file_path);
        return false;
    }
    free(line_buffer);
    return (O->vertc > 0 && O->tric > 0);
}

void object_parse_line(char* line,
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
            for (int i = 0; i < 3; i++) {
                /* atof converts string to float */
                vertices[*vertexc][i] = atof(tokens[i]);
            }
            *vertexc = *vertexc + 1;
        }
        else if (token_start[0] == 'f') {
            for (int i = 0; i < 3; i++) {
                char* index_str = strtok(tokens[i], "/");
                /* atoi() converts string to int */
                indices[*indexc][i] = atoi(index_str) - 1;
                /* subtract 1 because index in obj-files starts with 1 while
                 * the index in arrays starts with 0 */
            }
            *indexc = *indexc + 1;
        }
    }
}

void object_create_triangles(struct Object* O, int indices[][3]) {
    O->tris = (struct Triangle*)malloc(sizeof(struct Triangle)*O->tric);
    for (int i = 0; i < O->tric; i++) {
        triangle_create(O->verts, indices[i], O, &O->tris[i]);
    }
}
