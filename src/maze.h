#include <EEPROM.h>

template <class T> int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          EEPROM.write(ee++, *p++);
    return i;
}

template <class T> int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;
    for (i = 0; i < sizeof(value); i++)
          *p++ = EEPROM.read(ee++);
    return i;
}
float absolute(float x)
{
    if(x < 0){return -x;}
    return x;
}
enum mazeColors {red,green,white,yellow,unknown};
struct Color
{
    uint16_t r,g,b;
    mazeColors getMazeCol(){
        if(r > g)return mazeColors::red;
        else if(g > b)return mazeColors::yellow;    
        else if(g > r){
            if(b > 3000)return mazeColors::white;
            else return mazeColors::green;
        }
        else{
            return mazeColors::unknown;
        }
    }
};

#define max_path_len 60
char path[max_path_len];
int pathLength = 0;

bool justSolved = false;
void clean(){
    char neew[pathLength];
    int newlen = 0;
    for(int i=0;i<pathLength;i++){
        if(path[i] != 0){
            neew[newlen++] = path[i];
        }
    }
    neew[newlen++] = 0;

    pathLength = 0;
    for(int i=0;i<newlen;i++){
        path[pathLength++] = neew[i];
    }
}
void solve(int j,char res){
    path[j] = 0;
    path[j+1] = 0;
    path[j+2] = res;
    justSolved = true;
    clean();
}

bool check (char buff[3],String targ){
    return buff[0] == targ[0] &&buff[1] == targ[1] &&buff[2] == targ[2];
}
void shortPath(){
    for(int j=0;j<pathLength-3;j++){
        char buff[3] = {0,0,0};
        for(int i=0;i<3;i++){
            if(j+i >= pathLength)break;
            buff[i] = path[j+i];
        }
        if(check(buff,"LBR")){
            solve(j,'B');
        }
        else if(check(buff,"LBS")){
            solve(j,'R');
        }
        else if(check(buff,"RBL")){
            solve(j,'B');
        }
        else if(check(buff,"SBL")){
            solve(j,'R');
        }
        else if(check(buff,"SBS")){
            solve(j,'B');
        }
        else if(check(buff,"LBL")){
            solve(j,'S');

        }
        if(justSolved){
            j = -1;
            justSolved = false;
        }

    }

}

//L R B S
void add(char direction){
    path[pathLength++] = direction;
    path[pathLength] = '\0';
}
void resetPath(){
    for(int i=0;i<max_path_len;i++){
        path[i] = 0;
    }
    pathLength = 0;
}
void storePath(){
    EEPROM_writeAnything(0,path);
}
void showPath(){
    char read[max_path_len];
    EEPROM_readAnything(0,read);
    Serial.print("Collected : ");
    Serial.println(read);
}

mazeColors finish_line = mazeColors::unknown;