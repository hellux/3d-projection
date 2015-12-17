#ifndef bsp_h
#define bsp_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>

#define INFRONT 0
#define #BEHIND 1
#define COINCIDING 2
#define SPANNING 3

#define MINRELATION 1
#define MINRELATIONSCALE 2.5

struct BSPTree {
    BSPTreeNode root_node;
}

struct BSPTreeNode {
    struct BSPTree* tree;
    struct Triangle* divider;
    struct BSPTree right_child;
    struct BSPTree left_child;
    struct TriangleSet triangle_set;
}

int classify_point(struct Triangle* T, double point[]);
bool trianlge_in_front(struct Triangle* T1, struct Triangle* T2);
bool is_convex_set(struct TriangleSet* S);

#endif
