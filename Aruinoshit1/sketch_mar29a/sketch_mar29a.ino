 /*
 * 
 * 双轴按键摇杆
 */
#define pinX  9
#define pinY  10
#define pinK  2
 
int value = 0;
 
void setup()
{
  pinMode(pinK, INPUT);
  Serial.begin(115200);
}
 
void loop()
{
  value = analogRead(pinX);
  Serial.print("X: ");
  Serial.print(value);
 
  value = analogRead(pinY);
  Serial.print(" Y: ");
  Serial.print(value);
 
  value = digitalRead(pinK);
  Serial.print(" Z: ");
  Serial.println(value);
  
}
