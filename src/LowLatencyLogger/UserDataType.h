#ifndef UserDataType_h
#define UserDataType_h
const uint8_t ADC_DIM = 4;
struct data_t {
  char X1[6]; // -1234;
  char Y1[6]; // -1234;
  char Z1[6]; // -1234;
  char X2[6]; // -1234;
  char Y2[6]; // -1234;
  char Z2[7]; // -1234\r\n
};
#endif  // UserDataType_h


void acquireData(data_t* data) {
  memcpy(data->X1,"-1234;-1234;-1234;-1234;-1234;-1234\r\n",37);
}
