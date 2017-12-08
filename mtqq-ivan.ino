
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Unilink_Escritorio";
const char* password = "35060708";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int valor = 0;

void setup_wifi() {

  delay(10);
  // Inicia conexão Wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

//Enquanto não conectado fica em loop
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem chegada [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Liga o LED se o primeiro caracter recebido for o número 1 
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  //Liga o LED
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Desliga
  }

}

void reconnect() {
  // Loop até reconectar
  while (!client.connected()) {
    Serial.print("Tentando conexão com MQTT...");
    
    String clientId = "Cliente-Ivan";
    // Tenta conectar
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Quando conecta emite uma mensagem
      client.publish("outTopic", "Oi lazarento! Estou conectado.");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Inciia o led como saída
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++valor;
    snprintf (msg, 75, "Olá #%ld", valor);
    Serial.print("Publica menssagem: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
  }
}
