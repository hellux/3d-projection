#include "object.h"

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   double reflectiveness,
                   struct Object* O) {
    if (!object_set_reflectiveness(O, reflectiveness)) return false;
    object_allocate_memory_triangles(O);
    object_allocate_memory_vertices(O);
    if (!object_parse(O, obj_file_path)) return false;
    object_set_color(O, color);
    object_allocate_memory_verts_in_front(O);
    object_allocate_memory_verts_2d(O);
    object_adjust_position(O, pos);
    
    return true;
}

void object_allocate_memory_triangles(struct Object* O) {
    O->tris_buffer = OBJECT_TRIANGLES_START_BUFFER;
    O->tris = calloc(sizeof(struct Triangle), O->tris_buffer);
    O->tric = 0;
}

void object_reallocate_memory_triangles(struct Object* O) {
    O->tris_buffer += OBJECT_TRIANGLES_START_BUFFER;
    O->tris = realloc(O->tris, sizeof(struct Triangle)*O->tris_buffer);
}

void object_allocate_memory_vertices(struct Object* O) {
    O->verts_buffer = OBJECT_VERTICES_START_BUFFER;
    O->verts = calloc(sizeof(double*), O->verts_buffer);
    for (int i = 0; i < O->verts_buffer; i++)
        O->verts[i] = calloc(sizeof(double), 3);
    O->vertc = 0;
}

void object_reallocate_memory_vertices(struct Object* O) {
    int old_buffer = O->verts_buffer;
    O->verts_buffer += OBJECT_VERTICES_START_BUFFER;
    O->verts = realloc(O->verts, sizeof(double*)*O->verts_buffer);
    for (int i = old_buffer; i < O->verts_buffer; i++)
        O->verts[i] = calloc(sizeof(double), 3);
}

void object_allocate_memory_verts_in_front(struct Object* O) {
    O->verts_in_front = calloc(sizeof(bool), O->vertc);
}

void object_allocate_memory_verts_2d(struct Object* O) {
    O->verts_2d = calloc(sizeof(double*), O->vertc);
    for (int i = 0; i < O->vertc; i++) 
        O->verts_2d[i] = calloc(sizeof(double), 2);
}

void object_set_color(struct Object* O, uint8_t color[]) {
    for (int i = 0; i < 3; i++) {
        O->color[i] = color[i];
    }
    rgb_to_hsl(O->color, O->color_hsl);
}

bool object_set_reflectiveness(struct Object* O, double refl) {
    if (refl < 0 || refl > 1) {
        printf("object: invalid reflectiveness -- %f (required: 0<=r<=1)\n",
               refl);
        return false;
    }
    O->reflectiveness = refl;
    return true;
}

void object_adjust_position(struct Object* O, double pos[]) {
    for (int i = 0; i < O->vertc; i++)
        for (int j = 0; j < 3; j++)
            O->verts[i][j] += pos[j];
}

bool object_parse(struct Object* O, const char* file_path) {
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
            object_parse_line(line_buffer, O);
        }
        fclose(f);
        printf("object: %d vertices and %d triangles loaded from '%s'.\n",
               O->vertc, O->tric, file_path);
    }
    else { 
        fprintf(stderr, "object: file at '%s' could not be loaded\n", file_path);
        free(line_buffer);
        return false;
    }
    free(line_buffer);
    return (O->vertc > 0 && O->tric > 0);
}

void object_parse_line(char* line, struct Object* O) {
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
            double vertex[3];
            for (int i = 0; i < 3; i++) {
                /* atof converts string to float */
                vertex[i] = atof(tokens[i]);
            }
            object_add_vertex(O, vertex);
        }
        else if (token_start[0] == 'f') {
            int indices[3];
            for (int i = 0; i < 3; i++) {
                char* index_str = strtok(tokens[i], "/");
                /* atoi() converts string to int */
                indices[i] = atoi(index_str) - 1;
                /* subtract 1 because index in obj-files starts with 1 while
                 * the index in arrays starts with 0 */
            }
            object_create_triangle(O, indices);
        }
    }
}

void object_add_vertex(struct Object* O, double vertex[]) {
    if (O->vertc == O->verts_buffer-1) object_reallocate_memory_vertices(O);
    for (int i = 0; i < 3; i++)
        O->verts[O->vertc][i] = vertex[i];
    O->vertc++;
}

void object_create_triangle(struct Object* O, int indices[]) {
    if (O->tric == O->tris_buffer-1) object_reallocate_memory_triangles(O);
    triangle_create(O->verts, indices, O, &O->tris[O->tric++]);
}
