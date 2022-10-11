# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/maria/esp/esp-idf/components/bootloader/subproject"
  "D:/GitHub/Camara-de-Conservacao/CamaraConservacao/build/bootloader"
  "D:/GitHub/Camara-de-Conservacao/CamaraConservacao/build/bootloader-prefix"
  "D:/GitHub/Camara-de-Conservacao/CamaraConservacao/build/bootloader-prefix/tmp"
  "D:/GitHub/Camara-de-Conservacao/CamaraConservacao/build/bootloader-prefix/src/bootloader-stamp"
  "D:/GitHub/Camara-de-Conservacao/CamaraConservacao/build/bootloader-prefix/src"
  "D:/GitHub/Camara-de-Conservacao/CamaraConservacao/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/GitHub/Camara-de-Conservacao/CamaraConservacao/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
