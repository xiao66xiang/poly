//答案数据
#define ANSWER_SIZE 2000000

#define swap(x,y) (x)+=(y);(y)=(x)-(y);(x)-=(y);

#define MAX_POINT_NUMBER 1000005
#define eps 1e-9

/** polygons */
#define MAX_POLYGON_NUMBER 502
#define MAX_LINE_POINT_NUMBER 2005
#define MAX_EXTERIOR_POINT_NUMBER (501 * 2005)
#define MAX_INTERIOR_POINT_NUMBER (501 * 2005)
#define MAX_INTERIOR_REGION_NUMBER (501 * 4)

#define max(x,y) ((x)>(y)?(x):(y))
#define min(x,y) ((x)<(y)?(x):(y))


/** grid divide slice */
#define GRID_SLICE 16



/** points */
typedef struct Point { double x, y; } Point;

/** Outer R-Tree */
#define MAXCH 8
#define MAXP 512
typedef struct RTreeNode{
	double x1,x2,y1,y2;
	struct RTreeNode* next[MAXCH];
	int num,id;
} RTreeNode;

typedef struct arg{
    int ProNum;
    int stP;
    int enP;
} argType;

int first_query_point[2000005];
int next_query_point[2000005];
int Rans[10][MAXP];
int Rnum[10];
int first_query_point[2000005];
int next_query_point[2000005];
int now_polygon[MAX_POLYGON_NUMBER];
int lsequence[MAXP];
int rsequence[MAXP];
RTreeNode RTree[MAXP],_RTree[MAXP];
int MBR_id;
double MBR_x1[MAXP],MBR_x2[MAXP],MBR_y1[MAXP],MBR_y2[MAXP];
RTreeNode FreeNode;
double change_x1,change_x2,change_y1,change_y2;
int now_polygon[MAX_POLYGON_NUMBER];
int lsequence[MAXP];
int rsequence[MAXP];
RTreeNode RTree[MAXP],_RTree[MAXP];
int ALLRTree;
int MBR_id;
RTreeNode FreeNode;
double MBR_x1[MAXP],MBR_x2[MAXP],MBR_y1[MAXP],MBR_y2[MAXP];
double change_x1,change_x2,change_y1,change_y2;

int len_buffer[4];
#define buffer_limit 9500000
char str_buffer[4][10000000];

/** Inner R-Tree */
typedef struct In_RTreeNode{
        double stx , edx , sty , edy ;
        int stPoint , edPoint ;
        int lable ;
        int next[5] ;
}In_RTreeNode;

int Done[10];

int ans_count[10];

// read thread
int number_of_points_th[4];
int point_id_th[4][MAX_POINT_NUMBER];
int point_sequence_th[4][MAX_POINT_NUMBER];
Point point_th[4][MAX_POINT_NUMBER];

int number_of_polygons_th[4];
int polygon_id_th[4][MAX_POLYGON_NUMBER];
int polygon_sequence_th[4][MAX_POLYGON_NUMBER];

int number_of_exterior_points_th[4];
int number_of_interior_points_th[4];
int	number_of_interior_regions_th[4];

int	first_exterior_point_th[4][MAX_EXTERIOR_POINT_NUMBER], next_exterior_point_th[4][MAX_EXTERIOR_POINT_NUMBER];
int	first_interior_point_th[4][MAX_INTERIOR_POINT_NUMBER], next_interior_point_th[4][MAX_INTERIOR_POINT_NUMBER];
Point temp_list_th[4][MAX_LINE_POINT_NUMBER];
int poly_point_number_th[4];

int	first_interior_region_th[4][MAX_INTERIOR_REGION_NUMBER], next_interior_region_th[4][MAX_INTERIOR_REGION_NUMBER];
Point exterior_point_th[4][MAX_EXTERIOR_POINT_NUMBER];
Point interior_point_th[4][MAX_INTERIOR_POINT_NUMBER];