#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <set>
#include <algorithm>
#include  <map>

using namespace std;

#define MAXSIZE    1024     //定义数据缓冲区大小

//判断某个点名是否缺席(是否有效点名)
//no-学号，1到90
//k2-课程号， 1到5
//ci-课次， 1到20
//data-从文件读取的，由Gen程序随机生成的全部课程全部人员的缺勤信息表
int IsQueXi(int no, int ke, int ci, const map<int, map<int, set<int> > > &data)
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
struct XueSheng
{
	int no;//学号
	int jd;//绩点
	int cnt[6];//该生五门课程的缺席数.0号元素不用
	int Notcnt[6];//该生五门课程的未缺席数.0号元素不用
	double gailu;//缺席的概率
};

bool MyCmp(const XueSheng& x,const XueSheng& y)
{
	return x.gailu>y.gailu;
}

double JiSuanGaiLv(int jd, int cnt, int Notcnt)
{
	if(20-cnt-Notcnt <= 0)
	{
		return 0;
	}

	double gailu=0;
	if(jd==1)
	{//绩点1的学生(学渣)缺席所有(5门)课程90%的课次
		gailu= (double)(18-cnt)/(20-cnt-Notcnt);					
	}
	else if(jd==2)
	{//绩点2的学生(准学渣)缺席4门课程80%的课次
		gailu= ((double)16*0.8-cnt)/(20-cnt-Notcnt);
	}
	else if(jd==3)
	{//绩点3的学生(后进生)缺席1门课程80%的课次
		gailu= ((double)4*0.8-cnt)/(20-cnt-Notcnt);				
	}
	else if(jd==4)
	{//绩点4的学生(普通生)，每门课都有0-3其他原因缺席的学生
		gailu=(1.5/90*20 - cnt)/(20-cnt-Notcnt);
	}
	else
	{	//it->jd==5的学霸，概率维持初始值0
		gailu = 0;
	}
	return gailu;
}

int main(int argc, char *argv[])
{
	set<int>jd1s;//绩点1的学生集合
	set<int>jd2s;//绩点2的学生集合
	set<int>jd3s;//绩点3的学生集合
	set<int>jd5s;//绩点5的学生集合
	set<int> *pjd=NULL;

	//<课程, <课次, <缺席号集> >
	map<int, map<int, set<int> > > data;

	FILE * pf=fopen("./Gen.txt", "r");
	if(NULL==pf)
	{
		printf("fopen error:%s\n\a",strerror(errno));
		exit(1);
	}
	int in=0;
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
		if(in<4)
		{//开头4行是绩点信息
			int tmp=0;
			sscanf(line, "jd%d=", &tmp);
			if(tmp==1)
				pjd=&jd1s;
			else if(tmp==2)
				pjd=&jd2s;
			else if(tmp==3)
				pjd=&jd3s;
			else if(tmp==5)
				pjd=&jd5s;
		}
		else
		{
			sscanf(line, "%d_%d=", &ke, &ci);
		}
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

			if(in<4)
			{//开头4行是绩点信息
				pjd->insert(atoi(start));
			}
			else
			{
				data[ke][ci].insert(atoi(start));
			}

			start = end+1;
			end=strchr(start, ',');
		}

		in++;
	}
	fclose(pf);
	//到这里，data保存了全部课程全部人员的缺勤信息,后面通过调用函数IsQueXi可以在表data中查找某个点名是否是有效点名

	FILE * pf2=fopen("./Test.txt", "w");

	//double e=0;
	int total=0;
	int quexiTotal=0;

	vector<XueSheng> xs;
	//初始化
	for(int i = 0;i<90;i++)
	{
		XueSheng tmp={0};
		tmp.no=i+1;	
		if(jd1s.find(tmp.no)!=jd1s.end())
			tmp.jd = 1;
		else if(jd2s.find(tmp.no)!=jd2s.end())
			tmp.jd = 2;
		else if(jd3s.find(tmp.no)!=jd3s.end())
			tmp.jd = 3;
		else if(jd5s.find(tmp.no)!=jd5s.end())
			tmp.jd = 5;
		else
			tmp.jd = 4;
		
		xs.push_back(tmp);
	}

	for(int ke=1;ke<=5;ke++)
	{
		//每门课开始前，一些变量为0
		vector<XueSheng>::iterator it;
		for(it=xs.begin();it!=xs.end();it++)
		{
			it->gailu=JiSuanGaiLv(it->jd, 0, 0);
		}

		int iCI=0;
		for(int j=1;j<=20;j++,iCI++)
		{
			sort(xs.begin(),xs.end(),MyCmp);
			int currQueXi=0;//某次课的缺席人数
			int mm=0;
			for(it=xs.begin();it!=xs.end();it++,mm++)
			{				
				char line[128]={0};
				total++;
				if(IsQueXi(it->no, ke, j, data))
				{
					it->cnt[ke]++;
					quexiTotal++;
					currQueXi++;
					sprintf(line, "课程%d第%d次-抽检学号%d结果:缺1\n", ke, j, it->no);					
				}
				else
				{
					it->Notcnt[ke]++;
					sprintf(line, "课程%d第%d次-抽检学号%d结果:到0\n", ke, j, it->no);
				}
				JiSuanGaiLv(it->jd, it->cnt[ke], it->Notcnt[ke]);
				fputs(line, pf2);

				//与课次相关的次数控制
				//static int maxQueXiCnt[]={0,7,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};//E=0.30170	
				//if(currQueXi >= maxQueXiCnt[j])
				//if(currQueXi >= 4)//这个越小，e值越大。
				static int maxQueXiCnt[]={0,8,7,6,6,5,5,5,4,4,4,4,4,4,4,4,4,4,4,4,4};
				if(currQueXi >= maxQueXiCnt[j])
				{
					break;
				}
				if(j>=5 && mm > 10 && (double)currQueXi/mm < 0.60)
				{
					break;
				}	
			}			
		}
	}

	char line[128]={0};
	float e=(float)quexiTotal/total;
	sprintf(line, "total=%d,quexiTotal=%d,E=%.5f\n", total, quexiTotal, e);
	fputs(line, pf2);

	fclose(pf2);

	printf(line, "total=%d,quexiTotal=%d,E=%.5f\n", total, quexiTotal, e);

	return 0;
}
