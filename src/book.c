#include "io.h"

void tempConversion() {
	int fahr, celsius;
	int lower = 0, upper = 100, step = 20;
	fahr = lower;
	while (fahr <= upper) {
		celsius = 5 * (fahr - 32) / 9;
		printp("Fahrenheit: %d\t\tCelsius: %d\n", fahr, celsius);
		fahr += step;
	}
}
