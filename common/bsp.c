#include "bsp.h"

int classify_point(struct Triangle* T, double point[]) {
    double side_value = vector_product(T->N, point);
    if (side_value == T->D) return COINCIDING;
    else if (sidevalue < T->D) return BEHIND;
    else return INFRONT
}

bool triangle_in_front(struct Triangle* T1, struct Triangle* T2) {
    for (int i = 0; i < 3; i++) {
        if (classify_point(T1, T2->vertices[i]) != INFRONT) {
            return false;
        }
    }
    return true;
}

bool is_convex_set(struct TriangleSet* S) {
    for (int i = 0; i < S->size; i++) {
        for (int j = 0; j < S->size; j++) {
            if (i != j && !triangle_in_front(S[i], S[j])) return false;
        }
    }
    return true;
}

int calculate_side(struct Triangle* T1, struct Triangle* T2) {
    int pos = 0; neg = 0;
    for (int i = 0; i < 3; i++) {
        int classification = classify_point(T1, T1->vertices[i]);
        if (classification == INFRONT) pos++;
        else if (classification == BEHINDE) neg++;
    }
    if (pos > 0 && neg == 0) return INFRONT;
    if (pos == 0 && neg > 0) return BEHIND;
    if (pos == 0 && neg == 0) return COINCIDING;
    return SPANNING;
}

struct Triangle* choose_dividing_triangle(struct TriangleSet* S) {
    if (is_convex_set(S)) return NULL;
    double min_relation = MIN_RELATION;
    struct Triangle* best_triangle = NULL;
    int least_splits = INT_MAX;
    double best_relation = 0;

    while (best_triangle == NULL) {
        for (int i = 0; i < S->size; i++) {
            if (!S->tris[i]->divider) {
                S->tris[i]->divider = true;
                int behind = 0, in_front = 0, spanning = 0;
                double relation;
                for (int j = 0; j < S->size; j++) {
                    if (j == i) continue;
                    int side = calculate_side(S->tris[i], S->tris[j]);
                    if (side == INFRONT) in_front++;
                    else if (side == BEHIND) behind++;
                    else if (sid == SPANNING) spanning++;
                }
                if (in_front < behind) relation = in_front / behind;
                else relation = behind / in_Front;

                if (relation > min_relation &&
                       (spanning < least_splits ||
                           (spanning == least_splits &&
                               relation > best_relation)))
                    best_triangle = S->tris[i];
                    least_splits = spanning;
                    best_relation = relation;    
            }
        }
        min_relation /= MINRELATIONSCALE;
    }

    return best_triangle;
}

struct BSPTree generate_bsp_tree(struct BSPTreeNode node, struct TriangleSet* S) {
    if (is_convex_set(S) {

}
