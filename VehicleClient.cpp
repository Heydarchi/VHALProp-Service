#include <aidl/android/hardware/automotive/vehicle/IVehicle.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <iostream>
#include <memory>

using aidl::android::hardware::automotive::vehicle::IVehicle;
using aidl::android::hardware::automotive::vehicle::VehiclePropConfig;
using aidl::android::hardware::automotive::vehicle::VehiclePropValue;

class VehicleClient {
public:
    VehicleClient() {
        std::shared_ptr<IVehicle> vehicleService = IVehicle::fromBinder(
            ndk::SpAIBinder(AServiceManager_getService("android.hardware.automotive.vehicle.IVehicle")));

        if (!vehicleService) {
            std::cerr << "Failed to connect to IVehicle service" << std::endl;
            return;
        }
        mVehicleService = vehicleService;
    }

    void subscribeToProperty(int32_t propertyId) {
        if (!mVehicleService) {
            std::cerr << "Vehicle service is not initialized" << std::endl;
            return;
        }

        VehiclePropValue request;
        request.prop = propertyId;
        request.areaId = 0;  // 0 means global property
        request.value.int32Values = {0}; // Subscription request type
        
        auto status = mVehicleService->subscribe(request);
        if (!status.isOk()) {
            std::cerr << "Failed to subscribe to property: " << propertyId << std::endl;
        } else {
            std::cout << "Subscribed to property: " << propertyId << std::endl;
        }
    }

    void processEvents() {
        // Here you would add code to receive and process events from VHAL
        // Typically done using a callback or polling approach
    }

private:
    std::shared_ptr<IVehicle> mVehicleService;
};

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);
    ABinderProcess_startThreadPool();

    VehicleClient client;
    client.subscribeToProperty(0x00000100); // Example property ID, e.g., VEHICLE_PROPERTY_CURRENT_GEAR

    client.processEvents();

    return 0;
}
