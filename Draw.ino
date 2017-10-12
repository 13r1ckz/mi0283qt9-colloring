#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <Wire.h>
#include <GraphicsLib.h>
#include <MI0283QT9.h>

#define BAUDRATE 19200

//Declare display !
MI0283QT9 lcd;  //MI0283QT9 Adapter v1
int count = 0;
int RGBC = 0;
int R = 0;
int G = 0;
int B = 0;
int oldloce = 0;
int oldlocX = 0;
int oldlocY = 0;
int nummer = 0;
volatile uint8_t teller = 0;
int nunchuk_read(int *jx, int *jy,int *ax, int *ay, int *az,int *bz, int *bc);
void nunchuk_init();

ISR(TIMER1_OVF_vect){
	

}

int main(void){ //arduino start
	init();
	Serial.begin(BAUDRATE);
	int valY;
	int valx;
	DDRB = (1<<PORTB3);			//	setup (digital pen 6 = PD6)
	TCCR2A |= (1 << CS02) | (1 << CS00);
	TIMSK1 |= (1<<TOIE0);
	TCNT1 = 0;
	sei();						//	enable interrupts!
	
	//	MI0283QT9 lcd;  //MI0283QT9 Adapter v1
	uint8_t clear_bg=0x00; //0x80 = dont clear background for fonts (only for DisplayXXX)

	//init display
	lcd.begin();

	//clear screen
	lcd.fillScreen(RGB(255,255,255));
	lcd.fillRect (0, 225, 200, 250, RGB(0,0,0));
	lcd.drawText(5, 230, "Place a name", RGB(255,255,0), RGB(0,0,0), 1);
	
	nunchuk_init();
	
	while (1){//arduino loop
		if (TCNT1 >= 250 )			//bij elke 250e interrupt ...
		{
			nummer ++;
			if(nummer >= 3){
				lcd.drawInteger(10, 10, count, DEC, RGB(0,0,0), RGB(255,255,255), 1);
				count++;
				nummer= 0;
			}
			
		}

		if(RGBC == 0){
			R = 255;
			G = 0;
			B = 0;
		}
		if(RGBC == 1){
			R = 0;
			G = 255;
			B = 0;
		}
		if(RGBC == 2){
			R = 0;
			G = 0;
			B = 255;
		}
		if (RGBC == 3){
			RGBC = 0;
		}
		
		int jx, jy, ax, ay, az, bz, bc;
		
		Wire.beginTransmission(0x52); // Request new values from nunchuk.
		Wire.write((byte)0x00);
		Wire.endTransmission();
		delayMicroseconds(200); // Give nunchuk time to respond.
		Wire.requestFrom(0x52, 6);
		byte buf[8];
		int cnt = 0;
		while (Wire.available()) {
			buf[cnt++] = (Wire.read() ^ 0x17) + 0x17;
		}
		if (cnt < 6) {
			return 0;
		}
		jx = buf[0] - 128;
		jy = buf[1] - 128;
		byte b = buf[5];
		ax = ((buf[2] << 2) | ((b >> 2) & 3)) - 512;
		ay = ((buf[3] << 2) | ((b >> 4) & 3)) - 512;
		az = ((buf[4] << 2) | ((b >> 6) & 3)) - 512;
		bz = (b & 1) ^ 1;
		bc = ((b >> 1) & 1) ^ 1;
		
		valY = jy;
		valx = jx;
		valx = valx + 130;
		valY = valY + 400;
		valx = map(valx, 30, 230, 0, 320);
		if(oldloce == 0){

		}
		if(oldloce == 1){
			lcd.fillCircle(oldlocX, oldlocY, 10,RGB(255,255,255));
			oldlocX = valx;
			oldlocY = (valY*-1);
		}
		if(oldloce == 2){
			oldloce = 0;
		}
		lcd.fillCircle(valx, (valY*-1), 10,RGB(R,G,B));
		
		if (bz == 1 && bc == 1)
		{
			lcd.fillScreen(RGB(255,255,255));
			lcd.fillRect (0, 225, 200, 250, RGB(0,0,0));
			lcd.drawText(5, 230, "Place a name", RGB(255,255,0), RGB(0,0,0), 1);
			count = 0;
		}
		if(bz == 1){
			_delay_ms(150);
			oldloce++;
		}
		if (bc == 1){
			_delay_ms(150);
			RGBC++;
		}
	}
	return 0;
}

void nunchuk_init() {
	Wire.begin();
	Wire.beginTransmission(0x52);
	Wire.write((byte)0x40);
	Wire.write((byte)0x00);
	Wire.endTransmission();
}