#include <stdlib.h>
#include <string.h>
#include "define.h"

/*
double min(double a , double b){
       if (a < b) return a ;
       return b ;
}
double max(double a , double b){
       if (a > b) return a ;
       return b ;
}
*/

void build_edge_R_Tree(){
     int i , j ,k ;
       int nowcnt = 0 , lastcnt = 0 ;
     for (i = 0 ; i < number_of_polygons ; i++){
         int cntpoint = first_exterior_point[i] ;
		 lastcnt = nowcnt ; 
         for (j = first_exterior_point[i] ; j != - 1 ; j = next_exterior_point[j]){
             nowcnt++ ;
             memset(exteriorRTree[nowcnt].next , 0 ,sizeof(exteriorRTree[nowcnt].next)) ;
            // exteriorRTree[i][nowcnt].next[1]=exteriorRTree[i][nowcnt].next[2]=exteriorRTree[i][nowcnt].next[3]=exteriorRTree[i][nowcnt].next[4]=0;
             exteriorRTree[nowcnt].lable = 0 ;
             if (next_exterior_point[j] == -1) exteriorRTree[nowcnt].edPoint = cntpoint ;
             else exteriorRTree[nowcnt].edPoint = next_exterior_point[j] ; 
             exteriorRTree[nowcnt].stPoint = j ;
             exteriorRTree[nowcnt].stx = min(exterior_point[exteriorRTree[nowcnt].stPoint].x , exterior_point[exteriorRTree[nowcnt].edPoint].x);
             exteriorRTree[nowcnt].edx = max(exterior_point[exteriorRTree[nowcnt].stPoint].x , exterior_point[exteriorRTree[nowcnt].edPoint].x);
             exteriorRTree[nowcnt].sty = min(exterior_point[exteriorRTree[nowcnt].stPoint].y , exterior_point[exteriorRTree[nowcnt].edPoint].y);
             exteriorRTree[nowcnt].edy = max(exterior_point[exteriorRTree[nowcnt].stPoint].y , exterior_point[exteriorRTree[nowcnt].edPoint].y);
         }//exteriorRTreeStart[i] = nowcnt ;
         while (nowcnt - lastcnt > 1){
               int tcnt = nowcnt ;
               if (nowcnt - lastcnt <= 5){
                          nowcnt ++ ;
                          exteriorRTree[nowcnt] = exteriorRTree[lastcnt+1] ;
                          memset(exteriorRTree[nowcnt].next , 0 ,sizeof(exteriorRTree[nowcnt].next)) ;
                          exteriorRTree[nowcnt].next[0] = lastcnt+1 ;
                          exteriorRTree[nowcnt].lable = 1 ;
                          for (k = lastcnt + 2 ; k < nowcnt ; k++){
                               exteriorRTree[nowcnt].next[k-lastcnt-1] = k ;
                               exteriorRTree[nowcnt].stx = min(exteriorRTree[nowcnt].stx , exteriorRTree[k].stx);
                               exteriorRTree[nowcnt].edx = max(exteriorRTree[nowcnt].edx , exteriorRTree[k].edx);
                               exteriorRTree[nowcnt].sty = min(exteriorRTree[nowcnt].sty , exteriorRTree[k].sty);
                               exteriorRTree[nowcnt].edy = max(exteriorRTree[nowcnt].edy , exteriorRTree[k].edy);
                          }
                          exteriorRTreeStart[i] = nowcnt ;
                          break ;
               }
               while (tcnt - lastcnt >= 5){
                     nowcnt++ ;
                     exteriorRTree[nowcnt] = exteriorRTree[lastcnt+1] ;
                          memset(exteriorRTree[nowcnt].next , 0 ,sizeof(exteriorRTree[nowcnt].next)) ;
                          exteriorRTree[nowcnt].next[0] = lastcnt+1 ;
                          exteriorRTree[nowcnt].lable = 1 ;
                          for (k = lastcnt + 2 ; k <= lastcnt + 5 ; k++){
                               exteriorRTree[nowcnt].next[k-lastcnt-1] = k ;
                               exteriorRTree[nowcnt].stx = min(exteriorRTree[nowcnt].stx , exteriorRTree[k].stx);
                               exteriorRTree[nowcnt].edx = max(exteriorRTree[nowcnt].edx , exteriorRTree[k].edx);
                               exteriorRTree[nowcnt].sty = min(exteriorRTree[nowcnt].sty , exteriorRTree[k].sty);
                               exteriorRTree[nowcnt].edy = max(exteriorRTree[nowcnt].edy , exteriorRTree[k].edy);
                          }
                     lastcnt += 5 ;
               }
         }
     }
     
     for (i = 0 ; i < number_of_interior_regions ; i++){
         int cntpoint = first_interior_point[i] ;
		 lastcnt = nowcnt ; 
         for (j = first_interior_point[i] ; j != - 1 ; j = next_interior_point[j]){
             nowcnt++ ;
             memset(interiorRTree[nowcnt].next , 0 ,sizeof(interiorRTree[nowcnt].next)) ;
            // interiorRTree[i][nowcnt].next[1]=interiorRTree[i][nowcnt].next[2]=interiorRTree[i][nowcnt].next[3]=interiorRTree[i][nowcnt].next[4]=0;
             interiorRTree[nowcnt].lable = 0 ;
             if (next_interior_point[j] == -1) interiorRTree[nowcnt].edPoint = cntpoint ;
             else interiorRTree[nowcnt].edPoint = next_interior_point[j] ; 
             interiorRTree[nowcnt].stPoint = j ;
             interiorRTree[nowcnt].stx = min(interior_point[interiorRTree[nowcnt].stPoint].x , interior_point[interiorRTree[nowcnt].edPoint].x);
             interiorRTree[nowcnt].edx = max(interior_point[interiorRTree[nowcnt].stPoint].x , interior_point[interiorRTree[nowcnt].edPoint].x);
             interiorRTree[nowcnt].sty = min(interior_point[interiorRTree[nowcnt].stPoint].y , interior_point[interiorRTree[nowcnt].edPoint].y);
             interiorRTree[nowcnt].edy = max(interior_point[interiorRTree[nowcnt].stPoint].y , interior_point[interiorRTree[nowcnt].edPoint].y);
         }//interiorRTreeStart[i] = nowcnt ;
         while (nowcnt - lastcnt > 1){
               int tcnt = nowcnt ;
               if (nowcnt - lastcnt <= 5){
                          nowcnt ++ ;
                          interiorRTree[nowcnt] = interiorRTree[lastcnt+1] ;
                          memset(interiorRTree[nowcnt].next , 0 ,sizeof(interiorRTree[nowcnt].next)) ;
                          interiorRTree[nowcnt].next[0] = lastcnt+1 ;
                          interiorRTree[nowcnt].lable = 1 ;
                          for (k = lastcnt + 2 ; k < nowcnt ; k++){
                               interiorRTree[nowcnt].next[k-lastcnt-1] = k ;
                               interiorRTree[nowcnt].stx = min(interiorRTree[nowcnt].stx , interiorRTree[k].stx);
                               interiorRTree[nowcnt].edx = max(interiorRTree[nowcnt].edx , interiorRTree[k].edx);
                               interiorRTree[nowcnt].sty = min(interiorRTree[nowcnt].sty , interiorRTree[k].sty);
                               interiorRTree[nowcnt].edy = max(interiorRTree[nowcnt].edy , interiorRTree[k].edy);
                          }
                          interiorRTreeStart[i] = nowcnt ;
                          break ;
               }
               while (tcnt - lastcnt >= 5){
                     nowcnt++ ;
                     interiorRTree[nowcnt] = interiorRTree[lastcnt+1] ;
                          memset(interiorRTree[nowcnt].next , 0 ,sizeof(interiorRTree[nowcnt].next)) ;
                          interiorRTree[nowcnt].next[0] = lastcnt+1 ;
                          interiorRTree[nowcnt].lable = 1 ;
                          for (k = lastcnt + 2 ; k <= lastcnt + 5 ; k++){
                               interiorRTree[nowcnt].next[k-lastcnt-1] = k ;
                               interiorRTree[nowcnt].stx = min(interiorRTree[nowcnt].stx , interiorRTree[k].stx);
                               interiorRTree[nowcnt].edx = max(interiorRTree[nowcnt].edx , interiorRTree[k].edx);
                               interiorRTree[nowcnt].sty = min(interiorRTree[nowcnt].sty , interiorRTree[k].sty);
                               interiorRTree[nowcnt].edy = max(interiorRTree[nowcnt].edy , interiorRTree[k].edy);
                          }
                     lastcnt += 5 ;
               }     
         }                
     }
      
}
