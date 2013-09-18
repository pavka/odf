project(odf-samples CXX)

SET(ODF_SAMPLES_LINK_LIBRARIES ${LIBRARY_OUTPUT_PATH}/libodf.so)
SET(ODF_SAMPLES_TARGET samples)
SET(ODF_SAMPLE_SETTINGS EXCLUDE_FROM_ALL)

add_custom_target(${ODF_SAMPLES_TARGET})

MACRO(add_sample target sample_SOURCES)
    SET(sample_TARGET samples-${target})
    add_executable(${sample_TARGET} EXCLUDE_FROM_ALL
                   ${sample_SOURCES}
                   ${sample_common_SOURCES})
    target_link_libraries(${sample_TARGET}
                          ${ODF_LINK_LIBRARIES}
                          ${ODF_SAMPLES_LINK_LIBRARIES})

    add_dependencies(${sample_TARGET} ${ODF_LIBRARY_TARGET})
    add_dependencies(${ODF_SAMPLES_TARGET} ${sample_TARGET})
ENDMACRO(add_sample)

file(GLOB sample_common_SOURCES "common/*.cpp")

include_directories(".")
add_subdirectory("face-skin")
