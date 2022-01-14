#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

  
int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }else{
    int funcID = atoi(argv[1]);
    double a = atof(argv[2]);
    double b = atof(argv[3]);
    int64_t n = atoll(argv[4]);
    int intensity = atoi(argv[5]);

    if(funcID < 1 || funcID > 4){
      std::cerr<<"<functionid> must be 1, 2, 3, or 4"<<std::endl;
      return -1;
    }else{
      auto sTime = std::chrono::system_clock::now();
      double lead = (b-a)/n;
      double sum = 0;
      for(unsigned int i=0;i<n;i++){
	double num = (a+i+.5)*(lead);
	if(funcID == 1){
	  sum += f1(num,intensity);
	}else if(funcID == 2){
	  sum += f2(num,intensity);
	}else if(funcID == 3){
	  sum += f3(num,intensity);
	}else{
	  sum += f4(num,intensity);
	}
      }
      auto eTime = std::chrono::system_clock::now();
      std::chrono::duration<double> tTime = eTime - sTime;
      std::cerr<<tTime.count();
      double output = lead*sum;
      std::cout<<output;
    }
  }
  
  return 0;
}
