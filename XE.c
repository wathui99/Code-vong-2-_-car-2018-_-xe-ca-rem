﻿#include "function.h"
#define time_get_sample 40.0
#define err_ratio_PID_servo 150.0

#define kp_m 23
#define ki_m 0.005
#define kd_m 0.46

#define distand_square 113

#define handle_case_21	3 //3
#define handle_case_22	12 //12
#define handle_case_23	15 //15 
#define handle_case_24	15 //15
#define handle_case_25	18 //18
#define handle_case_26	27 //27
#define handle_case_27	50 //50
#define handle_case_28	70 //70
#define handle_case_29	90 //90
#define handle_case_210	110 //75
#define handle_case_211	140 //91

#define handle_case_31	-5 //5
#define handle_case_32	-15 //15
#define handle_case_33	-18 //18
#define handle_case_34	-18 //18
#define handle_case_35	-(24) //24
#define handle_case_36	-(29) //29
#define handle_case_37	-50 //-50
#define handle_case_38	-66 //-66
#define handle_case_39	-90 //-90
#define handle_case_310	-115 //90
#define handle_case_311	-140 //95


void case_40();	// cua vuong
void case_41(); // noline
void case_50(); // trai
void case_60(); // phai
void PID_ctrl_motor();
void PID_ctrl_servo();
//dinh huong
uint8_t  pattern_dir;
uint8_t pattern_level;
//toc do pid
volatile uint8_t pulse_v;
volatile uint16_t pulse_v_special=0;
volatile int pre_err;
volatile uint32_t pulse_time_get_sample;
volatile uint8_t vloc=0;
//bien dem cho case dac biet
volatile uint16_t pulse_time_case_special;
//tang toc
volatile uint16_t pulse_time_increase=0;
volatile uint8_t increase_flag=false;
#define time_increase 200

int main(void)
{
	INIT();
	sel_mode();
	//init elements pid
	pulse_time_get_sample=0;
	pulse_v=0;
	pre_err=0;
	//end init
	pattern_dir=10;
	uint8_t conti=true;
	int previous=0; //truong hop truoc do
	pulse_time_case_special=0;
	uint8_t value_special;
	index_list_case=-1;
	while(1)
	{
		value_special = sensor_cmp(0xFF);
		//cua vuong
		if (value_special==0b11111111||value_special==0b11101111) { //truong hop case_40 (cua vuong phai)
			if(index_list_case+1 >= sum) index_list_case=-1;
			index_list_case++;
			if(list_case[index_list_case]==1 || list_case[index_list_case]==2)
				case_40();
			if(list_case[index_list_case]==0) case_41();
		}
		//re trai
		if (value_special==0b11111000 || value_special==0b11111100 || value_special==0b11111110) { // case 50 (queo trai)
			if(previous==210||previous==211||previous==310||previous==311) goto runNormal;
			special_flag=true;
			crook_flag=false;
			straight_flag=false;
			pulse_time_case_special=0;
			pulse_v_special=0;
			while (pulse_time_case_special<50) {
				if(sensor_cmp(0xFF)==0xFF) {
					if(index_list_case+1 >= sum) index_list_case=-1;
					index_list_case++;
					if(list_case[index_list_case]==1 || list_case[index_list_case]==2)
						case_40();
					if(list_case[index_list_case]==0) case_41();
					goto runNormal;
				}
			}
			if(vloc>12) {
				special_ratio=1;
				speed(-255,-255,100);
				_delay_ms(200);
			}
			handle(-3);
			special_ratio=0.5;
			speed(255,255,80);
			while(pulse_v_special<30);
			handle(-40);
			special_ratio=0.7;
			speed(40,200,80);
			case_50();
			special_flag=false;
		}
		//re phai
		if (value_special==0b00011111 || value_special==0b00111111 || value_special==0b01111111 ){
			if(previous==210||previous==211||previous==310||previous==311) goto runNormal;
			special_flag=true;
			crook_flag=false;
			straight_flag=false;
			pulse_v_special=0;
			pulse_time_case_special=0; //thoi gian chong nhieu~
			while (pulse_time_case_special<50) {
				if(sensor_cmp(0xFF)==0xFF) {
					if(index_list_case+1 >= sum) index_list_case=-1;
					index_list_case++;
					if(list_case[index_list_case]==1 || list_case[index_list_case]==2)
						case_40();
					if(list_case[index_list_case]==0) case_41();
					goto runNormal;
				}
			}
			if(vloc>12) {
				special_ratio=1;
				speed(-255,-255,100);
				_delay_ms(200);
			}
			handle(3);
			special_ratio=0.5;
			speed(255,255,80);
			while(pulse_v_special<30);
			handle(40);
			special_ratio=0.7;
			speed(200,40,80);
			case_60();
			special_flag=false;
		}
		//nhap du lieu led 7
		runNormal:
		led7_data.sensor_out=sensor_cmp(0xFF);
		//===========xet truong hop queo trai, phai, thang
		if (conti) {
			//led7(pattern_dir);
			switch(sensor_cmp(0xFF)) {
				case 0b00111100:
					pattern_dir=10; //di thang
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					break;
				case 0b00011100:
				case 0b00011110:
				case 0b00010110:
				case 0b00001110:
				case 0b00000110:
				case 0b00000111:
				case 0b00000011:
					pattern_dir=20; //queo phai
					break;
				case 0b00111000:
				case 0b01111000:
				case 0b01101000:
				case 0b01100000:
				case 0b11100000:
				case 0b11000000:
					pattern_dir=30; //queo trai
					break;
				default:
					speed(0,0,0);
					break;
			}
		}
		
		switch(pattern_dir) {
			case 10:
				if(previous!=10) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(255,255,100);
					previous=10;
				}
				led7(10);
				break;
			case 20:
				pattern_level=sensor_cmp(0xFF);
				switch(pattern_level) {
					
					case 0b00011100: //21
						handle(handle_case_21);
						if (previous!=21) {
							straight_flag=true;
							crook_flag=false;
							special_flag=false;
							speed(255,200,100);
						}
						led7(21);
						previous=21;
						conti=false;
						break;
						
					case 0b00011110: //22
						handle(handle_case_22);					
						if(previous!=22) {
							straight_flag=true;
							crook_flag=false;
							special_flag=false;
							speed(255,180,100);
						}
						led7(22);
						previous=22;
						conti=false;
						break;
	
					case 0b00010110: //23
					case 0b00001110: //24
						handle(handle_case_23);
						if(previous!=30) {
							uint16_t temp=pulse_v_crook_des;
							pulse_v_crook_des=(pulse_v_crook_des/100)*90;
							straight_flag=false;
							crook_flag=true;
							special_flag=false;
							speed(255,160,100);
							pulse_v_crook_des=temp;
						}
						led7(2324);
						previous=23;
						conti=false;
						break;
					
					case 0b00000100:
					case 0b00000110: //25
						handle(handle_case_25);
						if(previous!=25) {
							uint16_t temp=pulse_v_crook_des;
							pulse_v_crook_des=(pulse_v_crook_des/100)*90;
							straight_flag=false;
							crook_flag=true;
							special_flag=false;
							speed(255,140,100);
							pulse_v_crook_des=temp;
						}
						led7(25);
						previous=25;
						conti=false;
						break;
						
					case 0b00000111: //26
						handle(handle_case_26);
						if(previous!=27) {
							special_flag=true;
							special_ratio=1;
							speed(255,-20,100);
						}
						special_flag=false;
						led7(26);
						previous=26;
						conti=false;
						break;
						
					case 0b00000011: //27
						handle(handle_case_27);
						if(previous!=27) {
							special_flag=true;
							special_ratio=1;
							speed(255,-40,100);
						}
						special_flag=false;
						led7(27);
						previous=27;
						conti=false;
						break;
						
					case 0b00000001: //28
						handle(handle_case_28);
						if(previous!=28) {
							special_flag=true;
							special_ratio=1;
							speed(255,-60,80);
						}
						special_flag=false;
						led7(28);
						previous=28;
						conti=false;
						break;
						
					case 0b00000000: //29
						handle(handle_case_29);
						if(previous!=29){
							special_flag=true;
							special_ratio=1;
							speed(255,-70,80);
						}
						special_flag=false;
						led7(29);
						previous=29;
						conti=false;
						break;
						
					case 0b10000000: //210
						special_flag=true;
						special_ratio=1;
						handle(handle_case_210);
						if(previous!=210) {
							speed(255,-80,70);
						}
						special_flag=false;
						led7(210);
						previous=210;
						conti=false;
						break;
					case 0b11000000: //211
						special_flag=true;
						special_ratio=1;
						handle(handle_case_211);
						if(previous!=211) {
							speed(255,-100,100);
						}
						special_flag=false;
						led7(211);
						previous=211;
						conti=false;
						break;
					case 0b00111000:
						if(previous==210||previous==211) break;
						pattern_dir=30;
						conti=true;
						break;
					
					case 0b00111100:
						if(previous==210||previous==211) break;
						straight_flag=true;
						crook_flag=false;
						special_flag=false;
						speed(255,255,100);
						pattern_dir=10;
						conti=true;
						break;
				}
				break;
			case 30:
				pattern_level=sensor_cmp(0xFF);
				switch(pattern_level) {
					case 0b00111000: //31
						handle(handle_case_31);
						if(previous!=31) {
							straight_flag=true;
							crook_flag=false;
							special_flag=false;
							speed(200,255,100);
						}
						led7(31);
						previous=31;
						conti=false;
						break;
						
					case 0b01111000: //32
						handle(handle_case_32);
						if(previous!=32) {
							straight_flag=true;
							crook_flag=false;
							special_flag=false;
							speed(180,255,100);
						}
						led7(32);
						previous=32;
						conti=false;
						break;
					
					case 0b01101000: //33
						handle(handle_case_33);
						if(previous!=33) {
							uint16_t temp=pulse_v_crook_des;
							pulse_v_crook_des=(pulse_v_crook_des/100)*90;
							straight_flag=false;
							crook_flag=true;
							special_flag=false;
							speed(160,255,100);
							pulse_v_crook_des=temp;
						}
						led7(33);
						previous=33;
						conti=false;
						break;
						
					case 0b01100000: //35
						handle(handle_case_35);
						if(previous!=35) {
							uint16_t temp=pulse_v_crook_des;
							pulse_v_crook_des=(pulse_v_crook_des/100)*90;
							straight_flag=false;
							crook_flag=true;
							special_flag=false;
							speed(140,255,100);
							pulse_v_crook_des=temp;
						}
						previous=35;
						led7(35);
						conti=false;
						break;
					case 0b11100000: //36
						handle(handle_case_36);
						if(previous!=37) {
							special_flag=1;
							special_ratio=1;
							speed(-20,255,100);
						}
						special_flag=false;
						previous=36;
						led7(36);
						conti=false;
						break;
					case 0b11000000: //37
						handle(handle_case_37);
						if(previous!=37) {
							special_flag=1;
							special_ratio=1;
							speed(-40,255,100);
						}
						special_flag=false;
						previous=37;
						led7(37);
						conti=false;
						break;
					case 0b10000000: //38
						handle(handle_case_38);
						if(previous!=38) {
							special_flag=1;
							special_ratio=1;
							speed(-60,255,80);
						}
						special_flag=false;
						previous=38;
						led7(38);
						conti=false;
						break;
						
					case 0b00000000: //39
						handle(handle_case_39);
						if(previous!=39) {
							special_flag=1;
							special_ratio=1;
							speed(-70,255,100);
						}
						special_flag=false;
						previous=39;
						led7(39);
						conti=false;
						break;
						
					case 0b00000001: //310
						handle(handle_case_310);
						special_flag=true;
						special_ratio=1;
						if(previous!=310) {
							speed(-80,255,100);
						}
						special_flag=false;
						previous=310;
						led7(310);
						conti=false;
						break;
						
					case 0b00000011: //311
						handle(handle_case_311);
						special_flag=true;
						special_ratio=1;
						if(previous!=311) {
							speed(-100,255,100);
						}
						special_flag=false;
						previous=311;
						led7(311);
						conti=false;
						break;
					
					case 0b00011100:
						if(previous==310||previous==311) break;
						pattern_dir=20;
						conti=true;
						break;
						
					case 0b00111100:
						if(previous==310||previous==311) break;
						straight_flag=true;
						crook_flag=false;
						special_flag=false;
						pattern_dir=10;
						conti=true;
						break;
				}
				break;
		}		
	}
}

// special case
void case_40() {
	led7(4444);
	uint8_t conti=true;
	uint8_t pattern_dir=10;
	uint8_t pattern_level;
	int previous=10;
	uint8_t value_case_40;
	value_case_40=sensor_cmp(0xFF);
	while (1) {
		if (value_case_40==0b00011100||
		value_case_40==0b00011110||
		value_case_40==0b00010110||
		value_case_40==0b00001110||
		value_case_40==0b00000110||
		value_case_40==0b00000111||
		value_case_40==0b00000011||
		value_case_40==0b00000001||
		
		value_case_40==0b00111000||
		value_case_40==0b01111000||
		value_case_40==0b01100000||
		value_case_40==0b11100000||
		value_case_40==0b11000000||
		value_case_40==0b10000000) // delay den khi nao thoat khoi vach ngang bao hieu
		break;
		value_case_40=sensor_cmp(0xff);
	}
	pulse_v_special=0;
	pulse_time_case_special=0;
	while(1) {
		if(pulse_v_des>10) pulse_v_des=10;
		if(pulse_v_crook_des>10) pulse_v_crook_des=10;
		if(pulse_v_des<10) {
			if(pulse_time_case_special>time_increase) {
				pulse_v_des++;
				pulse_time_case_special=0;
			}
		}
		if(pulse_v_crook_des<10) {
			if(pulse_time_case_special>time_increase) {
				pulse_v_crook_des++;
				pulse_time_case_special=0;
			}
		}
		led7(pulse_v_special);
		value_case_40=sensor_cmp(0xFF);
		if(pulse_v_special>=distand_square && (value_case_40==0b00011111 || value_case_40==0b00111111
		|| value_case_40==0b01111111 || value_case_40==0b01101111 || value_case_40==0b00101111 || 
		value_case_40==0x00 || value_case_40==0b11111110 || value_case_40==0b11101110 || value_case_40==0b11111100
		|| value_case_40==0b11101100 || value_case_40==0b11111000 || value_case_40==0b11101000)) { //1 trai 2 phai
			if(list_case[index_list_case]==1){
				handle(-150);
				special_flag=true;
				special_ratio=1;
				speed(-80,255,100);
				pulse_v_special=0;
				while (pulse_v_special<10);
				uint8_t value=sensor_cmp(0xFF);
				while(1) {
					if(value==0b00111100||
					value==0b00011100||
					value==0b00111000) {
						special_flag=false;
						increase_flag=true;
						pulse_time_increase=0;
						pulse_v_des=10;
						pulse_v_crook_des=10;
						return;
					}
					value=sensor_cmp(0xFF);
				}
			}
			if(list_case[index_list_case]==2){
				handle(150);
				special_flag=true;
				special_ratio=1;
				speed(255,-80,100);
				pulse_v_special=0;
				while (pulse_v_special<10);
				uint8_t value=sensor_cmp(0xFF);
				while(1) {
					if(value==0b00111100||
					value==0b00011100||
					value==0b00111000) {
						special_flag=false;
						increase_flag=true;
						pulse_time_increase=0;
						pulse_v_des=10;
						pulse_v_crook_des=10;
						return;
					}
					value=sensor_cmp(0xFF);
				}
			}
		}
		if (conti) {
			//led7(pattern_dir);
			switch(sensor_cmp(0xFF)) {
				case 0b00111100:
				pattern_dir=10; //di thang
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				break;
				case 0b00011100:
				case 0b00011110:
				case 0b00010110:
				case 0b00001110:
				case 0b00000110:
				case 0b00000111:
				case 0b00000011:
				case 0b00000001:
				pattern_dir=20; //queo phai
				break;
				case 0b00111000:
				case 0b01111000:
				case 0b01101000:
				case 0b01100000:
				case 0b11100000:
				case 0b11000000:
				case 0b10000000:
				pattern_dir=30; //queo trai
				break;
				default:
				speed(0,0,0);
				break;
			}
		}
		
		switch(pattern_dir) {
			case 10:
			if(previous!=10) {
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				speed(255,255,100);
				previous=10;
			}
			break;
			case 20:
			pattern_level=sensor_cmp(0xFF);
			switch(pattern_level) {
				
				case 0b00011100: //21
				handle(handle_case_21);
				if (previous!=21) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(255,200,100);
				}
				previous=21;
				conti=false;
				break;
				
				case 0b00011110: //22
				handle(handle_case_22);
				if(previous!=22) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(255,180,100);
				}
				previous=22;
				conti=false;
				break;
				
				case 0b00010110: //23
				case 0b00001110: //24
				handle(handle_case_23);
				if(previous!=30) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(255,160,100);
					pulse_v_crook_des=temp;
				}
				previous=23;
				conti=false;
				break;
				
				case 0b00000100:
				case 0b00000110: //25
				handle(handle_case_25);
				if(previous!=25) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(255,140,100);
					pulse_v_crook_des=temp;
				}
				previous=25;
				conti=false;
				break;
				
				case 0b00000111: //26
				handle(handle_case_26);
				if(previous!=27) {
					special_flag=true;
					special_ratio=1;
					speed(255,-20,100);
				}
				special_flag=false;
				previous=26;
				conti=false;
				break;
				
				case 0b00000011: //27
				handle(handle_case_27);
				if(previous!=27) {
					special_flag=true;
					special_ratio=1;
					speed(255,-40,100);
				}
				special_flag=false;
				previous=27;
				conti=false;
				break;
				
				case 0b00000001: //28
				handle(handle_case_28);
				if(previous!=28) {
					special_flag=true;
					special_ratio=1;
					speed(255,-60,80);
				}
				special_flag=false;
				previous=28;
				conti=false;
				break;
				
				case 0b00111000:
				if(previous==210||previous==211) break;
				pattern_dir=30;
				conti=true;
				break;
				
				case 0b00111100:
				if(previous==210||previous==211) break;
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				speed(255,255,100);
				pattern_dir=10;
				conti=true;
				break;
			}
			break;
			case 30:
			pattern_level=sensor_cmp(0xFF);
			switch(pattern_level) {
				case 0b00111000: //31
				handle(handle_case_31);
				if(previous!=31) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(200,255,100);
				}
				previous=31;
				conti=false;
				break;
				
				case 0b01111000: //32
				handle(handle_case_32);
				if(previous!=32) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(180,255,100);
				}
				previous=32;
				conti=false;
				break;
				
				case 0b01101000: //33
				handle(handle_case_33);
				if(previous!=33) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(160,255,100);
					pulse_v_crook_des=temp;
				}
				previous=33;
				conti=false;
				break;
				
				case 0b01100000: //35
				handle(handle_case_35);
				if(previous!=35) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(140,255,100);
					pulse_v_crook_des=temp;
				}
				previous=35;
				conti=false;
				break;
				case 0b11100000: //36
				handle(handle_case_36);
				if(previous!=37) {
					special_flag=1;
					special_ratio=1;
					speed(-20,255,100);
				}
				special_flag=false;
				previous=36;
				conti=false;
				break;
				case 0b11000000: //37
				handle(handle_case_37);
				if(previous!=37) {
					special_flag=1;
					special_ratio=1;
					speed(-40,255,100);
				}
				special_flag=false;
				previous=37;
				conti=false;
				break;
				case 0b10000000: //38
				handle(handle_case_38);
				if(previous!=38) {
					special_flag=1;
					special_ratio=1;
					speed(-60,255,80);
				}
				special_flag=false;
				previous=38;
				conti=false;
				break;
				
				case 0b00011100:
				if(previous==310||previous==311) break;
				pattern_dir=20;
				conti=true;
				break;
				
				case 0b00111100:
				if(previous==310||previous==311) break;
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				pattern_dir=10;
				conti=true;
				break;
			}
			break;
		}
	} //ket thuc while
	special_flag=false;
	pulse_v_des=10;
	pulse_v_crook_des=10;
	increase_flag=true;
}
void case_41() {
	led7(4444);
	uint8_t conti=true;
	uint8_t pattern_dir=10;
	uint8_t pattern_level;
	int previous=10;
	uint8_t value_case_40;
	value_case_40=sensor_cmp(0xFF);
	uint8_t return_flag=false;
	while (1) {
		if (value_case_40==0b00011100||
		value_case_40==0b00011110||
		value_case_40==0b00010110||
		value_case_40==0b00001110||
		value_case_40==0b00000110||
		value_case_40==0b00000111||
		value_case_40==0b00000011||
		value_case_40==0b00000001||
		
		value_case_40==0b00111000||
		value_case_40==0b01111000||
		value_case_40==0b01100000||
		value_case_40==0b11100000||
		value_case_40==0b11000000||
		value_case_40==0b10000000) // delay den khi nao thoat khoi vach ngang bao hieu
		break;
		value_case_40=sensor_cmp(0xff);
	}
	pulse_time_case_special=0;
	while(1) {
		switch(sensor_cmp(0xff)) {
			case 0b00111100:
			
			case 0b00011100:
			case 0b00011110:
			case 0b00010110:
			case 0b00001110:
			case 0b00000110:
			case 0b00000111:
			case 0b00000011:
			case 0b00000001:
			
			case 0b00111000:
			case 0b01111000:
			case 0b01101000:
			case 0b01100000:
			case 0b11100000:
			case 0b11000000:
			case 0b10000000:
				if(return_flag && pulse_v_special>=90) {
					return;
				}
		}
		if(pulse_time_case_special>time_increase) {
			special_flag=false;
			straight_flag=true;
			crook_flag=false;
			if(sensor_cmp(0xff)==0x00) {
				if(pulse_v_crook_des>pulse_v_crook_des_base) pulse_v_crook_des=pulse_v_crook_des_base;
				if(pulse_v_crook_des<pulse_v_crook_des_base) {
					pulse_v_crook_des++;
				}
				if(pulse_v_des>pulse_v_des_base) pulse_v_des=pulse_v_des_base;
				if(pulse_v_des<pulse_v_des_base) {
					pulse_v_des++;
				}
			}
			speed(230,255,100);
			handle(0);
			return_flag=true;
			pulse_v_special=0;
			pulse_time_case_special=0;
		}
		if (conti) {
			//led7(pattern_dir);
			switch(sensor_cmp(0xFF)) {
				case 0b00111100:
				pattern_dir=10; //di thang
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				break;
				case 0b00011100:
				case 0b00011110:
				case 0b00010110:
				case 0b00001110:
				case 0b00000110:
				case 0b00000111:
				case 0b00000011:
				case 0b00000001:
				pattern_dir=20; //queo phai
				break;
				case 0b00111000:
				case 0b01111000:
				case 0b01101000:
				case 0b01100000:
				case 0b11100000:
				case 0b11000000:
				case 0b10000000:
				pattern_dir=30; //queo trai
				break;
			}
		}
		
		switch(pattern_dir) {
			case 10:
			if(previous!=10) {
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				speed(255,255,100);
				previous=10;
			}
			break;
			case 20:
			pattern_level=sensor_cmp(0xFF);
			switch(pattern_level) {
				
				case 0b00011100: //21
				handle(handle_case_21);
				if (previous!=21) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(255,200,100);
				}
				previous=21;
				conti=false;
				break;
				
				case 0b00011110: //22
				handle(handle_case_22);
				if(previous!=22) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(255,180,100);
				}
				previous=22;
				conti=false;
				break;
				
				case 0b00010110: //23
				case 0b00001110: //24
				handle(handle_case_23);
				if(previous!=30) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(255,160,100);
					pulse_v_crook_des=temp;
				}
				previous=23;
				conti=false;
				break;
				
				case 0b00000100:
				case 0b00000110: //25
				handle(handle_case_25);
				if(previous!=25) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(255,140,100);
					pulse_v_crook_des=temp;
				}
				previous=25;
				conti=false;
				break;
				
				case 0b00000111: //26
				handle(handle_case_26);
				if(previous!=27) {
					special_flag=true;
					special_ratio=1;
					speed(255,-20,100);
				}
				special_flag=false;
				previous=26;
				conti=false;
				break;
				
				case 0b00000011: //27
				handle(handle_case_27);
				if(previous!=27) {
					special_flag=true;
					special_ratio=1;
					speed(255,-40,100);
				}
				special_flag=false;
				previous=27;
				conti=false;
				break;
				
				case 0b00000001: //28
				handle(handle_case_28);
				if(previous!=28) {
					special_flag=true;
					special_ratio=1;
					speed(255,-60,80);
				}
				special_flag=false;
				previous=28;
				conti=false;
				break;
				
				case 0b00111000:
				if(previous==210||previous==211) break;
				pattern_dir=30;
				conti=true;
				break;
				
				case 0b00111100:
				if(previous==210||previous==211) break;
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				speed(255,255,100);
				pattern_dir=10;
				conti=true;
				break;
			}
			break;
			case 30:
			pattern_level=sensor_cmp(0xFF);
			switch(pattern_level) {
				case 0b00111000: //31
				handle(handle_case_31);
				if(previous!=31) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(200,255,100);
				}
				previous=31;
				conti=false;
				break;
				
				case 0b01111000: //32
				handle(handle_case_32);
				if(previous!=32) {
					straight_flag=true;
					crook_flag=false;
					special_flag=false;
					speed(180,255,100);
				}
				previous=32;
				conti=false;
				break;
				
				case 0b01101000: //33
				handle(handle_case_33);
				if(previous!=33) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(160,255,100);
					pulse_v_crook_des=temp;
				}
				previous=33;
				conti=false;
				break;
				
				case 0b01100000: //35
				handle(handle_case_35);
				if(previous!=35) {
					uint16_t temp=pulse_v_crook_des;
					pulse_v_crook_des=(pulse_v_crook_des/100)*90;
					straight_flag=false;
					crook_flag=true;
					special_flag=false;
					speed(140,255,100);
					pulse_v_crook_des=temp;
				}
				previous=35;
				conti=false;
				break;
				case 0b11100000: //36
				handle(handle_case_36);
				if(previous!=37) {
					special_flag=1;
					special_ratio=1;
					speed(-20,255,100);
				}
				special_flag=false;
				previous=36;
				conti=false;
				break;
				case 0b11000000: //37
				handle(handle_case_37);
				if(previous!=37) {
					special_flag=1;
					special_ratio=1;
					speed(-40,255,100);
				}
				special_flag=false;
				previous=37;
				conti=false;
				break;
				case 0b10000000: //38
				handle(handle_case_38);
				if(previous!=38) {
					special_flag=1;
					special_ratio=1;
					speed(-60,255,80);
				}
				special_flag=false;
				previous=38;
				conti=false;
				break;
				
				case 0b00011100:
				if(previous==310||previous==311) break;
				pattern_dir=20;
				conti=true;
				break;
				
				case 0b00111100:
				if(previous==310||previous==311) break;
				straight_flag=true;
				crook_flag=false;
				special_flag=false;
				pattern_dir=10;
				conti=true;
				break;
			}
			break;
		}
	} //ket thuc while
}
void case_50() {
	led7(5555);
	while(sensor_cmp(0xFF)!=0x00) {}
	uint8_t temp=sensor_cmp(0xFF);
	while(temp!=0b00111000 && temp!=0b01111000 && temp!=0b01101000 && temp!=0b00011100 && temp!=0b00011110 && temp!=0b00010110 && temp!=0b00111100 && temp !=0b01100000 && temp != 0b00000110 && temp!=0b11100000 && temp != 0b00000111) {
		led7_data.sensor_out=temp;
		temp=sensor_cmp(0xFF);
	}
	increase_flag=true;
	pulse_time_increase=0;
	pulse_v_des=8;
	pulse_v_crook_des=8;
}
void case_60()
{
	led7(6666);
	while(sensor_cmp(0xFF)!=0x00) {}
	uint8_t temp=sensor_cmp(0xFF);
	while(temp!=0b00111000 && temp!=0b01111000 && temp!=0b01101000 && temp!=0b00011100 && temp!=0b00011110 && temp!=0b00010110 && temp!=0b00111100 && temp !=0b01100000 && temp != 0b00000110 && temp!=0b11100000 && temp != 0b00000111) {
		led7_data.sensor_out=temp;
		temp=sensor_cmp(0xFF);
	}
	increase_flag=true;
	pulse_time_increase=0;
	pulse_v_des=8;
	pulse_v_crook_des=8;
}

//PID ctrl
void PID_ctrl_motor() {
	volatile int err;
	if(straight_flag && !crook_flag) {
		err=pulse_v_des-pulse_v;
	}
	if(!straight_flag && crook_flag) {
		err=pulse_v_crook_des-pulse_v;
	}
	volatile float pPart=0;
	volatile float iPart=0;
	volatile float dPart=0;
	pPart=(float)err*kp_m;
	dPart=kd_m*(float)(err-pre_err)*(float)time_get_sample;
	iPart+=ki_m*(float)err*(float)time_get_sample/1000; //don vi chuan la s
	PID_ratio=(pPart+dPart+iPart)/(float)err_ratio_PID_servo;
	pre_err=err;
}
ISR(TIMER0_COMP_vect)
{
	pulse_time_increase++;
	pulse_time_case_special=(pulse_time_case_special+1)%60000; //thoi gian trong 2s
	pulse_time_get_sample++;
	if(pulse_time_get_sample==time_get_sample) {
		vloc=pulse_v;
		PID_ctrl_motor();
		pulse_v=0; //reset
		pulse_time_get_sample=0;
	}
	//de thiet lap che do tang toc do, pulse_time_increase=0;
									// increase_flag=true;
									// pulse_v_des=toc do bat dau;
									// redefine lai thoi gian tang len
	if(pulse_time_increase==time_increase) {
		if(increase_flag) {
			if(pulse_v_des+1<pulse_v_des_base) {
				pulse_v_des++;
			}
			if(pulse_v_crook_des+1<pulse_v_crook_des_base) {
				pulse_v_crook_des++;
			}
			if(pulse_v_crook_des>=pulse_v_crook_des_base&&pulse_v_des>=pulse_v_des_base) {
				increase_flag=false;
			}
		}
		pulse_time_increase=0;
	}
	print();
}
ISR(INT0_vect)
{
	pulse_v++;
	pulse_v_special++;
}
