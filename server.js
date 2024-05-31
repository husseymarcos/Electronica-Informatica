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


async function connectToMongoDB(){
  const client = new MongoClient(mongoUri);
  await client.connect();
  return client;
}


// Función asíncrona para insertar un documento en MongoDB
async function addBookToDB(message) { // Acá defino los datos que debe recibir la estructura del dato a agregar. Tiene que tener ese mismo formato
  // Crear un nuevo cliente y conectar a MongoDB
  const client = await connectToMongoDB();

  try {
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


// Función asíncrona para eliminar un libro de la DB
async function deleteBookFromDB(bookToDelete){ // Proba este método!
  const client = await connectToMongoDB();

  try{
      var database = client.db(config.mongodb.database);
      var query = {content: bookToDelete}
      
      database.collection("books").deleteOne(query, function(err){ // Tengo que eliminar de books. Me sirve de algo tener el topic books/delete? Considero que no, podemos el mismo books para eliminar datos desde allí!!
        if(err) {
          console.log(`No se encontró ningún documeto que coincida con: ${JSON.stringify(bookToDelete)}`);
          throw err;
        }

        console.log(`Documento eliminado: ${JSON.stringify(bookToDelete)}`);
        client.close();
      });
    } catch(error){
        console.error(`Error al eliminar el documento: `, error);
    } finally{
        console.log("Hay algo que no está funcionando bien");
  }
  /*  
    const database = client.db(config.mongodb.database);
    const collection = database.collection("books");

    const doc = {
      content: bookToDelete
    }
    const result = await collection.deleteOne(doc);

    if(result.deletedCount > 0){
      console.log(`Documento eliminado: ${JSON.stringify(bookToDelete)}`);
    } else{
      console.log(`No se encontró ningún documento que coincida con: ${JSON.stringify(bookToDelete)}`);
    }
  } catch (error){
    console.error(`Error al eliminar el documento: `, error);
  } finally{
    await client.close();
    */
}

async function insertRandomNumber(number){
  const client = await connectToMongoDB();
  const database = client.db(config.mongodb.database);
  const collection = database.collection(config.mongodb.randomNumberCollection);
  collection.insertOne({number: number, timestamp: new Date()});
  console.log(`Número insertado: ${number}`);
  await client.close();
}





// Conectar al broker MQTT y suscribirse a los tópicos -> Este escucha toda la información que se va ir publicando. Luego esa información la sube a la db
mqttClient.on("connect", () => {
  mqttClient.subscribe("library/books", (err) => { // con el + indico que quiero que se suscriba a todos. Si en lugar de + especifico uno particular, evidentemente va a escuchar solo ese topic.
    if (!err) {
      console.log("Cliente conectado y suscrito a todos los tópicos");
    } else {
      console.error("Error al suscribirse a los tópicos:", err);
    }
  });

  mqttClient.subscribe("library/books/delete", (err) =>{ // Considero que el error está acá!
    if(!err){
      console.log("Cliente conectado y suscrito al tópico library/books/delete");
    } else{
      console.error("Error al suscribirse al tópico library/books/delete: ", err);
    }

  });

  // Test para ver si el código puede agregar números aleatorios al mongoDB
  mqttClient.subscribe("library/randomNumbers", (err) =>{
    if(!err){
      console.log("Cliente conectado y suscrito al tópico library/randomNumbers");
    } else{
      console.error("Error al suscribirse al tópico library/randomNumbers: ", err);
    }

  });
});




// Manejar los mensajes recibidos en los tópicos 
mqttClient.on("message", (topic, message) => {
  if(topic === "library/books"){
    const messageString = message.toString();
    console.log(`Mensaje recibido en el tópico ${topic}: ${messageString}`);
    addBookToDB(messageString).catch(console.dir);

  } else if(topic === "library/books/delete"){ // Esta parte, quizá es innecesaria, a nivel topic, pero el método debe estar. Fijate como podes identificar para agregar o eliminar un libro!
    const bookToDelete = JSON.parse(message.toString());
    console.log(`Solicitud de eliminación recibida en el tópico ${topic}`);
    deleteBookFromDB(bookToDelete).catch(console.dir);
  }

  if(topic === "library/randomNumbers"){
    console.log(`Mensaje recibido en ${topic}: ${message.toString()}`);
    const number = parseInt(message.toString(), 10); // no sé de que sirve
    if(!isNaN(number)){
      insertRandomNumber(number);
    } else{
      console.error('El mensaje recibido no es un número válido: ', message.toString());
    }
  }

});


/*Si queres eliminar un archivo, por ejemplo, si queres hacerlo desde MQTT Explorer
podes hacerlo sí:
{
  content.title = "El principito"
}

Te elimina el doc que tenga de title "El principito"
*/ 


