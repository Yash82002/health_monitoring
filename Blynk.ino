//#define BLYNK_PRINT Serial
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_TEMPLATE_ID "TMPL3WZEWhuEc"
#define BLYNK_TEMPLATE_NAME "health monitring"
#define BLYNK_AUTH_TOKEN "_h9GBvOcN_TF3bmfdaglBRyTLEaFl67R"      //WiFi Passwordint temperature;

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "esp";       //Enter your WIFI Name
char pass[] = "wifi1234";  //Enter your WIFI Password

int bmp,spo2,temp;
char cmd_arr1[100];
int cmd_count1;
int i;
//*******************************************************************************
void setup()
{
  Serial.begin(9600);
  Serial.println("");
  Blynk.begin(auth, ssid, pass,"blynk.cloud", 80);
  
}
//*******************************************************************************

void serial_get_command()
{
  char inchar=0;
  if(Serial.available() > 0)
  {
    inchar = Serial.read();    
    if(inchar == '<')
    {
      cmd_count1=0;
      while(inchar != '>' && cmd_count1<15)
      {
        if(Serial.available() > 0)
        {
          inchar = Serial.read();
          cmd_arr1[cmd_count1++] = inchar;
        }        
      }
      if(inchar == '>')
      {
        if(cmd_arr1[0]=='A')
        {
          cmd_arr1[0]='0';
          bmp = atoi(cmd_arr1);
          Blynk.virtualWrite(V1, bmp);
         }
     else if(cmd_arr1[0]=='B')
        {
          cmd_arr1[0]='0';
          spo2 = atoi(cmd_arr1);
          Blynk.virtualWrite(V2, spo2);
         }
         else if(cmd_arr1[0]=='C')
         {
          cmd_arr1[0]='0';
          temp = atoi(cmd_arr1);
          Blynk.virtualWrite(V3,temp );
         }
         else
         {
          
         }
       }
    }
  }
}
//*******************************************************************************
void loop() 
{
 Blynk.run();
 serial_get_command();
}