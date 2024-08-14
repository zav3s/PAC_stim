
## Uploading Firmware to ESP32

This project requires the ESP32 Arduino Core version 3.0.4 for proper functionality. Follow these steps to set up your environment and upload the firmware:

### Prerequisites

1. Arduino IDE (version 1.8.x or later)
2. ESP32 board support for Arduino IDE
3. ESP32 Arduino Core version 3.0.4

### Setting Up ESP32 Board Support

1. Open Arduino IDE
2. Go to File > Preferences
3. In "Additional Board Manager URLs", add: https://dl.espressif.com/dl/package_esp32_index.json
4. Click "OK" to save the preferences

### Installing ESP32 Arduino Core 3.0.4

1. Go to Tools > Board > Boards Manager
2. Search for "esp32"
3. Find "ESP32 by Espressif Systems"
4. Click on the version dropdown and select "3.0.4"
5. Click "Install"

### Uploading the Firmware

1. Connect your ESP32 to your computer via USB
2. Open the project's .ino file in Arduino IDE
3. Select your board: Tools > Board > ESP32 Arduino > [Your ESP32 Board]
4. Select the correct port: Tools > Port > [Your ESP32 Port]
5. Click the Upload button (arrow icon) or go to Sketch > Upload

### Troubleshooting

- If you encounter upload errors, try pressing and holding the BOOT button on your ESP32 board while initiating the upload
- Ensure you're using a data-capable USB cable, not a charge-only cable
- Some ESP32 boards may require you to press the EN (Enable) button after initiating the upload

### Verifying the Upload

After successful upload, you can verify the firmware is working by:
1. Opening the Serial Monitor (Tools > Serial Monitor)
2. Setting the baud rate to 115200
3. Checking for any startup messages or expected output

Note: Always ensure you're using the specified Arduino Core version (3.0.4) to avoid compatibility issues with the timer functions used in this project.

## Using the Stimulation Device

After successfully uploading the firmware to your ESP32, follow these steps to use the stimulation device:

### Preparation and Usage

1. Insert Batteries:
   - Locate the battery holder on the device.
   - Insert the appropriate batteries into the holder, ensuring correct polarity.

2. Turn On the Device:
   - Locate both ON/OFF switches on the device.
   - Turn both switches to the ON position.

3. Stimulation Session:
   - The device is now ready for use.
   - Conduct the stimulation session for approximately 20 minutes.
   - Monitor the subject and be prepared to use the emergency stop button (just turn all swithes off)

### After Stimulation

1. Turn Off the Device:
   - After completing the stimulation session, turn both ON/OFF switches to the OFF position.

2. Battery Maintenance:
   - Remove the batteries from the holder.
   - Place the batteries in the charger to recharge them for the next session.

### Safety Reminders

- Always ensure the device is turned off when not in use.
- Regularly check the battery levels to ensure optimal performance.
- Clean and maintain the electrodes according to proper hygiene protocols.
- If you encounter any unusual behavior from the device, discontinue use and check the firmware and hardware connections.





# Theta-Gamma Phase-Amplitude Coupling Stimulation

This project implements a transcranial electrical stimulation device using an ESP32 microcontroller. It generates a combined 6Hz (theta) and 84Hz (gamma) waveform for phase-amplitude coupled stimulation. Use theta_gamma_pac.ino firmware for this mode.



## Adjusting Stimulation Parameters

### Changing Frequencies

To modify the stimulation frequencies:

1. Lower frequency (theta):
   const double frequency_base = 6; // Change 6 to desired frequency

2. Upper frequency (gamma):
   const double frequency = 84; // Change 84 to desired frequency


### Frequency Limits

- Practical upper limit: 200-300 Hz
- Practical lower limit: 0.1-1 Hz

### Adjusting Sampling Rate

Current sampling rate: ~1953 Hz (1 / 0.000512)

Guidelines:
- Up to 195 Hz: Current rate sufficient
- 195-488 Hz: Usable, decreasing quality
- Above 488 Hz: Consider increasing sampling rate

To modify sampling rate:

const int samples_per_cycle = int(round(1/(float(frequency) * .000512)));
// Change .000512 to adjust sampling rate

### Setting Maximum Current (4mA peak-to-peak)

1. Change amplitude:
   double amplitude = 1.0; // Changed from 0.5

2. Adjust safety limits:
   if(offset > 4.002) offset = 4.001;
   if(offset < -4.002) offset = -4.001;
   if(amplitude > 4.002) amplitude = 4.001;

3. Modify DAC value calculation:
   wave_list_dacwrite[x] = (int(round(((16383*1.0866)/5)*(2.5-value_in_mA/2))));

4. Update value checks:
   if(value_in_mA > 4.002) value_in_mA = 4.001;
   if(value_in_mA < -4.002) value_in_mA = -4.001;


## Examples
1. Find this line in the code:
   double amplitude = 1.0; // Set to 1.0 for 4mA peak-to-peak

2. Change the value to adjust the power:
   - For 2mA peak-to-peak: double amplitude = 0.5;
   - For 3mA peak-to-peak: double amplitude = 0.75;
   - For 4mA peak-to-peak: double amplitude = 1.0;


3. Ensure the safety limits are appropriate:
   if(amplitude > 4.002) amplitude = 4.001;

4. The DAC value calculation should already be set for the full range:
   wave_list_dacwrite[x] = (int(round(((16383*1.0866)/5)*(2.5-value_in_mA/2))));

Remember:
- The relationship between amplitude and current is linear in this setup.
- Always verify the actual output current with appropriate measurement equipment.
- Ensure you're operating within safe limits for transcranial stimulation.
- Test the output thoroughly after making any changes.

## Safety Warnings

- Verify safety guidelines for transcranial electrical stimulation
- Ensure hardware compatibility with increased current
- Calibrate and verify actual output current
- Test gradually, increasing from lower currents to 4mA

Always verify actual output and adhere to safety standards.


# Fixed Frequency Stimulation Mode

This mode generates a single frequency waveform for transcranial electrical stimulation.Use 10hzmode.ino firmware

## Adjusting Stimulation Parameters

### Changing Frequency

To modify the stimulation frequency:

1. Locate this line in the code:
   const double frequency = 10; // 10 Hz stimulation

2. Change the value (10) to your desired frequency. For example:
   const double frequency = 20; // 20 Hz stimulation

Note: The practical frequency range is approximately 0.1 Hz to 200 Hz.

### Changing Stimulation Power

To adjust the power of stimulation:

1. Find this line in the code:
   double amplitude = 1.0; // Set to 1.0 for 4mA peak-to-peak

2. Modify the amplitude value:
   - For 2mA peak-to-peak: double amplitude = 0.5;
   - For 3mA peak-to-peak: double amplitude = 0.75;
   - For 4mA peak-to-peak: double amplitude = 1.0;

3. Ensure the safety limits are appropriate:
   if(amplitude > 4.002) amplitude = 4.001;

## Important Considerations

- Ensure you're operating within safe limits for transcranial stimulation.
- Test the output thoroughly after making any changes.
- The relationship between amplitude and current is linear in this setup.
- Higher frequencies may require adjustments to the sampling rate for optimal waveform quality.

## Safety Warning

Modifying stimulation parameters can potentially cause harm if not done correctly. Always prioritize safety and consult relevant guidelines and regulations for transcranial electrical stimulation.
