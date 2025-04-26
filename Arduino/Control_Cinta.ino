#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ----------------- Definición de pines -----------------
#define PULSADOR1 2    // Jog manual: avanzar
#define PULSADOR2 3    // Jog manual: retroceder
#define PULSADOR3 4    // Activa modo AUTOMÁTICO
#define PULSADOR4 5    // SELECT: en manual, alterna entre motor y servo; en automático, cancela
#define SENSOR_PIN 6   // Sensor láser
#define DIR_PIN    7   // Motor PAP direccion
#define PUL_PIN    8   // Motor PAP pulsos
#define SERVO_PIN  9   // Servo MG996R PWM

// ----------------- Configuración del LCD -----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ----------------- Parámetros del motor PAP -----------------
const long stepsPerRev = 3200;          // 3200 micropasos por revolución
const unsigned int steadyDelay = 235;   // µs por mitad del pulso en velocidad estable (~3 s/rev)
volatile long pasos = 0;                // Contador global de micropasos
const float DISTANCIA_MOVIMIENTO = 320.0; // Distancia en mm a recorrer tras detección del sensor

// Relación de transmisión y conversión a pasos
const float relacionTransmision = 60.0 / 20.0; // Relación de poleas 60T/20T = 3
const float diametroRodillo = 32.0 + (1.3 * 2); // Diámetro del rodillo + espesor de la banda
const float circunferenciaRodillo = 3.1416 * diametroRodillo; // Circunferencia total
const long pasosPorMM = (stepsPerRev * relacionTransmision) / circunferenciaRodillo; // Pasos por mm
const long pasosMovimiento = DISTANCIA_MOVIMIENTO * pasosPorMM; // Pasos necesarios para la distancia

// ----------------- Constantes para evaluación -----------------
const float LIMITE_MIN_ANCHO = 16.0; // mm
const float LIMITE_MAX_ANCHO = 20.0; // mm
const float LIMITE_MIN_ALTO = 10.0;  // mm
const float LIMITE_MAX_ALTO = 14.0;  // mm

const int SERVO_POS_OK = 90;         // Posición del servo si es OK
const int SERVO_POS_DE = 140;        // Posición del servo si es DE
const int SERVO_POS_NEUTRO = 90;     // Posición neutral del servo

const float LONGITUD_SALIDA = 450.0; // mm a mover luego de clasificar
const long pasosLargoSalida = LONGITUD_SALIDA * pasosPorMM; // Pasos para los 500 mm

// ----------------- Variables de control -----------------
bool autoModeActive = false;
bool manualDeviceIsPAP = true;          // En modo manual: true = motor PAP; false = servo
Servo servoMotor;
int servoAngle = 90;                    // Ángulo inicial del servo

// ----------------- Variables para mantener últimos datos -----------------
bool datosDisponibles = false;
float ultimoAncho = 0;
float ultimoAlto = 0;
float ultimoLargo = 0;
String ultimoEstado = "";

// ----------------- Función para generar un micropaso -----------------
void stepMotor(unsigned int halfCycleDelay) {
  digitalWrite(PUL_PIN, HIGH);
  delayMicroseconds(halfCycleDelay);
  digitalWrite(PUL_PIN, LOW);
  delayMicroseconds(halfCycleDelay);
}

// ----------------- Función para actualizar el LCD -----------------
void actualizarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (autoModeActive) {
    lcd.print("AUTOMATIC MODE");
  } else if (datosDisponibles) {
    lcd.print("W=");
    lcd.print(ultimoAncho, 1);
    lcd.print(" H=");
    lcd.print(ultimoAlto, 1);
    lcd.setCursor(0, 1);
    lcd.print("L=");
    lcd.print(ultimoLargo, 1);
    lcd.print(" S=");
    lcd.print(ultimoEstado);
  } else {
    lcd.print("                ");  // Pantalla vacía
  }
}

// ----------------- Modo AUTOMÁTICO -----------------
void autoMode() {
  autoModeActive = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AUTOMATIC MODE");
  lcd.setCursor(0, 1);
  lcd.print("Waiting for part");

  digitalWrite(DIR_PIN, HIGH); // Dirección de avance
  long startSteps = pasos;
  bool midiendo = false;

  // Esperar detección de pieza
  while (digitalRead(SENSOR_PIN) == HIGH && digitalRead(PULSADOR4) == LOW) {
    stepMotor(steadyDelay);
    pasos++;
  }

  // Mientras la pieza está bajo el sensor
  while (digitalRead(SENSOR_PIN) == LOW && digitalRead(PULSADOR4) == LOW) {
    if (!midiendo) {
      midiendo = true;
      startSteps = pasos;
    }
    stepMotor(steadyDelay);
    pasos++;
  }

  // Calcular largo (L)
  long detectedSteps = pasos - startSteps;
  float detectedDistance = ((float)detectedSteps / pasosPorMM) + 2.0; // Se suma corrección del sensor

  // Avance de separación para medición
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AUTOMATIC MODE");
  lcd.setCursor(0, 1);
  lcd.print("Moving to scan");
  for (long i = 0; i < pasosMovimiento && digitalRead(PULSADOR4) == LOW; i++) {
    stepMotor(steadyDelay);
    pasos++;
  }

  // Solicitud de medición a Python
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AUTOMATIC MODE");
  lcd.setCursor(0, 1);
  lcd.print("Measuring...");
  Serial.print("MEDIR");

  while (!Serial.available()) {
    delay(10);
  }

  String data = Serial.readStringUntil('\n');
  float ancho = 0;
  float alto = 0;

  int separador = data.indexOf(' ');
  if (separador > 0) {
    String parte1 = data.substring(0, separador);
    String parte2 = data.substring(separador + 1);

    ancho = parte1.toFloat();
    alto = parte2.toFloat();
  }

  // Evaluación del perfil
  String estado = "DE";
  if (ancho > LIMITE_MIN_ANCHO && ancho < LIMITE_MAX_ANCHO &&
      alto > LIMITE_MIN_ALTO && alto < LIMITE_MAX_ALTO) {
    estado = "OK";
  }

  // Mostrar resultado inmediatamente
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("W=");
  lcd.print(ancho, 1);
  lcd.print(" H=");
  lcd.print(alto, 1);
  lcd.setCursor(0, 1);
  lcd.print("L=");
  lcd.print(detectedDistance, 1);
  lcd.print(" S=");
  lcd.print(estado);
  delay(1500);

  // Actuar según evaluación
  if (estado == "OK") {
    servoMotor.write(SERVO_POS_OK);
  } else {
    servoMotor.write(SERVO_POS_DE);
  }

  delay(500);

  // Avance hacia salida
  for (long i = 0; i < pasosLargoSalida && digitalRead(PULSADOR4) == LOW; i++) {
    stepMotor(steadyDelay);
  }

  delay(300);
  servoMotor.write(SERVO_POS_NEUTRO);
  delay(500);

  // Guardar datos
  datosDisponibles = true;
  ultimoAncho = ancho;
  ultimoAlto = alto;
  ultimoLargo = detectedDistance;
  ultimoEstado = estado;

  autoModeActive = false;
  actualizarLCD();
}

// ----------------- Modo MANUAL (JOG) para motor PAP -----------------
void manualMotorJog(bool direction) {
  digitalWrite(DIR_PIN, direction ? LOW : HIGH);
  while ((direction ? digitalRead(PULSADOR1) : digitalRead(PULSADOR2)) == HIGH) {
    stepMotor(steadyDelay);
    pasos += (direction ? 1 : -1);
  }
  actualizarLCD();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MANUAL MODE");
  lcd.setCursor(0, 1);
  lcd.print("JOG MOTOR");
}

// ----------------- Modo MANUAL (JOG) para el servomotor -----------------
void manualServoJog(bool direction) {
  while ((direction ? digitalRead(PULSADOR1) : digitalRead(PULSADOR2)) == HIGH) {
    servoAngle += direction ? 1 : -1;
    if (servoAngle > 180) servoAngle = 180;
    if (servoAngle < 0) servoAngle = 0;
    servoMotor.write(servoAngle);
    delay(20);
  }
  actualizarLCD();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MANUAL MODE");
  lcd.setCursor(0, 1);
  lcd.print("JOG SERVO");
}

// ----------------- Setup y Loop -----------------
void setup() {
  pinMode(DIR_PIN, OUTPUT);
  pinMode(PUL_PIN, OUTPUT);
  pinMode(PULSADOR1, INPUT);
  pinMode(PULSADOR2, INPUT);
  pinMode(PULSADOR3, INPUT);
  pinMode(PULSADOR4, INPUT);
  pinMode(SENSOR_PIN, INPUT);

  lcd.init();
  lcd.backlight();

  // Pantalla de bienvenida fija
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("QUALITY STATION");
  lcd.setCursor(0, 1);
  lcd.print("BY PROFILE SCAN");

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(servoAngle);

  Serial.begin(115200);
  delay(2000);
}

void loop() {
  if (!autoModeActive) {
    if (digitalRead(PULSADOR3) == HIGH) {
      autoMode();
      while (digitalRead(PULSADOR3) == HIGH);
      delay(200);
    }
    if (digitalRead(PULSADOR4) == HIGH) {
      manualDeviceIsPAP = !manualDeviceIsPAP;
      actualizarLCD();
      while (digitalRead(PULSADOR4) == HIGH);
        if (manualDeviceIsPAP == true) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MANUAL MODE");
        lcd.setCursor(0, 1);
        lcd.print("Motor");
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("MANUAL MODE");
        lcd.setCursor(0, 1);
        lcd.print("Servo");
      }
      delay(200);
    }
    if (digitalRead(PULSADOR1) == HIGH) {
      if (manualDeviceIsPAP) {
        manualMotorJog(true);
      } else {
        manualServoJog(true);
      }
    }
    if (digitalRead(PULSADOR2) == HIGH) {
      if (manualDeviceIsPAP) {
        manualMotorJog(false);
      } else {
        manualServoJog(false);
      }
    }
  }
}


