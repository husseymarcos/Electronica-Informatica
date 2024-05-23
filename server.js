// Importar las dependencias necesarias
const { MongoClient } = require('mongodb');
const mqtt = require("mqtt");
var config = require('./config');

// Configuración de MongoDB

// mongodb://172.31.90.101/library
var mongoUri = 'mongodb://' + config.mongodb.hostname + ':' + config.mongodb.port + '/' + config.mongodb.database;

// Configuración de MQTT
var mqttUri  = 'mqtt://' + config.mqtt.hostname + ':' + config.mqtt.port;
const mqttClient = mqtt.connect(mqttUri);

// Función asíncrona para insertar un documento en MongoDB
async function addBookToDB(message) { // Acá defino los datos que debe recibir la estructura del dato a agregar. Tiene que tener ese mismo formato
  // Crear un nuevo cliente y conectar a MongoDB
  const client = new MongoClient(mongoUri);

  try {
    // Conectar a la base de datos especificada en la configuración
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection("books");

    // Crear un documento para insertar - Acá especifico que tipo de formato de dato tiene que recibir cuando este escuchando. 
    const doc = {
      content: message // Este message engloba toda la información del formato json, es decir title, author, etc.
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


// Conectar al broker MQTT y suscribirse a los tópicos -> Este escucha toda la información que se va ir publicando. Luego esa información la sube a la db
mqttClient.on("connect", () => {
  mqttClient.subscribe("+", (err) => { // con el + indico que quiero que se suscriba a todos. Si en lugar de + especifico uno particular, evidentemente va a escuchar solo ese topic.
    if (!err) {
      console.log("Cliente conectado y suscrito a todos los tópicos");
    } else {
      console.error("Error al suscribirse a los tópicos:", err);
    }
  });
});

// Manejar los mensajes recibidos en los tópicos
mqttClient.on("books", (topic, message) => {
  if(topic === "library/books"){
    const messageString = message.toString();
    console.log(`Mensaje recibido en el tópico ${topic}: ${messageString}`);
  }
  console.log("Saca el topic === library/books, porque no está configurado anteriormente con el +")

  // Insertar el mensaje en la base de datos
  addBookToDB(messageString).catch(console.dir);
});

