#include <16F877A.h>
#use delay (clock=4000000,RESTART_WDT)
#fuses XT,NOWDT,NOPROTECT,PUT
#include <DS1302.C>

int hora, minuto, segActual, segAnterior=0;
boolean outSegundo = true;

void mostrarHoraMinuto(int, int);
void mostrarSegundo();

void main(){
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_OFF);
   set_tris_a(0b000000);  //(2)salida segundo, (1)incremento, (0)config
   set_tris_c(0b00000000); //salida de bits, hora
   set_tris_d(0b00000000); //salida de bits, minuto   
   set_tris_e(0b000);
   output_a(0x00);
   output_d(0x00);
   output_c(0x00);
   output_e(0x00);
      
   rtc_init();
      
   for(;;){
      rtc_get_time(hora, minuto, segActual);      
      
      if(segAnterior != segActual){    
         segAnterior = segActual;
         mostrarSegundo();
         mostrarHoraMinuto(hora, minuto);
           
      }
        
    }
    delay_ms(1000);
      
}     
  


void mostrarHoraMinuto(hora, minuto){
  byte binMinuto, binHora;
      
  binMinuto = get_bcd(minuto);
  binHora   = get_bcd(hora);
         
  output_c(binHora);
  output_d(binMinuto);         
}

void mostrarSegundo(){
   if(outSegundo == true){
      //output_c(0b000100);
       output_high(PIN_E0);
   }else{
      //output_c(0b000000);
      output_low(PIN_E0);
   }
   outSegundo = !outSegundo;
}

