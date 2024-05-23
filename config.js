var config = {};

config.debug = process.env.DEBUG || true;

config.mqtt  = {};
config.mqtt.namespace = process.env.MQTT_NAMESPACE || '#';
config.mqtt.hostname  = process.env.MQTT_HOSTNAME  || '18.232.155.48'; // Public IP
config.mqtt.port      = process.env.MQTT_PORT      || 1883;

config.mongodb = {};
config.mongodb.hostname   = process.env.MONGODB_HOSTNAME   || '172.31.90.101'; // Private IP 
config.mongodb.port       = process.env.MONGODB_PORT       || 27017;
config.mongodb.database   = process.env.MONGODB_DATABASE   || 'test';
config.mongodb.collection = process.env.MONGODB_COLLECTION || 'message';

module.exports = config;