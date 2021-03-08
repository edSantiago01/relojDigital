#include <16F877A.h>
#use delay (clock=4000000,RESTART_WDT)
#fuses XT,NOWDT,NOPROTECT,PUT
#include <DS1302.C>

boolean OUT_SEGUNDO = true;

void mostrarHoraMinuto(int, int);
void mostrarSegundo();
void config();
void actualizarTiempo(int, int, int, int, int, int);
int verificaFecha(int, int, int);

void main(){
   int hora, minuto, segActual, segAnterior=0;
   int inConfig;
   
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);
   set_tris_a(0b000000);  //(1)setHora,(2)setMin,(3)setDia,(4)setMes,(5)setanio,(6)setDiaSemana
   set_tris_c(0b00000000); //salida multiplexor, indicar que display encender
   set_tris_d(0b00000000); //salida de bits, multiplexación 
   set_tris_e(0b111); //(0)config más de 1 seg. (1) decremento (2)incremento
   output_a(0x00);
   output_d(0x00);
      
   rtc_init();
      
   for(;;){   
      inConfig = INPUT(PIN_E0);
      while(inConfig == 0){          
         inConfig = INPUT(PIN_E0);
         rtc_get_time(hora, minuto, segActual); 
         if(segAnterior != segActual){    
            segAnterior = segActual;
            mostrarSegundo();
                     
         }
          mostrarHoraMinuto(hora, minuto); 
         
      }
      config();
   }
          
}     

void config(){
   int set = 0;   //0 min, 1 hora, 2 dia, 3 mes, 4 anio
   int cambio;
   int anio, mes, dia, semana, hora, minuto, segundo;   
   int inConfig, inIncremento, inDecremento;
   delay_ms(400);
   
   while(set<6){
      inConfig = INPUT(PIN_E0);      
      if(inConfig == 1){        
         set++;
         delay_ms(400);
      }
      
      rtc_get_date(dia, mes, anio, semana);
      rtc_get_time(hora, minuto, segundo);
      if(set == 0){
         output_a(0b000001);         
         mostrarHoraMinuto(hora, minuto);
      }else if(set == 1){
         output_a(0b000010);         
         mostrarHoraMinuto(hora, minuto);
      }else if(set == 2){
         output_a(0b000100);         
          mostrarHoraMinuto(0, dia);
      }else if(set == 3){
         output_a(0b001000);         
         mostrarHoraMinuto(0, mes);
      }else if(set == 4){
         output_a(0b010000);                  
         mostrarHoraMinuto(0, anio);
      }else if(set == 5){
         output_a(0b100000);         
         mostrarHoraMinuto(0, semana);
      }
      
      
      inIncremento = INPUT(PIN_E2);
      inDecremento = INPUT(PIN_E1);
      if((inIncremento == 1)||(inDecremento == 1)){
         if(inIncremento == 1) cambio = 1;
         else cambio = -1;
      
         delay_ms(500);
         rtc_get_time(hora, minuto, segundo); 
         
         if(set == 0){                         // minuto
            if((inDecremento == 1) &&(minuto == 0)) minuto = 59;
            else minuto += cambio;            
            if(minuto > 59) minuto = 0;            
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }else if(set == 1){                  // hora
            if((inDecremento == 1) && (hora == 0)) hora = 23;
            else hora += cambio;
            if(hora > 23) hora = 0;            
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         } else if(set == 2){                // día
            dia += cambio;
            dia = verificaFecha(dia, mes, anio);
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);         
         }else if(set == 3){                 // mes
            mes += cambio;
            if(mes > 12) mes = 1;
            if(mes < 1) mes = 12;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }else if(set == 4){                 //año
            anio += cambio;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }else if(set == 5){                 //semana 7=domingo
            semana += cambio;
            if(semana > 7) semana = 1;
            if(semana < 1) semana = 7;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }         
      }      
   }
   output_a(0b000000);   
}

void mostrarHoraMinuto(hora, minuto){
  byte binMinuto, binHora, binDecMinuto, binDecHora;
  int minutoDec, horaDec, minutoUnidad, horaUnidad;
  
  minutoDec = minuto / 10;
  minutoUnidad = minuto%10;
  horaDec   = hora / 10;
  horaUnidad = minuto%10;
  
  binMinuto    = get_bcd(minutoUnidad);
  binDecMinuto = get_bcd(minutoDec);  
  binHora      = get_bcd(horaUnidad);
  binDecHora   = get_bcd(horaDec);
  
  
  output_d(binMinuto);
  output_c(0b000001);
  delay_ms(5);  
  
  output_d(binDecMinuto);  //decimales minuto
  output_c(0b000010);
  delay_ms(5);
  
  output_d(binhora);
  output_c(0b000100);
  delay_ms(5);  
  
  output_d(binDecHora);  //decimales minuto
  output_c(0b001000);
  delay_ms(5);
}

void mostrarSegundo(){
   if(OUT_SEGUNDO){
      output_high(PIN_B0);
   }else{      
      output_low(PIN_B0);
   }
   OUT_SEGUNDO = !OUT_SEGUNDO;
}

int verificaFecha(int dia, int mes, int anio){
   int limiteDias = 31;
   int dia2;
   int bisiesto;
   
   if((mes == 4) || (mes == 6) || (mes == 9) || (mes == 11)){
      limiteDias = 30;
   }else if(mes == 2){
      bisiesto = anio%4;
      if(bisiesto == 0)  limiteDias = 29;
      else limiteDias = 28;
   }  
   
   if(dia > limiteDias){
      dia2 = dia - limiteDias;
   }else if(dia < 1) dia2 = limiteDias;
   else dia2 = dia;
   
   return dia2;
}
