#include <stdio.h>
#include <sensors/sensors.h>
#include <string.h>

int main(void) {
    if (sensors_init(NULL) != 0) {
        fprintf(stderr, "Failed to initialize libsensors\n");
        return 1;
    }

    const sensors_chip_name *chip;
    int chip_nr = 0;

    while ((chip = sensors_get_detected_chips(NULL, &chip_nr)) != NULL) {
        printf("%s\n", chip->prefix);
        printf("Adapter: %s\n", sensors_get_adapter_name(&chip->bus));

        const sensors_feature *feat;
        int feat_nr = 0;

        while ((feat = sensors_get_features(chip, &feat_nr)) != NULL) {
            const char *label = sensors_get_label(chip, feat);
            printf("%s:\n", label);

            const sensors_subfeature *sub;
            int sub_nr = 0;

            while ((sub = sensors_get_all_subfeatures(chip, feat, &sub_nr)) != NULL) {
                if (!(sub->flags & SENSORS_MODE_R)) continue;

                double value;
                if (sensors_get_value(chip, sub->number, &value) != 0)
                    continue;

                const char *type_label = NULL;
                const char *unit = "";

                switch (sub->type) {
                    case SENSORS_SUBFEATURE_TEMP_INPUT:       type_label = "Value"; unit = "°C"; break;
                    case SENSORS_SUBFEATURE_TEMP_CRIT:        type_label = "Crit"; unit = "°C"; break;
                    case SENSORS_SUBFEATURE_TEMP_EMERGENCY:   type_label = "Emerg"; unit = "°C"; break;
                    case SENSORS_SUBFEATURE_TEMP_MAX:         type_label = "Max"; unit = "°C"; break;
                    case SENSORS_SUBFEATURE_TEMP_MIN:         type_label = "Min"; unit = "°C"; break;

                    case SENSORS_SUBFEATURE_FAN_INPUT:        type_label = "Fan"; unit = "RPM"; break;
                    case SENSORS_SUBFEATURE_IN_INPUT:         type_label = "Voltage"; unit = "V"; break;
                    case SENSORS_SUBFEATURE_POWER_INPUT:      type_label = "Power"; unit = "W"; break;

                    default: continue; // skip uninteresting types
                }

                printf("  %-8s %6.2f %s\n", type_label, value, unit);
            }
        }
        printf("\n");
    }

    sensors_cleanup();
    return 0;
}
