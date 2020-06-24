#include "tess/engine.h"

#include <iostream>

int main()
{
  tess::Engine engine;

  try
  {
    engine.Run();
  }
  catch (const std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }

  return 0;
}
