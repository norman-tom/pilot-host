#ifndef MQTT_H
#define MQTT_H

#include "network/IClientInterface.hpp"
#include "mqtt/async_client.h"
#include "mqtt/delivery_token.h"
#include "mqtt/types.h"
#include <vector>
#include <string>

using namespace std;

namespace nectar::network
{
    class MQTT : public IClientInterface, public mqtt::callback
    {
        public:
            MQTT(shared_ptr<condition_variable> recievedCommand);

        private:
            string borkerIP;
            string protocol;
            string port;
            int qos;
            int retain;
            string clientID;
            string username;
            string password;
            bool isConnected;
            mqtt::async_client *client;

        private:
            void subscribe(string const url);
            
            /**
             * Parses the URL with parameters
             * 
             * @param input A URL string, recieved from the server or broker
             * @param args A command structure
             */
            void parseArgs(string &input, command &args);
        
        public:
            MQTT();
            int connect() override;
            int disconnect() override;
            int transmit(URL const &url, string data) const override;
            int responseCallback(void *data) override;
            command nextCommand() override;
            bool isCommand() override;
            
            /******** MQTT Callbacks ************************/
            void connected(const string& cause) override;
	        void connection_lost(const string& cause) override;
	        void message_arrived(mqtt::const_message_ptr msg) override;
	        void delivery_complete(mqtt::delivery_token_ptr tok) override;
    };
}

#endif