var config = {};

config.debug = process.env.DEBUG || true;

config.mqtt  = {};
config.mqtt.namespace = process.env.MQTT_NAMESPACE || '#';
config.mqtt.hostname  = process.env.MQTT_HOSTNAME  || '54.196.204.53'; // public IP
config.mqtt.port      = process.env.MQTT_PORT      || 1883;

config.mongodb = {};
config.mongodb.hostname   = process.env.MONGODB_HOSTNAME   || '172.31.90.101'; // Private IP 
config.mongodb.port       = process.env.MONGODB_PORT       || 27017;
config.mongodb.database   = process.env.MONGODB_DATABASE   || 'library';
config.mongodb.bookCollection = process.env.MONGODB_COLLECTION || 'books';
config.mongodb.usersCollection = process.env.MONGODB_USERS_COLLECTION ||'usersVerification'; 
config.mongodb.usersRegisterCollection = process.env.MONGODB_USERS_REGISTER_COLLECTION || 'registerUsers'; 
config.mongodb.bookRequestCollection = process.env.MONGODB_BOOK_REQUEST_COLLECTION || 'bookRequests'; 
config.mongodb.confirmVerificationCollection = process.env.MONGODB_CONFIRM_VERIFICATION_COLLECTION || 'confirmVerification';
config.mongodb.myBooksCollection = process.env.MONGODB_MY_BOOKS_COLLECTION || 'myBooks';


module.exports = config;    