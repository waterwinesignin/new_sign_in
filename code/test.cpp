#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <set>
#include <algorithm>
#include <map>
using namespace std;

#define MAXSIZE    1024     //定义数据缓冲区大小

//判断某个点名是否缺席(是否有效点名)
//no-学号，1到90
//k2-课程号， 1到5
//ci-课次， 1到20
//data-从文件读取的，由Gen程序随机生成的全部课程全部人员的缺勤信息表
int IsAbsence(int no, int ke, int ci, const map<int, map<int, set<int> > > &data)
{
	map<int, map<int, set<int> > >::const_iterator i1=data.find(ke);
	if(i1==data.end())
	{
		return 0;
	}

	map<int, set<int> >::const_iterator i2=i1->second.find(ci);
	if(i2==i1->second.end())
	{
		return 0;
	}

	set<int>::const_iterator i3=i2->second.find(no);
	if(i3==i2->second.end())
	{
		return 0;
	}

	return 1;
}


//某门课的点名册表项
//点名册由90个下面元素组成，每个元素表示该课程的学生当前信息
struct Student 
{
	int no;//学号
	int cnt;//已缺席该课程的次数
	int noCnt;//未缺席次数
	int pro;
};

int GetLevel(const Student& x)
{//返回越大，优先级越高
	if(x.cnt==16)
	{
		return 0;
	}
	if(x.noCnt >= 5)
	{		
		return 1;
	}

	if(x.cnt>=3)
	{
		return 20-x.cnt; //5,6,...,17
	}
	//*
	if(x.pro>=3)
	{
		return 20-x.cnt; //
	}
	//*/
	
	return x.cnt+1;//1或2
}
bool MyCmp(const Student& x,const Student& y)
{
	int levelX=GetLevel(x);
	int levelY=GetLevel(y);
	if(levelX!=levelY)
	{
		return levelX > levelY;
	}

	return x.no<y.no;
}

int main(int argc, char *argv[])
{
	//<课程, <课次, <缺席号集> >
	map<int, map<int, set<int> > > data;

	FILE * pf=fopen("./Gen.txt", "r");
	if(NULL==pf)
	{
		printf("fopen error:%s\n\a",strerror(errno));
		exit(1);
	}

	while(1)
	{
		char line[MAXSIZE+1]={0};
		if(NULL==fgets(line, MAXSIZE, pf))
		{
			break;
		}

		//去掉空格字符
		char *p=line;
		while(*p)
		{
			if(*p=='\x20' || *p=='\t' || *p=='\n' || *p=='\r')
			{
				for(char *q=p;*q;q++)
					*q=*(q+1);
			}
			else
			{
				p++;
			}
		}
		if(line[0]==0)
		{
			continue;
		}

		if(line[strlen(line)-1]!=',')
		{
			strcat(line, ",");
		}

		//2_12=17,39,57,59,64,79,
		int ke,ci;//课号，次数
		sscanf(line, "%d_%d=", &ke, &ci);
		char *start=strchr(line, '=');
		if(start==NULL)
		{
			continue;
		}
		start++;

		char *end=strchr(start, ',');
		while(end)
		{
			*end=0;

			data[ke][ci].insert(atoi(start));

			start = end+1;
			end=strchr(start, ',');
		}
	}
	fclose(pf);
	//到这里，data保存了全部课程全部人员的缺勤信息,后面通过调用函数IsAbsence可以在表data中查找某个点名是否是有效点名
	//如何设计算法，输入（no, ke, ci)以使函数调用IsAbsence返回为真的概率最大，是后续需要完善的。

	FILE * pf2=fopen("./Test.txt", "w");

	//double e=0;
	int total=0;
	int absenceTotal=0;

	vector<Student> xs;
	//初始化
	for(int i = 0;i<90;i++)
	{
		Student tmp={0};
		tmp.no=i+1;		
		xs.push_back(tmp);
	}

	for(int ke=1;ke<=5;ke++)
	{
		//每门课开始前，一些变量为0
		vector<Student>::iterator it;
		for(it=xs.begin();it!=xs.end();it++)
		{
			it->cnt=0;
			it->noCnt=0;
		}

		for(int j=1;j<=20;j++)
		{
			sort(xs.begin(),xs.end(),MyCmp);
			int currabsence=0;//某次课的缺席人数
			int mm=0;
			for(it=xs.begin();it!=xs.end();it++,mm++)
			{				
				char line[128]={0};
				total++;
				if(IsAbsence(it->no, ke, j, data))
				{
					it->cnt++;
					absenceTotal++;
					currabsence++;
					sprintf(line, "课程%d第%d次-抽检学号%d结果:缺1\n", ke, j, it->no);
				}
				else
				{
					it->noCnt++;
					sprintf(line, "课程%d第%d次-抽检学号%d结果:到0\n", ke, j, it->no);
				}
				fputs(line, pf2);

				//与课次相关的次数控制
				static int maxabsenceCnt[]={0,7,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};//E=0.30170
				if(currabsence >= maxabsenceCnt[j])
				{
					break;
				}

				if(j>=5 && mm > 10 && (double)currabsence/90 < 0.30)
				{
					break;
				}		
			}			
		}

		for(it=xs.begin();it!=xs.end();it++)
		{
			if(it->cnt >= 2)
			{
				it->pro++;
			}
		}
	}

	char line[128]={0};
	float e=(float)absenceTotal/total;
	sprintf(line, "total=%d,absenceTotal=%d,E=%.5f\n", total, absenceTotal, e);
	fputs(line, pf2);

	fclose(pf2);

	printf(line, "total=%d,absenceTotal=%d,E=%.5f\n", total, absenceTotal, e);

	return 0;
}