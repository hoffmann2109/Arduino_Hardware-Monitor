#include <stdio.h>
#include <sensors/sensors.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int main(void) {
    // 1) init libsensors once
    if (sensors_init(NULL) != 0) {
        fprintf(stderr, "Failed to initialize libsensors\n");
        return 1;
    }

    // 2) open & configure serial port once
    int serial = open("/dev/ttyACM0", O_WRONLY | O_NOCTTY);
    if (serial < 0) { perror("open"); return 1; }

    struct termios tty;
    memset(&tty, 0, sizeof tty);
    tcgetattr(serial, &tty);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag  = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);
    tcsetattr(serial, TCSANOW, &tty);

    // 3) infinite loop
    while (1) {
        const sensors_chip_name *chip;
        int chip_nr = 0;

        // scan through all sensors
        while ((chip = sensors_get_detected_chips(NULL, &chip_nr)) != NULL) {
            const sensors_feature *feat;
            int feat_nr = 0;

            while ((feat = sensors_get_features(chip, &feat_nr)) != NULL) {
                const sensors_subfeature *sub;
                int sub_nr = 0;

                while ((sub = sensors_get_all_subfeatures(chip, feat, &sub_nr)) != NULL) {
                    if (!(sub->flags & SENSORS_MODE_R)) continue;

                    double value;
                    if (sensors_get_value(chip, sub->number, &value) != 0)
                        continue;

                    // for example only send CPU temp subfeatures:
                    if (sub->type == SENSORS_SUBFEATURE_TEMP_INPUT) {
                        char buf[64];
                        int len = snprintf(buf, sizeof buf,
                                           "%s: %.2f\n",
                                           sensors_get_label(chip, feat),
                                           value);
                        write(serial, buf, len);
                    }
                }
            }
        }

        sleep(1);
    }

    // never reached
    close(serial);
    sensors_cleanup();
    return 0;
}
