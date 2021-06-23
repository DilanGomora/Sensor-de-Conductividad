#include <LiquidCrystal_I2C.h>
#include <OneWire.h>                
#include <DallasTemperature.h>

LiquidCrystal_I2C lcd (0x27, 16,2);

#define TdsSensorPin 15
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0,copyIndex = 0;
float averageVoltage = 0,tdsValue = 0,temperature = 25;

#define TdsSensorPin2 4
#define VREF2 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT2  30           // sum of sample point
int analogBuffer2[SCOUNT2];    // store the analog value in the array, read from ADC
int analogBufferTemp2[SCOUNT2];
int analogBufferIndex2 = 0,copyIndex2 = 0;
float averageVoltage2 = 0,tdsValue2 = 0,temperature2 = 25;

OneWire ourWireBus1(25);                  //Se establece el pin 4  como bus OneWire 
DallasTemperature sensor1(&ourWireBus1); //Se declara una variable u objeto para nuestro sensor

OneWire ourWireBus2(26);                  //Se establece el pin 4  como bus OneWire 
DallasTemperature sensor2(&ourWireBus2); //Se declara una variable u objeto para nuestro sensor

void setup()
{
    Serial.begin(115200);
    lcd.init();  // Initialize the LCD connected   
    lcd.backlight();  // Turn on the backlight on LCD. 
    pinMode(TdsSensorPin,INPUT);
    pinMode(TdsSensorPin2,INPUT);
    sensor1.begin();   //Se inicia el sensor
    sensor2.begin();   //Se inicia el sensor
}

void loop()
{
   static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      tdsValue= (tdsValue*0.025509483)-10;
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm");
      lcd.setCursor(0, 0);    //Here cursor is placed on first position (col: 0) of the second line (row: 1)   
      lcd.print ( "ppm:" );
      lcd.print( tdsValue,0);
   }
  
   static unsigned long analogSampleTimepoint2 = millis();
   if(millis()-analogSampleTimepoint2 > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint2 = millis();
     analogBuffer2[analogBufferIndex2] = analogRead(TdsSensorPin2);    //read the analog value and store into the buffer
     analogBufferIndex2++;
     if(analogBufferIndex2 == SCOUNT2) 
         analogBufferIndex2 = 0;
   }   
   static unsigned long printTimepoint2 = millis();
   if(millis()-printTimepoint2 > 800U)
   {
      printTimepoint2 = millis();
      for(copyIndex2=0;copyIndex2<SCOUNT2;copyIndex2++)
        analogBufferTemp2[copyIndex2]= analogBuffer2[copyIndex2];
      averageVoltage2 = getMedianNum2(analogBufferTemp2,SCOUNT2) * (float)VREF2 / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient2=1.0+0.02*(temperature2-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge2=averageVoltage2/compensationCoefficient2;  //temperature compensation
      tdsValue2=(133.42*compensationVolatge2*compensationVolatge2*compensationVolatge2 - 255.86*compensationVolatge2*compensationVolatge2 + 857.39*compensationVolatge2)*0.5; //convert voltage value to tds value
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
      tdsValue2= (tdsValue2*0.013126029)-12;
      Serial.print("TDS Value2:");
      Serial.print(tdsValue2,0);
      Serial.println("ppm");
      lcd.print ( "  ppm:" );    
      lcd.print( tdsValue2,0);
   }
   delay(1000);

   sensor1.requestTemperatures();   //Se envía el comando para leer la temperatura
   float temp1= sensor1.getTempCByIndex(0); //Se obtiene la temperatura en ºC
   Serial.print(temp1);
   Serial.println(" C");
   lcd.setCursor(0, 1);      
   lcd.print ( "C:" );
   lcd.print ( temp1 );

   sensor2.requestTemperatures();   //Se envía el comando para leer la temperatura
   float temp2= sensor2.getTempCByIndex(0); //Se obtiene la temperatura en ºC
   Serial.print(temp2);
   Serial.println(" C");
   lcd.print ( " C:" );      
   lcd.print ( temp2 );  
    
  delay(1000);
}
int getMedianNum(int bArray[], int iFilterLen) 
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++) 
      {
      for (i = 0; i < iFilterLen - j - 1; i++) 
          {
        if (bTab[i] > bTab[i + 1]) 
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}

int getMedianNum2(int bArray2[], int iFilterLen2) 
{
      int bTab2[iFilterLen2];
      for (byte i2 = 0; i2<iFilterLen2; i2++)
      bTab2[i2] = bArray2[i2];
      int i2, j2, bTemp2;
      for (j2 = 0; j2 < iFilterLen2 - 1; j2++) 
      {
      for (i2 = 0; i2 < iFilterLen2 - j2 - 1; i2++) 
          {
        if (bTab2[i2] > bTab2[i2 + 1]) 
            {
        bTemp2 = bTab2[i2];
            bTab2[i2] = bTab2[i2 + 1];
        bTab2[i2 + 1] = bTemp2;
         }
      }
      }
      if ((iFilterLen2 & 1) > 0)
    bTemp2 = bTab2[(iFilterLen2 - 1) / 2];
      else
    bTemp2 = (bTab2[iFilterLen2 / 2] + bTab2[iFilterLen2 / 2 - 1]) / 2;
      return bTemp2;
}
