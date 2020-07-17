#include<stdio.h>
#include<fcntl.h>
#include <iostream>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#include <unistd.h>
#include<string>
using namespace std;
     

char reg[19];
class Realtime {

private:
	
	string path ;
    char addr ;
   
public:


int bus;
int bcdtodec (char b);
int dec2bcd (char x);
 
virtual void check();
virtual void inicom();
virtual void rst();
virtual void buffer();
virtual int write1(char , char);
Realtime(string, char);
virtual ~Realtime();
virtual void day();
virtual void temp();
virtual int w(char, char);

virtual void t(int, int, int);
virtual void d(int, int, int, int);
virtual void a2(int, int, int);
virtual void a1(int, int, int, int);
virtual void frequency(int, int);
virtual void display();
virtual void EOSC(char);
virtual void BATB(int);
};

Realtime :: Realtime (string path, char addr){
	this->path = path;
	this->addr = addr;
		}
	
Realtime::~Realtime(){
	close(bus);
	cout<< "Closed file"<< endl;
	}
	
void Realtime :: check (){
	if ((bus = open(path.c_str(), O_RDWR)) < 0 ){
	cout << "Failed to open the bus" << endl;
	}
}

void Realtime :: inicom (){
	if(ioctl(bus, I2C_SLAVE, addr) < 0){
	cout << "Failed to connect to the sensor" << endl;
	}
}

unsigned char writebuf[1] = {0x00,};
void Realtime :: rst(){
	 if(write(bus, writebuf, 1)!=1)
	{
	cout << "Failed to reset the read address" << endl;
	}
}

int Realtime :: write1(char a, char b){
	char ip[2];
ip[0] = a;
ip[1] = dec2bcd(b);

	 if(write(bus, ip, 2)!=2)
	 {
		 cout << "Writing problem" << endl;
		 return 1;
	 }
	 return 0;
	}
	
	
void Realtime :: buffer(){
	 if(read(bus, reg, 19)!=19){
	 cout << "Failed to read in the buffer" << endl;
	 }
	}
	
int Realtime :: bcdtodec(char b){
return (b/16)*10 + (b%16);
}


int Realtime :: dec2bcd( char x){
return(((x/10) << 4) | (x % 10));
   }
  
 void Realtime:: day(){ 
if(bcdtodec(reg[0x3]) == 1)
{cout<< "Sunday :" ; }
else if(bcdtodec(reg[0x3]) == 2)
{cout<< "Monday :";}
else if(bcdtodec(reg[0x3]) == 3)
{cout<< "Tuesday :";}
else if(bcdtodec(reg[0x3]) == 4)
{cout<< "Wednesday :";}
else if(bcdtodec(reg[0x3]) == 5)
{cout<< "Thursday :";}
else if(bcdtodec(reg[0x3]) == 6)
{cout<< "Friday :";}
else if(bcdtodec(reg[0x3]) == 7)
{cout<< "Saturday :";}
   }
   
   void Realtime:: temp(){
   float temperature = reg[0x11] + ((reg[0x12]>>6)*0.25);
cout << "Temperature :" << temperature << endl;
   }
   
   int Realtime :: w(char a, char b){
	char ip[2];
ip[0] = a;
ip[1] = b;

	 if(write(bus, ip, 2)!=2)
	 {
		 cout << "Writing problem" << endl;
		 return 1;
	 }
	 return 0;
	}
	
void Realtime :: t(int s, int m, int h){
		cout << "writing HH : MM : SS " << endl;
write1(0x00, s);
write1(0x01, m);
write1(0x02, h);
rst();
buffer();
cout << bcdtodec(reg[0x2]) <<":" << bcdtodec(reg[0x1])<< ":" << bcdtodec(reg[0x0]) << endl;
	}
	
void Realtime :: d(int d, int da, int m, int y){
		cout << "writing Day : Date : Month : Year "  << endl;
write1(0x03, d);
write1(0x04, da);
write1(0x05, m);
write1(0x06, y);
day();
rst();
buffer();
cout << bcdtodec(reg[0x4])<< ":" << bcdtodec(reg[0x5])<< ":" <<  bcdtodec(reg[0x6])<< endl;
	}
	
void Realtime :: a2(int min, int hr, int day){
cout << "Setting alaram 2 at" << hr <<":" <<min << ":" << "day" << endl;	
w(0x0F, 0x00);
w(0x0E, 0x07);
write1(0x0B, min);
write1(0x0C, hr);
write1(0x0D, day);
if(bcdtodec(reg[15]) == 02 || bcdtodec(reg[15]) == 01 || bcdtodec(reg[15]) == 03) // checking is any alarm flag is set or not, then making INTCN bit high for intrupt. So it will not distrub for Square wave.
{
	w(0x0E, 0x07);
	
}

}
	
void Realtime :: a1(int s, int m, int h, int da){	
	cout << "Setting alaram 1 at" << h <<":" <<m << ":" << s << ":" << "date" << da << endl;
w(0x0F, 0x00);
w(0x0E, 0x07);
write1(0x07, s);
write1(0x08, m);
write1(0x09, h);
write1(0x0A, da);
if(bcdtodec(reg[15]) == 02 || bcdtodec(reg[15]) == 01 || bcdtodec(reg[15]) == 03) // checking is any alarm flag is set or not, then making INTCN bit high for intrupt. So it will not distrub for Square wave.
{
	w(0x0E, 0x07);
	
}
	}
	
void Realtime :: frequency(int a, int b)
{

w(0x0F, 0x00);
w(0x0E, 0x04);

if(a == 1){
	cout << "generating 1Hz for " << b << "sec" << endl;
w(0x0E, 0x03);sleep(b);}
else if(a == 2){ 
	cout << "generating 1KHz" << b << "sec" << endl;
w(0x0E, 0x0b);
sleep(b);
}
else if(a == 3){
cout << "generating 4KHz"  << b << "sec"<< endl;
w(0x0E, 0x13);
sleep(b);
}
else if(a ==4){
cout << "generating 8KHz" << b << "sec" << endl;
w(0x0E, 0x1b);
sleep(b);
}

w(0x0E, 0x00);
	}
	
void Realtime :: display()
{
	cout << " If LED turns off at specified alarm time then alarm is triggered " << endl;
rst();
buffer();
cout << "Current time is " ;
cout << bcdtodec(reg[0x2]) <<":" << bcdtodec(reg[0x1])<< ":" << bcdtodec(reg[0x0]) << endl;

}	

void Realtime :: EOSC(char c){
	w(0x0E, c);
	cout << " Oscillator will stop only when Vcc changed to Vbat " << endl;
	}
	
void Realtime ::BATB(int a){
 cout << " Frequency is seen across INT/SQW only when Vcc changed to Vbat" << endl;
if(a==1)
w(0x0E, 0x43); // 1Hz
else if(a==2)
w(0x0E, 0x4b); // 1 KHz
else if(a==3)
w(0x0E, 0x53); // 4 KHz
else if(a==4)
w(0x0E, 0x5b); // 8 KHz
else{
	cout << "No frequency selected" << endl;
}
	
}	
	
int main() {

Realtime a = Realtime("/dev/i2c-1", 0x68) ;
a.check();
a.inicom();
a.rst();
a.buffer();

cout << "Before setting Time :" << endl;
cout << a.bcdtodec(reg[0x02]) <<":" << a.bcdtodec(reg[0x01])<< ":" << a.bcdtodec(reg[0x0]) << endl;
cout << "Before setting Date :" << endl;
cout << a.bcdtodec(reg[0x03])<< ":" <<a.bcdtodec(reg[0x04])<< ":" << a.bcdtodec(reg[0x5]) << ":" <<a.bcdtodec(reg[0x6]) << endl;

a.t(50, 55, 4);
a.d(05, 21, 05, 20);
cout << "Reading temperature" << endl;
a.temp();

a.a2(56, 04, 45); // only min:hr:day should be given. where 4 in day should not be changed
a.display();

sleep(15);

a.a1(20, 56, 4, 21); // only sec:min:hr:date can be given
a.display();

sleep(20);

a.frequency(1, 5);
//a.frequency(2, 10);
//a.frequency(3, 5);
//a.frequency(4, 5);


a.EOSC(0x9E); // one should use this function only when oscillators need to be stop and 
               //works only when VCc change to Vbat

a.BATB(0); // should chnge to Vbat
//a.BATB(2);

}

