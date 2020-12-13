int prev_error=0;
int P,I=0,D;

float Kp = 30;
float Ki = 0.2;
float Kd = 0.4;
int PID(int error){
    P =error;
    I += error;
    D = error - prev_error;
    prev_error = error;
    return P*Kp + I/50000 + D*4/1;

}