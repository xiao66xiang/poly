#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "define.h"
#include "interior.h"
#include "polyapproximate.h"
#include "file.h"
//#include "RTree_SZQ.h"
#include "core.h"
#include "innerrtree.h"
#include "clusterprocessing.h"
#include <windows.h>
#include <process.h>

int main(int argc, char *argv[]) {
	/** read commands, or just replace by codes below for shell reading */
/*	strcpy(request_cmd, argv[1]);
	if (argc == 5) {
		strcpy(point_input_file_path, argv[2]);
		strcpy(polygon_input_file_path, argv[3]);
		strcpy(output_file_path, argv[4]);
    } else {
		strcpy(requst_dist_cmd, argv[2]);
		//request_dist = trans_char_to_int(request_dist_cmd); // undo
		strcpy(point_input_file_path, argv[3]);
		strcpy(polygon_input_file_path, argv[4]);
		strcpy(output_file_path, argv[5]);
    }*/

    /** shell reading */
    /*scanf("%s", request_cmd);
    if (!strcmp(request_cmd, "WITHIN")) scanf("%s", requst_dist_cmd);
    scanf("%s", point_input_file_path);
    scanf("%s", polygon_input_file_path);
    scanf("%s", output_file_path);*/

    /** directly set paths (practice) */
	clock_t  clockBegin, clockEnd;

    argType* argNode0,* argNode1,* argNode2,* argNode3;
	if(argc<5) return 0;
	if(argv[1][0]=='I')
	{
	if(argc<5) return 0;
	if(argc>5) return 0;
	strcpy(request_cmd, argv[1]);
	strcpy(point_input_file_path, argv[2]);
	strcpy(polygon_input_file_path, argv[3]);
	strcpy(output_file_path, argv[4]);
	}else if(argv[1][0]=='W')
	{
	if(argc<6) return 0;
	if(argc>6) return 0;
	strcpy(request_cmd, argv[1]);
	request_dist=atof(argv[2]);
	strcpy(point_input_file_path, argv[3]);
	strcpy(polygon_input_file_path, argv[4]);
	strcpy(output_file_path, argv[5]);
	}else return 0;
	totalmaxx = -100000000.0 ; totalminx = 1000000000.0 ;
	totalmaxy = -100000000.0 ; totalminy = 1000000000.0 ;
/*
	fprintf(stderr,"cmd:");
	scanf("%s",request_cmd);
	fprintf(stderr,"points:");
	scanf("%s",point_input_file_path);
	fprintf(stderr,"poly:");
	scanf("%s",polygon_input_file_path);
	fprintf(stderr,"out:");
	scanf("%s",output_file_path);
*/
    argNode0=(argType*)malloc(sizeof(argType));
    argNode1=(argType*)malloc(sizeof(argType));
    argNode2=(argType*)malloc(sizeof(argType));
    argNode3=(argType*)malloc(sizeof(argType));

    clockBegin = clock();
	/** read points */

	read_points_threading(); // read document "points500.txt"(7.4M) 100 times, costs 3.0s
	/** read polygons */
	read_polygons_threading(); // read document "polygon10.txt"(253KB) 100 times, costs 0.170s

    clockEnd = clock();
 //   printf("%dms\n", clockEnd - clockBegin);
//	fprintf(stderr,"poly:%d\n",number_of_polygons);
    clockBegin = clock();
    //approximate_polygons();
	

	pre_do_MBR();

	interior_approximate();
	
	
	//Inner R-Tree
	build_edge_R_Tree() ;
    //Clustering
    processing_points() ;
//	parse_id_cluster();
	/** Main (to be complete) */
	initRtree();
	//fprintf(stderr,"RT:%d\n",ALLRTree);
	polygon_sequence[number_of_polygons]=987654321;
    freopen(output_file_path, "w" , stdout);

    argNode0->ProNum=0;
    argNode0->stP=0;
    argNode0->enP=interval_number/4;

    argNode1->ProNum=1;
    argNode1->stP=interval_number/4+1;
    argNode1->enP=interval_number/4*2;

    argNode2->ProNum=2;
    argNode2->stP=interval_number/4*2+1;
    argNode2->enP=interval_number/4*3;

    argNode3->ProNum=3;
    argNode3->stP=interval_number/4*3+1;
    argNode3->enP=interval_number-1;

    clockEnd=clock();
 //   fprintf(stderr,"%dms\n", clockEnd - clockBegin);
    clockBegin = clock();

	_beginthread(Solve, 0, (void*)argNode0);
	_beginthread(Solve, 0, (void*)argNode1);
	_beginthread(Solve, 0, (void*)argNode2);
	_beginthread(Solve, 0, (void*)argNode3);

    while(Done[0]==0) Sleep(25);
    while(Done[1]==0) Sleep(25);
    while(Done[2]==0) Sleep(25);
    while(Done[3]==0) Sleep(25);
	stottimes = stimes[1] + stimes[2] + stimes[3] + stimes[0] ;

	polygon_edge_process();
//	fprintf(stderr , "InnerRtreeUseTimes: %d\n" , stottimes);
//	fprintf(stderr , "InnerRtreeUsePercent : %.3lf\n", stottimes / (number_of_points*1.0));
	
//	for (stimes[1] = 0 ; stimes[1] <= 9 ; stimes[1]++){
//		for (stimes[2] = 0 ; stimes[2] <= 9 ; stimes[2]++) fprintf(stderr , "%d " , point_distribute[stimes[1]][stimes[2]]) ;
//		fprintf(stderr , "\n");
//	}
//    fprintf(stderr,"%d\n",ans_count[0]+ans_count[1]+ans_count[2]+ans_count[3]);
    clockEnd=clock();
  //  fprintf(stderr,"%dms\n", clockEnd - clockBegin);
	system("pause");
    return 0;
}
