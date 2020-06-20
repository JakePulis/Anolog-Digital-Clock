

void PrintDisplayText(int z, String stg){
  // z=line number
  // stg = text
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,z);             // Start at top-left corner
  display.println(stg);
  display.display();
}
void PrintDisplayText3Lines(String stg1, int val1, String stg2, int val2, String stg3, int val3){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(stg1);
  display.setCursor(70,0);             // Start at top-left corner
  display.println(val1);
  display.setCursor(0,10);             // Start at top-left corner
  display.println(stg2);
  display.setCursor(70,10);             // Start at top-left corner
  display.println(val2);
  display.setCursor(0,20);             // Start at top-left corner
  display.println(stg3);
  display.setCursor(70,20);             // Start at top-left corner
  display.println(val3);
  display.display();
}


void PrintDisplayValue(int z, int y){
  // z=line number
  // y=value
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,z);             // Start at top-left corner
  display.println(y);
  display.display();
  
}

void PrintDisplayHallVal(){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("ANALOG VAL:"));
  display.setCursor(0,10);             // Start at top-left corner
  display.println(F("HALL 1:"));
  display.setCursor(50,10);             // Start at top-left corner
  display.println(analogRead(A0));
  display.setCursor(0,20);             // Start at top-left corner
  display.println(F("HALL 2:"));
  display.setCursor(50,20);             // Start at top-left corner
  display.println(analogRead(A1));
  display.display();
  
}
