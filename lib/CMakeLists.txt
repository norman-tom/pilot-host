add_library(nectariio SHARED nectariio.cpp)
add_library(pid SHARED pid.cpp)

install(TARGETS nectariio LIBRARY DESTINATION bin/usr/local/lib)
install(TARGETS pid LIBRARY DESTINATION bin/usr/local/lib)