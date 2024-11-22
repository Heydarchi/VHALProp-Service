# VHALProp-Service
This project build a HAL service which make the selected VHAL properties available in Automotive Android

## How to add it to AOSP

Find the .mk file of the variant you're willing to add the service and add the below line:

``
PRODUCT_PACKAGES += \
	vehicle_service  \
``

