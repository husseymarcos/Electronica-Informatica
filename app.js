const mqtt = require('mqtt');

// Conexión al servidor MQTT
const client = mqtt.connect('mqtt://52.23.254.208'); // A TENER EN CUENTA! - Este link se renueva cada vez que volves a iniciar el Server desde AWS.

// Manejo de eventos de conexión
client.on('connect', () => {
  console.log('Conectado al servidor MQTT');
  
  // Suscripción a un topic
  client.subscribe('topic/ejemplo', (err) => {
    if (err) {
      console.error('Error al suscribirse:', err);
    } else {
      console.log('Suscripción exitosa a topic/ejemplo');
    }
  });

  // Publicación de un mensaje
  client.publish('topic/ejemplo', 'Hola desde Node.js');
});

// Manejo de mensajes recibidos
client.on('message', (topic, message) => {
  console.log('Mensaje recibido:', message.toString(), 'en el topic', topic);
});
