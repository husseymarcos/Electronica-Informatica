// Importar las dependencias necesarias
const { MongoClient } = require('mongodb');
const mqtt = require("mqtt");
var config = require('./config');
const { mongo, connect } = require('mongoose');

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
    await client.connect();
    // Conectar a la base de datos especificada en la configuración
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.bookCollection);

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


// Agregar al usuario 
async function addUserCardToDB(uuid) { // Acá defino los datos que debe recibir la estructura del dato a agregar. Tiene que tener ese mismo formato
  // Crear un nuevo cliente y conectar a MongoDB
  const client = new MongoClient(mongoUri);

  try {
    await client.connect();
    // Conectar a la base de datos especificada en la configuración
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.usersRegisterCollection);

    // Crear un documento para insertar - Acá especifico que tipo de formato de dato tiene que recibir cuando este escuchando. 
    const doc = {
      uuid: uuid // Este message engloba toda la información del formato json, uuid.
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



// Verificación de la tarjeta RFID
async function verifyCard(uuid) {
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.usersCollection);
    const card = await collection.findOne({ uuid: uuid });
    return card !== null;
  } catch (error) {
    console.error("Error verifying card:", error);
    return false;
  } finally {
    await client.close();
  }
}


// Conectar al broker MQTT y suscribirse a los tópicos -> Este escucha toda la información que se va ir publicando. Luego esa información la sube a la db
mqttClient.on("connect", () => {
  mqttClient.subscribe("library/books", (err) => { // con el + indico que quiero que se suscriba a todos. Si en lugar de + especifico uno particular, evidentemente va a escuchar solo ese topic.
    if (!err) {
      console.log("Cliente conectado y suscrito al tópico library/books");
    } else {
      console.error("Error al suscribirse al tópico library/books:", err);
    }
  });

  mqttClient.subscribe("library/registerUsers", (err) =>{
    if (!err) {
      console.log("Connected and subscribed to topic library/registerUsers");
    } else {
      console.error("Error subscribing to topic library/registerUsers:", err);
    }
  })

  mqttClient.subscribe("library/usersVerification", (err) => {
      if (!err) {
        console.log("Connected and subscribed to topic library/usersVerification");
      } else {
        console.error("Error subscribing to topic library/usersVerification:", err);
      }
  });

  
});




// Manejar los mensajes recibidos en los tópicos 
mqttClient.on("message", async (topic, message) => {
  if(topic === "library/books"){
    const messageString = message.toString();
    console.log(`Mensaje recibido en el tópico ${topic}: ${messageString}`);
    addBookToDB(messageString).catch(console.dir);

  } 

  // Registra al usuario 
  if(topic === "library/registerUsers"){
    console.log(`Received message on topic ${topic}: ${message}`);
    const uuid = message.toString();
    console.log(`Mensaje recibido en el tópico ${topic}: ${uuid}`);
    addUserCardToDB(uuid).catch(console.dir);
  }
  
  // Verifica el estado del usuario
  if (topic === "library/usersVerification") {
    console.log(`Received message on topic ${topic}: ${message}`);
    const uuid = message.toString();
    const isAuthorized = await verifyCard(uuid);
    const responseTopic = `library/usersVerification/${uuid}`;
    mqttClient.publish(responseTopic, isAuthorized ? "authorized" : "unauthorized");
    console.log(`Card with UUID ${uuid} is ${isAuthorized ? "authorized" : "unauthorized"}`);
  }
});

  


/*Si queres eliminar un archivo, por ejemplo, si queres hacerlo desde MQTT Explorer
podes hacerlo sí:
{
  content.title = "El principito"
}

Te elimina el doc que tenga de title "El principito"
*/ 


