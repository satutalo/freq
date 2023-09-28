#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

const double A3 = 440.0;

double semitone(double freq1, double freq2) {
	return 12.0 * log2(freq1 / freq2);
}

double transpose(double freq, double semitones) {
	return freq * pow(2.0, semitones / 12.0);
}

int main(int argc, char* argv[]) {

	if (argc > 1) {
		char* argChar = argv[1];

		// If the first argument starts with a number or a period, we're converting frequency into note
		// Returns note name with accidental, octave, finetune, and the actual frequency for the in-tune note
		// 
		// Note syntax returned is one of the following:
		//    A-4  (A natural, octave 4)
		//    A--4 (A natural, octave -4)
		//    A#2  (A sharp, octave 2)
		//    A#-2 (A sharp, octave -2)
		// Finetune given in cents in the range +/-50, 100 being a semitone
		// Note name is chosen from the 12-tone equal temperament (A 440) whichever is closest to the given frequency

		if ((*argChar >= 48 && *argChar <= 57) || *argChar == 46) {
			const char notenames[13] = "AABCCDDEFFGG";
			const char sharps[13]    = "-#--#-#--#-#";

			double freq = stod(argv[1]);
			long long semi = semitone(transpose(freq, 0.5), A3) + 1200;
			long long octave = (semi - 3) / 12 - 96;
			double finetune = (semitone(freq, A3) - double(semi) + 1200.0) * 100;
			double freq_in_tune = transpose(A3, double(semi - 1200));

			string notename;
			notename += notenames[semi % 12];
			notename += sharps[semi % 12];
			notename += to_string(octave);

			cout << freq << " Hz: " << notename << " "
			     << fixed << setprecision(1) << (finetune >= 0 ? "+" : "") << finetune << " (in tune "
			     << defaultfloat << setprecision(6) << freq_in_tune << " Hz)" << endl;
		}

		// If the first argument doesn't start with a number or a period, let's try converting note into frequency
		// Returns both the actual frequency and the frequency of the in-tune note
		// 
		// First argument accepts the following forms:
		//     A    (A natural, octave 3 assumed)
		//     A4   (A natural, octave 4)
		//     A-4  (A natural, octave 4, tracker type syntax)
		//     A#4  (A sharp, octave 4)
		//     Ab4  (A flat, octave 4)
		//     A--4 (A natural, octave -4)
		//     A#-4 (A sharp, octave -4)
		//     note names ABCDEFG
		// Second argument is the finetune in cents, 100 being a semitone, can be positive or negative and exceed +/-100

		else {
			for (size_t i = 1; i < argc; i++) {
				while (*argChar) {
					*argChar = toupper(*argChar);
					argChar++;
				}
			}
			argChar = argv[1];

			if (*argChar >= 65 && *argChar <= 71) {
				string note;
				note += *argChar;

				// Get semitone offset from note name and assume octave if it's not in the input
				const long long addsemi[7] = { 0, 1, -11, -10, -9, -9, -8 };
				long long semi = (*argChar + addsemi[*argChar - 65] - 65);
				long long octave = 3;

				// Check for accidentals (# or b)
				argChar++;
				bool accidental = false;
				switch (*argChar) {
				case 35: semi++; accidental = true; note += "#"; argChar++; break;
				case 66: semi--; accidental = true; note += "b"; argChar++; break;
				default: note += "-";
				}

				// Check if octave is negative
				int octave_sign = 1;
				if (*argChar == 45) {
					argChar++;
					if (accidental) {
						octave_sign = -1;
					}
					else {
						if (*argChar == 45) {
							octave_sign = -1;
							argChar++;
						}
					}
				}

				// Get octave number
				string sOctave;
				while (*argChar >= 48 && *argChar <= 57) {
					sOctave += *argChar;
					argChar++;
				}
				if (sOctave.length()) octave = stod(sOctave) * octave_sign;
				semi += (octave - 3) * 12;
				note += to_string(octave);

				// Check for finetune
				string sFinetune;
				double finetune = 0.0;
				if (argc > 2) {
					argChar = argv[2];
					while (*argChar == 43) argChar++;
					while ((*argChar >= 48 && *argChar <= 57) || *argChar == 45 || *argChar == 46) {
						sFinetune += *argChar;
						argChar++;
					}
					if (sFinetune.length()) finetune = stod(sFinetune);
				}

				double freq = transpose(A3, semi + finetune / 100.0);
				double freq_in_tune = transpose(A3, semi);

				cout << note << " " << (finetune >= 0.0 ? "+" : "") << finetune << ": "
				     << defaultfloat << setprecision(6) << freq << " Hz (in tune " << freq_in_tune << " Hz)" << endl;
			}
		}
	}

	cin.get();
	return 0;
}