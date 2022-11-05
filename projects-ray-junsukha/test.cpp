#include <tuple>
#define GLM_ENABLE_EXPERIMENTAL
//#include "glm/glm.hpp"
//#include "glm/ext.hpp"
#include <functional>
//#include "./cs123helper.hpp_file/CS123Helper.hpp"
#include <iostream>
#include <cmath>
#include <time.h>
//#include <utils/rgba.h>
//bool mycompare (const std::tuple<int, glm::vec4> &lhs, const std::tuple<int, glm::vec4> &rhs){
//  return get<0>(lhs) < get<0>(rhs);
//}

int main() {
    srand(time(NULL));
  int n = (rand() % 10);
  n =6;
  bool test = false;
again:
  if ( test == true){
      n = 3;
      test = false;
  }
  if (n > 5 )
      test = true;

  if (test == true)
      goto again;
  std::cout << n << std::endl;
}

