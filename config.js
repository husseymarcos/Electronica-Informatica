var config = {};

config.debug = process.env.DEBUG || true;

config.mqtt  = {};
config.mqtt.namespace = process.env.MQTT_NAMESPACE || '#';
config.mqtt.hostname  = process.env.MQTT_HOSTNAME  || '54.173.227.140'; // Public IP
config.mqtt.port      = process.env.MQTT_PORT      || 1883;

config.mongodb = {};
config.mongodb.hostname   = process.env.MONGODB_HOSTNAME   || '172.31.90.101'; // Private IP 
config.mongodb.port       = process.env.MONGODB_PORT       || 27017;
config.mongodb.database   = process.env.MONGODB_DATABASE   || 'library';
config.mongodb.bookCollection = process.env.MONGODB_COLLECTION || 'books';
config.mongodb.usersCollection = process.env.MONGODB_USERS_COLLECTION ||'usersVerification'; 
config.mongodb.usersRegisterCollection = process.env.MONGODB_USERS_REGISTER_COLLECTION || 'registerUsers'; 



module.exports = config;    