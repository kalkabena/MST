#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define sqr(x) ((x) * (x))

// jika output tidak benar masukkan [Console]::OutputEncoding = [System.Text.Encoding]::UTF8 di console
typedef struct 
{
	float x;
	float y;
	float z;
} point;

typedef struct 
{
	int idx1;
	int idx2;
	float dist;
} segment;

typedef struct tnode* address;
typedef struct tnode
{
	int mode;
	/*
	0 = OR
	1 = AND
	2 = >
	-2 = <
	*/
	char var;
	float data;
	float interval[100];
	int Neff;
	address left;
	address right;
} node;

address newNode(int mode, char var, float data) {
    address p = (address) malloc(sizeof(node));
    if (p != NULL) {
        p->mode = mode;
		p->var = var;
		p->data = data;
		p->Neff = 0;
        p->left = NULL;
		p->right = NULL;
    }
    return p;
}

void mergeIntervals(float (*arrptr)[100], int *neff) {
	int i = 0, j = 2;
	while (i < *neff && j < *neff) {
        if ((*arrptr)[i + 1] >= (*arrptr)[j] && (*arrptr)[j + 1] >= (*arrptr)[i]) {
            (*arrptr)[i] = fmin((*arrptr)[j], (*arrptr)[i]);
            (*arrptr)[i + 1] = fmax((*arrptr)[j + 1], (*arrptr)[i + 1]);
            for (int c = j + 2; c < *neff; c++) {
                (*arrptr)[c - 2] = (*arrptr)[c];
            }
            i = 0;
            j = 0;
            *neff -= 2;
        }
        j += 2;
        if (j >= *neff) {
            i += 2;
            j = i + 2;
        }
    }
}

void processTree(address p, point a, point b) {
    switch (p->mode) {
        case 0:
        if (p->left != NULL) {
            processTree(p->left, a, b);
        }
        if (p->right != NULL) {
            processTree(p->right, a, b);
        }
        for (int i = 0; i < (p->left)->Neff; i += 2) {
                p->interval[p->Neff] = (p->left)->interval[i];
                p->interval[p->Neff + 1] = (p->left)->interval[i + 1];
                p->Neff += 2;
        }
        for (int i = 0; i < (p->right)->Neff; i += 2) {
                p->interval[p->Neff] = (p->right)->interval[i];
                p->interval[p->Neff + 1] = (p->right)->interval[i + 1];
                p->Neff += 2;
        }
        mergeIntervals(&(p->interval), &(p->Neff));
        break;
        case 1:
        if (p->left != NULL) {
            processTree(p->left, a, b);
        }
        if (p->right != NULL) {
            processTree(p->right, a, b);
        }
        for (int i = 0; i < (p->left)->Neff; i += 2) {
            for (int j = i; j < (p->right)->Neff; j += 2) {
                if ((p->left)->interval[i + 1] >= (p->right)->interval[j] && (p->right)->interval[j + 1] >= (p->left)->interval[i]) {
                    p->interval[p->Neff] = fmax((p->left)->interval[i], (p->right)->interval[j]);
                    p->interval[p->Neff + 1] = fmin((p->left)->interval[i + 1], (p->right)->interval[j + 1]);
                    p->Neff += 2;
                }
            }
        }
        mergeIntervals(&(p->interval), &(p->Neff));
        break;
        case 2:
        if (p->var == 'x') {
            if (a.x > p->data && b.x > p->data) {
                p->interval[0] = fmin(a.x, b.x);
                p->interval[1] = fmax(a.x, b.x);
                p->Neff = 2;
            } else if (a.x > p->data) {
                p->interval[0] = p->data;
                p->interval[1] = a.x;
                p->Neff = 2;
            } else if (b.x > p->data) {
                p->interval[0] = p->data;
                p->interval[1] = b.x;
                p->Neff = 2;
            }
        } else if (p->var == 'y') {
            if (a.y > p->data && b.y > p->data) {
                p->interval[0] = fmin(a.x, b.x);
                p->interval[1] = fmax(a.x, b.x);
                p->Neff = 2;
            } else if (a.y > p->data) {
                p->interval[0] = fmin((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, a.x);
                p->interval[1] = fmax((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, a.x);
                p->Neff = 2;
            } else if (b.y > p->data) {
                p->interval[0] = fmin((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, b.x);
                p->interval[1] = fmax((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, b.x);
                p->Neff = 2;
            }
        } else {
            if (a.z > p->data && b.z > p->data) {
                p->interval[0] = fmin(a.x, b.x);
                p->interval[1] = fmax(a.x, b.x);
                p->Neff = 2;
            } else if (a.z > p->data) {
                p->interval[0] = fmin((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, a.x);
                p->interval[1] = fmax((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, a.x);
                p->Neff = 2;
            } else if (b.z > p->data) {
                p->interval[0] = fmin((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, b.x);
                p->interval[1] = fmax((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, b.x);
                p->Neff = 2;
            }
        }
        break;
        case (-2):
        if (p->var == 'x') {
            if (a.x < p->data && b.x < p->data) {
                p->interval[0] = fmin(a.x, b.x);
                p->interval[1] = fmax(a.x, b.x);
                p->Neff = 2;
            } else if (a.x < p->data) {
                p->interval[1] = p->data;
                p->interval[0] = a.x;
                p->Neff = 2;
            } else if (b.x < p->data) {
                p->interval[1] = p->data;
                p->interval[0] = b.x;
                p->Neff = 2;
            }
        } else if (p->var == 'y') {
            if (a.y < p->data && b.y < p->data) {
                p->interval[0] = fmin(a.x, b.x);
                p->interval[1] = fmax(a.x, b.x);
                p->Neff = 2;
            } else if (a.y < p->data) {
                p->interval[1] = fmax((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, a.x);
                p->interval[0] = fmin((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, a.x);
                p->Neff = 2;
            } else if (b.y < p->data) {
                p->interval[1] = fmax((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, b.x);
                p->interval[0] = fmin((p->data - a.y) * (b.x - a.x) / (b.y - a.y) + a.x, b.x);
                p->Neff = 2;
            }
        } else {
            if (a.z < p->data && b.z < p->data) {
                p->interval[0] = fmin(a.x, b.x);
                p->interval[1] = fmax(a.x, b.x);
                p->Neff = 2;
            } else if (a.z < p->data) {
                p->interval[1] = fmax((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, a.x);
                p->interval[0] = fmin((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, a.x);
                p->Neff = 2;
            } else if (b.z < p->data) {
                p->interval[1] = fmax((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, b.x);
                p->interval[0] = fmin((p->data - a.z) * (b.x - a.x) / (b.z - a.z) + a.x, b.x);
                p->Neff = 2;
            }
        }
        break;
    }
}

void writeTree(address *root, int depth, int lr) {
    for (int i = 0; i < depth - 1; i++) {
        printf("║");
    }
    if (lr == 1) {
        printf("\u2560");
    } else if (lr == 2) {
        printf("╚");
    }
    char *input;
    input = (char*) malloc(50 * sizeof(char));
    scanf("%s", input);
    if (strcmp(input, "or") == 0) {
        *root = newNode(0, 'a', 0);
        writeTree(&((*root)->left), depth + 1, 1);
        writeTree(&((*root)->right), depth + 1, 2);
    } else if (strcmp(input, "and") == 0) {
        *root = newNode(1, 'a', 0);
        writeTree(&((*root)->left), depth + 1, 1);
        writeTree(&((*root)->right), depth + 1, 2);
    } else {
        char *expr = input;
        int mode = 2;
        char var;
        float data;
        if (atof(expr) != 0 || *expr == '0') {
            mode *= -1;
        }
        while(*expr) {
            if (*expr == 'x' || *expr == 'y' || *expr == 'z') {
                var = *expr;
                expr++;
            } else if (*expr == '>') {
                expr++;
            } else if (*expr == '<') {
                mode *= -1;
                expr++;
            } else {
                data = strtof(expr, &expr);
            }
        }
        *root = newNode(mode, var, data);
        }
    free(input);
}

void resetTree(address *root) {
    (*root)->Neff = 0;
    for (int i = 0; i < 100; i++) {
        (*root)->interval[i] = 0;
    }
    if ((*root)->left != NULL) {
        resetTree(&((*root)->left));
    }
    if ((*root)->right != NULL) {
        resetTree(&((*root)->right));
    }
}

float dist(point a, point b) {
	return sqrt(sqr(a.x - b.x) + sqr(a.y - b.y) + sqr(a.z - b.z));
}

int comp(const void *a, const void *b) {
    if (((segment *)a)->dist - ((segment *)b)->dist > 0) {
		return 1;
	} else {
		return -1;
	}
}

int main() {
	int c = 0, iter = 1, finalCount = 1, n;
	printf("masukkan jumlah simpul\n");
	scanf("%d", &n);
	point points[n];
	int explored[n];
	segment segments[n*(n-1)/2];
	segment tree[n - 1];

	srand(time(NULL));
	for (int i = 0; i < n; i++) {
		points[i].x = (float)(rand() % 1000)/500 - 1;
		points[i].y = (float)(rand() % 1000)/500 - 1;
		points[i].z = (float)(rand() % 1000)/500 - 1;
		explored[i] = 0;
	}

	/* for manual input of coordinates
	char *inputCoords;
	char *token;
	int counter = 0;
	inputCoords = (char*) malloc(1000 * sizeof(char));
	printf("input koordinat 3D dari desmos (maks 1000 chars)\n");
	scanf("%1000s", inputCoords);
	token = strtok(inputCoords, ",");
    while (token != NULL) {
        if (counter % 3 == 0) {
            if (*(token + 0) == '(') {
				token++;
			} else {
				token += 6;
			}
			points[counter / 3].x = atof(token);
        } else if (counter % 3 == 1) {
			points[counter / 3].y = atof(token);
		} else {
			points[counter / 3].z = atof(token);
		}
        token = strtok(NULL, ",");
        counter++;
    }
    free(inputCoords);
	free(token);
	*/
	address root;
	printf("isi tree (or, and, atau pertidaksamaan linear, hanya variabel tanpa koefisien atau konstanta, > atau <, tidak boleh membandingkan dua variabel)\n");
	writeTree(&root, 0, 0);
	
	printf("(%.3f,%.3f,%.3f)", points[0].x, points[0].y, points[0].z);
	for (int i = 1; i < n; i++) {
		printf(",(%.3f,%.3f,%.3f)", points[i].x, points[i].y, points[i].z);
	}
	printf("\n");
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			processTree(root, points[i], points[j]);
			if (root->Neff == 0) {
				segments[c].idx1 = i;
				segments[c].idx2 = j;
				segments[c].dist = dist(points[i], points[j]);
				c++;
			}
			resetTree(&root);
		}
	}
	qsort(segments, c, sizeof(segment), comp);
	tree[0] = segments[0];
	explored[segments[0].idx1] = 1;
	explored[segments[0].idx2] = 1;
	while (iter < c && finalCount < n - 1) {
		if (!(explored[segments[iter].idx1] == 1 && explored[segments[iter].idx2] == 1) &&
		!(explored[segments[iter].idx1] == 0 && explored[segments[iter].idx2] == 0)) {
			tree[finalCount] = segments[iter];
			explored[segments[iter].idx1] = 1;
			explored[segments[iter].idx2] = 1;
			finalCount++;
			iter = 0;
		}
		iter++;
	}
	printf("segment((%.3f,%.3f,%.3f),(%.3f,%.3f,%.3f))",
	points[tree[0].idx1].x, points[tree[0].idx1].y, points[tree[0].idx1].z,
	points[tree[0].idx2].x, points[tree[0].idx2].y, points[tree[0].idx2].z);
	for (int i = 1; i < finalCount; i++) {
		printf(",segment((%.3f,%.3f,%.3f),(%.3f,%.3f,%.3f))",
		points[tree[i].idx1].x, points[tree[i].idx1].y, points[tree[i].idx1].z,
		points[tree[i].idx2].x, points[tree[i].idx2].y, points[tree[i].idx2].z);
	}
}