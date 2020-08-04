#include"HT1621.h"
HT1621 H;
void setup() {
  H.begin(8, 6, 7);
  H.clear();
}
void loop() {

    for(int j = 0;j < 17;++j){
      H.writeFomart(j, 0xff);

    }

}

