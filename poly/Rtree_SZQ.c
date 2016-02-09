#include <math.h>
#include <malloc.h>
#include "define.h"

void point_list_make()
{
int i;
	for( i=0;i<number_of_points;i++) {first_query_point[i]=-1;next_query_point[i]=-1;}
	for( i=number_of_points-1;i>=0;i--)
	{
		next_query_point[i]=first_query_point[point_id[i]];
		first_query_point[point_id[i]]=i;
	}
}
void re(RTreeNode* tmp)
{
    RTreeNode* t1=tmp->next[0];
	double xx1=t1->x1,xx2=t1->x2,yy1=t1->y1,yy2=t1->y2;
	int i;
    for( i=1;i<tmp->num;i++)
	{
		t1=tmp->next[i];
		if(t1->x1<xx1) xx1=t1->x1;
		if(t1->x2>xx2) xx2=t1->x2;
		if(t1->y1<yy1) yy1=t1->y1;
		if(t1->y2>yy2) yy2=t1->y2;
	}
	tmp->x1=xx1;
	tmp->y1=yy1;
	tmp->x2=xx2;
	tmp->y2=yy2;
}
void MBR(int k)
{
	int kk=first_exterior_point[k];
	MBR_x1[k]=exterior_point[kk].x;
	MBR_x2[k]=exterior_point[kk].x;
	MBR_y1[k]=exterior_point[kk].y;
	MBR_y2[k]=exterior_point[kk].y;
	while(kk!=-1)
	{
		if(exterior_point[kk].x<MBR_x1[k]) MBR_x1[k]=exterior_point[kk].x;
		if(exterior_point[kk].x>MBR_x2[k]) MBR_x2[k]=exterior_point[kk].x;
		if(exterior_point[kk].y<MBR_y1[k]) MBR_y1[k]=exterior_point[kk].y;
		if(exterior_point[kk].y>MBR_y2[k]) MBR_y2[k]=exterior_point[kk].y;
		kk=next_exterior_point[kk];
	}
}
RTreeNode* NodeCopy()
{
	RTreeNode* temp=(RTreeNode*)malloc(sizeof(RTreeNode));
	int i;
	temp->id=FreeNode.id;
	temp->num=FreeNode.num;
	temp->x1=FreeNode.x1;
	temp->x2=FreeNode.x2;
	temp->y1=FreeNode.y1;
	temp->y2=FreeNode.y2;
	//printf("copy%d\n",FreeNode.num);
    for(i=0;i<temp->num;i++)
	temp->next[i]=FreeNode.next[i];
	return temp;
}
double change_rec(RTreeNode* p1,RTreeNode* p2)
{
	if(p1->x1<p2->x1) change_x1=p1->x1; else change_x1=p2->x1;
	if(p1->x2>p2->x2) change_x2=p1->x2; else change_x2=p2->x2;
	if(p1->y1<p2->y1) change_y1=p1->y1; else change_y1=p2->y1;
	if(p1->y2>p2->y2) change_y2=p1->y2; else change_y2=p2->y2;
	return ((change_x2-change_x1)*(change_y2-change_y1)-(p1->x2-p1->x1)*(p1->y2-p1->y1));
}
int insertRT(RTreeNode* father,RTreeNode* me)
{
    //system("pause");
	if(me->num==0)
	{
		return 0;
	}else
	{
        //printf("1me%d\n",me->num);
		double temp=1e100,tmp;
		int best,i;
		for(i=0;i<me->num;i++)
		{
			tmp=change_rec(me->next[i],&FreeNode);
			if(tmp<temp){temp=tmp;best=i;}
		}
		//printf("2me%d %x\n",me->num,me->next[best]);
		if(!insertRT(me,me->next[best]))
		{
		    //printf("8me%d %x\n",me->num,me->next[best]);
			if(me->num<MAXCH)
			{
			    //printf("10me%d %x\n",me->num,me->next[best]);
				me->num++;
				me->next[(me->num)-1]=NodeCopy();
				re(me);
				return 1;
			}else
			{
			    //printf("3me%d\n",me->num);
				RTreeNode* t=(RTreeNode*)malloc(sizeof(RTreeNode));
				int i;
				t->num=0;
                for(i=MAXCH/2;i<MAXCH;i++)
				{
					t->next[t->num]=me->next[i];
					t->num++;
				}
				me->num=MAXCH/2;
				re(me);
				t->next[t->num]=NodeCopy();
				t->num++;
				re(t);
				FreeNode.id=t->id;
				FreeNode.num=t->num;
				FreeNode.x1=t->x1;
				FreeNode.y1=t->y1;
				FreeNode.x2=t->x2;
				FreeNode.y2=t->y2;
				for(i=0;i<t->num;i++) FreeNode.next[i]=t->next[i];
				free(t);
				return 0;
			}
		}else
		{
		    //printf("9me%d %x\n",me->num,me->next[best]);
			re(me);
			//printf("9me%d %x\n",me->num,me->next[best]);
			return 1;
		}
	}
}
void insert(int k,int kk)
{
     //printf(",%d\n",k);
	if(RTree[k].num==-2)//表头没用过
	{
        //system("pause");
		RTreeNode* t1;
		RTree[k].num=-1;//开始用表头
		RTree[k].next[0]=(RTreeNode*)malloc(sizeof(RTreeNode));
        t1=RTree[k].next[0];
		t1->num=0;
		t1->id=kk;
		t1->x1=exMin[kk];
		t1->x2=exMax[kk];
		t1->y1=eyMin[kk];
		t1->y2=eyMax[kk];
		//printf(",%d\n",t1->num);
	}else
	{
         //system("pause");
		FreeNode.id=kk;
		FreeNode.num=0;
		FreeNode.x1=exMin[kk];
		FreeNode.x2=exMax[kk];
		FreeNode.y1=eyMin[kk];
		FreeNode.y2=eyMax[kk];
		if(!insertRT(&RTree[k],RTree[k].next[0]))
		{
			RTreeNode* root=(RTreeNode*)malloc(sizeof(RTreeNode));
			root->num=2;
			root->next[0]=RTree[k].next[0];
			root->next[1]=NodeCopy();
			re(root);
			RTree[k].next[0]=root;
		}
	}
}
void _insert(int k,int kk)
{
    //printf("_%d\n",k);
	if(_RTree[k].num==-2)//表头没用过
	{
		RTreeNode* t1;
        _RTree[k].num=-1;//开始用表头
		_RTree[k].next[0]=(RTreeNode*)malloc(sizeof(RTreeNode));
        t1=_RTree[k].next[0];
		t1->num=0;
		t1->id=kk;
		t1->x1=exMin[kk]-request_dist;
		t1->x2=exMax[kk]+request_dist;
		t1->y1=eyMin[kk]-request_dist;
		t1->y2=eyMax[kk]+request_dist;
	}else
	{
		FreeNode.id=kk;
		FreeNode.num=0;
		FreeNode.x1=exMin[kk]-request_dist;
		FreeNode.x2=exMax[kk]+request_dist;
		FreeNode.y1=eyMin[kk]-request_dist;
		FreeNode.y2=eyMax[kk]+request_dist;
		if(!insertRT(&_RTree[k],_RTree[k].next[0]))
		{
			RTreeNode* root=(RTreeNode*)malloc(sizeof(RTreeNode));
			root->num=2;
			root->next[0]=_RTree[k].next[0];
			root->next[1]=NodeCopy();
			re(root);
			_RTree[k].next[0]=root;
		}
	}
}
void initRtree()
{
	int i;
	int now,future;
    //point_list_make();
	//for(i=0;i<number_of_polygons;i++) MBR(i);
	for(i=0;i<MAXP;i++) {RTree[i].num=-2;_RTree[i].num=-2;}
	for(i=0;i<MAX_POLYGON_NUMBER;i++) now_polygon[i]=-1;
	for(i=0;i<number_of_polygons;)
	{
		int j;
		now=polygon_sequence[i];
		while((i<number_of_polygons-1)&&(polygon_sequence[i]==polygon_sequence[i+1]))
		{
			now_polygon[polygon_id[i]]=i;
			i++;
		}
		now_polygon[polygon_id[i]]=i;
		if(i==number_of_polygons-1)
		{
			future=987654321;
		}else
		{
			future=polygon_sequence[i+1];
		}
		lsequence[ALLRTree]=now;
		rsequence[ALLRTree]=future;
		for(j=0;j<=number_of_polygons;j++)
		{
		if(now_polygon[j]==-1) continue;
		if(request_cmd[0]=='I')
			insert(ALLRTree,now_polygon[j]);
		else _insert(ALLRTree,now_polygon[j]);
		}
		i++;ALLRTree++;
	}
}
int continue_search(int ProNum,RTreeNode* me,double xx,double yy)
{
	if(me->num==0)
	{
		if(!((me->x1<=xx)&&(me->x2>=xx)&&(me->y1<=yy)&&(me->y2>=yy))) return 0;
		Rans[ProNum][Rnum[ProNum]]=me->id;
		Rnum[ProNum]++;
		return Rnum[ProNum];
	}else
	{
        int i;
		for(i=0;i<me->num;i++)
		{
            RTreeNode* t1=me->next[i];
			if((t1->x1<=xx)
			&&(t1->x2>=xx)
			&&(t1->y1<=yy)
			&&(t1->y2>=yy))
			continue_search(ProNum,me->next[i],xx,yy);
		}
		return Rnum[ProNum];
	}
}
int rec_continue_search(int ProNum,RTreeNode* me,double x1,double y1,double x2,double y2)
{
	if(me->num==0)
	{
		double xx1,xx2,yy1,yy2;
		if(x2<me->x2) xx2=x2; else xx2=me->x2;
		if(x1>me->x1) xx1=x1; else xx1=me->x1;
		if(xx1>xx2) return 0;
		if(y2<me->y2) yy2=y2; else yy2=me->y2;
		if(y1>me->y1) yy1=y1; else yy1=me->y1;
		if(yy1>yy2) return 0;
		Rans[ProNum][Rnum[ProNum]]=me->id;
		Rnum[ProNum]++;
		return Rnum[ProNum];
	}else
	{
		int i ;
		double xx1,xx2,yy1,yy2;
		for(i=0;i<me->num;i++)
		{
			RTreeNode* t1=me->next[i];
			if(x2<t1->x2) xx2=x2; else xx2=t1->x2;
			if(x1>t1->x1) xx1=x1; else xx1=t1->x1;
			if(xx1>xx2) continue;
			if(y2<t1->y2) yy2=y2; else yy2=t1->y2;
			if(y1>t1->y1) yy1=y1; else yy1=t1->y1;
			if(yy1>yy2) continue;
			rec_continue_search(ProNum,me->next[i],xx1,yy1,xx2,yy2);
		}
		return Rnum[ProNum];
	}
}
int full_search(int ProNum,RTreeNode* me)
{
	if(me->num==0)
	{
		Rans[ProNum][Rnum[ProNum]]=me->id;
		Rnum[ProNum]++;
		return Rnum[ProNum];
	}else
	{
        int i;
		for(i=0;i<me->num;i++)
		{
            RTreeNode* t1=me->next[i];
			full_search(ProNum,me->next[i]);
		}
		return Rnum[ProNum];
	}
}
int compare(const void* p,const void* q)
{
	return polygon_id[(*(int*)p)]-polygon_id[(*(int*)q)];
}
int search(int ProNum,RTreeNode* p,double xx,double yy,int time)
{
	int l=0,r=ALLRTree-1,mid;
	Rnum[ProNum]=0;
	while(l!=r)
	{
		mid=(l+r)/2+1;
		if(time<lsequence[mid]) r=mid-1;
		else l=mid;
	}
	if(lsequence[l]>time) return 0;
	if(rsequence[l]<=time) return 0;
	if(p[l].num==-2) return 0;
	else
	{
	continue_search(ProNum,p[l].next[0],xx,yy);
	qsort(&Rans[ProNum][0],Rnum[ProNum],sizeof(Rans[0][0]),compare);
    return Rnum[ProNum];
	}
}
int Rec_search(int ProNum,RTreeNode* p,double x1,double y1,double x2,double y2,int time)
{
	double temp;
	int ii;
	int l=0,r=ALLRTree-1,mid;
	Rnum[ProNum]=0;
	while(l!=r)
	{
		mid=(l+r)/2+1;
		if(time<lsequence[mid]) r=mid-1;
		else l=mid;
	}
	if(lsequence[l]>time) return 0;
	if(rsequence[l]<=time) return 0;
	if(p[l].num==-2) return 0;
	else
	{
	if(x1>x2) {temp=x1;x1=x2;x2=temp;}
	if(y1>y2) {temp=y1;y1=y2;y2=temp;}
	rec_continue_search(ProNum,p[l].next[0],x1,y1,x2,y2);
	qsort(&Rans[ProNum][0],Rnum[ProNum],sizeof(Rans[0][0]),compare);
    //Rnum[ProNum]=0;
	//full_search(ProNum,p[l].next[0]);
    return Rnum[ProNum];
	}
}
//RTree_end----------------------------------------------
