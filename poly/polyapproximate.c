#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "define.h"

// Approximating Polygon Process
/** get approximate tolerance, need to be implemented */
double my_cross(Point a,Point b,Point c){return (b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);}
double my_length(Point a, Point b) {return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));}
#define alpha 0.025
double get_tolerance() {
    double res, perimeter = 0, area = 0;
    int i;
    Point O; O.x = O.y = 0;
    for (i = 0; i < poly_point_number; i++) {
        Point a = temp_list[i];
        Point b = temp_list[(i + 1) % poly_point_number];
        perimeter += my_length(a, b);
        area += my_cross(O, a, b);
    }
    area = fabs(area);
    res = area / perimeter * alpha;
    //printf("%d %lf %lf, %lf\n", poly_point_number, area, perimeter, res);
	return res*2.0;
    //return 0;
}

double get_tolerance2(int ProNum) {
    double res, perimeter = 0, area = 0;
    int i;
    Point O; O.x = O.y = 0;
    for (i = 0; i < poly_point_number_th[ProNum]; i++) {
        Point a = temp_list_th[ProNum][i];
        Point b = temp_list_th[ProNum][(i + 1) % poly_point_number_th[ProNum]];
        perimeter += my_length(a, b);
        area += my_cross(O, a, b);
    }
    area = fabs(area);
    res = area / perimeter * alpha;
    //printf("%d %lf %lf, %lf\n", poly_point_number, area, perimeter, res);
	return res * 2.0;
    //return 0;
}

/** distance between a point and an interval */
  double point_2_interval(Point point, Point side1, Point side2) {
	double x1 = side2.x - side1.x, y1 = side2.y - side1.y, x2 = point.x - side1.x, y2 = point.y - side1.y;
	//printf("%.8f\t",fabs(x1 * y2 - x2 * y1) / sqrt(x1 * x1 + y1 * y1));
	return fabs(x1 * y2 - x2 * y1) / sqrt(x1 * x1 + y1 * y1);
}

int point_2_interval_change(Point point,Point side1,Point side2)
{
	double x1 = side2.x - side1.x, y1 = side2.y - side1.y, x2 = point.x - side1.x, y2 = point.y - side1.y;
	return  (((x1 * y2 - x2 * y1)*(x1 * y2 - x2 * y1))>((x1 * x1 + y1 * y1)*tolerance*tolerance));
}

int point_2_interval_change2(Point point,Point side1,Point side2, double tolerance)
{
	double x1 = side2.x - side1.x, y1 = side2.y - side1.y, x2 = point.x - side1.x, y2 = point.y - side1.y;
	return  (((x1 * y2 - x2 * y1)*(x1 * y2 - x2 * y1))>((x1 * x1 + y1 * y1)*tolerance*tolerance));
}