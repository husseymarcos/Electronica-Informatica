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


// Función asíncrona para eliminar un libro de la DB
async function deleteBookFromDB(bookToDelete){
  const client = new MongoClient(mongoUri);

  try{
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection("books");

    const result = await collection.deleteOne(bookToDelete);

    if(result.deletedCount > 0){
      console.log(`Documento eliminado: ${JSON.stringify(bookToDelete)}`);
    } else{
      console.log(`No se encontró ningún documento que coincida con: ${JSON.stringify(bookToDelete)}`);
    }
  } catch (error){
    console.error(`Error al eliminar el documento: `, error);
  } finally{
    await client.close();
  }
}



console.log("No llego a la parte de mqtt y no sé por qué")

// Conectar al broker MQTT y suscribirse a los tópicos -> Este escucha toda la información que se va ir publicando. Luego esa información la sube a la db
mqttClient.on("connect", () => {
  mqttClient.subscribe("library/books", (err) => { // con el + indico que quiero que se suscriba a todos. Si en lugar de + especifico uno particular, evidentemente va a escuchar solo ese topic.
    if (!err) {
      console.log("Cliente conectado y suscrito a todos los tópicos");
    } else {
      console.error("Error al suscribirse a los tópicos:", err);
    }
  });

  mqttClient.subscribe("library/books/delete", (err) =>{
    if(!err){
      console.log("Cliente conectado y suscrito al tópico library/books/delete");
    } else{
      console.error("Error al suscribirse al tópico library/books/delete: ", err);
    }

  });
});




// Manejar los mensajes recibidos en los tópicos 
mqttClient.on("message", (topic, message) => {
  if(topic === "library/books"){
    const messageString = message.toString();
    console.log(`Mensaje recibido en el tópico ${topic}: ${messageString}`);
    addBookToDB(messageString).catch(console.dir);

  } else if(topic === "library/books/delete"){
    const bookToDelete = JSON.parse(message.toString());
    console.log(`Solicitud de eliminación recibida en el tópico ${topic}`);
    deleteBookFromDB(bookToDelete).catch(console.dir);
  }
  
});


/*Si queres eliminar un archivo, por ejemplo, si queres hacerlo desde MQTT Explorer
podes hacerlo sí:
{
  content.title = "El principito"
}

Te elimina el doc que tenga de title "El principito"
*/ 