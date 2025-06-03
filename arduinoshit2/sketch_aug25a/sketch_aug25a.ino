// ADS Touch Sensor Test Example Program (IC P/N:TSM12S, 24QFN)
// Code:
// Date: 2023.07.06  Ver.: 0.0.5
// H/W Target: ARDUINO UNO R3, S/W: Arduino IDE  2.1.1
// Author: Park, Byoungbae ( yni2yni@hanmail.net )
// Note: More information? Please ,send to e-mail.
// Uno R3, A4:SDA, A5: SCL, Leonardo 2:SDA,3:SCL, Uno R3, A4:SDA, A5: SCL,
// Register setting values are subject to change without prior notice to improve touch operation.

#include <Wire.h>

#define LF        0x0A //New Line
#define CR        0x0D //Carriage  return
#define SPC       0x20       //Space

#define Sensitivity1  0x02 //ch2,ch1
#define Sensitivity2  0x03 //ch4,ch3
#define Sensitivity3  0x06 //ch6,ch5
#define Sensitivity4  0x07 //ch8,ch7
#define Sensitivity5  0x22 //ch10,ch9
#define Sensitivity6  0x23 //ch12,ch11

// Sensitivity levlel for Half Sensing Mode time
// Sensitivity threshold is lower than or equal to that in normal mode.
#define HSensitivity1 0x31 //ch2,ch1 
#define HSensitivity2 0x32 //ch4,ch3
#define HSensitivity3 0x35 //ch6,ch5
#define HSensitivity4 0x36 //ch8,ch7
#define HSensitivity5 0x37 //ch10,ch9
#define HSensitivity6 0x38 //ch12,ch11

#define CTRL1     0x08  
#define CTRL2     0x09
#define Ref_rst1  0x0A
#define Ref_rst2  0x0B
#define Ch_Hold1  0x0C //Touch Key Channel Enable/Disable (Enable = 0x00)
#define Ch_Hold2  0x0D  //Touch Key Channel Enable/Disable (Enable = 0x00)
#define Cal_Hold1 0x0E //Calibration Enable/Disable (Enable = 0x00)
#define Cal_Hold2 0x0F //Calibration Enable/Disable (Enable = 0x00)

// Hidden Register
#define Error_Percent   0x20 // Error Percent
#define Safe_Percent    0x21  // Safe Percent

#define Cal_bf    0x24  //Calibration Speeed Control (normal_up_cnt_bf / normal_down_cnt_bf)
#define Cal_bs    0x25  //Calibration Speeed Control (normal_up_cnt_bs / normal_down_cnt_bs)
#define HCal_bf   0x26 //Calibration Speeed Control (half_up_cnt_bf / half_down_cnt_bf)
#define HCal_bs   0x27 //Calibration Speeed Control (half_up_cnt_bs / half_down_cnt_bs)
#define Hcon      0x42 //Half Sense Control Register (Half Sensing Enable / Disable)
#define Half_th   0x43 //Half Sense Control Register 1

#define OUTPUT_REG1   0x10 //cs1~cs4 output
#define OUTPUT_REG2   0x12 //cs5~cs8 output
#define OUTPUT_REG3   0x13 //cs9~cs12 output

#define Lock_mask	  0x3B //Lock Mask
#define Force_en	  0x41 //Lock Mask Force_en

// =============== TSM12S I2C Chip ID ================================================
#define TSM12S_SLAVE_GND  0x68 //7bit address: 8bit address 0xD0>>1 //Chip_ID Pin = GND
#define TSM12S_SLAVE_VDD  0x78 //7bit address: 8bit address 0xF0>>1 //Chip_ID Pin = VDD

void  Init_TSM12S(void); //Initialize TSM12S (24QFN)

#define RESET_PIN 3 //Reset pin (H/W Reset Pin , or RC Reset Circuit)
#define EN_PIN   42 //I2C Enable Pin (TSM12 I2C Block Enable Pin, active Low)

void Register_Dump()
{
   byte read_data[1] = {0};

   for (int i = 0; i < 256; i += 16)
   {
      for (int j = 0; j <= 15; j++)
      {
         Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
         Wire.write((i + j));                      // sends register address
         Wire.endTransmission();                   // stop transmitting
         Wire.requestFrom(TSM12S_SLAVE_GND, 1);    // data read (2 byte)
         read_data[0] = Wire.read();               //
         print2hex(read_data, 1);                  //
      }
      Serial.write(LF);
      Serial.write(CR);
   }
   delay(500);
}

void print2hex(byte *data, byte length) //Print Hex code
{
   Serial.print("0x");
   for (int i = 0; i < length; i++)
   {
      if (data[i] < 0x10)
      {
         Serial.print("0");
      }
      Serial.print(data[i], HEX);
      Serial.write(SPC);
   }
}

void setup(){
	
  delay(1); // wait for 1[msec]	
  Wire.begin(39,40);       // join i2c bus (address optional for master)
  Wire.setClock(200000); // 200Khz
  Serial.begin(115200);  // start serial for output (Spped)
  // put your setup code here, to run once:
 
  pinMode(RESET_PIN, OUTPUT);
  //SDA, SCL = Hi-z (need to pull-up Resistor)

  // IC H/W reset signal control ,Active  High Reset
  digitalWrite(RESET_PIN, LOW); // Reset pin = low
  digitalWrite(RESET_PIN, HIGH); // Reset pin = High
  delay(2); //Min: wait for 2[msec]
  digitalWrite(RESET_PIN, LOW); // Reset pin = low
  delay(100); //wait for 100[msec]
  Init_TSM12S(); //Initialize TSM12S
  delay(100);    //wait for 100[msec]

}

void loop() {

   byte read_data[3] = {0};

   // Touch Key read
  Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
  Wire.write(byte(OUTPUT_REG1)); // sends register address
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(TSM12S_SLAVE_GND,1); // read OUTPUT1, 1Byte Read
  read_data[0]=Wire.read();		//OUTPUT1 Data
 
  Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
  Wire.write(byte(OUTPUT_REG2)); // sends register address
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(TSM12S_SLAVE_GND,1); // read OUTPUT2, 1Byte Read
  read_data[1]=Wire.read();		//OUTPUT2 Data
  
  Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
  Wire.write(byte(OUTPUT_REG3)); // sends register address
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(TSM12S_SLAVE_GND,1); // read OUTPUT3, 1Byte Read
  read_data[2]=Wire.read();		//OUTPUT3 Data
  
   Serial.write(10);
   Serial.print(" Touch Sensor Output Data (Hex) ---- > "); // Test Code

   print2hex(read_data, 1);
   print2hex((read_data+1), 1);
   print2hex((read_data+2), 1);

   //Serial.write(SP);
   //Serial.write(LF);
   //Serial.write(CR);
   //delay(5000);   
   //Register_Dump();

   delay(50);   
}

void  Init_TSM12S(void)
{
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(CTRL2)); // sends register address
   Wire.write(byte(0x0F)); // sends register data Software Reset Enable(Set)
   Wire.endTransmission(); // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Sensitivity1)); // sends register address
   Wire.write(byte(0xBB)); // sends register data
   Wire.endTransmission(); // stop transmitting
   
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Sensitivity2)); // sends register address
   Wire.write(byte(0xBB)); // sends register data
   Wire.endTransmission(); // stop transmitting
  
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Sensitivity3)); // sends register address
   Wire.write(byte(0xBB)); // sends register data
   Wire.endTransmission(); // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Sensitivity4)); // sends register address
   Wire.write(byte(0xBB)); // sends register data
   Wire.endTransmission(); // stop transmitting
   
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Sensitivity5)); // sends register address
   Wire.write(byte(0xBB)); // sends register data
   Wire.endTransmission(); // stop transmitting   
  
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Sensitivity6)); // sends register address
   Wire.write(byte(0xBB)); // sends register data
   Wire.endTransmission(); // stop transmitting   

   // --------------- Half Sensign Mode Sensitivity ------------------------------
   // Sensitivity threshold is lower than or equal to that in normal mode.
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HSensitivity1));           // sends register address
   Wire.write(byte(0xBB));                   // sends register data
   Wire.endTransmission();                   // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HSensitivity2));           // sends register address
   Wire.write(byte(0xBB));                    // sends register data
   Wire.endTransmission();                   // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HSensitivity3));           // sends register address
   Wire.write(byte(0xBB));                    // sends register data
   Wire.endTransmission();                   // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HSensitivity4));           // sends register address
   Wire.write(byte(0xBB));                    // sends register data
   Wire.endTransmission();                   // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HSensitivity5));           // sends register address
   Wire.write(byte(0xBB));                    // sends register data
   Wire.endTransmission();                   // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HSensitivity6));           // sends register address
   Wire.write(byte(0xBB));                    // sends register data 
   Wire.endTransmission();                   // stop transmitting
// ---------------------------------------------------------------------

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(CTRL1)); // sends register address
   Wire.write(byte(0x22)); // sends register data
   Wire.endTransmission(); // stop transmitting   
    
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Ref_rst1)); // sends register address
   Wire.write(byte(0xF0)); // sends register data
   Wire.endTransmission(); // stop transmitting
   
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Ref_rst2)); // sends register address
   Wire.write(byte(0x00)); // sends register data
   Wire.endTransmission(); // stop transmitting
  
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Ch_Hold1)); // sends register address
   Wire.write(byte(0xF0)); // sends register data
   Wire.endTransmission(); // stop transmitting

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Ch_Hold2)); // sends register address
   Wire.write(byte(0x00)); // sends register data
   Wire.endTransmission(); // stop transmitting
   
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Cal_Hold1)); // sends register address
   Wire.write(byte(0x00)); // sends register data
   Wire.endTransmission(); // stop transmitting   
   
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Cal_Hold2)); // sends register address
   Wire.write(byte(0x00)); // sends register data
   Wire.endTransmission(); // stop transmitting

   //------------ Hidden Register ----------------------------------------
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Error_Percent));          // sends register address, 0x20h
   Wire.write(byte(0x0C));                   // sends register data
   Wire.endTransmission();                   // stop

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Safe_Percent));           // sends register address, 0x21h
   Wire.write(byte(0x08));                   // sends register data
   Wire.endTransmission();                   // stop

   //------------ Calibration speed Control Register --------------------------
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Cal_bf));                 // sends register address, 0x24h
   Wire.write(byte(0x1F));                   //BF 0b10(5), 0b111(15)
   Wire.endTransmission();                   // stop

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Cal_bs));                 // sends register address, 0x25h
   Wire.write(byte(0x14));                   // BS , 0b10(5), 0b100(9)
   Wire.endTransmission();                   // stop

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HCal_bf));                // sends register address, 0x26h
   //Wire.write(byte(0x14));                 // Default value
   Wire.write(byte(0x1F)); // Half, BF 0b10(5), 0b111(15)
   Wire.endTransmission(); // stop

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(HCal_bs));                // sends register address, 0x27h
   //Wire.write(byte(0x1F));                 // Default value
   Wire.write(byte(0x14)); // Half, BS , 0b10(5), 0b100(9)
   Wire.endTransmission();

   //------------ Half Sensing Control Register --------------------------

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Lock_mask)); // sends register address
   Wire.write(byte(0xA5)); // Register Write Unlock
   Wire.endTransmission(); // stop  
      
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Hcon));                   // sends register address, 0x42h
   Wire.write(byte(0x12));                   // Half Sensing Enable(Default)
   // Wire.write(byte(0x10));  // Half Sensing Mode Disable
   // Disable the Half Sensing function in systems where the temperature
   // rises rapidly or the battery is not used.
   Wire.endTransmission();

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Half_th));                // sends register address, 0x43h
   // Wire.write(byte(0x77));                // Half Sensing ratio(IC H/W Reset Value)
   Wire.write(byte(0x70));                   // Half Sensing ratio 1:1 (Tunning Value)
   Wire.endTransmission();                  

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Lock_mask)); // sends register address
   Wire.write(byte(0x00)); // Register Write Lcok
   Wire.endTransmission(); // stop 

   //====================================================================
   //  Single Mode setup
   /*
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(CTRL2)); // sends register address
   Wire.write(byte(0x2F)); // sends register data
   Wire.endTransmission(); // stop transmitting   

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Lock_mask)); // sends register address
   Wire.write(byte(0xA5)); // sends register data
   Wire.endTransmission(); // stop transmitting  

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Force_en)); // sends register address
   Wire.write(byte(0x94)); // sends register data
   Wire.endTransmission(); // stop transmitting  

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(Lock_mask)); // sends register address
   Wire.write(byte(0x00)); // sends register data
   Wire.endTransmission(); // stop transmitting  

   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(CTRL2)); // sends register address
   Wire.write(byte(0x27)); // sends register data 
   //sleep mode enable, S/W Reset Clear, Impedance High, Multi Output, Single Mode
   Wire.endTransmission(); // stop transmitting   
*/
   //================================================================================
   Wire.beginTransmission(TSM12S_SLAVE_GND); // sned ic slave address
   Wire.write(byte(CTRL2)); // sends register address
   Wire.write(byte(0x07)); // sends register data , Software Reset Disable (Clear)
   //sleep mode enable, S/W Reset Clear, Impedance High, Multi Output
   Wire.endTransmission(); // stop transmitting   

   }
// End