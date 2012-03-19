list (APPEND SRCFILES 
  adr.c
  afd.c
  alex.c
  asin.c
  code.c
  token.c
  
  main.c
)

list (APPEND SRCFILES 
  adr.h
  afd.h
  alex.h
  asin.h
  code.h
  token.h
)

add_executable (${CMAKE_PROJECT_NAME} ${SRCFILES})