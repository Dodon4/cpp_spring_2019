#include "numbers.dat"
#include <iostream>
int checkSimple(int x);
int error(int argc);
int point(int A1);
int count(int A1,int A2);
int main(int argc, char* argv[])
{
    if(error(argc) !=0 ){//проверка правильности ввода аргументов
        std::cout << -1 << "\n";
        return -1;
    }
    int A1 = std::atoi(argv[1]);
    int A2 = std::atoi(argv[2]);
    for (int i = 4; i <= argc; i += 2){
      if(std::atoi(argv[i-1]) > A1)
          A1 = std::atoi(argv[i-1]);
      if(std::atoi(argv[i]) < A2)
          A2 = std::atoi (argv[i]);
    }
    if(A1 == A2){   //если пара чисел равны
        std::cout << point(A1) << "\n";
        return point(A1);
    }
    if(A1 > A2){    //если первое число больше,то ошибка
        std::cout << 0 <<"\n";
        return 0;
    }
    
      
    std::cout << count(A1,A2) << "\n";
    return 0;
}

int checkSimple(int x){     //проверка на простое число
    if(x == 1)
        return 0;
    for(int i = 2; i * i <= x; i++)
        if( x % i == 0)
            return 0;
    return 1;
}
int error(int argc){    //проверка на ошибку
    if(argc == 1){      //если нет аргументов
      return -1;
    }
    if(argc % 2 == 0){  //если нечетное число аргументов,то ошибка
        return -1;
    }
    return 0;
}
int point(int A1){      //число простых чисел,если одинаковое число
    int sum=0;
    for(int i=0; i <= Size; i++){
            if(A1 == Data[i])
                sum += checkSimple(A1);
            if(A1 < Data[i])
                break;
        }
    return sum;
}
int count(int A1,int A2){   //подсчет простых чисел
    int con1, con2, pr=0;
    for(int j = 0; j <= Size; j++){
            if(A1 == Data[j]){
                con1 = j - pr;
                pr++;
            }
            if (A1< Data[j] && pr==0){  //если нет первого числа в массиве
                return 0;
            }
            if(A2 < Data[j]){
                if(A2 == Data[j-1]){
                    con2 = j-1;
                } 
                else {  //если нет второго числа в массиве
                    return 0; 
                }
                break;
            }
    }
    int sum=0;
    for(int k = con1; k <= con2; k++)
        sum += checkSimple(Data[k]);    //число простых чисел
    return sum;

}
