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

    // Verificar si ya existe un usuario con el mismo UUID
    const existingBook = await collection.findOne(doc);

    if(!existingBook){
      // Insertar el documento en la colección
      const result = await collection.insertOne(doc);
      console.log(`El libro "${content.title}" fue insertado`);
      console.log(`Documento insertado con el _id: ${result.insertedId}`);
    } else{
      console.log(`El libro con título "${content.title}" ya existe en la base de datos.`);
    }
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
async function verifyCard(uuid) { // Creo que por este lado ocurre el error de agregar 40 veces lo mismo
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.usersRegisterCollection); // Busca en la base de los usuarios registrados
    const card = await collection.findOne({ uuid: uuid }); // Acá lo busca si lo encuentra.
    const verificationCollections = database.collection(config.mongodb.usersCollection); // Creo que la colección de usuarios a verificar
    
    if(card != null){ // Si encuentra la carta, agrega eso, a la base de datos de usersCollection
      const userData = {
        uuid: uuid
      }
      await verificationCollections.insertOne(userData); // Agregamos esa data de registro a la de verificación. De esta forma aseguramos que todo usuario registrado esté autorizado.
      const verifyUser = await verificationCollections.findOne(userData); // lo busca.
      return verifyUser != null; // Si está true, en teoría está autorizado.
    }
    else{
      return false; // el usuario no existe y por tanto la lógica no se hace
    }
  } catch (error) {
    console.error("Error verifying card:", error);
    return false;
  } finally {
    await client.close();
  }
}


// Solicitud de un libro
async function requestBook(bookId){
  const client = new MongoClient(mongoUri);

  try{
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.bookCollection); // Libros que ya están agregados
    const book = await collection.findOne({_id : bookId}); 
    const requestCollection = database.collection(config.mongodb.bookRequestCollection); // Coleccion de libros a solicitar

    if(book != null){
      const bookData = {
        _id: bookId
      }
      await requestCollection.insertOne(bookData); // Agrego la información del libro que quiero solicitar, para luego poder solicitarlo. 
      const requestedBook = await requestCollection.findOne(bookData); // Busco el libro que fue solicitado en esa colección.
      return requestedBook != null; // Si es true, quiere decir que el libro que solicitaste está disponible
    } else{
      return false; // El libro no se encontró, porque ni siquiera está en la colección de libros agregados.
    }
  } catch(error){
    console.error("Error requesting book:", error);
    return 'error';
  } finally{
    await client.close();
  }
}

async function confirmVerification(successMsg){
  const client = new MongoClient(mongoUri);
  try{
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.confirmVerificationCollection);

    const doc = {
      success: successMsg
    }

    await collection.insertOne(doc);

  } catch(error){
    console.error("Error al insertar el documento:", error);
  } finally{
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

  mqttClient.subscribe("library/bookRequests/#", (err) =>{
    if(!err){
      console.log("Connected and subscribed to topic library/bookRequests/#");
    } else{
      console.error("Error subscribing to topic library/bookRequests/#", err);
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

    // Publicar confirmación en el topic adecuado - Vinculación con el ESP32 
    if (isAuthorized) {
      confirmVerification(`Tarjeta con UUID ${uuid} ingresó correctamente a LibrosExpress`);
    }
  }

  // Manejar las solicitudes de libros
  if(topic.startsWith("library/bookRequests/")){
    const bookId = topic.split('/').pop();
    const status = await requestBook(bookId);
    const responseTopic = `library/bookRequests/${bookId}`;
    mqttClient.publish(responseTopic, status);
    console.log(`Book with ID ${bookId} is ${status}`);
  }
});

  


/*Si queres eliminar un archivo, por ejemplo, si queres hacerlo desde MQTT Explorer
podes hacerlo sí:
{
  content.title = "El principito"
}

Te elimina el doc que tenga de title "El principito"
*/ 

module.exports = {
  addBookToDB
}
