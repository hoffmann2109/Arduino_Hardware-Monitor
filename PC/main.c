#include <stdio.h>
// To use libsensors, include its header file:
#include <sensors/sensors.h>

int main(void) {
    // Initialize the libsensors library using the sensors_init function:
    if (sensors_init(NULL) != 0) {
        fprintf(stderr, "Failed to initialize libsensors\n");
        return 1;
    }

    // Use sensors_get_detected_chips to retrieve the list of available chips:
    const sensors_chip_name *chip;
    int chip_number = 0;

    while ((chip = sensors_get_detected_chips(NULL, &chip_number)) != NULL) {
        printf("Chip: %s\n", sensors_get_adapter_name(&chip->bus));

        // Use sensors_get_features to iterate through all features of a chip:
        const sensors_feature *feature;
        int feature_number = 0;

        while ((feature = sensors_get_features(chip, &feature_number)) != NULL) {
            printf(" Feature: %s\n", sensors_get_label(chip, feature));

            // To obtain sensor readings, use sensors_get_value:
            const sensors_subfeature *subfeature;
            int subfeature_number = 0;

            while ((subfeature = sensors_get_all_subfeatures(
                        chip,
                        feature,
                        &subfeature_number)) != NULL) {
                if (subfeature->flags & SENSORS_MODE_R) {
                    double value;
                    if (sensors_get_value(chip,
                                          subfeature->number,
                                          &value) == 0) {
                        printf("  Subfeature %-2d: %.2f\n",
                               subfeature->number, value);
                    }
                }
            }
        }
    }

    sensors_cleanup();
    return 0;
}
