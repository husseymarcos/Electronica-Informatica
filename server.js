// Importar las dependencias necesarias
const { MongoClient } = require('mongodb');
const mqtt = require("mqtt");
const config = require('./config');

// Configuración de MongoDB
const mongoUri = `mongodb://${config.mongodb.hostname}:${config.mongodb.port}/${config.mongodb.database}`;

// Configuración de MQTT
const mqttUri = `mqtt://${config.mqtt.hostname}:${config.mqtt.port}`;
const mqttClient = mqtt.connect(mqttUri);

// Función asíncrona para insertar un documento en MongoDB
async function addBookToDB(message) {
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.bookCollection);

    const doc = {
      content: message
    }
    const existingBook = await collection.findOne(doc);

    if (!existingBook) {
      const result = await collection.insertOne(doc);
      console.log(`El libro "${doc.content.title}" fue insertado con el _id: ${result.insertedId}`);
    } else {
      console.log(`El libro con título "${doc.content.title}" ya existe en la base de datos.`);
    }
  } catch (error) {
    console.error("Error al insertar el documento:", error);
  } finally {
    await client.close();
  }
}

async function addUserCardToDB(uuid) {
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.usersRegisterCollection);

    const existingUser = await collection.findOne({ uuid: uuid});

    if (!existingUser) {
      const result = await collection.insertOne({ uuid });
      console.log(`Usuario con UUID ${uuid} registrado con el _id: ${result.insertedId}`);
    } else {
      console.log(`El usuario con UUID ${uuid} ya existe en la base de datos.`);
    }
  } catch (error) {
    console.error("Error al insertar el documento:", error);
  } finally {
    await client.close();
  }
}

async function verifyCard(uuid) {
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const registeredCollection = database.collection(config.mongodb.usersRegisterCollection);
    const card = await registeredCollection.findOne({ uuid: uuid });
    const verificationCollection = database.collection(config.mongodb.usersCollection);

    if(!card){
      const existingVerification = await verificationCollection.findOne({ uuid: uuid });

      if (!existingVerification) { // Si no existe lo agrega
        await verificationCollection.insertOne({ uuid: uuid });
        return true;
      }
    } else{
      console.log(`El usuario con UUID ${uuid} ya existe en la base de datos.`);
      return true; 
    }
  } catch (error) {
    console.error("Error verifying card:", error);
    return false;
  } finally {
    await client.close();
  }
}

async function requestBook(bookId) {
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const bookCollection = database.collection(config.mongodb.bookCollection);
    const book = await bookCollection.findOne({ _id: bookId });

    if (book) {
      const requestCollection = database.collection(config.mongodb.bookRequestCollection);
      const myBooksCollection = database.collection(config.mongodb.myBooksCollection);

      const existingRequest = await requestCollection.findOne({ _id: bookId });

      if (!existingRequest) {
        await requestCollection.insertOne({ _id: bookId });
        await myBooksCollection.insertOne({ _id: bookId });
        return true;
      }
      return false; // Ya estaba solicitado previamente.
    }
    return false; // El libro no se encontró.
  } catch (error) {
    console.error("Error requesting book:", error);
    return 'error';
  } finally {
    await client.close();
  }
}


// FIXME: ConfirmVerification provoca un loop impresionante. Hay que corregir eso
async function confirmVerification(successMsg) {
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.confirmVerificationCollection);
    const existingConfirmVerification = await collection.findOne({success: successMsg});

    if(!existingConfirmVerification){ // Si no encuentra ese mensaje en la collection, lo agrega
      await collection.insertOne({ success: successMsg });
      // mqttClient.publish('library/confirmVerification', successMsg); // FIXME: Esto estará bien acá? No entiendo :/
    } else{ // si el mensaje ya existe, informa que la verificación ya fue hecha. No inserta más nada.
      console.log("Se ha confirmado tu verificación, bienvenido!");
    }    
  } catch (error) {
    console.error("Error al insertar el documento:", error);
  } finally {
    await client.close();
  }
}

mqttClient.on("connect", () => {
  const topics = [
    "library/books",
    "library/registerUsers",
    "library/usersVerification",
    "library/bookRequests/#",
    "library/myBooks"
  ];

  topics.forEach(topic => {
    mqttClient.subscribe(topic, (err) => {
      if (!err) {
        console.log(`Connected and subscribed to topic ${topic}`);
      } else {
        console.error(`Error subscribing to topic ${topic}:`, err);
      }
    });
  });
});

mqttClient.on("message", async (topic, message) => {
  const messageString = message.toString();
  console.log(`Mensaje recibido en el tópico ${topic}: ${messageString}`);

  if (topic === "library/books") {
    addBookToDB(messageString).catch(console.dir);
    mqttClient.publish(`Mensaje recibido en el tópico ${topic}: ${messageString}`);
  }

  if (topic === "library/registerUsers") {
    const uuid = messageString;
    addUserCardToDB(uuid).catch(console.dir);
    mqttClient.publish(`Mensaje recibido en el tópico ${topic}: ${messageString}`);
  }

  if (topic === "library/usersVerification") { 
    const uuid = messageString;
    const isAuthorized = await verifyCard(uuid);
    // const responseTopic = `library/usersVerification/${uuid}`;
    // console.log("Topic actual: ", responseTopic);
    // mqttClient.publish(responseTopic, isAuthorized ? "authorized" : "unauthorized");

    // console.log("Acabo de realizar un publish con el estado de si esta autorizado o no");
    console.log("Estas autorizado? : ", isAuthorized);


    // FIXME: Es probable que acá también se produzca un error. Debido a que siempre hace el publish en confirm verification
    // Quizá una forma viable de poder evitar eso, es usar true y false, si es true lo hace como si fuera la primera vez, y sino ya está no hace nada.
    // TODO: Ver que carajo hace si ya está previamente autorizado.
    if (isAuthorized) {
      confirmVerification(`Tarjeta con UUID ${uuid} ingresó correctamente a LibrosExpress`).catch(console.dir);
      console.log(`Tarjeta con UUID ${uuid} ingresó correctamente a LibrosExpress`);
    }
    // console.log("El isAuthorized dio true");
  }

  if (topic.startsWith("library/bookRequests/")) {
    const bookId = topic.split('/').pop();
    const status = await requestBook(bookId);
    const responseTopic = `library/bookRequests/${bookId}`;
    mqttClient.publish(responseTopic, status ? "success" : "fail");
  }

  if (topic.startsWith("library/usersVerification/")) {
    const cardUUID = topic.split('/').pop();
    const status = await requestBook(bookId);
    const responseTopic = `library/bookRequests/${bookId}`;
    mqttClient.publish(responseTopic, status ? "success" : "fail");
  }


});

module.exports = {
  addBookToDB,
  mqttClient,
  mongoUri
};
