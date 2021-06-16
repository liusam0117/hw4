#include "mbed.h"
#include "bbcar.h"
#include "bbcar_rpc.h"

Ticker servo_ticker;
Ticker encoder_ticker;
PwmOut pin5(D5), pin6(D6);
DigitalIn encoder(D11);
BufferedSerial xbee(D1, D0);
BufferedSerial pc(USBTX, USBRX);
BBCar car(pin5, pin6, servo_ticker);

volatile int steps;
volatile int last;

void encoder_control();
void park(Arguments *in, Reply *out);
RPCFunction Parking(&park, "park");

int main() {
    pc.set_baud(9600);
    encoder_ticker.attach(&encoder_control, 10ms);

    char buf[256], outbuf[256];
    FILE *devin = fdopen(&xbee, "r");
    FILE *devout = fdopen(&xbee, "w");
    while (1) {
        memset(buf, 0, 256);
        for( int i = 0; ; i++ ) {
            char recv = fgetc(devin);
            if(recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
    RPC::call(buf, outbuf);
    }
}
void encoder_control() {
   int value = encoder;
   if (!last && value) steps++;
   last = value;
}

void park(Arguments *in, Reply *out) {
    double d1 = in->getArg<double>();
    double d2 = in->getArg<double>();

    if (d2 < 0) {   // west
        steps = 0;
        last = 0;
        car.goStraight(-50);
        while(steps*6.5*3.14/32 < d1) {
            // printf("encoder = %d\r\n", steps);
            ThisThread::sleep_for(10ms);
        }
        car.stop();
        ThisThread::sleep_for(3s);

        car.turn(-100, 0.01);
        ThisThread::sleep_for(1100ms);
        car.stop();
        ThisThread::sleep_for(3s);

        steps = 0;
        last = 0;
        car.goStraight(-50);
        while(steps*6.5*3.14/32 < -d2-10.5) {
            // printf("encoder = %d\r\n", steps);
            ThisThread::sleep_for(10ms);
        }
        car.stop();
    }
    else {          // east
        steps = 0;
        last = 0;
        car.goStraight(-100);
        while(steps*6.5*3.14/32 < d1) {
            // printf("encoder = %d\r\n", steps);
            ThisThread::sleep_for(100ms);
        }
        car.stop();
        ThisThread::sleep_for(3s);

        car.turn(-100, -0.01);
        ThisThread::sleep_for(1200ms);
        car.stop();
        ThisThread::sleep_for(3s);
        steps = 0;
        last = 0;
        car.goStraight(-100);
        while(steps*6.5*3.14/32 < d2-7) {
            // printf("encoder = %d\r\n", steps);
            ThisThread::sleep_for(100ms);
        }
        car.stop();
    }
}