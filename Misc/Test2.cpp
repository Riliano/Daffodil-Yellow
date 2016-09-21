#include<iostream>
#include<fstream>

int file[5];
int main()
{
	std::string temp[3000];
	int i = 0;
	std::fstream cfg ("pmet.cfg");

	cfg.

	temp[0] = "Hello! :)\nlol";
	cfg<<temp[0];
	if(cfg.is_open())
	{
	}else
	{
		std::cout<<"derp"<<std::endl;
	}
	cfg.close();
	/*if(cfg.is_open())
	{
		while (cfg>>temp[i])
		{
			int t = 1;
			for(int j = temp[i].size()-1;j>=0;j--)
:			{
			//	if(temp[i][j]>= '0' and temp[i][j]<='9')
			//	{
					file[i] = file[i] + t*(temp[i][j] - '0');
					t=t*10;
			//	}
			}
			i++;
		}
		cfg.close();
	}
	for(int k = 0;k<i;k++)
	{
		
	}
	std::cout<<file[0]<<" "<<file[1]<<" "<<file[2]<<" "<<file[3]<<":"<<i<<std::endl;
	std::cout<<temp[0]<<" "<<temp[1]<<std::endl;*/
}
