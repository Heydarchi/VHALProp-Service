#include "DefaultVehicleHal.h"
#include <android-base/logging.h>
#include <android/hardware/automotive/vehicle/2.0/types.h>
#include <chrono>
#include <thread>

using namespace android::hardware::automotive::vehicle::V2_0;

DefaultVehicleHal::DefaultVehicleHal() : mRunning(true) {
    mSimulationThread = std::thread(&DefaultVehicleHal::simulateVehicleSpeed, this);
}

DefaultVehicleHal::~DefaultVehicleHal() {
    mRunning = false;
    if (mSimulationThread.joinable()) {
        mSimulationThread.join();
    }
}

android::hardware::Return<void> DefaultVehicleHal::getAllPropConfigs(getAllPropConfigs_cb _hidl_cb) {
    std::vector<VehiclePropConfig> configs;

    VehiclePropConfig speedConfig = {
        .prop = static_cast<int32_t>(VehicleProperty::PERF_VEHICLE_SPEED),
        .access = VehiclePropertyAccess::READ_WRITE,
        .changeMode = VehiclePropertyChangeMode::CONTINUOUS,
        .configArray = {0, 200}, // Speed range: 0 to 200 km/h.
        .minSampleRate = 0.1f,   // 10Hz.
        .maxSampleRate = 1.0f    // 1Hz.
    };
    configs.push_back(speedConfig);

    _hidl_cb(configs);
    return android::hardware::Void();
}

android::hardware::Return<void> DefaultVehicleHal::getPropConfigs(
    const android::hardware::hidl_vec<int32_t>& props, getPropConfigs_cb _hidl_cb) {
    std::vector<VehiclePropConfig> configs;

    for (int32_t prop : props) {
        if (prop == static_cast<int32_t>(VehicleProperty::PERF_VEHICLE_SPEED)) {
            VehiclePropConfig speedConfig = {
                .prop = prop,
                .access = VehiclePropertyAccess::READ_WRITE,
                .changeMode = VehiclePropertyChangeMode::CONTINUOUS,
                .configArray = {0, 200}, // Speed range: 0 to 200 km/h.
                .minSampleRate = 0.1f,   // 10Hz.
                .maxSampleRate = 1.0f    // 1Hz.
            };
            configs.push_back(speedConfig);
        }
    }

    // Use the callback with a status code and the list of configs.
    if (!configs.empty()) {
        _hidl_cb(StatusCode::OK, configs);
    } else {
        _hidl_cb(StatusCode::INVALID_ARG, configs);
    }

    return android::hardware::Void();
}


android::hardware::Return<StatusCode> DefaultVehicleHal::set(const VehiclePropValue& value) {
    mProperties[value.prop] = value.value.floatValues[0];
    return StatusCode::OK;
}

android::hardware::Return<void> DefaultVehicleHal::get(const VehiclePropValue& requestedProp,
                                                       get_cb _hidl_cb) {
    auto it = mProperties.find(requestedProp.prop);
    if (it != mProperties.end()) {
        VehiclePropValue response = requestedProp;
        response.value.floatValues[0] = it->second;
        _hidl_cb(StatusCode::OK, response);
    } else {
        _hidl_cb(StatusCode::INVALID_ARG, {});
    }
    return android::hardware::Void();
}

android::hardware::Return<StatusCode> DefaultVehicleHal::subscribe(
    const android::sp<IVehicleCallback>& callback,
    const android::hardware::hidl_vec<SubscribeOptions>& options) {
    for (const auto& option : options) {
        if (option.propId == static_cast<int32_t>(VehicleProperty::PERF_VEHICLE_SPEED)) {
            mCallbacks.push_back(callback);
            return StatusCode::OK;
        }
    }
    return StatusCode::INVALID_ARG;
}

android::hardware::Return<StatusCode> DefaultVehicleHal::unsubscribe(
    const android::sp<IVehicleCallback>& callback, int32_t propId) {
    mCallbacks.erase(std::remove(mCallbacks.begin(), mCallbacks.end(), callback), mCallbacks.end());
    return StatusCode::OK;
}

android::hardware::Return<void> DefaultVehicleHal::debugDump(debugDump_cb _hidl_cb) {
    _hidl_cb("DefaultVehicleHal: Debug dump not implemented yet.");
    return android::hardware::Void();
}

void DefaultVehicleHal::simulateVehicleSpeed() {
    float speed = 0.0f;

    while (mRunning) {
        mProperties[static_cast<int32_t>(VehicleProperty::PERF_VEHICLE_SPEED)] = speed;

        // Notify callbacks if any
        for (const auto& callback : mCallbacks) {
            VehiclePropValue value = {};
            value.prop = static_cast<int32_t>(VehicleProperty::PERF_VEHICLE_SPEED);
            value.value.floatValues.resize(1);
            value.value.floatValues[0] = speed;

            callback->onPropertyEvent({value});
        }

        speed += 5.0f;  // Increase speed by 5km/h every second.
        if (speed > 200.0f) {
            speed = 0.0f;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
