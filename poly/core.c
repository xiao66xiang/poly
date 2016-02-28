#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "define.h"
#include "interior.h"
#include "Rtree_SZQ.h"
#include "clusterprocessing.h"
#include"stdbool.h"

struct GeoPoint
{
	double lat, lon;
};
struct pair
{
	int first, second;
};
int dcmp(double a , double b){
     if (fabs(a-b) <= eps) return 1 ;
     return 0 ;
}

int Ls , Rs ;
int gridWidth, gridHeight; double gridSizeDeg;
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
  double lenth(Point a,Point b){return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));}
double cross(Point a,Point b,Point c){return (b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);}
double dis(Point A , Point B , Point C){
	    double ax=lenth(A,B);
	    double bx=lenth(B,C);
        double cx=lenth(A,C);
        if (ax < request_dist) return ax ;
        if (cx < request_dist) return cx ;
        if(ax*ax+bx*bx<cx*cx||bx*bx+cx*cx<ax*ax) return min(ax, cx);
        return fabs(cross(A,B,C))/lenth(B,C) ;
}

/*
  double lenth(Point a,Point b){return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));}
double dis(Point c , Point b , Point a){
    double xi = (a.x - b.x);
    double yi = (a.y - b.y);

    double s1 = (a.y - b.y) * c.y + (a.x - b.x) * c.x ;
    double s2 = (a.x * b.y - a.y * b.x) ;
    double dlen = min(lenth(a , c) , lenth(c, b));

    if (dcmp(a.x , b.x)){
        if (dcmp(a.y , b.y)) return dlen ;
        if ( (c.y >= a.y && c.y <= b.y) || (c.y <=a.y && c.y >= b.y))
            return fabs(c.x-a.x);
        else return dlen ;
    }
    if (dcmp(a.y , b.y)){
        if ( (c.x >= a.x && c.x <= b.x) || (c.x <=a.x && c.x >=b.x)){
            return fabs(c.y - a.y);
        }
        else return dlen ;
    }
    double x4 = (s1 + s2) / (2 * (a.x-b.x)) ;
    double y4 = (s1 - s2) / (2 * (a.y-b.y)) ;
    if ( (x4 >= a.x && x4 <= b.x) || (x4 <=a.x && x4 >=b.x)){
            return sqrt( (c.x-x4) * (c.x-x4) + (c.y-y4) *(c.y-y4));
    }
    else return dlen ;
}*/


int on_line(double rx , double ry , double tx , double ty , double x2 , double y2){
     if (dcmp(rx , tx)){
          if (!dcmp(rx , x2)) return 0 ;
          if ( (y2 - ry + eps) * (ty - y2 + eps) > 0 ) return 1 ;
          return 0 ;
     }
     if (dcmp(ry , ty)){
          if (!dcmp(ry , y2)) return 0 ;
          if ( (x2 - rx + eps) * (tx - x2 + eps) > 0 ) return 1 ;
          return 0 ;
     }
     if (dcmp ( (ty - ry) * x2 , y2 * (tx - rx))) return 1 ;
     return 0 ;
}

int check_in_exterior_polygon(int ProNum , int x , double x2, double y2){
     int rects = 0 , on_top = 0 , on_below = 0 ;
     int i , p , q ;
	// return 0;
    // int times1 = 0 , times2 = 0 ;
  //   for (i = first_exterior_point[x] ; i != -1 ; i=next_exterior_point[i]){times2++ ;}
     p = 0 ; q = 1 ; exstack[ProNum][1] = exteriorRTreeStart[x] ;
        while (p != q){
              p++ ;
              if (( x2 > exteriorRTree[exstack[ProNum][p]].edx)  ||
                  (y2 < exteriorRTree[exstack[ProNum][p]].sty || y2 > exteriorRTree[exstack[ProNum][p]].edy))
              continue ;
              if (exteriorRTree[exstack[ProNum][p]].lable == 1){
                                                   for (i = 0 ; i <= 4 ; i++) if (exteriorRTree[exstack[ProNum][p]].next[i] != 0){
                                                       q++ ;
                                                       exstack[ProNum][q] = exteriorRTree[exstack[ProNum][p]].next[i] ;
                                                       }else break ;
                                                   continue ;
              } else {
            //         times1 ++ ;
              double rx = exterior_point[exteriorRTree[exstack[ProNum][p]].stPoint].x , ry = exterior_point[exteriorRTree[exstack[ProNum][p]].stPoint].y;
			  double tx = exterior_point[exteriorRTree[exstack[ProNum][p]].edPoint].x , ty = exterior_point[exteriorRTree[exstack[ProNum][p]].edPoint].y;
			  if ( dcmp(x2 , rx) && dcmp(y2 , ry)) return 1 ;
		      if (on_line(rx , ry ,tx , ty , x2 ,y2)) return 1 ;
              if (dcmp(ty ,y2)){ swap(tx, rx) ; swap (ty , ry) ;}
              if (dcmp(ry ,y2)){ //有一个点在延长线上
                 if (!dcmp(ty , y2)) //另一个点不在延长线上 如果都在那么不变
                    if (x2 <= rx)  //而且延长线上的的点在右边
                       if (ty > y2) on_top ++ ; //上方判定数量+
                       else on_below ++ ; //下方判定数量+
              }
              else {
                   if ( (ty - y2) * (y2 - ry) >= 0.0){ //一上一下
                      if (dcmp (tx , rx)){ //X重合
                         if (tx > x2) rects ++ ;
                      }
                      else {//ry是小的 ty是大的
                           if (!dcmp(ty , ry)){
						      if (ry > ty + eps){
							     swap(tx , rx) ;
							     swap(ty , ry);
                              }
				              if  (  (tx - rx) * (y2 - ry)  >= (x2 - rx) * (ty - ry))
                              rects++ ;
		                   }
	                  }
                   }
             } }
     }
   //  return times2 <= exstack[1] ;
     return ( (rects + (on_top & 1)) & 1) ;
}


int check_in_interior_polygon(int ProNum , int x , double x2, double y2){
     int rects = 0 , on_top = 0 , on_below = 0 ;  int i , p , q ;
	// return 0;
    // int times1 = 0 , times2 = 0 ;
  //   for (i = first_exterior_point[x] ; i != -1 ; i=next_exterior_point[i]){times2++ ;}
     p = 0 ; q = 1 ; instack[ProNum][1] = interiorRTreeStart[x] ;
        while (p != q){
              p++ ;
              if (( x2 > interiorRTree[instack[ProNum][p]].edx)  ||
                  (y2 < interiorRTree[instack[ProNum][p]].sty || y2 > interiorRTree[instack[ProNum][p]].edy))
              continue ;
              if (interiorRTree[instack[ProNum][p]].lable == 1){
                                                   for (i = 0 ; i <= 4 ; i++) if (interiorRTree[instack[ProNum][p]].next[i] != 0){
                                                       q++ ;
                                                       instack[ProNum][q] = interiorRTree[instack[ProNum][p]].next[i] ;
                                                       }else break ;
                                                   continue ;
              } else {
              double rx = interior_point[interiorRTree[instack[ProNum][p]].stPoint].x , ry = interior_point[interiorRTree[instack[ProNum][p]].stPoint].y;
			  double tx = interior_point[interiorRTree[instack[ProNum][p]].edPoint].x , ty = interior_point[interiorRTree[instack[ProNum][p]].edPoint].y;
			  if ( dcmp(x2 , rx) && dcmp(y2 , ry)) return 1 ;
			  if (on_line(rx , ry ,tx , ty , x2 ,y2)) return 1 ;
              if (dcmp(ty ,y2)){ swap(tx, rx) ; swap (ty , ry) ;}
              if (dcmp(ry ,y2)){ //有一个点在延长线上
                 if (!dcmp(ty , y2)) //另一个点不在延长线上 如果都在那么不变
                    if (x2 <= rx)  //而且延长线上的的点在右边
                       if (ty > y2) on_top ++ ; //上方判定数量+
                       else on_below ++ ; //下方判定数量+
              }
              else {
                   if ( (ty - y2) * (y2 - ry) >= 0.0){ //一上一下
                      if (dcmp (tx , rx)){ //X重合
                         if (tx > x2) rects ++ ;
                      }
                      else {//ry是小的 ty是大的
                           if (!dcmp(ty , ry)){
						      if (ry > ty + eps){
							     swap(tx , rx) ;
							     swap(ty , ry);
                              }
				              if  (  (tx - rx) * (y2 - ry) >= (x2-rx) * (ty - ry))
                              rects++ ;
		                   }
	                  }
                   }
             } }
     }
   //  return times2 <= instack[1] ;
     return ( (rects + (on_top & 1)) & 1) ;
}

int check_in_n_exterior_polygon(int ProNum , double request_dist , int x , double x2, double y2){
   //  for (i = first_exterior_point[x] ; i != -1 ; i=next_exterior_point[i]){
     int i , p , q ;

    // int times1 = 0 , times2 = 0 ;
  //   for (i = first_exterior_point[x] ; i != -1 ; i=next_exterior_point[i]){times2++ ;}
     p = 0 ; q = 1 ; exstack[ProNum][1] = exteriorRTreeStart[x] ;
        while (p != q){
              p++ ;
              if (( x2 > exteriorRTree[exstack[ProNum][p]].edx + request_dist)  ||  x2 < exteriorRTree[exstack[ProNum][p]].stx - request_dist ||
                  (y2 < exteriorRTree[exstack[ProNum][p]].sty - request_dist || y2 > exteriorRTree[exstack[ProNum][p]].edy + request_dist))
              continue ;
              if (exteriorRTree[exstack[ProNum][p]].lable == 1){
                                                   for (i = 0 ; i <= 4 ; i++) if (exteriorRTree[exstack[ProNum][p]].next[i] != 0){
                                                       q++ ;
                                                       exstack[ProNum][q] = exteriorRTree[exstack[ProNum][p]].next[i] ;
                                                       }else break ;
                                                   continue ;
              } else {
                 // return 0 ;
                Point c1 ;
				Point b1 = exterior_point[exteriorRTree[exstack[ProNum][p]].edPoint] ;
                Point a1 = exterior_point[exteriorRTree[exstack[ProNum][p]].stPoint] ;
                c1.x = x2 ; c1.y = y2 ;
             //   if ( dcmp(x2 , a1.x) && dcmp(y2 , a1.y)) return 1 ;
             //   if (on_line(a1.x , a1.y ,b1.x , b1.y , x2 ,y2)) return 1 ;
              //  return 0 ;
                if (dis(c1 , a1 , b1) < request_dist) return 1 ;
              }
     }
     return 0 ;
}

int check_in_n_interior_polygon(int ProNum , double request_dist , int x , double x2, double y2){   int i , p , q ;
    // int times1 = 0 , times2 = 0 ;
  //   for (i = first_exterior_point[x] ; i != -1 ; i=next_exterior_point[i]){times2++ ;}
     p = 0 ; q = 1 ; instack[ProNum][1] = interiorRTreeStart[x] ;
        while (p != q){
              p++ ;
              if (( x2 > interiorRTree[instack[ProNum][p]].edx + request_dist)  ||  x2 < interiorRTree[instack[ProNum][p]].stx - request_dist ||
                  (y2 < interiorRTree[instack[ProNum][p]].sty - request_dist || y2 > interiorRTree[instack[ProNum][p]].edy + request_dist))
              continue ;
              if (interiorRTree[instack[ProNum][p]].lable == 1){
                                                   for (i = 0 ; i <= 4 ; i++) if (interiorRTree[instack[ProNum][p]].next[i] != 0){
                                                       q++ ;
                                                       instack[ProNum][q] = interiorRTree[instack[ProNum][p]].next[i] ;
                                                       }else break ;
                                                   continue ;
              } else {
                Point c1 ;
                Point b1 = interior_point[interiorRTree[instack[ProNum][p]].edPoint] ;
                Point a1 = interior_point[interiorRTree[instack[ProNum][p]].stPoint] ;
                c1.x = x2 ; c1.y = y2 ;
              //  if ( dcmp(x2 , a1.x) && dcmp(y2 , a1.y)) return 1 ;
              //  if (on_line(a1.x , a1.y ,b1.x , b1.y , x2 ,y2)) return 1 ;
                if (dis(c1 , a1 , b1) < request_dist) return 1 ;
        }
     }
     return 0 ;
}

int check_in_area(int ProNum , int x , double x2 , double y2){
     int tmp = 0 ;
     int i ;
     Point nowt ;
	 int grid_filter;
	// return 0;
     nowt.x = x2 ; nowt.y = y2 ;
     if (x2 > exMax[x] || x2 < exMin[x] ) return 0 ;
     if ( y2 > eyMax[x] || y2 < eyMin[x]) return 0 ;
     grid_filter = interpior_point_in_polygon(nowt , x);
	 
	 //grid_filter=2;

     if (grid_filter == 0) return 0 ;
     if (grid_filter == 1) return 1 ;
     if (!check_in_exterior_polygon(ProNum , x , x2 , y2) ) return 0;
     for (i = first_interior_region[x]; i != -1; i = next_interior_region[i]) {
          if (!(x2 > ixMax[i] || y2 > iyMax[i]))
          if (!(y2 < iyMin[i] || x2 < ixMin[i]))
         if (check_in_interior_polygon(ProNum , i , x2 , y2)) return 0 ;
     }
     return 1 ;
}

int check_in_n_area(int ProNum , int x , double request_dist , double x2 , double y2){
     int tmp = 0 ;
     int i ; double w = request_dist ;
	// return 0;
     if (x2 - w > exMax[x] || x2 + w < exMin[x] ) return 0 ;
     if ( y2 - w > eyMax[x] || y2 + w < eyMin[x]) return 0 ;
    // return 0 ;
     if (check_in_n_exterior_polygon(ProNum ,request_dist , x , x2 , y2) ) return 1  ;
     for (i = first_interior_region[x]; i != -1; i = next_interior_region[i]) {
          if (!(x2 - w > ixMax[i] || y2 - w > iyMax[i]))
          if (!(y2 + w < iyMin[i] || x2 + w < ixMin[i]))
         if (check_in_n_interior_polygon(ProNum , request_dist , i , x2 , y2)) return 1 ;
     }
     return 0 ;
    // return (tmp) ;
}
void pre_do_MBR(){
     int i , j ;
   //  freopen("Testout.txt" , "w" ,stdout);
     for (i = 0 ; i < number_of_polygons ; i++){
         exMax[i] = -1000000000.0 ; exMin[i] = 1000000000.0 ;
         eyMax[i] = -1000000000.0 ; eyMin[i] = 1000000000.0 ;
         for (j = first_exterior_point[i] ; j != -1 ; j = next_exterior_point[j]){
             exMax[i] = max(exMax[i] , exterior_point[j].x) ;
             exMin[i] = min(exMin[i] , exterior_point[j].x) ;
             eyMax[i] = max(eyMax[i] , exterior_point[j].y) ;
             eyMin[i] = min(eyMin[i] , exterior_point[j].y) ;
         }

		 totalmaxx = max(totalmaxx , exMax[i]) ;
		 totalminx = min(totalminx , exMin[i]);
		 totalmaxy = max(totalmaxy , eyMax[i]) ;
		 totalminy = min(totalminy , eyMin[i]);
      //   printf("%.0lf %.0lf %.0lf %.0lf\n" , exMin[i] , exMax[i] , eyMin[i] , eyMax[i]);

     }
    // system("pause");
     for (i = 0 ; i < number_of_interior_regions ; i++){
         ixMax[i] = -1000000000.0 ; ixMin[i] = 1000000000.0 ;
         iyMax[i] = -1000000000.0 ; iyMin[i] = 1000000000.0 ;
         for (j = first_interior_point[i] ; j != -1 ; j = next_interior_point[j]){
             ixMax[i] = max(ixMax[i] , interior_point[j].x) ;
             ixMin[i] = min(ixMin[i] , interior_point[j].x) ;
             iyMax[i] = max(iyMax[i] , interior_point[j].y) ;
             iyMin[i] = min(iyMin[i] , interior_point[j].y) ;
         }

		 totalmaxx = max(totalmaxx , ixMax[i]) ;
		 totalminx = min(totalminx , ixMin[i]);
		 totalmaxy = max(totalmaxy , iyMax[i]) ;
		 totalminy = min(totalminy , iyMin[i]);
     }

}


double gx(int x){
       return point[point_id_cluster[x]].x ;
}
double gy(int y){
       return point[point_id_cluster[y]].y ;
}

//getcheck1 : Lx -> Rx , Ly -> Ry
void getcheck1(int ProNum , int L , int R , double stx , double sty , double edx , double edy){
     double mid = 0 ; int Lmid ,Rmid ;
     int L1 = R ;
     int R1 = L ;
     ansx[ProNum] = L ; ansy[ProNum] = R ; flagC[ProNum] = 0 ;
    // if (gx(L) > edx) return ;
    // if (gx(R) < stx) return ;
     while (L < L1){
           Lmid = ((L + L1) >> 1);
           mid = gx( Lmid );
           if (stx > mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
   //  L = L1 ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1);
           mid = gx( Rmid );
           if (edx >= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
     }
   //  R = R1 ;
     if (edx < gx(R)) R-- ;
     if (L > R) return ;
     //if (gy(L) > edy) return ;
    // if (gy(R) < sty) return ;
     L1 = R ;
     while (L < L1){
           Lmid = ((L + L1) >> 1);
           mid = gy( Lmid );
           if (sty > mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
   //  L = L1 ;
     R1 = L ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1);
           mid = gy( Rmid );
           if (edy >= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
     }
    // R = R1 ;
     if (edy < gy(R)) R-- ;
     if (L > R) return ;
     flagC[ProNum] = 1 ;
     ansx[ProNum] = L ;
     ansy[ProNum] = R ;
     return ;
}


//getcheck2 : Lx -> Rx , Ry -> Ly
void getcheck2(int ProNum , int L , int R , double stx , double sty , double edx , double edy ){
     double mid = 0 ; int Lmid ,Rmid ;
     int L1 = R ;
     int R1 = L ;
     ansx[ProNum] = L ; ansy[ProNum] = R ; flagC[ProNum] = 0 ;
    // if (gx(L) > edx) return ;
    // if (gx(R) < stx) return ;
     while (L < L1){
           Lmid = ((L + L1) >> 1);
           mid = gx( Lmid );
           if (stx > mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
    // L = L1 ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1);
           mid = gx( Rmid );
           if (edx >= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
     }
   //  R = R1 ;
     if (edx < gx(R)) R-- ;
     if (L > R) return ;
    // if (gy(R) > edy) return ;
    // if (gy(L) < sty) return ;
     L1 = R ;
     while (L < L1){
           Lmid = ((L + L1) >> 1);
           mid = gy( Lmid );
           if (edy < mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
    // L = L1 ;
     R1 = L ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1);
           mid = gy( Rmid );
           if (sty <= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
     }
   //  R = R1 ;
     if (sty > gy(R)) R-- ;
     if (L > R) return ;
     flagC[ProNum] = 1 ;
     ansx[ProNum] = L ;
     ansy[ProNum] = R ;
     return ;
}



//getcheck3 : Rx -> Lx , Ly -> Ry

void getcheck3(int ProNum , int L , int R , double stx , double sty , double edx , double edy ){
     double mid = 0 ; int Lmid ,Rmid ;
     int L1 = R ;
	 int R1 = L ;
     ansx[ProNum] = L ; ansy[ProNum] = R ; flagC[ProNum] = 0 ;
   //  if (gx(R) > edx) return ;
    // if (gx(L) < stx) return ;
     while (L < L1){
           Lmid = ((L + L1) >> 1);
           mid = gx( Lmid );
           if (edx < mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
    // L = L1 ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1);
           mid = gx( Rmid );
           if (stx <= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
     }
  //   R = R1 ;
     if (stx > gx(R)) R-- ;
   //  if (gy(L) > edy) return ;
   //  if (gy(R) < sty) return ;
     if (L > R) return ;
     L1 = R ;
     while (L < L1){
           Lmid = ((L + L1) >> 1) ;
           mid = gy( Lmid );
           if (sty > mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
    // L = L1 ;
     R1 = L ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1 );
           mid = gy( Rmid );
           if (edy >= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
     }
   //  R = R1 ;
     if (edy < gy(R)) R-- ;
     if (L > R) return ;
     flagC[ProNum] = 1 ;
     ansx[ProNum] = L ;
     ansy[ProNum] = R ;
     return ;
}


//getcheck4 : Rx -> Lx , Ry -> Ly
void getcheck4(int ProNum , int L , int R , double stx , double sty , double edx , double edy ){
     int L1 = R ;
     double mid = 0 ; int Lmid ,Rmid ;
     int R1 = L ;
	ansx[ProNum] = L ; ansy[ProNum] = R ; flagC[ProNum] = 0 ;
   //  if (gx(R) > edx) return ;
   //  if (gx(L) < stx) return ;
     while (L < L1){
           Lmid = ((L + L1) >> 1) ;
           mid = gx( Lmid );
           if (edx < mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
   //  L = L1 ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1) ;
           mid = gx( Rmid );
           if (stx <= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
     }
   //  R = R1 ;
     if (edx < gx(R)) R-- ;
     if (L > R) return ;
     L1 = R ;
    // if (gy(R) > edy) return ;
    // if (gy(L) < sty) return ;
     while (L < L1){
           Lmid = ((L + L1) >> 1) ;
           mid = gy( Lmid );
           if (edy < mid) L = Lmid + 1 ;
           else L1 = Lmid ;
     }
   //  L = L1 ;
     R1 = L ;
     while (R1 < R){
           Rmid = ((R + R1) >> 1) ;
           mid = gy( Rmid );
           if (sty <= mid) R1 = Rmid + 1 ;
           else R = Rmid ;
    }
    // R = R1 ;
     if (edy < gy(R)) R-- ;
     if (L > R) return ;
     flagC[ProNum] = 1 ;
     ansx[ProNum] = L ;
     ansy[ProNum] = R ;
     return ;
}

//getcheck End

void buffer_output(int ProNum,int a,int b,int c,int d)
{
	char temp[100];
    int len,i;
	//return;

    if(len_buffer[ProNum]>buffer_limit)
    {
        str_buffer[ProNum][len_buffer[ProNum]]='\0';
        printf("%s",str_buffer[ProNum]);
        len_buffer[ProNum]=0;
    }
/*
	i=0;
	itoa(a,temp,10);
	while(temp[i]!='\0')
	{
		str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
		i++;
		len_buffer[ProNum]++;
	}
	str_buffer[ProNum][len_buffer[ProNum]]=':';
    len_buffer[ProNum]++;
	i=0;
	itoa(b,temp,10);
	while(temp[i]!='\0')
	{
		str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
		i++;
		len_buffer[ProNum]++;
	}
	str_buffer[ProNum][len_buffer[ProNum]]=':';
    len_buffer[ProNum]++;
	i=0;
	itoa(c,temp,10);
	while(temp[i]!='\0')
	{
		str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
		i++;
		len_buffer[ProNum]++;
	}
	str_buffer[ProNum][len_buffer[ProNum]]=':';
    len_buffer[ProNum]++;
	i=0;
	itoa(d,temp,10);
	while(temp[i]!='\0')
	{
		str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
		i++;
		len_buffer[ProNum]++;
	}
	str_buffer[ProNum][len_buffer[ProNum]]='\n';
    len_buffer[ProNum]++;
*/

    len=0;
	if(a==0)
	{
		str_buffer[ProNum][len_buffer[ProNum]]='0';
		len_buffer[ProNum]++;
	}else
	{
    while(a!=0)
    {
        temp[len]=a%10+'0';
        len++;
        a=a/10;
    }
    for(i=len-1;i>=0;i--)
    {
        str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
        len_buffer[ProNum]++;
    }
	}
    str_buffer[ProNum][len_buffer[ProNum]]=':';
    len_buffer[ProNum]++;

    len=0;
	if(b==0)
	{
		str_buffer[ProNum][len_buffer[ProNum]]='0';
		len_buffer[ProNum]++;
	}else
	{
    while(b!=0)
    {
        temp[len]=b%10+'0';
        len++;
        b=b/10;
    }
    for(i=len-1;i>=0;i--)
    {
        str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
        len_buffer[ProNum]++;
    }
	}
    str_buffer[ProNum][len_buffer[ProNum]]=':';
    len_buffer[ProNum]++;

    len=0;
	if(c==0)
	{
		str_buffer[ProNum][len_buffer[ProNum]]='0';
		len_buffer[ProNum]++;
	}else
	{
    while(c!=0)
    {
        temp[len]=c%10+'0';
        len++;
        c=c/10;
    }
    for(i=len-1;i>=0;i--)
    {
        str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
        len_buffer[ProNum]++;
    }
	}
    str_buffer[ProNum][len_buffer[ProNum]]=':';
    len_buffer[ProNum]++;

    len=0;
	if(d==0)
	{
		str_buffer[ProNum][len_buffer[ProNum]]='0';
		len_buffer[ProNum]++;
	}else
	{
    while(d!=0)
    {
        temp[len]=d%10+'0';
        len++;
        d=d/10;
    }
    for(i=len-1;i>=0;i--)
    {
        str_buffer[ProNum][len_buffer[ProNum]]=temp[i];
        len_buffer[ProNum]++;
    }
	}
    str_buffer[ProNum][len_buffer[ProNum]]='\n';
    len_buffer[ProNum]++;

}

void clear_buffer(int ProNum)
{
    //return;
    str_buffer[ProNum][len_buffer[ProNum]]='\0';
    printf("%s",str_buffer[ProNum]);
}

void Solve(argType* argNode)
{
    int ProNum;
    int stP;
    int enP;
    int i,j,k;
	int L,R;
	double stx , sty , edx , edy , tmp ;
    ProNum=argNode->ProNum;
    stP=argNode->stP;
    enP=argNode->enP;
    Done[ProNum]=0;
    ans_count[ProNum]=0;
    len_buffer[ProNum]=0;


/*
	for(i=stP;i<=enP;i++)//对point_id类从小到大进行枚举
	{
		//if(first_query_point[i]==-1) continue;
		int k=i;
		//printf("%d\n",i);
			//printf("%d\n",k);
			if(request_cmd[0]=='I')
			{
                  search(ProNum,&RTree[0],point[k].x,point[k].y,point_sequence[k]);
			      for(j=0;j<Rnum[ProNum];j++)//遍历有序的polygon_id列表
			      {
				  if(check_in_area(ProNum,Rans[ProNum][j] , point[k].x , point[k].y)){//检查绝对的point_k是否在绝对的Polygon_Rans[j]中
				  ans_count[ProNum]++;
				  buffer_output(ProNum,point_id[k],point_sequence[k],polygon_id[Rans[ProNum][j]],polygon_sequence[Rans[ProNum][j]]);
                   }}
            }else
            {
                  search(ProNum,&_RTree[0],point[k].x,point[k].y,point_sequence[k]);
                  for(j=0;j<Rnum[ProNum];j++)//遍历有序的polygon_id列表
			      {
                  if(check_in_area(ProNum,Rans[ProNum][j] , point[k].x , point[k].y)||
                     check_in_n_area(ProNum,Rans[ProNum][j] ,request_dist, point[k].x , point[k].y)){//检查绝对的point_k是否在绝对的Polygon_Rans[j]中
                    ans_count[ProNum]++;
                    buffer_output(ProNum,point_id[k],point_sequence[k],polygon_id[Rans[ProNum][j]],polygon_sequence[Rans[ProNum][j]]);
                  }}
            }
	}
	*/




		for (i = stP ; i <=enP ; i++)
		{
			Ls = 0 ; Rs = 0 ; flagC[ProNum] = 0 ;
			L = interval_start[i] , R = interval_start[i+1] - 1 ;
			stx = gx(L) ; sty = gy(L) ; edx = gx(R) ; edy = gy(R);
			if (request_cmd[0] == 'I')
				Rec_search(ProNum,&RTree[0] , stx ,  sty , edx  , edy , interval_time[i]) ;
			else
				Rec_search(ProNum,&_RTree[0] , stx ,  sty , edx  , edy , interval_time[i]) ;
			//continue;
			if (Rnum[ProNum] > 0)
				{
				if (request_cmd[0] == 'I')
				{
					//continue;
					for (j=0 ; j < Rnum[ProNum] ; j++)
					{
						ansx[ProNum] = L ; ansy[ProNum] = R ;
							if (R-L>8)
							{
								stx = exMin[Rans[ProNum][j]] ; sty = eyMin[Rans[ProNum][j]] ;
								edx = exMax[Rans[ProNum][j]] ; edy = eyMax[Rans[ProNum][j]] ;
								if (point[point_id_cluster[L]].x < point[point_id_cluster[R]].x)
								{
									if (point[point_id_cluster[L]].y < point[point_id_cluster[R]].y)
										getcheck1(ProNum, L , R , stx , sty , edx + eps , edy + eps);
									else getcheck2(ProNum, L , R , stx , sty , edx + eps , edy + eps );
								}
								else
								{
									if (point[point_id_cluster[L]].y < point[point_id_cluster[R]].y)
										getcheck3(ProNum, L , R , stx , sty , edx + eps, edy + eps);
									else getcheck4(ProNum, L , R , stx , sty , edx + eps , edy + eps);
								}
                //if(flagC[ProNum])
								for (k = ansx[ProNum] ; k <= ansy[ProNum] ; k ++)
									if(check_in_area(ProNum , Rans[ProNum][j] , gx(k) , gy(k)))
									{//检查绝对的point_k是否在绝对的Polygon_Rans[j]中
									ans_count[ProNum]++;
									buffer_output(ProNum,point_id[point_id_cluster[k]],
									point_sequence[point_id_cluster[k]],polygon_id[Rans[ProNum][j]],polygon_sequence[Rans[ProNum][j]]);
									}
							}
							else 
							{
								for (k = L ; k <= R ; k ++)
									if(check_in_area(ProNum , Rans[ProNum][j] , gx(k) , gy(k)))
									{
										ans_count[ProNum]++;
										buffer_output(ProNum,point_id[point_id_cluster[k]],
										point_sequence[point_id_cluster[k]],polygon_id[Rans[ProNum][j]],polygon_sequence[Rans[ProNum][j]]);
									}
							}
					}
				}
				else 
				{
					for (j=0 ; j < Rnum[ProNum] ; j++)
					{
						ansx[ProNum] = L ; ansy[ProNum] = R ;
						if ((R- L) > 8)
						{
							stx = exMin[Rans[ProNum][j]] - request_dist ; sty = eyMin[Rans[ProNum][j]] - request_dist ;
							edx = exMax[Rans[ProNum][j]] + request_dist ; edy = eyMax[Rans[ProNum][j]] + request_dist ;
							if (point[point_id_cluster[L]].x < point[point_id_cluster[R]].x)
							{
								if (point[point_id_cluster[L]].y < point[point_id_cluster[R]].y)
									getcheck1(ProNum,L , R , stx , sty , edx + eps , edy + eps);
								else getcheck2(ProNum,L , R , stx , sty , edx + eps , edy + eps);
							}
							else 
							{
								if (point[point_id_cluster[L]].y < point[point_id_cluster[R]].y)
									getcheck3(ProNum,L , R , stx , sty , edx + eps , edy + eps);
								else getcheck4(ProNum,L , R , stx , sty , edx + eps , edy + eps);
							}
                //if(flagC[ProNum])
							for (k = ansx[ProNum] ; k <= ansy[ProNum] ; k ++)
								if(check_in_area(ProNum , Rans[ProNum][j] , gx(k) , gy(k)) ||
								check_in_n_area(ProNum , Rans[ProNum][j] ,request_dist, gx(k) , gy(k)))
								{
									ans_count[ProNum]++;
									buffer_output(ProNum,point_id[point_id_cluster[k]],
									point_sequence[point_id_cluster[k]],polygon_id[Rans[ProNum][j]],polygon_sequence[Rans[ProNum][j]]);
								}
						}
						else 
						{
							for (k = L ; k <= R ; k ++)
								if(check_in_area(ProNum , Rans[ProNum][j] , gx(k) , gy(k)) ||
								check_in_n_area(ProNum , Rans[ProNum][j] ,request_dist, gx(k) , gy(k)))
								{
									ans_count[ProNum]++;
									buffer_output(ProNum,point_id[point_id_cluster[k]],
									point_sequence[point_id_cluster[k]],polygon_id[Rans[ProNum][j]],polygon_sequence[Rans[ProNum][j]]);
								}
						}
					}
				}
				}
		}

    clear_buffer(ProNum);
	Done[ProNum]=1;
	//fclose(f1);
	//fclose(f2);
}
bool intersectCheck(Point* p1,Point* p2,Point* polyline1,Point*polyline2)
{
	double rate = 1000;
	double x1 = p1->y;
	double y1 = p1->x;
	double x2 = p2->y;
	double y2 = p2->x;

	x1 *= rate;
	y1 *= rate;
	x2 *= rate;
	y2 *= rate;

	double A = y2 - y1;
	double B = x1 - x2;
	double C = -A * x1 - B * y1;

		double _x1 = polyline1->y;
		double _y1 = polyline1->x;
		double _x2 = polyline2->y;
		double _y2 = polyline2->x;

		_x1 *= rate;
		_y1 *= rate;
		_x2 *= rate;
		_y2 *= rate;

		double _A = _y2 - _y1;
		double _B = _x1 - _x2;
		double _C = -_A * _x1 - _B * _y1;
		double val1 = A * _x1 + B * _y1 + C;
		double val2 = A * _x2 + B * _y2 + C;
		double _val1 = _A * x1 + _B * y1 + _C;
		double _val2 = _A * x2 + _B * y2 + _C;

		if ((abs(A) < eps && abs(_A) < eps) || (abs(B) < eps && abs(_B) < eps))
			return false;
		if ((abs(val1) < eps && abs(val2) < eps) || (abs(_val1) < eps && abs(_val2) < eps))
			return false;
		if (val1 * val2 < eps && _val1 * _val2 <= eps)
			return true;
	return false;
}
struct pair getRowCol(struct GeoPoint* pt)
{
	//////////////////////////////////////////////////////////////////////////
	///first row, second col
	//////////////////////////////////////////////////////////////////////////
	struct pair t = {((pt->lat - minLat) / gridSizeDeg),((pt->lon - minLon) / gridSizeDeg) };
	return t;
}
polygon_edge_process()
{
	fclose(stdout);
	freopen("Testout.txt", "w", stdout);
	int i, j,k; int **hasPolyline, **polylineNumber;
	gridWidth = 100;
	gridHeight = (int)((maxLon - minLon) / (maxLat - minLat) * (double)gridWidth) + 1;
	gridSizeDeg = (maxLat- minLat) / (double)gridWidth;
	hasPolyline = (int **)malloc(sizeof(int*)*(gridWidth+1));
	for (i = 0; i <= gridWidth; i++)
	{
		hasPolyline[i] = (int*)malloc(sizeof(int)*(gridHeight + 1));
		for (int j = 0; j <= gridHeight; j++)
			hasPolyline[i][j] = -1;
	}
	polylineNumber = (int **)malloc(sizeof(int*)*(gridWidth+1));
	for (i = 0; i <= gridWidth; i++)
	{
		polylineNumber[i] = (int*)malloc(sizeof(int)*(gridHeight + 1));
		for (int j = 0; j <= gridHeight; j++)polylineNumber[i][j] = -1;
	}
	for (i = 0; i < number_of_polygons; i++) {
		for (j = first_exterior_point[i]; j != -1; j = next_exterior_point[j]) {
			k = next_exterior_point[j];
			if (k == -1)break;
			int gridOfx = (exterior_point[k].x - exterior_point[j].x) / gridSizeDeg;
			int gridOfy = (exterior_point[k].y - exterior_point[j].y) / gridSizeDeg;
			if (gridOfx < 0)gridOfx *= -1;
			if (gridOfy < 0)gridOfy *= -1;
			int grids = max(gridOfx, gridOfy);
			grids += 2;
			double tx = exterior_point[j].x, ty = exterior_point[j].y;
			double kx = (exterior_point[k].x - exterior_point[j].x) / grids;
			double ky = (exterior_point[k].y - exterior_point[j].y) / grids;
			for (int k = 0; k <= grids; k++)
			{
				struct GeoPoint tgeopoint = { tx,ty };
//				printf("%f %f\n", tgeopoint.lat, tgeopoint.lon);
				struct pair tpair = getRowCol(&tgeopoint);
//				printf("%d %d\n", tpair.first, tpair.second);
//				fprintf(stderr, "%d %d\n", tpair.first, tpair.second);
				hasPolyline[tpair.first][tpair.second] = j; polylineNumber[tpair.first][tpair.second] = k;
				tx += kx;
				ty += ky;
			}
			struct GeoPoint tgeopoint = { exterior_point[j].x,exterior_point[j].y };
			struct pair tpair = getRowCol(&tgeopoint);
			hasPolyline[tpair.first][tpair.second] = j; polylineNumber[tpair.first][tpair.second] = k;
		}
	}

	for (i = 0; i < number_of_edges; i++)
	{
		j = edge[i].start; k = edge[i].end;
		int gridOfx = (point[k].x - point[j].x) / gridSizeDeg;
		int gridOfy = (point[k].y - point[j].y) / gridSizeDeg;
		if (gridOfx < 0)gridOfx *= -1;
		if (gridOfy < 0)gridOfy *= -1;
		int grids = max(gridOfx, gridOfy);
		grids += 2;
		double tx = point[j].x, ty = point[j].y;
		double kx = (point[k].x - point[j].x) / grids;
		double ky = (point[k].y - point[j].y) / grids;
		for (int k = 0; k <= grids; k++)
		{
			struct GeoPoint tgeopoint = { tx,ty };
			struct pair tpair = getRowCol(&tgeopoint);
			if (hasPolyline[tpair.first][tpair.second] != -1 && intersectCheck(&exterior_point[hasPolyline[tpair.first][tpair.second]], &exterior_point[polylineNumber[tpair.first][tpair.second]], &point[j], &point[k]))
			{
				printf("%d\n", edge[i].id);
				break;
			}
			tx += kx;
			ty += ky;
		}
		if (k > grids)
		{
			struct GeoPoint tgeopoint = { exterior_point[j].x,exterior_point[j].y };
			struct pair tpair = getRowCol(&tgeopoint);
			if (hasPolyline[tpair.first][tpair.second] != -1 && intersectCheck(&exterior_point[hasPolyline[tpair.first][tpair.second]], &exterior_point[polylineNumber[tpair.first][tpair.second]], &point[j], &point[k]))
				printf("%d\n", edge[i].id);
		}
	}
}