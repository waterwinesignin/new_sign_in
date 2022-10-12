#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <set>
#include  <map>

using namespace std;

#define MAXSIZE    1024     //定义数据缓冲区大小

/*
生成文件说明.每行表示某课程某次课的所有缺席学生学号，比如out.txt的一行数据:
3_18=34,40,44,56,77,43,//表示课程3第18次课，缺席人分别为34号、40号、44号、56号、77号、43号
*/

//获取>=1且<=maxNum的一组不相等的随机数，num为要得到的随机数个数
void GetRandVec(int num, int maxNum,const set<int> *in, set<int> &out)
{
    for(int i=0;i<num;i++)
    {
        while(1)
        {
            int n=rand()%maxNum + 1;//>=1且<=90的一的随机数

            if(in!=NULL && in->find(n)!=in->end())
            {
                continue;
            }

            if(out.find(n) != out.end())
            {
                continue;
            }

            out.insert(n);
            break;
        }
    }
}

//集合的加法:out=out+in
void SetHeBing(const set<int> &in, set<int>&out)
{
	for(set<int>::const_iterator it=in.begin();it!=in.end();it++)
	{
		out.insert(*it);
	}
}

int main(int argc, char *argv[])
{
    srand(time(NULL));//产生随机种子

//等级1的学生(学渣)缺席所有(5门)课程90%的课次
//等级2的学生(准学渣)缺席4门课程80%的课次
//等级3的学生(后进生)缺席1门课程80%的课次
//等级4的学生(普通生)，每门课都有0-3其他原因缺席的学生
//等级5的学生(学霸)，从不缺席任何课的任何课次
	set<int>used;//已使用的
	set<int>jd1s;//绩点1的学生集合
	GetRandVec(4, 90, NULL, jd1s);////随机生成“学渣”4人
	SetHeBing(jd1s,used);//used=jd1s

	set<int>jd2s;
	GetRandVec(8, 90, &used, jd2s);////随机生成“准学渣”8人.注意参数used表示生成的jd2s与used无交集
	SetHeBing(jd2s,used);//used=jd1s+jd2s

	set<int>jd3s;
	GetRandVec(18, 90, &used, jd3s);////随机生成“后进生”18人.注意参数used表示生成的jd3s与used无交集
	SetHeBing(jd3s,used);//used=jd1s+jd2s+jd3s

	set<int>jd5s;
	GetRandVec(5, 90, &used, jd5s);////随机生成“学霸”5人.注意参数used表示生成的jd5s与used无交集
	SetHeBing(jd5s,used);//used=jd1s+jd2s+jd3s+jd5s，到这里used为非普通生集合	

	//要写入文件的缺勤信息表
	//课号->课次->学号集
	map<int, map<int,set<int> > >toFileQueXiInfos;

	//等级1的学生(学渣)缺席所有(5门)课程90%的课次
	for(set<int>::const_iterator p1=jd1s.begin();p1!=jd1s.end();p1++)
	{
		for(int ke=1;ke<=5;ke++)
		{
			set<int> tmp;//tmp是课次集合，其元素为1-20的整数(课次编号)
			GetRandVec(18,20,NULL, tmp);//20的90%是18
			for(set<int>::const_iterator i1=tmp.begin();i1!=tmp.end();i1++)
			{
				toFileQueXiInfos[ke][*i1].insert(*p1);
			}
		}
	}

	//等级2的学生(准学渣)缺席4门课程80%的课次
	for(set<int>::const_iterator p2=jd2s.begin();p2!=jd2s.end();p2++)
	{
		set<int> kes;
		GetRandVec(4,5,NULL, kes);//五门课中随机4门
		for(set<int>::const_iterator ke=kes.begin();ke!=kes.end();ke++)
		{
			set<int> tmp;//tmp是课次集合，其元素为1-20的整数(课次编号)
			GetRandVec(16,20,NULL, tmp);//20的80%是16
			for(set<int>::const_iterator i1=tmp.begin();i1!=tmp.end();i1++)
			{
				toFileQueXiInfos[*ke][*i1].insert(*p2);
			}
		}
	}

	//等级3的学生(后进生)缺席1门课程80%的课次
	for(set<int>::const_iterator p3=jd3s.begin();p3!=jd3s.end();p3++)
	{
		int ke=rand()%5+1;
		set<int> tmp;//tmp是课次集合，其元素为1-20的整数(课次编号)
		GetRandVec(16,20,NULL, tmp);//20的80%是16
		for(set<int>::const_iterator i1=tmp.begin();i1!=tmp.end();i1++)
		{
			toFileQueXiInfos[ke][*i1].insert(*p3);
		}
	}	

	 //现在生成“每次课程均还有0-3位同学由于各种原因缺席”的缺勤信息.---注意:这些学生是普通生.
    for(int ke=1;ke<=5;ke++)
    {
        for(int i=1;i<=20;i++)//一学期共20次课
        {           
            int num2=rand()%4;
            set<int> outQueXi0_3;//其他原因缺席的
            if(num2>0)
            {
                GetRandVec(num2, 90, &used, outQueXi0_3);//传递 &used表示排除非普通生
				for(set<int>::const_iterator i1=outQueXi0_3.begin();i1!=outQueXi0_3.end();i1++)
				{
					toFileQueXiInfos[ke][i].insert(*i1);
				}
            }
		}
	}

	
	FILE * pf=fopen("./Gen.txt", "w");

	//把成绩写入文件，格式(只写等级1、2、3、5的人的学号，其余人为等级4):
	//jd1=2,3,8, ...
	//jd2=6,35,56, ..
	//jd3=...
	//jd5=...
	set<int> *pSets[]={NULL, &jd1s, &jd2s, &jd3s, NULL, &jd5s};
	for(int i=1;i<=5;i++)
	{
		if(pSets[i]==NULL)
			continue;
		char line[MAXSIZE]={0};
        sprintf(line, "jd%d=", i);
		for(set<int>::const_iterator pi=pSets[i]->begin();pi!=pSets[i]->end();pi++)
		{
			char tmp[32]={0};
            sprintf(tmp, "%d,", *pi);
            strcat(line, tmp);	
		}
		strcat(line, "\n");
        fputs(line, pf);
	}

	//把缺勤信息写入文件保存
	for(map<int, map<int,set<int> > >::const_iterator pk=toFileQueXiInfos.begin();pk!=toFileQueXiInfos.end();pk++)
	{
		for(map<int,set<int> >::const_iterator pc=pk->second.begin();pc!=pk->second.end();pc++)
		{
			char line[MAXSIZE]={0};
            sprintf(line, "%d_%d=", pk->first, pc->first);
			for(set<int>::const_iterator pn=pc->second.begin();pn!=pc->second.end();pn++)
			{
				char tmp[32]={0};
                sprintf(tmp, "%d,", *pn);
                strcat(line, tmp);	
			}
			strcat(line, "\n");
            fputs(line, pf);
		}
	}

    fclose(pf);

    return 0;
}
