#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "define.h"
#include <windows.h>
#include <process.h>

int flag_grid1 = 0,flag_grid2 = 0,flag_grid3 = 0,flag_grid4 = 0; 
int soleIntersect[MAX_POLYGON_NUMBER][GRID_SLICE][GRID_SLICE][3];	//与格子唯一相交的两个点的id,第三个 0表示外环，1表示内环
int twoDivide[MAX_POLYGON_NUMBER][GRID_SLICE][GRID_SLICE];			//0 左边在里面 1 右边在里面（首先要判断格子状态为2）

/** Interior Appproximation Section */
//whether a point is in the polygon by grid filter
// 0 out   1 in   2 need judge
  int interpior_point_in_polygon(Point p,int poly_id) {
	//horizon & vertical order
	int hori = (int)((p.x - polygon_bound[poly_id][0])/polygon_slice[poly_id][0]);
	int verti = (int)((p.y - polygon_bound[poly_id][2])/polygon_slice[poly_id][1]);
	//if it is sstatus "in"
	return grid_status[poly_id][hori][verti];
}
/*
//if a point is out the polygon by grid filter
  int interpior_point_out_polygon(Point p,int poly_id) {
	//horizon & vertical order
	int hori = (int)((p.x - polygon_bound[poly_id][0])/polygon_slice[poly_id][0]);
	int verti = (int)((p.y - polygon_bound[poly_id][2])/polygon_slice[poly_id][1]);
	//if it is sstatus "in"
	return (grid_status[poly_id][hori][verti] == 0);
}
*/

//calculate the grid status
  int interval_cross(double x1,double y1,double x2,double y2,double x3,double y3,double x4,double y4){
	double d1, d2, d3, d4, d5, d6;
	double cross1, cross2, cross3, cross4;

	d1 = x1 - x3; d2 = y1 - y3; d3 = x4 - x3; d4 = y4 - y3; d5 = x2 - x3; d6 = y2 - y3;
	cross1 = d1 * d4 - d2 * d3; cross2 = d5 * d4 - d6 * d3;
	if (cross1 * cross2 >= 0) return 0;
	
	d5 = -d1; d6 = -d2; d1 = x4 - x1; d2 = y4 - y1; d3 = x2 - x1; d4 = y2 - y1;
	cross3 = d1 * d4 - d2 * d3; cross4 = d5 * d4 - d6 * d3;
	if (cross3 * cross4 >= 0) return 0;

	return 1;
}

void gridPreprocess() {
	
	int i,j,it,it2,it3,it1,tot;
	maxLon = totalmaxy; maxLat = totalmaxx;
	minLon = totalminy; minLat = totalminx;
	totalmaxx -= totalminx ; totalmaxx = totalmaxx / 10.0 ;	//分度值
	totalmaxy -= totalminy ; totalmaxy = totalmaxy / 10.0 ;	//分度值
	memset(point_distribute , 0 , sizeof(point_distribute));
	for (i = 0 ; i < number_of_points ; i++){
		it = (int)( (point[i].x-totalminx)/totalmaxx) ;
		it2 = (int)( (point[i].y-totalminy)/totalmaxy) ;
		if (it >= 0 && it <= 9 && it2 >= 0 && it2 <= 9)
		point_distribute[it][it2] ++ ;
		//fprintf(stderr , "%d %d\n" ,it , it2);
	}
	
	//累计和
	for (i = 1; i < 10; ++i) {
		point_distribute[i][0] += point_distribute[i-1][0];
		point_distribute[0][i] += point_distribute[0][i-1];
	}
	for (i = 1; i < 10; ++i)
		for (j = 1; j < 10; ++j)
			point_distribute[i][j] += point_distribute[i-1][j] + point_distribute[i][j-1] - point_distribute[i-1][j-1];
	
}

void interior_approximating(int flag_thread) {
	int i,j,it,it2,it3,it1,tot;
	int poly_point_number;
	double area;
	int GRID_SLICE2;
	
	int piece = number_of_polygons / 4;
	int loopStart = piece * flag_thread;
	int loopEnd = piece * (flag_thread+1);
	if (flag_thread == 3) loopEnd = number_of_polygons;
	
	for (i = loopStart; i < loopEnd; i++) {
		double dx,dy;
		double eva;
		int k,l;
		int k2;
		///Outer Poly Section
		// init polygon bound
		polygon_bound[i][0]=polygon_bound[i][2]= 1e100;
		polygon_bound[i][1]=polygon_bound[i][3]= -1e100;
		
		poly_point_number = 0;
		
		//get exterior circle point id list
		for (it = first_exterior_point[i];it != -1; it = next_exterior_point[it]) {
			point_id_list[flag_thread][poly_point_number] = it;
			if (polygon_bound[i][0] > exterior_point[it].x) polygon_bound[i][0] = exterior_point[it].x;
			if (polygon_bound[i][1] < exterior_point[it].x) polygon_bound[i][1] = exterior_point[it].x;
			if (polygon_bound[i][2] > exterior_point[it].y) polygon_bound[i][2] = exterior_point[it].y;
			if (polygon_bound[i][3] < exterior_point[it].y) polygon_bound[i][3] = exterior_point[it].y;
			++poly_point_number;
		}
		
		it = (int)( (polygon_bound[i][0]-totalminx)/totalmaxx);
		it1 = (int)( (polygon_bound[i][1]-totalminx)/totalmaxx);
		it2 = (int)( (polygon_bound[i][2]-totalminy)/totalmaxy);
		it3 = (int)( (polygon_bound[i][3]-totalminy)/totalmaxy);
		--it;--it2;
		area = (it1-it)*(it3-it2)*totalmaxx*totalmaxy;
		tot = point_distribute[it1][it3];
		if (it > -1) tot -= point_distribute[it][it3];
		if (it2 > -1) tot -=point_distribute[it1][it2];
		if (it > -1 && it2 > -1) tot +=point_distribute[it][it2];
		eva = tot*(polygon_bound[i][1] - polygon_bound[i][0])*(polygon_bound[i][3] - polygon_bound[i][2])/area;
		//printf("[%d %.4lf]\t",i,eva);
		GRID_SLICE2 = eva / 500 - 4;
		if (GRID_SLICE2 < 8) GRID_SLICE2 = 8;
		if (GRID_SLICE2 > 16) GRID_SLICE2 = 16;
		if (GRID_SLICE2 % 2) --GRID_SLICE2;
		//printf("%d\t",GRID_SLICE2);
		

		//GRID_SLICE2 = 8;

		dx = (polygon_bound[i][1] - polygon_bound[i][0]) / GRID_SLICE2;
		dy = (polygon_bound[i][3] - polygon_bound[i][2]) / GRID_SLICE2;
		polygon_slice[i][0] = dx;
		polygon_slice[i][1]=dy;
		
		//intersect init
		memset(grid_line_intersect[flag_thread],0,sizeof(grid_line_intersect[flag_thread]));
		//cal intersect

		
		for (j = 0; j < poly_point_number - 1; ++j) {
			int hori = (int)((exterior_point[point_id_list[flag_thread][j]].x - polygon_bound[i][0])/dx);
			int verti = (int)((exterior_point[point_id_list[flag_thread][j]].y - polygon_bound[i][2])/dy);
			int hori2 = (int)((exterior_point[point_id_list[flag_thread][j+1]].x - polygon_bound[i][0])/dx);
			int verti2 = (int)((exterior_point[point_id_list[flag_thread][j+1]].y - polygon_bound[i][2])/dy);
			if (hori == GRID_SLICE2) --hori;
			if (hori2 == GRID_SLICE2) --hori2;
			if (verti == GRID_SLICE2) --verti;
			if (verti2 == GRID_SLICE2) --verti2;
			
			//if (i == 0 && (hori==hori2 || verti == verti2))
			//printf("%d: %d %d %d %d\n",j,hori, verti, hori2, verti2);
			
			if (hori == hori2) {
				if (verti > verti2) {verti ^=verti2;verti2^=verti;verti^=verti2;}
				for (k = verti;k<=verti2;++k) {
					grid_line_intersect[flag_thread][hori][k][j]=1;
					grid_status[i][hori][k]=2;
				}
			}else if (verti == verti2) {
				if (hori > hori2) {hori ^=hori2;hori2^=hori;hori^=hori2;}
				for (k = hori;k<=hori2;++k) {
					grid_line_intersect[flag_thread][k][verti][j]=1;
					grid_status[i][k][verti]=2;
				}
			}else {
				grid_line_intersect[flag_thread][hori][verti][j]=1;
				grid_status[i][hori][verti]=2;
				
				grid_line_intersect[flag_thread][hori2][verti2][j]=1;
				grid_status[i][hori2][verti2]=2;
				
				if (verti > verti2) {verti ^=verti2;verti2^=verti;verti^=verti2;}
				if (hori > hori2) {hori ^=hori2;hori2^=hori;hori^=hori2;}
				
				for (k = hori;k<=hori2;++k){
					for (l = verti;l<=verti2;++l) { 
						
						double mix = polygon_bound[i][0]+k*dx;
						double miy = polygon_bound[i][2]+l*dy;
						if (interval_cross(mix,miy,mix+dx,miy+dy,exterior_point[point_id_list[flag_thread][j]].x,exterior_point[point_id_list[flag_thread][j]].y,exterior_point[point_id_list[flag_thread][j+1]].x,exterior_point[point_id_list[flag_thread][j+1]].y)
						||  interval_cross(mix+dx,miy,mix,miy+dy,exterior_point[point_id_list[flag_thread][j]].x,exterior_point[point_id_list[flag_thread][j]].y,exterior_point[point_id_list[flag_thread][j+1]].x,exterior_point[point_id_list[flag_thread][j+1]].y)){
							grid_line_intersect[flag_thread][k][l][j]=1;
							grid_status[i][k][l]=2;
						}
					}
				}
				
				
			}
			
		}
		
		//四个方向的累计
		k2 = GRID_SLICE2 / 2 + GRID_SLICE2 % 2;
		
		//向左
		for (k = 1; k < k2; ++k) {
			int spt = GRID_SLICE2 - k;
			for (l = k; l < spt; ++l)
				for (j = 0; j < poly_point_number - 1; ++j)
					grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k-1][l][j];
		}
		
		//向上
		for (l = GRID_SLICE2 - 2; l >= k2; --l)
			for (k = GRID_SLICE2 -l; k <=l; ++k)
				for (j = 0; j < poly_point_number - 1; ++j)
					grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k][l+1][j];
		
		//向右
		for (k = GRID_SLICE2-2; k >k2 ; --k) {
			for (l = GRID_SLICE2 - k; l < k; ++l)
				for (j = 0; j < poly_point_number - 1; ++j)
					grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k+1][l][j];
		}
		
		//向下
		for (l = 1; l < k2; ++l)
			for (k = l+1; k <GRID_SLICE2 -l; ++k)
				for (j = 0; j < poly_point_number - 1; ++j)
					grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k][l-1][j];

		for (k = 0; k < GRID_SLICE2; ++k)
			for (l = 0; l < GRID_SLICE2; ++l) if (grid_status[i][k][l]<2){
				double midx,midy;
				//bounder without cross lines
				if (k ==0 || l==0 || k == GRID_SLICE2-1 || l == GRID_SLICE2-1) continue;
				
				//center coordinate
				midx = polygon_bound[i][0]+(k+0.5)*dx;
				midy = polygon_bound[i][2]+(l+0.5)*dy;
				
				//4 areas
				if (k<=l) {
					//to left
					if (k+l < GRID_SLICE2) {
						
						int p;
						int flag;
						//get all possible cross
						for (j = 0; j < poly_point_number - 1; ++j)
							if (grid_line_intersect[flag_thread][k][l][j])
								if (exterior_point[point_id_list[flag_thread][j]].y < midy && exterior_point[point_id_list[flag_thread][j+1]].y > midy
								||	exterior_point[point_id_list[flag_thread][j]].y > midy && exterior_point[point_id_list[flag_thread][j+1]].y < midy) //
									grid_status[i][k][l]^=1;
						
					}else {
					//to top
						int p;
						int flag;
						//get all possible cross
						//judge cross
						for (j = 0; j < poly_point_number - 1; ++j)
							if (grid_line_intersect[flag_thread][k][l][j])
								if (exterior_point[point_id_list[flag_thread][j]].x < midx && exterior_point[point_id_list[flag_thread][j+1]].x > midx
								||	exterior_point[point_id_list[flag_thread][j]].x > midx && exterior_point[point_id_list[flag_thread][j+1]].x < midx)
									grid_status[i][k][l]^=1;
						
					}
				}else {
					//to bottom
					if (k+l<GRID_SLICE2){
						int p;int flag;
						//get all possible cross
						//judge cross
						for (j = 0; j < poly_point_number - 1; ++j)
							if (grid_line_intersect[flag_thread][k][l][j])
								if (exterior_point[point_id_list[flag_thread][j]].x < midx && exterior_point[point_id_list[flag_thread][j+1]].x > midx
								||	exterior_point[point_id_list[flag_thread][j]].x > midx && exterior_point[point_id_list[flag_thread][j+1]].x < midx)
									grid_status[i][k][l]^=1;
					}else {
					//to right
						int p;int flag;
						//get all possible cross
						//judge cross
						for (j = 0; j < poly_point_number - 1; ++j)
							if (grid_line_intersect[flag_thread][k][l][j])
								if (exterior_point[point_id_list[flag_thread][j]].y < midy && exterior_point[point_id_list[flag_thread][j+1]].y > midy
								||	exterior_point[point_id_list[flag_thread][j]].y > midy && exterior_point[point_id_list[flag_thread][j+1]].y < midy)
									grid_status[i][k][l]^=1;
						
					}
				}
			}

		
		///Inner Poly Section
		for (it2 = first_interior_region[i]; it2 != -1; it2 = next_interior_region[it2]) {
			int j,k,l;
			poly_point_number = 0;
			
			for (it = first_interior_point[it2];it != -1; it = next_interior_point[it]) {
				point_id_list[flag_thread][poly_point_number] = it;
				++poly_point_number;
			}
			
			//intersect init
			memset(grid_line_intersect[flag_thread],0,sizeof(grid_line_intersect[flag_thread]));
			//cal intersect

			
			for (j = 0; j < poly_point_number - 1; ++j) {
				int hori = (int)((interior_point[point_id_list[flag_thread][j]].x - polygon_bound[i][0])/dx);
				int verti = (int)((interior_point[point_id_list[flag_thread][j]].y - polygon_bound[i][2])/dy);
				int hori2 = (int)((interior_point[point_id_list[flag_thread][j+1]].x - polygon_bound[i][0])/dx);
				int verti2 = (int)((interior_point[point_id_list[flag_thread][j+1]].y - polygon_bound[i][2])/dy);
				if (hori == GRID_SLICE2) --hori;
				if (hori2 == GRID_SLICE2) --hori2;
				if (verti == GRID_SLICE2) --verti;
				if (verti2 == GRID_SLICE2) --verti2;
				
				if (hori == hori2) {
					if (verti > verti2) {verti ^=verti2;verti2^=verti;verti^=verti2;}
					for (k = verti;k<=verti2;++k) {
						grid_line_intersect[flag_thread][hori][k][j]=1;
						grid_status[i][hori][k]=2;
					}
				}else if (verti == verti2) {
					if (hori > hori2) {hori ^=hori2;hori2^=hori;hori^=hori2;}
					for (k = hori;k<=hori2;++k) {
						grid_line_intersect[flag_thread][k][verti][j]=1;
						grid_status[i][k][verti]=2;
					}
				}else {
					grid_line_intersect[flag_thread][hori][verti][j]=1;
					grid_status[i][hori][verti]=2;
						
					grid_line_intersect[flag_thread][hori2][verti2][j]=1;
					grid_status[i][hori2][verti2]=2;
						
					if (verti > verti2) {verti ^=verti2;verti2^=verti;verti^=verti2;}
					if (hori > hori2) {hori ^=hori2;hori2^=hori;hori^=hori2;}
					
					for (k = hori;k<=hori2;++k){
						for (l = verti;l<=verti2;++l){ 
							double mix = polygon_bound[i][0]+k*dx;
							double miy = polygon_bound[i][2]+l*dy;
							if (interval_cross(mix,miy,mix+dx,miy+dy,interior_point[point_id_list[flag_thread][j]].x,interior_point[point_id_list[flag_thread][j]].y,interior_point[point_id_list[flag_thread][j+1]].x,interior_point[point_id_list[flag_thread][j+1]].y)
							||  interval_cross(mix+dx,miy,mix,miy+dy,interior_point[point_id_list[flag_thread][j]].x,interior_point[point_id_list[flag_thread][j]].y,interior_point[point_id_list[flag_thread][j+1]].x,interior_point[point_id_list[flag_thread][j+1]].y)){
								grid_line_intersect[flag_thread][k][l][j]=1;
								grid_status[i][k][l]=2;
							}
						}
					}
				}
				
			}
			
			
			//四个方向的累计
			k2 = GRID_SLICE2 / 2 + GRID_SLICE2 % 2;
			
			//向左
			for (k = 1; k < k2; ++k) {
				int spt = GRID_SLICE2 - k;
				for (l = k; l < spt; ++l)
					for (j = 0; j < poly_point_number - 1; ++j)
						grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k-1][l][j];
			}
			
			//向上
			for (l = GRID_SLICE2 - 2; l >= k2; --l)
				for (k = GRID_SLICE2 -l; k <=l; ++k)
					for (j = 0; j < poly_point_number - 1; ++j)
						grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k][l+1][j];
			
			//向右
			for (k = GRID_SLICE2-2; k >k2 ; --k) {
				for (l = GRID_SLICE2 - k; l < k; ++l)
					for (j = 0; j < poly_point_number - 1; ++j)
						grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k+1][l][j];
			}
			
			//向下
			for (l = 1; l < k2; ++l)
				for (k = l+1; k <GRID_SLICE2 -l; ++k)
					for (j = 0; j < poly_point_number - 1; ++j)
						grid_line_intersect[flag_thread][k][l][j] |= grid_line_intersect[flag_thread][k][l-1][j];
			
			for (k = 0; k < GRID_SLICE2; ++k)
				for (l = 0; l < GRID_SLICE2; ++l) if (grid_status[i][k][l]<2){
					double midx;
					double midy;
					//bounder without cross lines
					if (k ==0 || l==0 || k == GRID_SLICE2-1 || l == GRID_SLICE2-1) continue;
					
					//center coordinate
					midx = polygon_bound[i][0]+(k+0.5)*dx;
					midy = polygon_bound[i][2]+(l+0.5)*dy;
					
					//4 areas
					if (k<=l) {
						//to left
						if (k+l < GRID_SLICE2) {
							
							int p;int flag;
							//delta ebsilon

							//judge cross
							for (j = 0; j < poly_point_number - 1; ++j)
								if (grid_line_intersect[flag_thread][k][l][j])
									if (interior_point[point_id_list[flag_thread][j]].y < midy && interior_point[point_id_list[flag_thread][j+1]].y > midy
									||	interior_point[point_id_list[flag_thread][j]].y > midy && interior_point[point_id_list[flag_thread][j+1]].y < midy) //
										grid_status[i][k][l]^=1;
							
						}else {
						//to top
							int p;int flag;
							//delta ebsilon

							//judge cross
							for (j = 0; j < poly_point_number - 1; ++j)
								if (grid_line_intersect[flag_thread][k][l][j])
									if (interior_point[point_id_list[flag_thread][j]].x < midx && interior_point[point_id_list[flag_thread][j+1]].x > midx
									||	interior_point[point_id_list[flag_thread][j]].x > midx && interior_point[point_id_list[flag_thread][j+1]].x < midx)
										grid_status[i][k][l]^=1;
							
						}
					}else {
						//to bottom
						if (k+l<GRID_SLICE2){
							int p;int flag;


							//judge cross
							for (j = 0; j < poly_point_number - 1; ++j)
								if (grid_line_intersect[flag_thread][k][l][j])
									if (interior_point[point_id_list[flag_thread][j]].x < midx && interior_point[point_id_list[flag_thread][j+1]].x > midx
									||	interior_point[point_id_list[flag_thread][j]].x > midx && interior_point[point_id_list[flag_thread][j+1]].x < midx)
										grid_status[i][k][l]^=1;
						}else {
						//to right
							int p;int flag;

							//judge cross
							for (j = 0; j < poly_point_number - 1; ++j)
								if (grid_line_intersect[flag_thread][k][l][j])
									if (interior_point[point_id_list[flag_thread][j]].y < midy && interior_point[point_id_list[flag_thread][j+1]].y > midy
									||	interior_point[point_id_list[flag_thread][j]].y > midy && interior_point[point_id_list[flag_thread][j+1]].y < midy)
										grid_status[i][k][l]^=1;
							
						}
					}
				}
			
		}
	}
	
	/*
	//输出结果
	for (i = 0; i < number_of_polygons; i++) {
		printf("Polygon %d:\n",i);
		int j,k;
		for (j = 0; j < GRID_SLICE2; ++j){
			for (k = 0; k < GRID_SLICE2; ++k)
				printf("%d ",grid_status[i][k][j]);
			printf("\n");
		}
		printf("\n");
	}*/
}
/** End Interior Appproximation Section*/

void grid1() {
	interior_approximating(0);
	flag_grid1 = 1;
	_endthread();
}
void grid2() {
	interior_approximating(1);
	flag_grid2 = 1;
	_endthread();
}
void grid3() {
	interior_approximating(2);
	flag_grid3 = 1;
	_endthread();
}
void grid4() {
	interior_approximating(3);
	flag_grid4 = 1;
	_endthread();
}

void interior_approximate(){
	int i,j,k;
	
	gridPreprocess();
	
	_beginthread(grid1, 0, NULL);
	_beginthread(grid2, 0, NULL);
    _beginthread(grid3, 0, NULL);
    _beginthread(grid4, 0, NULL);

	while(flag_grid1==0) Sleep(25);
    while(flag_grid2==0) Sleep(25);
    while(flag_grid3==0) Sleep(25);
    while(flag_grid4==0) Sleep(25);
	/*
	for (i = 0; i < 100; ++i) {
		for (j = 0; j < GRID_SLICE; ++j) {
			for (k = 0; k < GRID_SLICE; ++k)
				printf("%d\t",grid_status[i][k][j]);
			puts("");
		}
		puts("");
	}*/
}