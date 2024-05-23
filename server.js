// Importar las dependencias necesarias
const { MongoClient } = require('mongodb');
const mqtt = require("mqtt");
var config = require('./config');

// Configuración de MongoDB

// mongodb://172.31.90.101/test
var mongoUri = 'mongodb://' + config.mongodb.hostname + ':' + config.mongodb.port + '/' + config.mongodb.database;

// Configuración de MQTT
var mqttUri  = 'mqtt://' + config.mqtt.hostname + ':' + config.mqtt.port;
const mqttClient = mqtt.connect(mqttUri);

// Función asíncrona para insertar un documento en MongoDB
async function insertMessageToDB(message) {
  // Crear un nuevo cliente y conectar a MongoDB
  const client = new MongoClient(mongoUri);

  try {
    // Conectar a la base de datos especificada en la configuración
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection("message");

    // Crear un documento para insertar
    const doc = {
      fecha: new Date(),
      content: message,
    };

    // Insertar el documento en la colección
    const result = await collection.insertOne(doc);

    // Imprimir el ID del documento insertado
    console.log(`Documento insertado con el _id: ${result.insertedId}`);
  } catch (error) {
    console.error("Error al insertar el documento:", error);
  } finally {
    // Cerrar la conexión del cliente de MongoDB
    await client.close();
  }
}


// Conectar al broker MQTT y suscribirse a los tópicos
mqttClient.on("connect", () => {
  mqttClient.subscribe("+", (err) => {
    if (!err) {
      console.log("Cliente conectado y suscrito a todos los tópicos");
    } else {
      console.error("Error al suscribirse a los tópicos:", err);
    }
  });
});

// Manejar los mensajes recibidos en los tópicos
mqttClient.on("message", (topic, message) => {
  const messageString = message.toString();
  console.log(`Mensaje recibido en el tópico ${topic}: ${messageString}`);

  // Insertar el mensaje en la base de datos
  insertMessageToDB(messageString).catch(console.dir);
});

