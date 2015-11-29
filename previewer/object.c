#include "object.h"

bool object_create(const char* obj_file_path,
                   double pos[],
                   uint8_t color[],
                   struct Object* O) {


    /* set color */
    for (int i = 0; i < 3; i++) O->color[i] = color[i];

    /* parse vertices */
    if (!load_obj_file(O, obj_file_path)) return false;
    
    /* move vertices */
    for (int i = 0; i < O->vertexc; i++)
        for (int j = 0; j < 3; j++)
            O->vertices[i][j] += pos[j];

    return true;
}

bool load_obj_file(struct Object* O, const char* file_path) {
    /* add vertices and faces from obj file
     * if provided with pos/rot, add position and rotation */
    int character, pos;
    char* line_buffer = (char*)malloc(OBJ_LINE_BUFFER_SIZE);
    FILE* f = fopen(file_path, "r");
    if (f) {
        while (1) {
            pos = 0;
            do {
                character = fgetc(f);
                if (character != EOF) line_buffer[pos++] = (char)character;
            } while (character != EOF && character != '\n');
            if (character == EOF) break;
            line_buffer[pos] = 0;
            parse_line(O, line_buffer);
        }
        fclose(f);
        printf("%d vertices loaded from '%s'.\n",
        O->vertexc, file_path);
    }
    else { 
        fprintf(stderr, "previewer: file at %s coult not be loaded\n", file_path);
        return false;
    }
    free(line_buffer);
    return O->vertexc > 0;
}

void parse_line(struct Object* O, char* line) {
    /* use strtok to tokenize / segment line
     * strtok modifies original string
     * strtok replaces separator string with null character
     * "v 1.0 2.0 3.0" | token_start = "v"
     * "v\01.0 2.0 3.0" | token_1 = "1.0"
     * "v\01.0\02.0 3.0" | token_2 = "2.0"
     * "v\01.0\02.0\03.0" | token_3 = "3.0"
     * hidden global variable keeps track of start for next token*/
    char* token_start = strtok(line, " ");
    char* tokens[3];
    for (int i = 0; i < 3; i++) tokens[i] = strtok(NULL, " ");
    if (token_start[1] == 0) {
        if (token_start[0] == 'v') {
            /* parse vertex coordinates */
            for (int i = 0; i < 3; i++) {
                /* string to double */
                O->vertices[O->vertexc][i] = atof(tokens[i]);
            }
            O->vertexc++;
        }
    }
}
