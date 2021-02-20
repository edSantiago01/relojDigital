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
   set_tris_a(0b000000);  //(0)salida segundo, (1)setHora, , (2)setMin
   set_tris_c(0b00000000); //salida de bits, hora
   set_tris_d(0b00000000); //salida de bits, minuto   
   set_tris_e(0b011); //(0)config más de 1 seg. (1) incremento
   output_a(0x00);
   output_d(0x00);
   output_c(0x00);       
   
   rtc_init();
      
   for(;;){   
      inConfig = INPUT(PIN_E0);
      while(inConfig == 0){          
         inConfig = INPUT(PIN_E0);
         rtc_get_time(hora, minuto, segActual); 
         if(segAnterior != segActual){    
            segAnterior = segActual;
            mostrarSegundo();
            mostrarHoraMinuto(hora, minuto);           
         }
         delay_ms(1000);
      }
      config();
   }
          
}     

void config(){
   int set = 0;   //0 min, 1 hora, 2 dia, 3 mes, 4 anio
   int anio, mes, dia, semana, hora, minuto, segundo;   
   int inConfig, inIncremento;
   
   while(set<6){
      inConfig = INPUT(PIN_E0);      
      if(inConfig == 1){        
         set++;
         delay_ms(400);
      }
      
      rtc_get_date(dia, mes, anio, semana);
      rtc_get_time(hora, minuto, segundo);
      if(set == 0){
         output_a(0b000010);
         output_low(PIN_E2);
         mostrarHoraMinuto(hora, minuto);
      }else if(set == 1){
         output_a(0b000100);
         output_low(PIN_E2);
         mostrarHoraMinuto(hora, minuto);
      }else if(set == 2){
         output_a(0b001000);
         output_low(PIN_E2);
          mostrarHoraMinuto(0, dia);
      }else if(set == 3){
         output_a(0b010000);
         output_low(PIN_E2);
         mostrarHoraMinuto(0, mes);
      }else if(set == 4){
         output_a(0b100000);
         output_low(PIN_E2);         
         mostrarHoraMinuto(0, anio);
      }else if(set == 5){
         output_a(0b111111);
         output_high(PIN_E2);
         mostrarHoraMinuto(0, semana);
      }
      
      
      inIncremento = INPUT(PIN_E1);      
      if(inIncremento == 1){
         delay_ms(600);
         rtc_get_time(hora, minuto, segundo); 
         
         if(set == 0){                         // minuto
            minuto++;
            if(minuto > 59) minuto = 0;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }else if(set == 1){                  // hora
            hora++;
            if(hora > 23) hora = 0;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         } else if(set == 2){                // día
            dia++;
            dia = verificaFecha(dia, mes, anio);
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);         
         }else if(set == 3){                 // mes
            mes++;
           if(mes>12) mes = 1;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }else if(set == 4){                 //año
            anio++;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }else if(set == 5){                 //semana 7=domingo
            semana++;
            if(semana > 7) semana = 1;
            rtc_set_datetime(dia, mes, anio, semana, hora, minuto);
         }         
      }      
   }
   output_a(0b000000);
   output_low(PIN_E2);
}

void mostrarHoraMinuto(hora, minuto){
  byte binMinuto, binHora;
      
  binMinuto = get_bcd(minuto);
  binHora   = get_bcd(hora);
         
  output_c(binHora);
  output_d(binMinuto);         
}

void mostrarSegundo(){
   if(OUT_SEGUNDO){
      output_high(PIN_A0);
   }else{      
      output_low(PIN_A0);
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
   }else dia2 = dia;
   
   return dia2;
}

