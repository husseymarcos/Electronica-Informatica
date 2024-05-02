const mqtt = require('mqtt');

// Función para conectar al servidor MQTT
function connectToMQTT(mqttURL) {
    const client = mqtt.connect(mqttURL);

    client.on('connect', () => {
        console.log('Conectado al servidor MQTT');
        // Suscribirse a los temas necesarios aquí
        client.subscribe('topic/ejemplo');
    });

    client.on('message', (topic, message) => {
        console.log('Mensaje recibido:', message.toString(), 'en el topic', topic);
        // Agregar lógica para manejar los mensajes recibidos aquí
    });

    // Exporta el cliente MQTT para poder usarlo en otros archivos si es necesario
    return client;
}

// Exporta la función para conectarse al servidor MQTT
module.exports = connectToMQTT;
