#include <SoftwareSerial.h>
#include <Wire.h>


/* HC-05    Arduino BT
 TX       RX/Pin10
 RX       TX/Pin11 */
#define MAX_BTCMDLEN 128 // BT 可容納最大byte長度
SoftwareSerial BTSerial(10,11);			// BT RX/TX
int   len = 0;											
byte  cmd[MAX_BTCMDLEN];						
int   checkLastState = 0;					  // BT 確認前一cycle的連線狀態
int btFlag = 0;

/* light sensor */
const int lightSensorPin = A0;
int lightSensorValue = 0;




//初始化
void setup() 
{
  Serial.begin(9600);						// Arduino起始鮑率：9600
  BTSerial.begin(9600);					// HC-05 鮑率
  pinMode (22, INPUT);					// BT"連線"狀態 0=>未連線 1=>已連線
}

// 主程式
void loop() 
{
  char str[MAX_BTCMDLEN] = {
  };
  int insize;
  

  while(1)
  {
    
    /*speaker_start*/
    int nowSensorValue = digitalRead(22);
    //Serial.println(nowSensorValue);
    if(checkLastState == 1 && nowSensorValue == 0)
    {
      for(int k = 0; k < 20; k++)
        Serial.println("Change State");
//      for(int note_index = 0;note_index < 7;note_index++)
//      {
//        sound(note_index);
//        delay(100);
//      }
    }
    /*speaker_end*/

    /*BT_receive_start*/
    while( (insize=(BTSerial.available())) >0 && (nowSensorValue == 1 && btFlag == 0))
    {
      Serial.print("input size = ");
      Serial.println(insize);

      for (int i=0; i<insize; i++ )
      {
        cmd[(len++)%MAX_BTCMDLEN]=char(BTSerial.read());
        sprintf(str,"%s",cmd);
        if(strstr (str, "end"))	//比對endTag字元
        {	
          Serial.println(str);
          btFlag = 1;
        }
      }
    }
    /*BT_receive_end*/

    /*print_sent_by_BTSerial_Serial*/
    if(nowSensorValue == 1)
    {
      String arduinoMsg = "";

      if(btFlag == 1)//BT_receive
      {		
        if((String)str == "lsend")
        {
          lightSensorValue = analogRead(lightSensorPin);
          arduinoMsg = "LightSensor: " + (String)lightSensorValue + " end";
        } 
        else
        {
          arduinoMsg = "please enter command \"ls\" to get light sensor value end";
        }
        
        BTSerial.print(arduinoMsg);
        Serial.println(arduinoMsg);
        btFlag = 0;
      }
      for( int i = 0; i < sizeof(cmd);  ++i )
      {
        cmd[i] = '\0';
        str[i] = '\0';
      }
      len = 0;
      break;
    }
    checkLastState = nowSensorValue;

  }
}

