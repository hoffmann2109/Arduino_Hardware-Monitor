#include <stdio.h>
#include <string.h>
#include <sensors/sensors.h>

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
            printf("%s:\n", sensors_get_label(chip, feat));

            const sensors_subfeature *sub;
            int sub_nr = 0;
            while ((sub = sensors_get_all_subfeatures(chip, feat, &sub_nr)) != NULL) {
                double v;
                if (sensors_get_value(chip, sub->number, &v) != 0)
                    continue;

                // strip trailing "_input" for display
                char disp[64];
                strncpy(disp, sub->name, sizeof(disp));
                disp[sizeof(disp)-1] = '\0';   // always null‑terminate

                char *p = strrchr(disp, '_');  // find last underscore
                if (p)
                    *p = '\0';                 // chop it off

                printf("  %-10s", disp);


                printf("  %-10s", disp);
                switch (sub->type) {
                    case SENSORS_SUBFEATURE_TEMP_INPUT:
                        printf(" %+0.1f°C\n", v);          break;
                    case SENSORS_SUBFEATURE_FAN_INPUT:
                        printf(" %.0f RPM\n", v);         break;
                    case SENSORS_SUBFEATURE_IN_INPUT:
                        printf(" %.2f V\n", v / 1000.0);  break;
                    case SENSORS_SUBFEATURE_POWER_INPUT:
                        printf(" %.2f W\n", v / 1000.0);  break;
                    default:
                        printf(" %f\n", v);               break;
                }
            }
        }
    }

    sensors_cleanup();
    return 0;
}
