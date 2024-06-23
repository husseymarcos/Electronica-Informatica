// Importar las dependencias necesarias
const { MongoClient, ObjectId } = require('mongodb');
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

async function verifyCard(uuid) { // Interactuan las colecciones de registerUsers y usersVerification
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const registeredCollection = database.collection(config.mongodb.usersRegisterCollection); // registerUsers
    const card = await registeredCollection.findOne({ uuid: uuid });
    const verificationCollection = database.collection(config.mongodb.usersCollection); // usersVerification

    if(!card){
      const existingVerification = await verificationCollection.findOne({ uuid: uuid });

      if (!existingVerification) { // Si no existe lo agrega
        await verificationCollection.insertOne({ uuid: uuid }); // Acá le agrego datos a usersVerification
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


/* TODO: Cosas a evaluar

- Lo que debería evaluarse, es si hay alguna forma de ver si está o no el libro que solicitas.

Es posible pensarlo, utilizando la collection de books, que contiene todos los libros que se fueron utilizando.

- Si el libro está, se debería agregar a la collection de myBooks, que es la que contiene los libros que tiene el usuario.

Bien, pensemos así la solicitud entonces:

1. Se recibe el mensaje con el id del libro.
2. Se busca el libro en la collection de books.
3. Si el libro está, se agrega a la collection de myBooks.
4. Se agrega a la collection de bookRequests.
5. Se responde al cliente con un mensaje de éxito o fracaso.
6. Se envía un mensaje al cliente con el resultado de la solicitud. 
7. Informar por el SerialMonitor de Arduino 2 cosas:
  - Por un lado el libro que solicitó.
  - Por otro lado, si la solicitud fue o no un éxito.

Entonces tenemos dos cuestiones:
- Por el lado de la página web, se avisa al usuario que lo usa: "Libro solicitado correctamente"
- Por el lado de Serial Monitor, debería informarse, "El libro solicitado fue: 'nombre del libro' y la solicitud fue un éxito o fracaso"

De esa forma interactuamos tanto con la página web, como con el proyecto físico. 

Casos de solicitud del mismo libro:
1. Se chequea en bookRequests. Si el libro está ahí quiere decir que no puede volver a ser solicitado.
2. Se le informa al usuario que "Libro no disponible"
3. Se le informa al SerialMonitor que "El libro solicitado fue: 'nombre del libro' y la solicitud fue un fracaso"

*/ 

/*TODO:

Fijate que no te duplique las cosas tanto por node server.js, como por node mqtt-http.js mira eso. 
*/

async function requestBook(bookId) { // TODO: Ver esto
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const bookCollection = database.collection(config.mongodb.bookCollection);
    const objectId = new ObjectId(bookId); // conversión de bookId a ObjectId para evitar problemas de casteo
    const book = await bookCollection.findOne({ _id: objectId }); // Chequea que el libro esté.

    if (book) {
      const requestCollection = database.collection(config.mongodb.bookRequestCollection); // TODO: Es necesario tener esta collection?
      const myBooksCollection = database.collection(config.mongodb.myBooksCollection);

      const existingRequest = await requestCollection.findOne({ _id: objectId });

      // TODO: La lógica que falta aquí, es eliminar el libro de la base de datos de books, de esa forma no aparece en los libros para ser solicitados.
      const deleteBook = await bookCollection.deleteOne({ _id: objectId }); // Acá removemos el libro que tenga esa id. Pero lo guardamos en esa variable.
      // TODO: deleteBook, puede usarse para el método de returnBook, fijate como podría vincularse.

      returnBook(deleteBook);

      if (!existingRequest) {
        const doc = {
          _id: objectId,
          title: book.content.title,
          author: book.content.author,
          genre: book.content.genre,
          year: book.content.year
        }  
        await requestCollection.insertOne(doc); 
        await myBooksCollection.insertOne(doc);
        console.log(`Libro: ${book.content.title} solicitado exitosamente. `);
        return true;
      } else {
        console.log(`El libro con id ${bookId} ya fue solicitado previamente.`);
        return false; // Ya estaba solicitado previamente.
      }
    } else{
      console.log(`El libro con id ${bookId} no se encontró.`);
      return false; // El libro no se encontró.
    }
  } catch (error) {
    console.error("Error solicitando libro:", error);
    return 'error';
  } finally {
    await client.close();
  }
}

// TODO: Devolución del libro

/*Posible forma de realizar la devolución, sería poner una sección: 

En primer lugar, una sección donde el usuario por tarjeta, tiene libros asociados, por lo que:

1. Puede elegir que libro quiere devolver
2. Poner un botón para devolver, donde cuando lo presiona, reincorpora el libro a la base de datos inicial. library/books.
*/

async function returnBook(bookToReturn){ // Acá vamos a usar el libro que desea devolver el usuario. 
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);

    // Debo eliminarlo de bookRequests y myBooks
    const requestCollection = database.collection(config.mongodb.bookRequestCollection);
    const myBooksCollection = database.collection(config.mongodb.myBooksCollection);
    const bookCollection = database.collection(config.mongodb.bookCollection);

    await bookCollection.insertOne(bookToReturn); // Vuelve a insertar el libro que fue pedido anteriormente.
    await myBooksCollection.deleteOne(bookToReturn);
    await requestCollection.deleteOne(bookToReturn);
    console.log(`El libro ${bookToReturn.title} fue devuelto exitosamente.`);
    return true; // TODO: Ver que quizá el hecho de tener un boolean nos sirve de algo.
  } catch (error){
    console.error("Error devolviendo libro:", error);
    return false;
  } finally {
    await client.close();
  }
}



async function confirmVerification(successMsg) {
  const client = new MongoClient(mongoUri);
  try {
    await client.connect();
    const database = client.db(config.mongodb.database);
    const collection = database.collection(config.mongodb.confirmVerificationCollection);
    const existingConfirmVerification = await collection.findOne({success: successMsg});

    if(!existingConfirmVerification){ // Si no encuentra ese mensaje en la collection, lo agrega
      await collection.insertOne({ success: successMsg });
    } else{ // si el mensaje ya existe, informa que la verificación ya fue hecha. No inserta más nada.
      console.log("Se ha confirmado tu verificación, bienvenido!");
    }    
  } catch (error) {
    console.error("Error al insertar el documento:", error);
  } finally {
    await client.close();
  }
}


// ------


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
    console.log(`Tarjeta con UUID ${uuid} ingresó correctamente a LibrosExpress. Informa server.js`);
  }

  if (topic.startsWith("library/bookRequests/")) { // TODO: Ver esto
    const bookId = topic.split('/').pop();
    console.log(bookId);
    console.log(`Libro con el id: ${bookId} fue solicitado exitosamente. Informa server.js.`);
  }
});

module.exports = {
  addBookToDB,
  mqttClient,
  mongoUri,
  verifyCard,
  requestBook,
  confirmVerification
};
