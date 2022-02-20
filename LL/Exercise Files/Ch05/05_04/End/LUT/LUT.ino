/*
  Blink: Function vs LookUp Table Benchmark
  Changes the state of an LED,
  and performs thousands of function calls 
  repeatedly.
 */

// Use the following line to use LUT. Comment it for regular function.
#define USE_LUT

// Use the following line to calculate logarithm. Comment it for squares.
#define USE_LOG

// Use the following line to report back to the serial monitor.
#define MEASURE


#define MASK(x) ((unsigned char)(1<<(x)))

#ifdef MEASURE
  uint32_t StartTime, CurrentTime;
#endif

#ifdef USE_LUT
  #ifdef USE_LOG
    // LookUp Table for the base 10 logarithm of 0 through 255.
    const double log_LUT[256] PROGMEM = {-1.0E30, 0.000000, 0.301030, 0.477121, 0.602060, 
    0.698970, 0.778151, 0.845098, 0.903090, 0.954243, 1.000000, 1.041393, 1.079181, 
    1.113943, 1.146128, 1.176091, 1.204120, 1.230449, 1.255273, 1.278754, 1.301030, 
    1.322219, 1.342423, 1.361728, 1.380211, 1.397940, 1.414973, 1.431364, 1.447158, 
    1.462398, 1.477121, 1.491362, 1.505150, 1.518514, 1.531479, 1.544068, 1.556303, 
    1.568202, 1.579784, 1.591065, 1.602060, 1.612784, 1.623249, 1.633468, 1.643453, 
    1.653213, 1.662758, 1.672098, 1.681241, 1.690196, 1.698970, 1.707570, 1.716003, 
    1.724276, 1.732394, 1.740363, 1.748188, 1.755875, 1.763428, 1.770852, 1.778151, 
    1.785330, 1.792392, 1.799341, 1.806180, 1.812913, 1.819544, 1.826075, 1.832509, 
    1.838849, 1.845098, 1.851258, 1.857332, 1.863323, 1.869232, 1.875061, 1.880814, 
    1.886491, 1.892095, 1.897627, 1.903090, 1.908485, 1.913814, 1.919078, 1.924279, 
    1.929419, 1.934498, 1.939519, 1.944483, 1.949390, 1.954243, 1.959041, 1.963788, 
    1.968483, 1.973128, 1.977724, 1.982271, 1.986772, 1.991226, 1.995635, 2.000000, 
    2.004321, 2.008600, 2.012837, 2.017033, 2.021189, 2.025306, 2.029384, 2.033424, 
    2.037426, 2.041393, 2.045323, 2.049218, 2.053078, 2.056905, 2.060698, 2.064458, 
    2.068186, 2.071882, 2.075547, 2.079181, 2.082785, 2.086360, 2.089905, 2.093422, 
    2.096910, 2.100371, 2.103804, 2.107210, 2.110590, 2.113943, 2.117271, 2.120574, 
    2.123852, 2.127105, 2.130334, 2.133539, 2.136721, 2.139879, 2.143015, 2.146128, 
    2.149219, 2.152288, 2.155336, 2.158362, 2.161368, 2.164353, 2.167317, 2.170262, 
    2.173186, 2.176091, 2.178977, 2.181844, 2.184691, 2.187521, 2.190332, 2.193125, 
    2.195900, 2.198657, 2.201397, 2.204120, 2.206826, 2.209515, 2.212188, 2.214844, 
    2.217484, 2.220108, 2.222716, 2.225309, 2.227887, 2.230449, 2.232996, 2.235528, 
    2.238046, 2.240549, 2.243038, 2.245513, 2.247973, 2.250420, 2.252853, 2.255273, 
    2.257679, 2.260071, 2.262451, 2.264818, 2.267172, 2.269513, 2.271842, 2.274158, 
    2.276462, 2.278754, 2.281033, 2.283301, 2.285557, 2.287802, 2.290035, 2.292256, 
    2.294466, 2.296665, 2.298853, 2.301030, 2.303196, 2.305351, 2.307496, 2.309630, 
    2.311754, 2.313867, 2.315970, 2.318063, 2.320146, 2.322219, 2.324282, 2.326336, 
    2.328380, 2.330414, 2.332438, 2.334454, 2.336460, 2.338456, 2.340444, 2.342423, 
    2.344392, 2.346353, 2.348305, 2.350248, 2.352183, 2.354108, 2.356026, 2.357935, 
    2.359835, 2.361728, 2.363612, 2.365488, 2.367356, 2.369216, 2.371068, 2.372912, 
    2.374748, 2.376577, 2.378398, 2.380211, 2.382017, 2.383815, 2.385606, 2.387390, 
    2.389166, 2.390935, 2.392697, 2.394452, 2.396199, 2.397940, 2.399674, 2.401401, 
    2.403121, 2.404834, 2.406540};
  #else
    // LookUp Table for the square of 0 through 255.
    const uint32_t sq_LUT[256] PROGMEM = {0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 
    169, 196, 225, 256, 289, 324, 361, 400, 441, 484, 529, 576, 625, 676, 729, 784, 
    841, 900, 961, 1024, 1089, 1156, 1225, 1296, 1369, 1444, 1521, 1600, 1681, 1764, 
    1849, 1936, 2025, 2116, 2209, 2304, 2401, 2500, 2601, 2704, 2809, 2916, 3025, 
    3136, 3249, 3364, 3481, 3600, 3721, 3844, 3969, 4096, 4225, 4356, 4489, 4624, 
    4761, 4900, 5041, 5184, 5329, 5476, 5625, 5776, 5929, 6084, 6241, 6400, 6561, 
    6724, 6889, 7056, 7225, 7396, 7569, 7744, 7921, 8100, 8281, 8464, 8649, 8836, 
    9025, 9216, 9409, 9604, 9801, 10000, 10201, 10404, 10609, 10816, 11025, 11236, 
    11449, 11664, 11881, 12100, 12321, 12544, 12769, 12996, 13225, 13456, 13689, 
    13924, 14161, 14400, 14641, 14884, 15129, 15376, 15625, 15876, 16129, 16384, 
    16641, 16900, 17161, 17424, 17689, 17956, 18225, 18496, 18769, 19044, 19321, 
    19600, 19881, 20164, 20449, 20736, 21025, 21316, 21609, 21904, 22201, 22500, 
    22801, 23104, 23409, 23716, 24025, 24336, 24649, 24964, 25281, 25600, 25921, 
    26244, 26569, 26896, 27225, 27556, 27889, 28224, 28561, 28900, 29241, 29584, 
    29929, 30276, 30625, 30976, 31329, 31684, 32041, 32400, 32761, 33124, 33489, 
    33856, 34225, 34596, 34969, 35344, 35721, 36100, 36481, 36864, 37249, 37636, 
    38025, 38416, 38809, 39204, 39601, 40000, 40401, 40804, 41209, 41616, 42025, 
    42436, 42849, 43264, 43681, 44100, 44521, 44944, 45369, 45796, 46225, 46656, 
    47089, 47524, 47961, 48400, 48841, 49284, 49729, 50176, 50625, 51076, 51529, 
    51984, 52441, 52900, 53361, 53824, 54289, 54756, 55225, 55696, 56169, 56644, 
    57121, 57600, 58081, 58564, 59049, 59536, 60025, 60516, 61009, 61504, 62001, 
    62500, 63001, 63504, 64009, 64516, 65025};
  #endif
#else
  #ifndef USE_LOG
    // Regular function for the square of 0 through 255.
    uint32_t sq_func(uint8_t x){ 
      return (uint32_t)x*x;
    }
  #endif
#endif

// x is a placeholder variable to make use of the functions/LUTs.
// It's a double for logarithms, and a 32 bit integer for squares.
#ifdef USE_LOG
  volatile double x = 0.0;
#else
  volatile uint32_t x = 0;
#endif

void setup() { 
  DDRB |= MASK(5);   // The LED pin is output. Mask: 00100000
  Serial.begin(9600);  
}

void loop() {
  PORTB ^= MASK(5);  // Toggle LED 

  // j is the main loop variable. Logarithms take longer, so different values are used.
  #ifdef USE_LOG
    uint32_t j = 100;   
  #else
    uint32_t j = 600; 
  #endif
  
  #ifdef MEASURE
    StartTime = millis();
  #endif
  
  while(j--)
    for(uint8_t i=0; i<255; i++){ // Call functions 255 times, according to #define symbols
      #ifdef USE_LUT
        #ifdef USE_LOG
          x = log_LUT[i];   // Using base 10 logarithm LookUp Table 
        #else
          x = sq_LUT[i];    // Using square LookUp Table 
        #endif
      #else
        #ifdef USE_LOG
          x = log10(i);     // Using base 10 logarithm function from math.h 
        #else
          x = sq_func(i);   // Using square function, defined above 
        #endif
      #endif
    }

  #ifdef MEASURE
    CurrentTime = millis();
    Serial.print("Took ");
    Serial.print((float)(CurrentTime - StartTime)*0.001,3);
    Serial.print(" seconds\n");
  #endif
}
