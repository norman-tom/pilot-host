#include "network/MQTT.hpp"
#include "BrewModule.hpp"
#include "Process.hpp"
#include "Controller.hpp"

using namespace std;
using namespace nectar::core;
using namespace nectar::hardware;
using namespace nectar::network;

MQTT::MQTT(shared_ptr<condition_variable> commandNotify)
{
    borkerIP = "192.168.20.9";
    protocol = "tcp://";
    port = ":1883";
    qos = 0;                //fire and forget
    retain = 0;             //use and forget
    clientID = "nectar";
    username = "nectar";
    password = "nectar";
    isConnected = false;

    string url = protocol + borkerIP + port;
    client = new mqtt::async_client(url, clientID);
    client->set_callback(*this);

    MQTT::recievedCommand = commandNotify;
}

int MQTT::connect()
{
    cout << "CONNECTING...." << endl;

    mqtt::binary_ref binPassword(password.c_str());

    try
    {
        mqtt::connect_options myOptions(username, binPassword);
        myOptions.set_clean_session(true);
        myOptions.set_connect_timeout(5);
        mqtt::token_ptr tok = client->connect(myOptions);
        while(!isConnected);
    }
    catch(mqtt::exception &e)
    {
        cout << e.what() << endl;
    }

    return 0;
}

int MQTT::disconnect()
{
    cout << "disconnect from broker" << endl;
    client->disconnect();
    return 0;
}

int MQTT::transmit(URL const &url, string data) const
{
    string topic = url;
    const char *topicArray = topic.c_str();
    const char *valArray = data.c_str();
    unsigned int arraySize = strlen(valArray);

    client->publish(topicArray, valArray, arraySize, qos, retain);
    
    return 0;
}

int MQTT::responseCallback(void *data)
{
    // 1. create an empty command structure and set it to zero
    command cmd;
    memset(&cmd, 0, sizeof(cmd));

    // 2. get the payload which will be the commands
    mqtt::const_message_ptr msgPtr = *((mqtt::const_message_ptr *) data);
    string payLoadStr(msgPtr->get_payload_str());

    // 3. parse the string into a argument structure
    parseArgs(payLoadStr, cmd);

    // 4. lock the command que
    lock_guard<mutex> mLock(cmdLock);

    // 5. Push the command to the back of the que
    commandQueue.push(cmd);

    // 6. Notify the consumer thread that there is a new command and it should wake up
    recievedCommand->notify_one();

    return 0;
}

void MQTT::parseArgs(string &input, command &args)
{
    int pos = -1;
    int cursor = 0;
    char at;
    int const MAX_LENGTH = 64;
    int const NUM_PARTS = 6;
    size_t size = sizeof(char) * MAX_LENGTH * NUM_PARTS;
    char const d0 = '/';
    char const d1 = '?';
    char const d2 = '=';
    string::iterator str = input.begin();

    char* parts = (char *) malloc(size);      //allocate the required memory
    memset(parts, '\0', size);                //set to null terminator

    #define part(i) (parts + (i * MAX_LENGTH))

    //UNSAFE BUFFER OVERFLOW
    //loop the string and place it componets in the repsective bins based on the delimiter
    while(str != input.end())
    {
        switch(*str)
        {
            case d0:
                ++pos;
                cursor = 0;
                break;
            case d1:
                pos = 4;
                cursor = 0;
                break;
            case d2:
                pos = 5;
                cursor = 0;
                break;
            default:
                *(part(pos) + cursor) = *str;
                ++cursor;
                break;

        }
        ++str;
    }

    //create c++ string from the parts
    string const p0 = string(part(0));
    string const p1 = string(part(1));
    string const p2 = string(part(2));
    string const p3 = string(part(3));
    string const p5 = string(part(5));

    //build the url
    string temp;
    if(!p0.empty())
        temp = d0 + p0;
    if(!p1.empty())
        temp += d0 + p1;
    if(!p2.empty())
        temp += d0 + p2;
    if(!p3.empty())
        temp += d0 + p3;

    args.url = temp;
    
    //get the action code
    switch(*part(4))
    {
        case 'r':
            args.action = Controller::CMD_READ;
            break;
        case 'w':
            args.action = Controller::CMD_WRITE;
            break;
        case 'x':
            args.action = Controller::CMD_START;
            break;
        case 's':
            args.action = Controller::CMD_STOP;
            break;
        case 'q':
            args.action = Controller::CMD_EXIT;
            break;
    }

    //get the value if it is not empty
    if(p5.empty())
    {
        args.value = 0;
    }
    else
    {
        args.value = stod(p5);
    }

    //free the memory
    free(parts);
}

command MQTT::nextCommand()
{
    lock_guard<mutex> mLock(cmdLock);
    command cmd = commandQueue.front();
    commandQueue.pop();
    
    return cmd;
}

bool MQTT::isCommand()
{
    lock_guard<mutex> mLock(cmdLock);
    return !commandQueue.empty();
}

void MQTT::subscribe(string const url)
{
    //string topic("/" + origin + "/" + module + "/" + node);

    /***************** CLASS FOR SUBSCRIPTION CALLBACK ****************/
    class SubListner : public mqtt::iaction_listener
    {
        void on_failure(const mqtt::token& asyncActionToken) override 
        {
            cout << "subscription failed" << endl;
        }

	    void on_success(const mqtt::token& asyncActionToken) override
        {
            cout << "subscription successful" << endl;
        }
    };

    SubListner *subCB = new SubListner;

    client->subscribe(url, qos, NULL, *subCB);
}

/********** MQTT ASYNC CLIENT INTERFACE METHODS *******************************/

void MQTT::connected(const string &cause)
{
    cout << "Connected to: " + protocol + borkerIP + port << endl;
    subscribe("/client/request");
    isConnected = true;
}

void MQTT::connection_lost(const string &cause)
{

}

void MQTT::message_arrived(mqtt::const_message_ptr msgPtr)
{
    MQTT::responseCallback(&msgPtr);
}

void MQTT::delivery_complete(mqtt::delivery_token_ptr tok)
{

}