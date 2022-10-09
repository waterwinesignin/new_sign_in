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

//判断某各点名是否缺席(是否有效点名)
//no-学号，1到90
//k2-课程号， 1到5
//ci-课次， 1到20
//data-从文件读取的，有Gen程序随机生成的全部课程全部人员的缺勤信息
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
    int cnt;//已缺席该课程的次数
    double gailu;//该生下次缺席该课程的概率
};
bool MyCmp(const XueSheng& x,const XueSheng& y)
{
    if(x.cnt>y.cnt)
    {
        return true;
    }
    if(x.cnt<y.cnt)
    {
        return false;
    }
    return x.no<y.no;
}
bool MyCmp2(const XueSheng& x,const XueSheng& y)
{
    if(x.gailu>y.gailu)
    {
        return true;
    }
    if(x.gailu<y.gailu)
    {
        return false;
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

    //到这里，data保存了全部课程全部人员的缺勤信息,后面通过调用函数IsQueXi可以在表data中查找某个点名是否是有效点名
    //如何设计算法，输入（no, ke, ci)以使函数调用IsQueXi返回为真的概率最大，是后续需要完善的。

    /*
    for(map<int, map<int, set<int> > >::const_iterator i1=data.begin();i1!=data.end();i1++)
    {
    for(map<int, set<int> >::const_iterator i2=i1->second.begin();i2!=i1->second.end();i2++)
    {
    for(set<int>::const_iterator i3=i2->second.begin();i3!=i2->second.end();i3++)
    {
    printf("%d_%d=%d\n", i1->first, i2->first, *i3);
    }
    }
    }
    */
    //有5-8位同学缺席了该学期80%的课,取平均值吧
    double quexi80=(5+6+7+8)/4.0;//无故缺席80%的人数
    double quexi0_3=(0+1+2+3)/4.0;//其他原因缺席的人数

    FILE * pf2=fopen("./Test.txt", "w");

    int exit=0;
    //double e=0;
    int total=0;
    int quexiTotal=0;

    for(int ke=1;ke<=5;ke++)
    {
        vector<XueSheng> xs;
        //初始化
        for(int i = 0;i<90;i++)
        {
            XueSheng tmp;
            tmp.no=i+1;
            tmp.cnt=0;
            tmp.gailu=quexi80/90*0.8;
            tmp.gailu += (1-tmp.gailu)*(quexi0_3/90);
            xs.push_back(tmp);
        }

		int iQuexiDuo=0;//缺席不低于2次的人数
        for(int j=1;j<=20;j++)
        {			
			for(int k=0;k<90;k++)
			{
				if(xs[k].cnt>=2)
				{
					iQuexiDuo++;
				}
			}

            for(int k=0;k<90;k++)
            {
                if(iQuexiDuo > 5 && total!=0 && xs[k].gailu <= (double)quexiTotal/total)
                {
                    break;
                }
                char line[128]={0};
                total++;
                if(IsQueXi(xs[k].no, ke, j, data))
                {
                   xs[k].cnt++;
                   quexiTotal++;
                   sprintf(line, "KE%d-CI%d-NO%d:QueXi?%d\n", ke, j, xs[k].no, 1);
                }
                else
                {
                   sprintf(line, "KE%d-CI%d-NO%d:QueXi?%d\n", ke, j, xs[k].no, 0);
                }
                fputs(line, pf2);
            }

            //调整概率
            sort(xs.begin(),xs.end(),MyCmp);
            int i =0;
            for(;i<90;i++)
            {
				if(i<5|| i<8&&xs[i].cnt>=2)// || i<8&&xs[i].cnt>=2)
				{
					xs[i].gailu = (16-xs[i].cnt)/16.0;
				}
				else// if(xs[i].cnt>=1)
				{
					//xs[i].gailu = (quexi0_3+xs[i].cnt)/90;
					xs[i].gailu = quexi0_3/90;
				}				
                //xs[i].gailu = quexi0_3/90;
            }

            sort(xs.begin(),xs.end(),MyCmp2);
            if(total!=0 && xs[0].gailu <= (double)quexiTotal/total)
            {
                exit=true;
                break;
            }
        }

        if(exit)
        {
            break;
        }

    }

    char line[128]={0};
    float e=(float)quexiTotal/total;
    sprintf(line, "total=%d,quexiTotal=%d,E=%.2f\n", total, quexiTotal, e);
    fputs(line, pf2);

    fclose(pf2);
	printf(line, "total=%d,quexiTotal=%d,E=%.2f\n", total, quexiTotal, e);
    return 0;
}
