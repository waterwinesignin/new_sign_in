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

int main(int argc, char *argv[])
{
    srand(time(NULL));//产生随机种子
    FILE * pf=fopen("./Gen.txt", "w");

    for(int ke=1;ke<=5;ke++)
    {
        //有5-8位同学缺席了该学期80%的课
        int num1=rand()%4 + 5;//缺席80%的人数
        set<int> outQueXi80;//缺席该课程80%的学号集合
        GetRandVec(num1, 90, NULL, outQueXi80);

        map<int, set<int> > QueXi80Info;
        for(set<int>::const_iterator it=outQueXi80.begin();it!=outQueXi80.end();it++)
        {
            set<int> tmp;
            GetRandVec(16,20,NULL, tmp);
            QueXi80Info[*it]=tmp;
        }

        for(int i=0;i<20;i++)//一学期共20次课
        {
            //此外每次课程均还有0-3位同学由于各种原因缺席
            int num2=rand()%4;
            set<int> outQueXi0_3;//其他原因缺席的
            if(num2>0)
            {
                GetRandVec(num2, 90, &outQueXi80, outQueXi0_3);
            }


            char line[MAXSIZE]={0};
            sprintf(line, "%d_%d=", ke, i+1);
            for(map<int, set<int> >::const_iterator it2=QueXi80Info.begin();it2!=QueXi80Info.end();it2++)
            {
                if(it2->second.find(i+1) != it2->second.end())
                {
                    char tmp[32]={0};
                    sprintf(tmp, "%d,", it2->first);
                    strcat(line, tmp);
                }
            }

            for(set<int>::const_iterator it3=outQueXi0_3.begin();it3!=outQueXi0_3.end();it3++)
            {
                char tmp[32]={0};
                sprintf(tmp, "%d,", *it3);
                strcat(line, tmp);
            }

            strcat(line, "\n");

            fputs(line, pf);
        }
    }


    fclose(pf);

    return 0;
}
