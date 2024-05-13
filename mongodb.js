const mongoose = require('mongoose');

// Función para conectar a MongoDB
async function connectToMongoDB(mongoURI) {
    try {
        await mongoose.connect(mongoURI, { useNewUrlParser: true, useUnifiedTopology: true });
        console.log('Conexión a MongoDB establecida correctamente');
    } catch (error) {
        console.error('Error al conectar a MongoDB:', error);
    }
}
// TENES QUE REALIZAR UN INSERT acá para poder definir que agregue el libro que estoy creando. Código de Fernando
// mongodb-nodejs.
// Exporta la función para conectar a MongoDB
module.exports = {
    connectToMongoDB
};
