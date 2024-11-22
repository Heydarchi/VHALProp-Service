#ifndef DEFAULT_VEHICLE_HAL_H
#define DEFAULT_VEHICLE_HAL_H

#include <android/hardware/automotive/vehicle/2.0/IVehicle.h>
#include <android/hardware/automotive/vehicle/2.0/IVehicleCallback.h>
#include <android/hidl/manager/1.0/IServiceManager.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <thread>
#include <unordered_map>
#include <vector>

using android::hardware::automotive::vehicle::V2_0::IVehicle;
using android::hardware::automotive::vehicle::V2_0::IVehicleCallback;
using android::hardware::automotive::vehicle::V2_0::VehiclePropValue;
using android::hardware::automotive::vehicle::V2_0::VehiclePropConfig;
using android::hardware::automotive::vehicle::V2_0::StatusCode;

class DefaultVehicleHal : public IVehicle {
public:
    DefaultVehicleHal();
    virtual ~DefaultVehicleHal();

    // IVehicle interface methods
    android::hardware::Return<void> getAllPropConfigs(getAllPropConfigs_cb _hidl_cb) override;
    android::hardware::Return<void> getPropConfigs(const android::hardware::hidl_vec<int32_t>& props,
                                                   getPropConfigs_cb _hidl_cb) override;
    android::hardware::Return<StatusCode> set(const VehiclePropValue& value) override;
    android::hardware::Return<void> get(const VehiclePropValue& requestedProp,
                                        get_cb _hidl_cb) override;
    android::hardware::Return<StatusCode> subscribe(
        const android::sp<IVehicleCallback>& callback,
        const android::hardware::hidl_vec<android::hardware::automotive::vehicle::V2_0::SubscribeOptions>& options) override;
    android::hardware::Return<StatusCode> unsubscribe(const android::sp<IVehicleCallback>& callback,
                                                      int32_t propId) override;
    android::hardware::Return<void> debugDump(debugDump_cb _hidl_cb) override;

private:
    std::unordered_map<int32_t, float> mProperties;  // Stores property values.
    void simulateVehicleSpeed();                     // Simulates vehicle speed property.

    std::thread mSimulationThread;
    bool mRunning;

    std::vector<android::sp<IVehicleCallback>> mCallbacks;  // Stores subscribed callbacks.
};

#endif // DEFAULT_VEHICLE_HAL_H
