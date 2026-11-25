cmake_minimum_required(VERSION 3.16)
project(multipad_firmware_tests C)
enable_testing()

add_executable(pet_care_power_loss pet_care_power_loss.c ../USER/Lcd/pet_care.c)
target_compile_features(pet_care_power_loss PRIVATE c_std_11)
target_include_directories(pet_care_power_loss PRIVATE host ../USER/Lcd ../HARDWARE/FLASH)
add_test(NAME pet_care_power_loss COMMAND pet_care_power_loss)

add_executable(pet_encoder_gestures pet_encoder_gestures.c ../USER/Lcd/pet_mode.c
    ../USER/Lcd/pet_care.c ../USER/Encoder/encoder_user.c ../HARDWARE/ENCODER/ENCODER3/encoder3.c)
target_compile_features(pet_encoder_gestures PRIVATE c_std_11)
target_include_directories(pet_encoder_gestures PRIVATE host ../USER/Lcd ../USER/Usb
    ../USER/Encoder ../USER/Oled ../HARDWARE/FLASH ../HARDWARE/LCD/LCD_47 ../HARDWARE/OLED/OLED_42
    ../HARDWARE/ENCODER/ENCODER1 ../HARDWARE/ENCODER/ENCODER2 ../HARDWARE/ENCODER/ENCODER3 ../HARDWARE/TIM/TIM4)
add_test(NAME pet_encoder_gestures COMMAND pet_encoder_gestures)
