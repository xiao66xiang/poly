#include "refdefine.h"

char *buffer;
int length;

/** commands */
char request_cmd[10], requst_dist_cmd[10];
char point_input_file_path[300], polygon_input_file_path[300], output_file_path[300];

double totalmaxx , totalminx , totalmaxy , totalminy ;
int point_distribute[20][20];

int number_of_points;
int point_id[MAX_POINT_NUMBER];
int point_sequence[MAX_POINT_NUMBER];
Point point[MAX_POINT_NUMBER];
int stimes[6] ;
int stottimes ; 

int number_of_polygons;
int polygon_id[MAX_POLYGON_NUMBER];
int polygon_sequence[MAX_POLYGON_NUMBER];

int number_of_exterior_points;
int number_of_interior_points;
int	number_of_interior_regions;

// approximated polygons
double tolerance;
int	first_exterior_point[MAX_POLYGON_NUMBER], next_exterior_point[MAX_EXTERIOR_POINT_NUMBER];
int	first_interior_point[MAX_INTERIOR_POINT_NUMBER], next_interior_point[MAX_INTERIOR_POINT_NUMBER];
Point temp_list[MAX_LINE_POINT_NUMBER];
int poly_point_number;
//
int	first_interior_region[MAX_INTERIOR_REGION_NUMBER], next_interior_region[MAX_INTERIOR_REGION_NUMBER];
Point exterior_point[MAX_EXTERIOR_POINT_NUMBER];
Point interior_point[MAX_INTERIOR_POINT_NUMBER];

double exMax[MAX_POLYGON_NUMBER] , exMin[MAX_POLYGON_NUMBER] , eyMax[MAX_POLYGON_NUMBER] , eyMin[MAX_POLYGON_NUMBER] ;
double ixMax[MAX_INTERIOR_REGION_NUMBER] , ixMin[MAX_INTERIOR_REGION_NUMBER] , iyMax[MAX_INTERIOR_REGION_NUMBER] , iyMin[MAX_INTERIOR_REGION_NUMBER] ;

int links[MAX_POLYGON_NUMBER] ;
int last[MAX_POINT_NUMBER], now[MAX_POINT_NUMBER] ,next[ANSWER_SIZE] , other[ANSWER_SIZE] , tTime[ANSWER_SIZE] ;
double request_dist ;

/** Interior Appproximation Section */
double polygon_bound[MAX_POLYGON_NUMBER][4];					//minx, maxx, minx, maxy
double polygon_slice[MAX_POLYGON_NUMBER][2];					//dx,dy
int grid_status[MAX_POLYGON_NUMBER][GRID_SLICE][GRID_SLICE];	//grid (not determined:2   in:1   out:0)

int point_id_list[4][MAX_LINE_POINT_NUMBER]; //polygon id list
int grid_line_intersect[4][GRID_SLICE][GRID_SLICE][MAX_LINE_POINT_NUMBER];

/** aaaa */
//int tmpRnum[MAX_POINT_NUMBER];
//short tmpRans[MAX_POINT_NUMBER][500];

/** Inner R-Tree */
In_RTreeNode exteriorRTree[MAX_EXTERIOR_POINT_NUMBER*2+20000] ;
In_RTreeNode interiorRTree[MAX_INTERIOR_POINT_NUMBER*2+20000] ;
int exteriorRTreeStart[MAX_POLYGON_NUMBER*2] , interiorRTreeStart[MAX_INTERIOR_REGION_NUMBER*2];
int exstack[4][MAX_LINE_POINT_NUMBER*2] ;
int instack[4][MAX_LINE_POINT_NUMBER*2] ;


int point_id_first[MAX_POINT_NUMBER] ;
int point_id_cluster[MAX_POINT_NUMBER] ;
int point_num[MAX_POINT_NUMBER] ;
int point_cnt[MAX_POINT_NUMBER] ;
int number_of_processing ;


int interval_number = 0;
int interval_start[MAX_POINT_NUMBER];
int interval_time[MAX_POINT_NUMBER];

int ansx[5],ansy[5],flagC[5];
