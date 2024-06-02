var config = {};

config.debug = process.env.DEBUG || true;

config.mqtt  = {};
config.mqtt.namespace = process.env.MQTT_NAMESPACE || '#';
config.mqtt.hostname  = process.env.MQTT_HOSTNAME  || '34.229.22.136'; // Public IP
config.mqtt.port      = process.env.MQTT_PORT      || 1883;

config.mongodb = {};
config.mongodb.hostname   = process.env.MONGODB_HOSTNAME   || '172.31.90.101'; // Private IP 
config.mongodb.port       = process.env.MONGODB_PORT       || 27017;
config.mongodb.database   = process.env.MONGODB_DATABASE   || 'library';
config.mongodb.bookCollection = process.env.MONGODB_COLLECTION || 'books';
config.mongodb.randomNumberCollection = process.env.MONGODB_RANDOM_NUMBER_COLLECTION || 'randomNumbers';



module.exports = config;    