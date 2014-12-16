unsigned int time, ltime; //variabili usate per verificare la velocità dell'algoritmo
int analogInPin = 0; //canale di acquisizione
int sps;    //indice di scorrimento
double Vcc; // conterrà il valore della Vcc dell'ATMEGA, sulla base del qule èpossibile ricavare il valore reale della Vref
float outputValue = 0; // conterrà il dato acquisito e ricalcolato sulla base della Vcc      
float streamsample[8]; //array utilizzato per creare il "condensatore" digitale, la dimensione dell'array è la sua "capacità" es. [4] è un valore sufficiente per risultati visibili

void setup() {
  Serial.begin(9600);     //inizializza la seriale
  Vcc = readVcc()/1000.0; //calcola la Vcc con cui è alimentato l'ATMEGA e ne ricava la reale Vref
  outputValue = analogRead(analogInPin);      //acquisice il dato analogico
  outputValue = (outputValue / 1023.0) * Vcc; // e ne calcola il valore reale tenendo conto della Vref
  streamsample[0] = outputValue;              //inserisce il valore acquisito nell'primo elemento dell'array
  for (sps=0; sps<8; sps++){                       
  streamsample[sps+1] = streamsample[0];      //e lo inizializza riempiendone l'intero stream 
  }
}

void loop() {
  //test acquisizione senza filtro
  Vcc = readVcc()/1000.0;  //calcola la Vcc( ossia la Vref)
  time = micros();         //inizia il "timer"
  outputValue = analogRead(analogInPin);       //acuisisce il valore analogico
  outputValue = (outputValue / 1023.0) * Vcc;  //lo converte in un valore reale sulla base della Vcc
  time = micros() - time;        //ferma il "timer"
  Serial.print(outputValue, 3); //Scrive il valore acquisito e il tempo impiegato nell'acquisizione e conversione
  Serial.print("  us:");
  Serial.print(time);
  Serial.print("  ");
  //test acquisizione con filtro digitale
  time = micros();        //inizia il "timer"
  outputValue = analogRead(analogInPin);      //acuisisce il valore analogico
  outputValue = (outputValue / 1023.0) * Vcc; //lo converte in un valore reale sulla base della Vcc
  //il valore analogico viene ricalcolato come media del valore misurato negli ultimi 10 campionamenti
  outputValue = (outputValue + streamsample[0] + streamsample[1] + streamsample[2] + streamsample[3] + streamsample[4] 
  + streamsample[5] + streamsample[6] + streamsample[7] + streamsample[8])/10;
  //lo array stream viene shiftato di una posizione ed è inserito il nuovo valore calcolato nella prima posizione
  //dell'array (quindi dopo 9 campionamenti, l'array è composto dai valori medi calcolati in precedenza
  streamsample[8] = streamsample[7];
  streamsample[7] = streamsample[6];
  streamsample[6] = streamsample[5];
  streamsample[5] = streamsample[4];
  streamsample[4] = streamsample[3];
  streamsample[3] = streamsample[2];
  streamsample[2] = streamsample[1];
  streamsample[1] = streamsample[0];
  streamsample[0] = outputValue;
  time = micros() - time; //ferma il "timer"
  Serial.print(outputValue, 3);  //Scrive il valore acquisito e il tempo impiegato nell'acquisizione e conversione
  Serial.print("  us:");
  Serial.println(time);
}

long readVcc() {
  long result;
  // legge la Vref interna a 1.1V dell'ATMEGA e la paragona alla Vcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Aspetta che la Vref si setti
  ADCSRA |= _BV(ADSC); // convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}
