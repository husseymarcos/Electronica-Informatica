const mqtt = require('mqtt');

let mqttClient;

// Función para conectar al servidor MQTT
function connectToMQTT(mqttURL) {
    mqttClient = mqtt.connect(mqttURL);

    mqttClient.on('connect', () => {
        console.log('Conectado al servidor MQTT');
        // Suscribirse a los temas necesarios aquí
        mqttClient.subscribe('topic/ejemplo', (err) => {
            if (err) {
              console.error('Error al suscribirse:', err);
            } else {
              console.log('Suscripción exitosa a topic/ejemplo');
            }
        });
    
    });

    mqttClient.on('message', (topic, message) => {
        console.log('Mensaje recibido:', message.toString(), 'en el topic', topic);
        // Agregar lógica para manejar los mensajes recibidos aquí
    });

    // Exporta el cliente MQTT para poder usarlo en otros archivos si es necesario
    return mqttClient;
}

// Función para publicar un mensaje en un tema MQTT
function publishToMQTT(topic, message) {
    if (mqttClient) {
        mqttClient.publish(topic, message);
    } else {
        console.error('Error: Cliente MQTT no conectado');
    }
}

// Exporta las funciones para conectarse al servidor MQTT y publicar mensajes
module.exports = { connectToMQTT, publishToMQTT };
