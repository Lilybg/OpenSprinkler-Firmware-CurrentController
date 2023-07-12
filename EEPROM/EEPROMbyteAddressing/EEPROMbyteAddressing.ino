 /*******************************************************************************

 M95M01 SPI EEPROM library
 -------------------------
 
 M95M01_test.ino - M95M01 test sketch
 
 Code written by Stefan Dzisiewski-Smith.
 
 This work is licensed under a Creative Commons Attribution-ShareAlike 3.0 
 Unported License (CC BY-SA 3.0) http://creativecommons.org/licenses/by-sa/3.0/
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

*******************************************************************************/

#include "M95M01.h"

const uint8_t CS = 5; // connect EEPROM CS pin to D2 on Arduino
const uint8_t HOLD = 6;
const uint32_t unaligned_array_length = 881; // prime number to avoid page alignment
const uint32_t aligned_array_length = 512; // page aligned - must be < unaligned_array_length

uint32_t i;
uint32_t j;
uint32_t array_limit;

uint8_t readback[unaligned_array_length];  
uint8_t test_array[unaligned_array_length];

void setup() {

  //digitalWrite(HOLD, HIGH);

  M95M01.begin(CS, 20000000); // I have limited this to a 2MHz clock speed, but feel free to increase

  Serial.begin(115200);

  while(!Serial); // wait for Serial service to start on Leonardo  
}

void loop() {

  Serial.println(F("Starting full address write and read test:")); 
 

/********* ALIGNED ARRAY *********/

  //M95M01.write_byte(0, 25);
  //M95M01.read_byte(0);
  Serial.println(M95M01.write_byte(8, 25));
  Serial.println(M95M01.read_byte(100));

/*************  END  *************/   


/*************  END  *************/ 

  while(!Serial.available()){

  }
  while(Serial.available()){
    Serial.read(); // chew up the chars so they don't retrigger us
  }

}