#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#define Config "A1-Config.txt"
#define DataFile "test.txt"
#define Z1LOW	120
#define Z1HIGH	136
#define Z2LOW	15
#define Z2HIGH	19
#define X1LOW	120
#define X1HIGH	136
#define X2LOW	15
#define X2HIGH	19
#define Y1LOW	120
#define Y1HIGH	136
#define Y2LOW	15
#define Y2HIGH	19

using namespace std;


inline void config_fill(int z1,int z2,int x1,int x2,int y1,int y2);
inline void newConfig_fill(void);

FILE *pData;
FILE *pConfig;
int lastChar = NULL;
int j = 0;
int n = 0;
int config[6];	 /*zxy*/
int z1,z2,x1,x2,y1,y2;
int newConfig[6];	  /*New Config Data*/
double data[162][5]; /*time,z,x,y*/
double buffer[4] = {0};	  /*Buffer for data*/
double mean = 0;	/*average total Gs*/
double sd = 0;	  	/*standard deviation of Total Gs samples*/
double new_sd = 100; /* standard deviation*/
double best_mean = 100;	/*best mean approximation*/
unsigned char time_char[4];
unsigned int time_int;
string str;
unsigned int currentChar = '\0';



int main()
{
	pData = fopen(DataFile,"r");
	pConfig = fopen(Config,"a");
	if(pData == NULL || pConfig == NULL) {printf("ERROR OPENING FILE"); return 0;}
	
	for(int i=0;i<6;i++)
	{
		newConfig[i] = 0;
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
				data[j][i] = (double)time_int;
				data[j][i] /= 100;
				time_int = 0;
			}
			else if(i>0)
			{
				if(!fscanf(pData,"%c",&currentChar)) printf("error");
				data[j][i] = currentChar;
			}
		}
		if(currentChar == EOF) break;
		j++;
	}
	for(z1 = Z1LOW;z1<=Z1HIGH;z1++){
		for(z2 = Z2LOW;z2<=Z2HIGH;z2++){
			for(x1 = X1LOW;x1<=X1HIGH;x1++){
				for(x2 = X2LOW;x2<=X2HIGH;x2++){
					for(y1 = Y1LOW;y1<=Y1HIGH;y1++){
						for(y2 = Y2LOW;y2<=Y2HIGH;y2++){
							config_fill(z1,z2,x1,x2,y1,y2);
							mean = 0;
							sd = 0;
							for(j=0;j<162;j++){
								for(int i=0;i<3;i++){
									buffer[i] = data[j][(i+1)];
									buffer[i] -= (double)config[(2*i)];
									buffer[i] /= (double)config[(2*i + 1)];
								}
								data[j][4] = sqrt(pow(buffer[0],2)+pow(buffer[1],2)+pow(buffer[2],2));
								mean += data[j][4];
							}
							mean /= 162.0;
							for(j=0;j<162;j++){
								sd += pow(abs(data[j][4] - mean),2);
							}
							sd = sqrt(sd)/162.0;
							if(abs(mean-1.0)<0.05 && sd<new_sd)
							{
								new_sd = sd;
								best_mean = mean;
								newConfig_fill();
							}
						}
					}
				}
				printf("%d%%\tbest_mean: %lf\tmean: %lf\tnew_sd: %lf\r",((100*n)/((Z1HIGH-Z1LOW+1)*(Z2HIGH-Z2LOW+1))),best_mean,mean,new_sd);
			}
			n++;
		}
	}
	printf("mean: %lf\tsd:%lf\nnewConfig: %d, %d, %d, %d, %d, %d\n",best_mean,new_sd,newConfig[0],newConfig[1],newConfig[2],newConfig[3],newConfig[4],newConfig[5]);
	fclose(pData);
	fclose(pConfig);
	return 0;
}

//github

inline void config_fill(int z1,int z2,int x1,int x2,int y1,int y2)
{
	config[0] = z1;
	config[1] = z2;
	config[2] = x1;
	config[3] = x2;
	config[4] = y1;
	config[5] = y2;
	return;
}

inline void newConfig_fill(void)
{
	for(int i=0;i<6;i++)
	{
		newConfig[i] = config[i];
	}
	return;
}

