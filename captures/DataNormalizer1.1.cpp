#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#define Config "A1-Config.txt"
#define DataFile "test.txt"
#define NewDataFile "NewFile.dat"
#define Header "#Time,Z,X,Y,Total Gs,\n"

using namespace std;

FILE *pData;
FILE *pNewData;
FILE *pConfig;
int lastChar = NULL;
int config[6];	 /*zxy*/
double data[5]; /*time,z,x,y*/
unsigned char time_char[4];
unsigned int time_int;
string str;
unsigned int currentChar = '\0';



int main()
{
	pData = fopen(DataFile,"r");
	pNewData = fopen(NewDataFile,"w");
	pConfig = fopen(Config,"r");
	if(pData == NULL || pNewData == NULL || pConfig == NULL) {printf("ERROR OPENING FILE"); return 0;}
	fprintf(pNewData,Header);
	
	for(int j = 0;j<6;j++)
	{
		currentChar = fgetc(pConfig);
		while(currentChar != ',' && currentChar != ';')
		{
			str.append(1,currentChar);
			currentChar = fgetc(pConfig);
		}
		config[j] = atoi(str.c_str());
		str.clear();
	}
	while(currentChar!= EOF)
	{
		for(int i = 0;i<4;i++)
		{
			if(currentChar == EOF) break;
			if(i==0)
			{
				if(fread((void*)time_char,1,4,pData)!=4) {currentChar = EOF;}
				for(int k=0;k<4;k++)
				{
					time_int += time_char[k] << (k*8);
				}
				data[i] = (double)time_int;
				data[i] /= 100;
				time_int = 0;
			}
			else if(i>0)
			{
				if(!fscanf(pData,"%c",&currentChar)) printf("error");
				data[i] = currentChar;
				data[i] -= config[(2*(i-1))];
				data[i] /= (double)config[(2*(i-1) + 1)];
			}
		}
		if(currentChar == EOF) break;
		data[4] = sqrt(pow(data[1],2)+pow(data[2],2)+pow(data[3],2));
		for(int i = 0;i<5;i++)
		{
			if(i<4)
			{
				fprintf(pNewData,"%f ",data[i]);
			}
			else
			{
				fprintf(pNewData,"%f",data[i]);
			}
		}
		fprintf(pNewData,"\n");
		printf("%lf,%lf,%lf,%lf,%lf\n",data[0],data[1],data[2],data[3],data[4]);
	}
	fclose(pData);
	fclose(pNewData);
	return 0;
}

