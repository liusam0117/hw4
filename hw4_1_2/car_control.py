import time
import serial
import sys,tty,termios
class _Getch:
    def __call__(self):
        fd = sys.stdin.fileno()
        old_settings = termios.tcgetattr(fd)
        try:
            tty.setraw(sys.stdin.fileno())
            ch = sys.stdin.read(1)
        finally:
            termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
        return ch

def get():
    inkey = _Getch()
   
    k = input()
    d1 = int(k)
    k = input()
    d2 = int(k)
    k = input()
    if k == 'east':
        print(d1)
        print(d2)
        distance = (d1, d2)
        s.write(("/park/run %d %d\n" % distance).encode())
        # s.write("/park/run  \n".encode())
        time.sleep(1)
    elif k == 'west':
        print(d1)
        print(d2)
        distance = (d1, -d2)
        s.write(("/park/run %d %d\n" % distance).encode())
        # s.write("/park/run 10 \n".encode())
        time.sleep(1)
    else:
        print ("not an arrow key!")
    return 1

if len(sys.argv) < 1:
    print ("No port input")
s = serial.Serial(sys.argv[1])
while get():
    i = 0