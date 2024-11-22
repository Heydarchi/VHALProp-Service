#include "DefaultVehicleHal.h"
#include <hidl/HidlTransportSupport.h>
#include <android-base/logging.h>

using namespace android::hardware::automotive::vehicle::V2_0;

int main() {
    LOG(INFO) << "Starting Vehicle HAL Service...";

    android::sp<IVehicle> service = new DefaultVehicleHal();
    android::hardware::configureRpcThreadpool(4, true);

    if (service->registerAsService() != android::OK) {
        LOG(ERROR) << "Failed to register Vehicle HAL service";
        return 1;
    }

    LOG(INFO) << "Vehicle HAL Service started";
    android::hardware::joinRpcThreadpool();
    return 0;
}
