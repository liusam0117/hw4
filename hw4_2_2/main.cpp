#include "mbed.h"
#include "bbcar.h"
#include "mbed_rpc.h"


Ticker servo_ticker;
PwmOut pin5(D5), pin6(D6);
BufferedSerial pc(USBTX,USBRX); //tx,rx
BufferedSerial uart(D1,D0); //tx,rx
BBCar car(pin5, pin6, servo_ticker);

void line(Arguments *in, Reply *out);
RPCFunction Line(&line, "line");


int main() {
    // The mbed RPC classes are now wrapped to create an RPC enabled version - see RpcClasses.h so don't add to base class
    // receive commands, and send back the responses
    char buf[256], outbuf[256];
    FILE *devin = fdopen(&uart, "r");
    FILE *devout = fdopen(&uart, "w");

    while(1) {
        memset(buf, 0, 256);      // clear buffer
        for(int i=0; ; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        printf("%s\r\n", outbuf);
    }
}

void line(Arguments *in, Reply *out){
    double x1 = in->getArg<double>();
    double y1 = in->getArg<double>();
    double x2 = in->getArg<double>();
    double y2 = in->getArg<double>();
    double length = in->getArg<double>();
    double theta = in->getArg<double>();
    double rho = in->getArg<double>();
    if (rho < 0) {
        theta = 180 - theta;
    }
    if (rho < 0 && theta < 70 && theta > 10) {
        car.goStraight(50);
        car.turn(25, 0.7);
        ThisThread::sleep_for(100ms);
    }
    else if (rho > 0 && theta < 70 && theta > 10) {
        car.goStraight(50);
        car.turn(25, -0.7);
        ThisThread::sleep_for(100ms);
    }
    else {
        car.goStraight(25);
        ThisThread::sleep_for(100ms);
    }
    return ;
    // if (rho < 0 && (theta-90 >= 1 || theta-90 <= -1)) {
    //     car.turn(30, 0.5);
    //     ThisThread::sleep_for(100ms);
    //     car.stop();
        
    // }
    // else if (rho > 0 && (theta-90 >= 1 || theta-90 <= -1)) {
    //     car.turn(30, -0.5);
    //     ThisThread::sleep_for(100ms);
    //     car.stop();
    // }
    // else {
    //     car.goStraight(30);
    //     ThisThread::sleep_for(100ms);
    // }
    
    // if (rho < 0 && theta != 0) {  // line from right down to left up
    //     car.turn(-30, -0.7);
    //     ThisThread::sleep_for(100ms);
    // }
    // else if (rho > 0 && theta != 0) {     // line from left down to right up
    //     car.turn(-30, 0.7);
    //     ThisThread::sleep_for(100ms);
    // }
    // else {
    //     car.goStraight(-30);
    //     ThisThread::sleep_for(100ms);
    // }
}