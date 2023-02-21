#include <LiquidCrystal_I2C.h>

int totalColumns = 16;
int totalRows = 2;

LiquidCrystal_I2C lcd(0x27, totalColumns, totalRows);

//Variables
String phrase = ""; 
char guesses [20];
int guessNum = 0;
int lives = 6;
int player = 1;
int correct = 0;
int p1W = 0;
int p1L = 0;
int p2W = 0;
int p2L = 0;

byte welcomeScreenPole[8] = { 0b00111,
                              0b00101,
                              0b00100,
                              0b00100,
                              0b00100,
                              0b00100,
                              0b00100,
                              0b11111 };
                          
byte wrong1[8] = { 0b11100,
                   0b01110,
                   0b01110,
                   0b00000,
                   0b00000,
                   0b00000,
                   0b00000,
                   0b00000 };

byte wrong2[8] = { 0b11100,
                   0b01110,
                   0b01110,
                   0b00100,
                   0b00100,
                   0b00100,
                   0b00000,
                   0b00000 };

byte wrong3[8] = { 0b11100,
                   0b01110,
                   0b01110,
                   0b00100,
                   0b00111,
                   0b00100,
                   0b00000,
                   0b00000 };

byte wrong4[8] = { 0b11100,
                   0b01110,
                   0b01110,
                   0b00100,
                   0b11111,
                   0b00100,
                   0b00000,
                   0b00000 };

byte wrong5[8] = { 0b11100,
                   0b01110,
                   0b01110,
                   0b00100,
                   0b11111,
                   0b00100,
                   0b01000,
                   0b01000 };

byte wrong6[8] = { 0b11100,
                   0b01110,
                   0b01110,
                   0b00100,
                   0b11111,
                   0b00100,
                   0b01010,
                   0b01010 };
                   
byte pole[8] = { 0b00111,
                 0b00100,
                 0b00100,
                 0b00100,
                 0b00100,
                 0b00100,
                 0b00100,
                 0b00100 };




// ********************************************* Setup *********************************************
void setup() 
{

  pinMode(18,OUTPUT);
  pinMode(19, OUTPUT);
  lcd.init(); 
  lcd.backlight(); 
  
  //Create custom chars
  lcd.createChar(0, welcomeScreenPole);
  
  lcd.createChar(1, wrong1);
  lcd.createChar(2, wrong2);
  lcd.createChar(3, wrong3);
  lcd.createChar(4, wrong4);
  lcd.createChar(5, wrong5);
  lcd.createChar(6, wrong6);

  lcd.createChar(7, pole);
  
  //For serial commpunication
  Serial.begin (115200);

  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  
  //Welcome Message
  lcd.write((uint8_t)0);
  lcd.write((uint8_t)0);
  lcd.write((uint8_t)0);
    
  lcd.print (" HANG MAN ");
  
  lcd.write((uint8_t)0);
  lcd.write((uint8_t)0);
  lcd.write((uint8_t)0);
  
  lcd.setCursor(3,1);
  lcd.print("2+ PLAYERS");
  delay (3000);
  lcd.clear();
}




// ********************************************* Loop *********************************************
void loop() 
{
  //get a Phrase to guess
  getPhrase ();


  //Setup Game Screen
  lcd.print("Guess a letter");//Prompt
  
  //pole
  lcd.setCursor (14,1);
  lcd.print("|");
  
  lcd.setCursor (14,0);
  lcd.write((uint8_t)7);

  //blanks
  lcd.setCursor(0,1);
  for (int i = 0; i < phrase.length() - 1; i ++)
  {
    lcd.print("_");
  }


  while (lives > 1 && correct != phrase.length() -1 )//Play Game
  {
    //get the guess, evaluate it and update accourdingly
    
    updateScreen(correctness(getGuess()));
  }
  
  delay (400);
  lcd.clear();
  
  //Switch Players and record loss/ win
  if (player == 1)
  {
    if (lives <= 1)
    {lcd.setCursor (0,1);
      lcd.print("    LOSS !!!    ");
      p1L ++;
    }
      
    else
    {
      lcd.print("     WIN!!!     ");
      p1W ++;
    }
      
    player = 2;
  }
  
  else
  {
    if (lives <= 1)
    {lcd.setCursor (0,1);
      lcd.print("    LOSS !!!    ");
      p2L ++;
    }
    else
    {
      lcd.print("     WIN!!!     ");
      p2W ++;
    }
      
    player = 1;
  }

  delay (1000);
  
  //reset values
  phrase = "";
  guessNum = 0;
  lives = 6;
  correct = 0;

  for (int i = 0; i < 20; i ++)
  {
    guesses[i] = '\0';
  }

  //Clear Screen
  lcd.clear();
}




// ********************************************* getPhrase *********************************************
void getPhrase ()
{
  //Prompt for word
  lcd.print ("PL");
  lcd.print (player);
  lcd.print("|");
  
  if (player == 1)
  {
    lcd.print("  W:");
    lcd.print(p1W);
    lcd.print("  L:");
    lcd.print(p1L);
  }
  else
  {
    lcd.print("  W:");
    lcd.print(p2W);
    lcd.print("  L:");
    lcd.print(p2L);
  }
  
  lcd.setCursor(0,1);
  lcd.print("  Enter a Word");
  Serial.println("Enter a word");
  
  while (phrase.length() == 0)//Wait for phrase to be entered
  {
    if (Serial.available() > 0)//If something entered
    {
      phrase = Serial.readString();

      if (phrase.length() > 14)//If too long
      {
        //Reprompt for smaller word
        lcd.clear();
        lcd.print ("PL");
        lcd.print (player);
        lcd.print("|  Too Long");

        delay(1000);
        lcd.clear ();

        lcd.print ("PL");
        lcd.print (player);
        lcd.print("|  Enter Word");
        
        phrase = "";
      } 
    }
  }

  phrase.toUpperCase();
  lcd.clear();
}




// ********************************************* getGuess *********************************************
char getGuess ()
{
  //Decleration of variables
  char guess = '-';
  
  while (guess == '-')//Wait for guess to be entered
  {
    if (Serial.available() > 0)//If something entered
    {
      String temp = Serial.readString();
      temp.toUpperCase();
      guess = temp.charAt(0);
    }
  }
  
  return guess;
}



// ********************************************* correctness *********************************************
int correctness (char guess)
{
  //Check if phrase contains char
  for (int i = 0; i < phrase.length(); i ++)
  {
    if (guess == guesses[i])//If previously entered
    {
      return 0;
    }
      
      
    if (phrase.charAt(i) == guess)//If correct
    { int j = 0;
      for(j=0; j<=2;j++){
      digitalWrite(18, HIGH);
      delay(200);
      digitalWrite(18, LOW);
      delay(200);
      digitalWrite(18, HIGH);
      delay(200);
      digitalWrite(18, LOW);
      }
      
      guessNum ++;
      guesses [guessNum-1] = guess;
      return 1;
    }
  }

  //If not correct
  int k = 0;
  for(k=0;k<=2;k++){
      digitalWrite(19, HIGH);
      delay(200);
      digitalWrite(19, LOW);
      delay(200);
      digitalWrite(19, HIGH);
      delay(200);
      digitalWrite(19, LOW);
   
  }

    lives --;
  guessNum ++;
  guesses [guessNum-1] = guess;
  
  return -1;
  
}




// ********************************************* updateScreen *********************************************
void updateScreen (int code)
{  
  if (code == 0)//Repeat of a character
  {
    for (int i = 0; i < 3; i ++)
    {
      lcd.setCursor(0,0);
      lcd.print("    REPEAT    ");
      delay(200);
      lcd.setCursor(0,0);
      lcd.print("              ");
      delay(200);
    }
    
    lcd.setCursor(0,0);
    lcd.print("Guess a letter");//Prompt
  }
  
  if (code == 1)//Correct character
  {
    for (int i = 0; i < phrase.length() - 1; i ++)
    {
      if (phrase.charAt(i) == guesses [guessNum-1])
      {
        correct ++;
        lcd.setCursor(i,1);
        lcd.print(guesses [guessNum-1]);
      }
    }
    
  }

  if (code == -1)//Incorrect character
  {
    lcd.setCursor(15,0);
    lcd.write((uint8_t)(7-lives));
  }
}
