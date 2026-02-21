cmake_minimum_required(VERSION 3.16)

project(ldog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CommonAPI
find_package(CommonAPI REQUIRED)
find_package(CommonAPI-SomeIP REQUIRED)
find_package(vsomeip3 REQUIRED)


include_directories( 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/fidl/src-gen
)

set(GENERATED_SOMEIP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fidl/src-gen/v1/bootloader/methods/bootloader2SomeIPDeployment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fidl/src-gen/v1/bootloader/methods/bootloader2SomeIPProxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fidl/src-gen/v1/bootloader/methods/bootloader2SomeIPStubAdapter.cpp
)

add_executable(
    ${PROJECT_NAME}
    src/Client.cpp
    ${GENERATED_SOMEIP_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    CommonAPI
    CommonAPI-SomeIP
    vsomeip3
)



add_executable(
    ${PROJECT_NAME}_service
    src/Server.cpp
    src/bootloader2StubImpl.cpp
    ${GENERATED_SOMEIP_SOURCES}
)

target_link_libraries(${PROJECT_NAME}_service PRIVATE 
CommonAPI
    CommonAPI-SomeIP
    vsomeip3
)