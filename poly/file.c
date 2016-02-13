#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <process.h>
#include "define.h"
#include "polyapproximate.h"

typedef struct ReadArgType {
    int ProNum;
    int cur;
    int length;
} ReadArgType;


/** From <filename> read characters to <buffer> */
void read_buffer(char *filename) {
	FILE* fin;
	fin = fopen(filename, "rb");
	if (fin == NULL) { fputs("File Error", stderr); exit(1); }
	// obtain file size
	fseek(fin, 0, SEEK_END);
	length = ftell(fin);
    rewind(fin);
	// allocate memory
	buffer = (char*)malloc(sizeof(char) * length);
	if (buffer == NULL) { fputs("Memory Error", stderr); exit(2); }
	// read to buffer[]
	fread(buffer, 1, length, fin);
	fclose(fin);
}

/** Processing start with buffer[cur], terminal when meets <end_with>, read a decimal number */
double get_decimal(int *cur, char end_with) {
	int sign, exp, exp_sign;
	double tmpd, muld;

	if (buffer[*cur] == '-') sign = -1, (*cur)++; else sign = 1;
	tmpd = 0;
	while (buffer[*cur] != '.' && buffer[*cur] != end_with) {
		tmpd = tmpd * 10 + (buffer[*cur] - '0');
		(*cur)++;
	}
	if (buffer[*cur] == '.') { // in case have decimal point
		(*cur)++;
		muld = 0.1;
		while (buffer[*cur] != 'E' && buffer[*cur] != end_with) {
			tmpd += muld * (buffer[*cur] - '0');
			muld *= 0.1;
			(*cur)++;
		}
		if (buffer[*cur] == 'E') { // in case have exponent
			(*cur)++;
			exp = 0;
			if (buffer[*cur] == '-') exp_sign = -1, (*cur)++; else exp_sign = 1;
			while (buffer[*cur] != end_with) {
				exp = exp * 10 + (buffer[*cur] - '0');
				(*cur)++;
			}
			if (exp_sign == 1)
				while (exp--) tmpd *= 10;
			else
				while (exp--) tmpd *= 0.1;
		}
	}
	return tmpd * sign;
}

double get_decimal2(int *cur, char end_with, char end_with2) {
	int sign, exp, exp_sign;
	double tmpd, muld;

	if (buffer[*cur] == '-') sign = -1, (*cur)++; else sign = 1;
	tmpd = 0;
	while (buffer[*cur] != '.' && buffer[*cur] != end_with && buffer[*cur] != end_with2) {
		tmpd = tmpd * 10 + (buffer[*cur] - '0');
		(*cur)++;
	}
	if (buffer[*cur] == '.') { // in case have decimal point
		(*cur)++;
		muld = 0.1;
		while (buffer[*cur] != 'E' && buffer[*cur] != end_with && buffer[*cur] != end_with2) {
			tmpd += muld * (buffer[*cur] - '0');
			muld *= 0.1;
			(*cur)++;
		}
		if (buffer[*cur] == 'E') { // in case have exponent
			(*cur)++;
			exp = 0;
			if (buffer[*cur] == '-') exp_sign = -1, (*cur)++; else exp_sign = 1;
			while (buffer[*cur] != end_with && buffer[*cur] != end_with2) {
				exp = exp * 10 + (buffer[*cur] - '0');
				(*cur)++;
			}
			if (exp_sign == 1)
				while (exp--) tmpd *= 10;
			else
				while (exp--) tmpd *= 0.1;
		}
	}
	return tmpd * sign;
}

/** Processing start with buffer[cur], terminal when meets <end_with>, read a integer number */
int get_integer(int *cur, const char end_with) {
	int tmp = 0;
	while (buffer[*cur] != end_with) {
		tmp = tmp * 10 + (buffer[*cur] - '0');
		(*cur)++;
	}
	return tmp;
}

/** read points */
void read_points() {
	int cur = 0;
	read_buffer(point_input_file_path);

	number_of_points = 0;
	while (cur < length && buffer[cur] == 'P') {
		// id
		while (buffer[cur] != ':') cur++;
		cur++;
		point_id[number_of_points] = get_integer(&cur, ':');
		// sequence
		cur++;
		point_sequence[number_of_points] = get_integer(&cur, ':');
		// x (ends with comma)
		while (buffer[cur] != '>') cur++;
		cur++;
		while (buffer[cur] != '>') cur++;
		cur++;
		point[number_of_points].x = get_decimal(&cur, ',');
		// y (ends with space)
		cur++;
		point[number_of_points].y = get_decimal2(&cur, ' ', '<');
		// rests
		while (buffer[cur] != '>') cur++;
		cur++;
		while (buffer[cur] != '>') cur++;
		while (cur < length && buffer[cur] != 'P') cur++;
		number_of_points++;
	}
	free(buffer);
}

void read_points2(ReadArgType* argNode) {
    int ProNum = argNode->ProNum;
    int cur = argNode->cur, length = argNode->length;
	
	number_of_points_th[ProNum] = 0;
	while (cur < length && buffer[cur] == 'P') {
		// id
		while (buffer[cur] != ':') cur++;
		cur++;
		point_id_th[ProNum][number_of_points_th[ProNum]] = get_integer(&cur, ':');
		// sequence
		cur++;
		point_sequence_th[ProNum][number_of_points_th[ProNum]] = get_integer(&cur, ':');
		// x (ends with comma)
		while (buffer[cur] != '>') cur++;
		cur++;
		while (buffer[cur] != '>') cur++;
		cur++;
		point_th[ProNum][number_of_points_th[ProNum]].x = get_decimal(&cur, ',');
		// y (ends with space)		
		cur++;
		point_th[ProNum][number_of_points_th[ProNum]].y = get_decimal2(&cur, ' ', '<');
		// rests
		while (buffer[cur] != '>') cur++;
		cur++;
		while (buffer[cur] != '>') cur++;
		while (cur < length && buffer[cur] != 'P') cur++;
		number_of_points_th[ProNum]++;
	}
    Done[ProNum] = 1;
}

void read_points_threading() {
    int i, j;
    ReadArgType* argNode0,* argNode1,* argNode2,* argNode3;

    read_buffer(point_input_file_path);

    argNode0=(ReadArgType*)malloc(sizeof(ReadArgType));
    argNode1=(ReadArgType*)malloc(sizeof(ReadArgType));
    argNode2=(ReadArgType*)malloc(sizeof(ReadArgType));
    argNode3=(ReadArgType*)malloc(sizeof(ReadArgType));

    argNode0->ProNum = 0;
    argNode1->ProNum = 1;
    argNode2->ProNum = 2;
    argNode3->ProNum = 3;

    argNode0->cur = 0;
    argNode1->cur = length / 4; while (argNode1->cur + 1 < length && !(buffer[argNode1->cur] == 'P' && buffer[argNode1->cur + 1] == 'O')) argNode1->cur++;
    argNode2->cur = length / 2; while (argNode2->cur + 1 < length && !(buffer[argNode2->cur] == 'P' && buffer[argNode2->cur + 1] == 'O')) argNode2->cur++;
    argNode3->cur = length / 4 * 3; while (argNode3->cur + 1 < length && !(buffer[argNode3->cur] == 'P' && buffer[argNode3->cur + 1] == 'O')) argNode3->cur++;

    argNode0->length = argNode1->cur;
    argNode1->length = argNode2->cur;
    argNode2->length = argNode3->cur;
    argNode3->length = length;

    memset(Done, 0, sizeof(Done));
    _beginthread(read_points2, 0, (void*)argNode0);
	_beginthread(read_points2, 0, (void*)argNode1);
	_beginthread(read_points2, 0, (void*)argNode2);
	_beginthread(read_points2, 0, (void*)argNode3);

    while(Done[0]==0) Sleep(5);
    while(Done[1]==0) Sleep(5);
    while(Done[2]==0) Sleep(5);
    while(Done[3]==0) Sleep(5);

    free(buffer);
    memset(Done, 0, sizeof Done);

    number_of_points = 0;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < number_of_points_th[i]; j++) {
            point_id[number_of_points] = point_id_th[i][j];
            point_sequence[number_of_points] = 1;
            point[number_of_points] = point_th[i][j];
            number_of_points++;
        }
    }
}

/** read polygons
* <Method for visiting polygons>
* For exterior regions:
*	for (it = first_exterior_point2[id]; it != -1; it = next_exterior_point2[it]) {
*		// do something with exterior_point[it]
*	}
* For interior regions:
*	for (it = first_interior_region[id]; it != -1; it = next_interior_region[it]) {
*		for (it2 = first_interior_point2[it]; it2 != -1; it2 = next_interior_point2[it2]) {
*			// do something with interior_point[it2]
*		}
*	}
*/
 void read_polygons() {
	int cur, pointer;
	int it;
	int i;
	
	read_buffer(polygon_input_file_path);

	/** !! following processing assumes that there must be an exterior
	region followed by several interior regions in GML Polygon format */
	//tolerance = get_tolerance();

	number_of_polygons = 0;
	cur = 0, pointer;
	number_of_exterior_points = 0;
	number_of_interior_points = 0;
	number_of_interior_regions = 0;
	memset(first_exterior_point, -1, sizeof first_exterior_point);
	memset(first_interior_point, -1, sizeof first_interior_point);
	memset(first_interior_region, -1, sizeof first_interior_region);
	while (cur < length && buffer[cur] == 'P') {
		// id
		while (cur < length && buffer[cur] != ':') cur++;
		if (cur == length) break;
		cur++;
		polygon_id[number_of_polygons] = get_integer(&cur, ':');
		// sequence
		cur++;
		polygon_sequence[number_of_polygons] = get_integer(&cur, ':');
		// exterior region
		while (buffer[cur] != '>') cur++; // <gml:Polygon...
		cur++;
		while (buffer[cur] != '>') cur++; // <gml:outer...
		cur++;
		while (buffer[cur] != '>') cur++; // <gml:Linear...
		cur++;
		while (buffer[cur] != '>') cur++; // <gml:coord...
		cur++;
		poly_point_number = pointer = 0;
		while (buffer[cur] != '<') { // while not meet with </end tag>
			temp_list[poly_point_number].x = get_decimal(&cur, ',');
			cur++;
			temp_list[poly_point_number].y = get_decimal2(&cur, ' ', '<');
			cur++;
			poly_point_number++;
		}
		tolerance = get_tolerance();

		exterior_point[number_of_exterior_points] = temp_list[0]; // first exterior point
		next_exterior_point[number_of_exterior_points] = -1;
		first_exterior_point[number_of_polygons] = number_of_exterior_points;
		number_of_exterior_points++;

		for (it = 1; it < poly_point_number; it++) {
			int flag = 1, i;
			for (i = pointer + 1; i < it; i++)
			{
				if(point_2_interval_change(temp_list[i], exterior_point[number_of_exterior_points-1], temp_list[it])) {
					flag = 0;
					break;
				}
			}
			if (!flag) {
				exterior_point[number_of_exterior_points] = temp_list[it - 1];  // save temp_list[poly_point_number-1]
				next_exterior_point[number_of_exterior_points] = first_exterior_point[number_of_polygons];
				first_exterior_point[number_of_polygons] = number_of_exterior_points;
				number_of_exterior_points++;
				pointer = it - 1;
			}
		}

		exterior_point[number_of_exterior_points] = temp_list[poly_point_number - 1]; // deal with last point
		next_exterior_point[number_of_exterior_points] = first_exterior_point[number_of_polygons];
		first_exterior_point[number_of_polygons] = number_of_exterior_points;
		number_of_exterior_points++;
		// interior regions
		while (buffer[cur] != '>') cur++; // </gml:coord...
		cur++;
		while (buffer[cur] != '>') cur++; // </gml:Linear...
		cur++;
		while (buffer[cur] != '>') cur++; // </gml:outer...
		cur++;
		while (buffer[cur + 1] != '/') { // not an end tag, ie. have interior regions
			while (buffer[cur] != '>') cur++; // <gml:inner...
			cur++;
			while (buffer[cur] != '>') cur++; // <gml:Linear...
			cur++;
			while (buffer[cur] != '>') cur++; // <gml:coord...
			cur++;
			poly_point_number = pointer = 0;
			while (buffer[cur] != '<') { // while not meet with </end tag>
				temp_list[poly_point_number].x = get_decimal(&cur, ',');
				cur++;
				temp_list[poly_point_number].y = get_decimal2(&cur, ' ', '<');
				cur++;
				poly_point_number++;
			}
			tolerance = get_tolerance();

			interior_point[number_of_interior_points] = temp_list[0]; // first interior point
			next_interior_point[number_of_interior_points] = -1;
			first_interior_point[number_of_interior_regions] = number_of_interior_points;
			number_of_interior_points++;
			for (it = 1; it < poly_point_number; it++) {
				int flag = 1, i;
				for (i = pointer + 1; i < it; i++)
				{
					if (point_2_interval_change(temp_list[i], interior_point[number_of_interior_points-1], temp_list[it])) {
						flag = 0;
						break;
					}
				}
				if (!flag) {
					interior_point[number_of_interior_points] = temp_list[it - 1];  // save temp_list[poly_point_number-1]
					next_interior_point[number_of_interior_points] = first_interior_point[number_of_interior_regions];
					first_interior_point[number_of_interior_regions] = number_of_interior_points;
					number_of_interior_points++;
					pointer = it - 1;
				}
			}

			interior_point[number_of_interior_points] = temp_list[poly_point_number - 1]; // deal with last point
			next_interior_point[number_of_interior_points] = first_interior_point[number_of_interior_regions];
			first_interior_point[number_of_interior_regions] = number_of_interior_points;
			number_of_interior_points++;

			next_interior_region[number_of_interior_regions] = first_interior_region[number_of_polygons]; // deal with region
			first_interior_region[number_of_polygons] = number_of_interior_regions;
			number_of_interior_regions++;

			while (buffer[cur] != '>') cur++; // </gml:Linear...
			cur++;
			while (buffer[cur] != '>') cur++; // </gml:coord...
			cur++;
			while (buffer[cur] != '>') cur++; // </gml:inner
			cur++;
		}
		number_of_polygons++;
		while (buffer[cur] != '>') cur++;
		cur++;
		while (cur < length && buffer[cur] != 'P') cur++;
	}
}

void read_polygons2(ReadArgType* argNode) {
    int ProNum = argNode->ProNum, cur = argNode->cur, length = argNode->length;
	int it;
    int pointer;
    double tolerance;
	
	number_of_polygons_th[ProNum] = 0;
	number_of_exterior_points_th[ProNum] = 0;
	number_of_interior_points_th[ProNum] = 0;
	number_of_interior_regions_th[ProNum] = 0;
	memset(first_exterior_point_th[ProNum], -1, sizeof first_exterior_point_th[ProNum]);
	memset(first_interior_point_th[ProNum], -1, sizeof first_interior_point_th[ProNum]);
	memset(first_interior_region_th[ProNum], -1, sizeof first_interior_region_th[ProNum]);
	while (cur < length && buffer[cur] == 'P') {
		// id
		while (cur < length && buffer[cur] != ':') cur++;
		if (cur == length) break;
		cur++;
		polygon_id_th[ProNum][number_of_polygons_th[ProNum]] = get_integer(&cur, ':');
		// sequence
		cur++;
		polygon_sequence_th[ProNum][number_of_polygons_th[ProNum]] = get_integer(&cur, ':');
		// exterior region
		while (buffer[cur] != '>') cur++; // <gml:Polygon...
		cur++;
		while (buffer[cur] != '>') cur++; // <gml:outer...
		cur++;
		while (buffer[cur] != '>') cur++; // <gml:Linear...
		cur++;
		while (buffer[cur] != '>') cur++; // <gml:coord...
		cur++;
		poly_point_number_th[ProNum] = pointer = 0;
		while (buffer[cur] != '<') { // while not meet with </end tag>
			temp_list_th[ProNum][poly_point_number_th[ProNum]].x = get_decimal(&cur, ',');
			cur++;
			temp_list_th[ProNum][poly_point_number_th[ProNum]].y = get_decimal2(&cur, ' ', '<');
			cur++;
			poly_point_number_th[ProNum]++;
		}
		tolerance = get_tolerance2(ProNum);

		exterior_point_th[ProNum][number_of_exterior_points_th[ProNum]] = temp_list_th[ProNum][0]; // first exterior point
		next_exterior_point_th[ProNum][number_of_exterior_points_th[ProNum]] = -1;
		first_exterior_point_th[ProNum][number_of_polygons_th[ProNum]] = number_of_exterior_points_th[ProNum];
		number_of_exterior_points_th[ProNum]++;

		for (it = 1; it < poly_point_number_th[ProNum]; it++) {
			int flag = 1, i;
			for (i = pointer + 1; i < it; i++)
			{
				if(point_2_interval_change2(temp_list_th[ProNum][i], exterior_point_th[ProNum][number_of_exterior_points_th[ProNum]-1], temp_list_th[ProNum][it], tolerance)) {
					flag = 0;
					break;
				}
			}
			if (!flag) {
				exterior_point_th[ProNum][number_of_exterior_points_th[ProNum]] = temp_list_th[ProNum][it - 1];  // save temp_list[poly_point_number-1]
				next_exterior_point_th[ProNum][number_of_exterior_points_th[ProNum]] = first_exterior_point_th[ProNum][number_of_polygons_th[ProNum]];
				first_exterior_point_th[ProNum][number_of_polygons_th[ProNum]] = number_of_exterior_points_th[ProNum];
				number_of_exterior_points_th[ProNum]++;
				pointer = it - 1;
			}
		}

		exterior_point_th[ProNum][number_of_exterior_points_th[ProNum]] = temp_list_th[ProNum][poly_point_number_th[ProNum] - 1]; // deal with last point
		next_exterior_point_th[ProNum][number_of_exterior_points_th[ProNum]] = first_exterior_point_th[ProNum][number_of_polygons_th[ProNum]];
		first_exterior_point_th[ProNum][number_of_polygons_th[ProNum]] = number_of_exterior_points_th[ProNum];
		number_of_exterior_points_th[ProNum]++;
		// interior regions
		while (buffer[cur] != '>') cur++; // </gml:coord...
		cur++;
		while (buffer[cur] != '>') cur++; // </gml:Linear...
		cur++;
		while (buffer[cur] != '>') cur++; // </gml:outer...
		cur++;
		while (buffer[cur + 1] != '/') { // not an end tag, ie. have interior regions
			while (buffer[cur] != '>') cur++; // <gml:inner...
			cur++;
			while (buffer[cur] != '>') cur++; // <gml:Linear...
			cur++;
			while (buffer[cur] != '>') cur++; // <gml:coord...
			cur++;
			poly_point_number_th[ProNum] = pointer = 0;
			while (buffer[cur] != '<') { // while not meet with </end tag>
				temp_list_th[ProNum][poly_point_number_th[ProNum]].x = get_decimal(&cur, ',');
				cur++;
				temp_list_th[ProNum][poly_point_number_th[ProNum]].y = get_decimal2(&cur, ' ', '<');
				cur++;
				poly_point_number_th[ProNum]++;
			}
			tolerance = get_tolerance2(ProNum);

			interior_point_th[ProNum][number_of_interior_points_th[ProNum]] = temp_list_th[ProNum][0]; // first interior point
			next_interior_point_th[ProNum][number_of_interior_points_th[ProNum]] = -1;
			first_interior_point_th[ProNum][number_of_interior_regions_th[ProNum]] = number_of_interior_points_th[ProNum];
			number_of_interior_points_th[ProNum]++;
			for (it = 1; it < poly_point_number_th[ProNum]; it++) {
				int flag = 1, i;
				for (i = pointer + 1; i < it; i++)
				{
					if (point_2_interval_change2(temp_list_th[ProNum][i], interior_point_th[ProNum][number_of_interior_points_th[ProNum]-1], temp_list_th[ProNum][it], tolerance)) {
						flag = 0;
						break;
					}
				}
				if (!flag) {
					interior_point_th[ProNum][number_of_interior_points_th[ProNum]] = temp_list_th[ProNum][it - 1];  // save temp_list[poly_point_number-1]
					next_interior_point_th[ProNum][number_of_interior_points_th[ProNum]] = first_interior_point_th[ProNum][number_of_interior_regions_th[ProNum]];
					first_interior_point_th[ProNum][number_of_interior_regions_th[ProNum]] = number_of_interior_points_th[ProNum];
					number_of_interior_points_th[ProNum]++;
					pointer = it - 1;
				}
			}

			interior_point_th[ProNum][number_of_interior_points_th[ProNum]] = temp_list_th[ProNum][poly_point_number_th[ProNum] - 1]; // deal with last point
			next_interior_point_th[ProNum][number_of_interior_points_th[ProNum]] = first_interior_point_th[ProNum][number_of_interior_regions_th[ProNum]];
			first_interior_point_th[ProNum][number_of_interior_regions_th[ProNum]] = number_of_interior_points_th[ProNum];
			number_of_interior_points_th[ProNum]++;

			next_interior_region_th[ProNum][number_of_interior_regions_th[ProNum]] = first_interior_region_th[ProNum][number_of_polygons_th[ProNum]]; // deal with region
			first_interior_region_th[ProNum][number_of_polygons_th[ProNum]] = number_of_interior_regions_th[ProNum];
			number_of_interior_regions_th[ProNum]++;

			while (buffer[cur] != '>') cur++; // </gml:Linear...
			cur++;
			while (buffer[cur] != '>') cur++; // </gml:coord...
			cur++;
			while (buffer[cur] != '>') cur++; // </gml:inner
			cur++;
		}
		number_of_polygons_th[ProNum]++;
		while (buffer[cur] != '>') cur++;
		cur++;
		while (cur < length && buffer[cur] != 'P') cur++;
	}
    Done[ProNum] = 1;
    _endthread();
}

void read_polygons_threading() {
    int i, j;
    ReadArgType* argNode0,* argNode1,* argNode2,* argNode3;

    read_buffer(polygon_input_file_path);

    argNode0=(ReadArgType*)malloc(sizeof(ReadArgType));
    argNode1=(ReadArgType*)malloc(sizeof(ReadArgType));
    argNode2=(ReadArgType*)malloc(sizeof(ReadArgType));
    argNode3=(ReadArgType*)malloc(sizeof(ReadArgType));

    argNode0->ProNum = 0;
    argNode1->ProNum = 1;
    argNode2->ProNum = 2;
    argNode3->ProNum = 3;

    argNode0->cur = 0;    
    argNode1->cur = length / 4; while (argNode1->cur + 1 < length && !(buffer[argNode1->cur] == 'P' && buffer[argNode1->cur + 1] == 'O')) argNode1->cur++;
    argNode2->cur = length / 2; while (argNode2->cur + 1 < length && !(buffer[argNode2->cur] == 'P' && buffer[argNode2->cur + 1] == 'O')) argNode2->cur++;
    argNode3->cur = length / 4 * 3; while (argNode3->cur + 1 < length && !(buffer[argNode3->cur] == 'P' && buffer[argNode3->cur + 1] == 'O')) argNode3->cur++;

    argNode0->length = argNode1->cur;
    argNode1->length = argNode2->cur;
    argNode2->length = argNode3->cur;
    argNode3->length = length;

    memset(Done, 0, sizeof(Done));
    _beginthread(read_polygons2, 0, (void*)argNode0);	
	_beginthread(read_polygons2, 0, (void*)argNode1);
	_beginthread(read_polygons2, 0, (void*)argNode2);
	_beginthread(read_polygons2, 0, (void*)argNode3);

    while(Done[0]==0) Sleep(5);
    while(Done[1]==0) Sleep(5);
    while(Done[2]==0) Sleep(5);
    while(Done[3]==0) Sleep(5);

    free(buffer);
    memset(Done, 0, sizeof Done);

    // combination
    number_of_polygons = 0;
	number_of_exterior_points = 0;
	number_of_interior_points = 0;
	number_of_interior_regions = 0;
	//memset(first_exterior_point, -1, sizeof first_exterior_point);
	//memset(first_interior_point, -1, sizeof first_interior_point);
	//memset(first_interior_region, -1, sizeof first_interior_region);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < number_of_polygons_th[i]; j++) {
            polygon_id[number_of_polygons + j] = polygon_id_th[i][j];
            polygon_sequence[number_of_polygons + j] =0;
            first_exterior_point[number_of_polygons + j] = number_of_exterior_points + first_exterior_point_th[i][j];
            if (first_interior_region_th[i][j] == -1) {
                first_interior_region[number_of_polygons + j] = -1;
            } else {
                first_interior_region[number_of_polygons + j] = number_of_interior_regions + first_interior_region_th[i][j];
            }
        }
        for (j = 0; j < number_of_exterior_points_th[i]; j++) {
            exterior_point[number_of_exterior_points + j] = exterior_point_th[i][j];
            if (next_exterior_point_th[i][j] == -1) {
                next_exterior_point[number_of_exterior_points + j] = -1;
            } else {
                next_exterior_point[number_of_exterior_points + j] = number_of_exterior_points + next_exterior_point_th[i][j];
            }
        }
        for (j = 0; j < number_of_interior_regions_th[i]; j++) {
            if (next_interior_region_th[i][j] == -1) {
                next_interior_region[number_of_interior_regions + j] = -1;
            } else {
                next_interior_region[number_of_interior_regions + j] = number_of_interior_regions + next_interior_region_th[i][j]; 
            }
            first_interior_point[number_of_interior_regions + j] = number_of_interior_points + first_interior_point_th[i][j];
        }
        for (j = 0; j < number_of_interior_points_th[i]; j++) {
            interior_point[number_of_interior_points + j] = interior_point_th[i][j];
            if (next_interior_point_th[i][j] == -1) {
                next_interior_point[number_of_interior_points + j] = -1;
            } else {
                next_interior_point[number_of_interior_points + j] = number_of_interior_points + next_interior_point_th[i][j];
            }
        }
        number_of_polygons += number_of_polygons_th[i];
        number_of_exterior_points += number_of_exterior_points_th[i];
        number_of_interior_regions += number_of_interior_regions_th[i];
        number_of_interior_points += number_of_interior_points_th[i];
    }
}