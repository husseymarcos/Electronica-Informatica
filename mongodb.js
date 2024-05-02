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

// Exporta la función para conectar a MongoDB
module.exports = {
    connectToMongoDB
};
