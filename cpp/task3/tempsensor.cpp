#include <iostream>
#include <fstream>
#include <memory>
#include <unistd.h>
#include <ctime>

/* ================= CONFIG ================= */
#define SAMPLE_TIME_SEC 3
#define LOG_FILE "log.txt"

/* ================= DATA ================= */
struct TemperatureData {
    float value;
};

/* ================= SENSOR ================= */
class LMSensor {
public:
    LMSensor() : humidity(40.0f) {}

    float readHumidity() 
    {
        humidity = rand()%60 + 20.0f; 
        return humidity;
    }

private:
    float humidity;
};

/* ================= UTILITY ================= */
std::string getTimeStamp() {
    time_t now = time(nullptr);
    return ctime(&now); 
}

/* ================= DISPLAY ================= */
void displayTemperature(const std::shared_ptr<TemperatureData>& data) {
    std::cout << " Humidity = "<< data->value<< " | shared_ptr count = "<< data.use_count()<< std::endl;
}

/* ================= LOGGER ================= */
void logTemperature(const std::shared_ptr<TemperatureData>& data)
{
    std::ofstream file(LOG_FILE, std::ios::app);
    file << getTimeStamp()<< "Humidity = "<< data->value<< std::endl;
}

/* ================= MAIN ================= */
int main() {

    
    std::unique_ptr<LMSensor> sensor = std::make_unique<LMSensor>();

    
    std::shared_ptr<TemperatureData> data = std::make_shared<TemperatureData>();

    std::cout << "System started...\n";

    while (true) {
        float humidity = sensor->readHumidity();
        data->value = humidity;
        displayTemperature(data);
        logTemperature(data);
        sleep(SAMPLE_TIME_SEC);
    }

    return 0;
}
