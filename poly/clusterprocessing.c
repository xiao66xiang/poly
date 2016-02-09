#include "define.h"
void processing_points(){
     int i ;
     memset(point_num , 0, sizeof(point_num)) ;
     for (i = 0 ; i < number_of_points ; i++)
         point_num[point_id[i]+1]++ ;
     for (i = 1 ; i < number_of_points ; i++)
         point_num[i] += point_num[i-1] ;
     for (i = 1 ; i < number_of_points ; i++)
         point_cnt[i] = point_num[i] ;
     for (i = 0 ; i < number_of_points ; i++){

         point_id_cluster[point_cnt[point_id[i]]] = i ;
         point_cnt[point_id[i]] ++ ;
     }
	 /* number_of_processing = -1 ;
     for (i = 0 ; i < number_of_points - 1 ; i++) if (point_num[i+1] - point_num[i] > 0){
         point_id_first[++number_of_processing] = point_num[i] ;
     }
     if (number_of_points  > point_num[number_of_points-1]){//MaybeError
        point_id_first[++number_of_processing] = number_of_points - 1 - point_num[number_of_points-1] ;
     }
     freopen("testing.out" , "w" , stdout);
     int j = 0 ;
     number_of_processing ++ ;
     for (i = 0 ; i < number_of_points ; i++){
         if (point_id_first[j] == i)
         { printf("%d\n" , point_id[point_id_cluster[i]]);j++ ;}
         printf("%d %d\n" , i , point_id[point_id_cluster[i]]);
     }*/
}

//relative direction from the previous point
int get_direction(double x1, double y1, double x2, double y2) {
	int d = 0;
	double dx = x2 - x1 ;
	double dy = y2 - y1 ;
	if (dx >=0) {
		if (dy >=0) d += 1;
		if (dy <=0) d += 8;
	}
	if (dx <=0) {
		if (dy >=0) d += 2;
		if (dy <=0) d += 4;
	}
	return d;
}

//parse trajectory with same Id into sub trajectories
void parse_id_cluster() {
	int i = 0;
	int id = -1;
	int direction;
	int time_segment;
	int flag = 1;
    inteval_number = 0 ;
  //  polygon_sequence[1] = 1000000 ;
	for (i = 0; i < number_of_points; ++i) {
		//new segment
		if (point_id[point_id_cluster[i]] > id) {
			id = point_id[point_id_cluster[i]];
			inteval_start[inteval_number] = i;
			time_segment = 0;
			while (time_segment < number_of_polygons && polygon_sequence[time_segment]<= point_sequence[point_id_cluster[i]])
				++time_segment;
			inteval_time[inteval_number] = point_sequence[point_id_cluster[i]];
			++inteval_number;
			direction = 15;
			flag = 1;
		}else {
			// old segment
			direction = (direction & (get_direction(point[point_id_cluster[i-1]].x,point[point_id_cluster[i-1]].y,point[point_id_cluster[i]].x,point[point_id_cluster[i]].y)));
			while (time_segment < number_of_polygons && polygon_sequence[time_segment]<= point_sequence[point_id_cluster[i]]) {
				++time_segment;
				flag = 0;
			}
			//direction = 1 ;
			if (direction == 0 || (!flag)) {
             //  if (!flag){
			//	if (1){
				inteval_start[inteval_number] = i;
				inteval_time[inteval_number] = point_sequence[point_id_cluster[i]];
				++inteval_number;
				direction = 15;
				flag = 1;
			}
		}
	}
	inteval_start[inteval_number] = number_of_points;
}

