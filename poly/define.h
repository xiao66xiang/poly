#include "refdefine.h"

/** buffer for reading files */
extern char *buffer;
extern int length;

/** commands */
extern char request_cmd[10], requst_dist_cmd[10];
extern char point_input_file_path[300], polygon_input_file_path[300], output_file_path[300];

extern int number_of_points;
extern int point_id[MAX_POINT_NUMBER];
extern int point_sequence[MAX_POINT_NUMBER];
extern Point point[MAX_POINT_NUMBER];

extern int number_of_polygons;
extern int polygon_id[MAX_POLYGON_NUMBER];
extern int polygon_sequence[MAX_POLYGON_NUMBER];

extern double totalmaxx , totalminx , totalmaxy , totalminy ;

extern int stimes[6] ;
extern int stottimes ;
extern int number_of_exterior_points;
extern int number_of_interior_points;
extern int	number_of_interior_regions;
// approximated polygons
extern double tolerance;
extern int	first_exterior_point[MAX_POLYGON_NUMBER], next_exterior_point[MAX_EXTERIOR_POINT_NUMBER];
extern int	first_interior_point[MAX_INTERIOR_POINT_NUMBER], next_interior_point[MAX_INTERIOR_POINT_NUMBER];

extern Point temp_list[MAX_LINE_POINT_NUMBER];
extern int poly_point_number;

extern int point_distribute[20][20] ;
//
extern int	first_interior_region[MAX_INTERIOR_REGION_NUMBER], next_interior_region[MAX_INTERIOR_REGION_NUMBER];
extern Point exterior_point[MAX_EXTERIOR_POINT_NUMBER];
extern Point interior_point[MAX_INTERIOR_POINT_NUMBER];

extern double exMax[MAX_POLYGON_NUMBER] , exMin[MAX_POLYGON_NUMBER] , eyMax[MAX_POLYGON_NUMBER] , eyMin[MAX_POLYGON_NUMBER] ;
extern double ixMax[MAX_INTERIOR_REGION_NUMBER] , ixMin[MAX_INTERIOR_REGION_NUMBER] , iyMax[MAX_INTERIOR_REGION_NUMBER] , iyMin[MAX_INTERIOR_REGION_NUMBER] ;

extern int links[MAX_POLYGON_NUMBER] ;
extern int last[MAX_POINT_NUMBER], now[MAX_POINT_NUMBER] ,next[ANSWER_SIZE] , other[ANSWER_SIZE] , tTime[ANSWER_SIZE] ;
extern double request_dist ;

/** Interior Appproximation Section */
extern double polygon_bound[MAX_POLYGON_NUMBER][4];					//minx, maxx, minx, maxy
extern double polygon_slice[MAX_POLYGON_NUMBER][2];					//dx,dy
extern int grid_status[MAX_POLYGON_NUMBER][GRID_SLICE][GRID_SLICE];	//grid (not determined:2   in:1   out:0)

extern int point_id_list[4][MAX_LINE_POINT_NUMBER]; //polygon id list
extern int grid_line_intersect[4][GRID_SLICE][GRID_SLICE][MAX_LINE_POINT_NUMBER];

/** aaaa */
//extern int tmpRnum[MAX_POINT_NUMBER];
//extern short tmpRans[MAX_POINT_NUMBER][500];
/** Inner R-Tree */
extern In_RTreeNode exteriorRTree[MAX_EXTERIOR_POINT_NUMBER*2+20000] ;
extern In_RTreeNode interiorRTree[MAX_INTERIOR_POINT_NUMBER*2+20000] ;
extern int exteriorRTreeStart[MAX_POLYGON_NUMBER*2] , interiorRTreeStart[MAX_INTERIOR_REGION_NUMBER*2];
extern int exstack[4][MAX_LINE_POINT_NUMBER*2] ;
extern int instack[4][MAX_LINE_POINT_NUMBER*2] ;



extern int point_id_first[MAX_POINT_NUMBER] ;
extern int point_id_cluster[MAX_POINT_NUMBER] ;
extern int point_num[MAX_POINT_NUMBER] ;
extern int point_cnt[MAX_POINT_NUMBER] ;
extern int number_of_processing ;


extern int inteval_number ;
extern int inteval_start[MAX_POINT_NUMBER];
extern int inteval_time[MAX_POINT_NUMBER];

extern int ansx[5],ansy[5],flagC[5];


