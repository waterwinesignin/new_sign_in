#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <set>
#include  <map>
#include "../Public/Public.h"

using namespace std;

#define MAXSIZE    1024     //定义数据缓冲区大小

/*
生成文件说明.每行表示某课程某次课的所有缺席学生学号，比如out.txt的一行数据:
3_18=34,40,44,56,77,43,//表示课程3第18次课，缺席人分别为34号、40号、44号、56号、77号、43号
*/

//为了增加不同课程之间缺席的关联，规定“每门课程有5-8位同学缺席了该学期80%的课, 其中有3-4位缺席所有课程80%的课”


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

int main(int argc, char *argv[])
{
    srand(time(NULL));//产生随机种子
   

	//要写入文件的缺勤信息表
	//课号->课次->学号集
	map<int, map<int,set<int> > >toFileQueXiInfos;

	set<int> jdNot4XS;//非普通生
	//先生成绩点相关的缺勤信息
	for(int i=1;i<=90;i++)
	{//i为学号
		int jd=GetJiDianInfo(i);

		if(jd==4)
		{//绩点4的学生(普通生)，每门课都有0-3其他原因缺席的学生. 缺勤信息后面处理
			continue;
		}
		jdNot4XS.insert(i);

		if(jd==5)
		{//不处理
			continue;
		}
		if(jd==1)
		{//绩点1的学生(学渣)缺席所有(5门)课程90%的课次
			for(int ke=1;ke<=5;ke++)
			{
				 set<int> tmp;//tmp是课次集合，其元素为1-20的整数(课次编号)
				 GetRandVec(18,20,NULL, tmp);//20的90%是18
				 for(set<int>::const_iterator i1=tmp.begin();i1!=tmp.end();i1++)
				 {
					 toFileQueXiInfos[ke][*i1].insert(i);
				 }
			}
			continue;
		}

		if(jd==2)
		{//绩点2的学生(准学渣)缺席4门课程80%的课次
			set<int> kes;
			GetRandVec(4,5,NULL, kes);//五门课中随机4门
			for(set<int>::const_iterator ke=kes.begin();ke!=kes.end();ke++)
			{
				set<int> tmp;//tmp是课次集合，其元素为1-20的整数(课次编号)
				GetRandVec(16,20,NULL, tmp);//20的80%是16
				for(set<int>::const_iterator i1=tmp.begin();i1!=tmp.end();i1++)
				{
					toFileQueXiInfos[*ke][*i1].insert(i);
				}
			}
			continue;
		}

		if(jd==3)
		{//绩点3的学生(后进生)缺席1门课程80%的课次
			int ke=rand()%5+1;
			set<int> tmp;//tmp是课次集合，其元素为1-20的整数(课次编号)
			GetRandVec(16,20,NULL, tmp);//20的80%是16
			for(set<int>::const_iterator i1=tmp.begin();i1!=tmp.end();i1++)
			{
				toFileQueXiInfos[ke][*i1].insert(i);
			}
			continue;
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
                GetRandVec(num2, 90, &jdNot4XS, outQueXi0_3);//传递 &jdNot4XS表示排除非普通生
				for(set<int>::const_iterator i1=outQueXi0_3.begin();i1!=outQueXi0_3.end();i1++)
				{
					toFileQueXiInfos[ke][i].insert(*i1);
				}
            }
		}
	}

	//把缺勤信息写入文件保存
	FILE * pf=fopen("./Gen.txt", "w");
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
