#include <stdio.h>
#include <sensors/sensors.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main(void) {
    // Init libsensors once
    if (sensors_init(NULL) != 0) {
        fprintf(stderr, "Failed to initialize libsensors\n");
        return 1;
    }

    // Open serial port once (on Linux serial ports show up like files)
    int serial = open("/dev/ttyACM0", O_WRONLY | O_NOCTTY);

    if (serial < 0) {
        perror("open");
        return 1;
    }

    // Configure serial port once using the termios API
    struct termios tty;
    memset(&tty, 0, sizeof tty); // set every byte to 0 (&tty means the address of tty)
    tcgetattr(serial, &tty);
    // 115200 baud rate (how many bits are transmitted per second)
    // 8 data bits, no parity, 1 stop bit ("8N1" setting):
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag  = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);
    tcsetattr(serial, TCSANOW, &tty); //apply settings

    // Infinite loop:
    while (1) {
        const sensors_chip_name *chip; // *chip is a pointer to the sensors_chip_name struct
        int chip_nr = 0;

        // For every Chip (Hardware chip - CPU, NVME, etc.):
        while ((chip = sensors_get_detected_chips(NULL, &chip_nr)) != NULL) {
            const sensors_feature *feat;
            int feat_nr = 0;

            // For every Sensor (Temperature, Fan, Usage, etc.):
            while ((feat = sensors_get_features(chip, &feat_nr)) != NULL) {
                const sensors_subfeature *sub;
                int sub_nr = 0;

                // For every Subfeature (Temp_INPUT, Temp_MAX, etc.) :
                while ((sub = sensors_get_all_subfeatures(chip, feat, &sub_nr)) != NULL) {
                    if (!(sub->flags & SENSORS_MODE_R)) continue; // if not readable skip

                    double value;
                    if (sensors_get_value(chip, sub->number, &value) != 0) // if value is 0, skip
                        continue;

                    // CPU subfeatures:
                    if (sub->type == SENSORS_SUBFEATURE_TEMP_INPUT) { // '->' access a struct through a pointer
                        char buf[64];

                        // Format and store into buffer:
                        int len = snprintf(buf, sizeof buf,
                                           "%s: %.2f\n",
                                           sensors_get_label(chip, feat),
                                           value);
                        printf("%s: %.2f %c \n", sensors_get_label(chip, feat), value, 'C');
                        // Composite: Nvme drive overall
                        // Sensor 1: Nvme individual
                        // temp1: ACPI interface
                        // Package id 0: Overall CPU temperature
                        // Cores: individual CPU core temperature
                        write(serial, buf, len);
                    }
                }
            }
        }
        sleep(1);
    }
    // Not reached because of the infinite loop:
    close(serial);
    sensors_cleanup();
    return 0;
}
