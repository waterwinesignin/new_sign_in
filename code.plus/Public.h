#pragma once


inline int GetJiDianInfo(int no)
{
	static int  jd5[]={44, 89, 24, 17, 13};
	static int  jd1[]={3,33, 77, 55};
	static int  jd2[]={12,22,32,43,53,64,74,84};
	static int  jd3[]={6,8,16,18,26,28,36,38,46,48,56,58,66,68,76,78,86,88};
	
	int *jds[]={jd1, jd2, jd3, jd5};
	int lens[]={sizeof(jd1), sizeof(jd2),sizeof(jd3),sizeof(jd5)};
	int vals[]={1,2,3,5};
	for(int i=0;i<4;i++)
	{
		for(int j = 0;j<lens[i];j++)
		{
			if(no==jds[i][j])
			{
				return vals[i];
			}
		}
	}
	return 4;
}