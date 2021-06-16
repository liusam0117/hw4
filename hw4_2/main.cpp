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
    //The mbed RPC classes are now wrapped to create an RPC enabled version - see RpcClasses.h so don't add to base class
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
    double x2 = in->getArg<double>();
    double y1 = in->getArg<double>();
    double y2 = in->getArg<double>();
    double del_x = x1 - x2;
    double del_y = y1 - y2;
    double slope = (del_x) / (del_y);

    if (x2 > x1+20){
        car.turn(-30, -0.5);
        ThisThread::sleep_for(100ms);
        car.stop();
    }
    else if (x2 < x1-20) {
        car.turn(-30, 0.5);
        ThisThread::sleep_for(100ms);
        car.stop();
    }
    else {
        car.goStraight(-30);
        ThisThread::sleep_for(100ms);
        car.stop();
    }
    // if (slope > 0.3) {           // turn left
    //     car.turn(-30, -0.5);
    //     ThisThread::sleep_for(100ms);
    //     car.stop();
    // }
    // else if (slope < -0.3) {    // turn right
    //     car.turn(-30, 0.5);
    //     ThisThread::sleep_for(100ms);
    //     car.stop();
    // }
    // else {                      // gostraight
    //     car.goStraight(-30);
    //     ThisThread::sleep_for(100ms);
    //     car.stop();
    // }
}