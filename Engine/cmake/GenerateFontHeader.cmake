function(format_bytes BYTES OUT_VAR)
    set(result "")
    set(line "    ")
    set(count 0)
    set(total_count 0)
    
    foreach(byte ${BYTES})
        string(APPEND line "0x${byte},")
        math(EXPR count "${count} + 1")
        math(EXPR total_count "${total_count} + 1")
        
        if(${count} EQUAL 12)
            string(APPEND result "${line} // ${total_count}\n")
            set(line "    ")
            set(count 0)
        endif()
    endforeach()
    
    if(${count} GREATER 0)
        string(APPEND result "${line} // ${total_count}\n")
    endif()
    
    set(${OUT_VAR} "${result}" PARENT_SCOPE)
endfunction()

# Read binary file as HEX
message(STATUS "Reading font file: ${FONT_FILE}")
file(READ ${FONT_FILE} FONT_DATA HEX)
string(REGEX MATCHALL "[A-Fa-f0-9][A-Fa-f0-9]" FONT_BYTES "${FONT_DATA}")

# Format bytes into organized lines
format_bytes("${FONT_BYTES}" FORMATTED_BYTES)

# Generate header content
set(HEADER_CONTENT "#pragma once\n\n")
string(APPEND HEADER_CONTENT "// Auto-generated from: ${FONT_FILE}\n\n")
string(APPEND HEADER_CONTENT "namespace EmbeddedFonts {\n\n")
string(APPEND HEADER_CONTENT "static const unsigned char ${FONT_VARIABLE}[] = {\n")
string(APPEND HEADER_CONTENT "${FORMATTED_BYTES}")
string(APPEND HEADER_CONTENT "};\n\n")
string(APPEND HEADER_CONTENT "static const size_t ${FONT_VARIABLE}_size = sizeof(${FONT_VARIABLE});\n\n")
string(APPEND HEADER_CONTENT "} // namespace EmbeddedFonts\n")

# Write to file
message(STATUS "Writing to: ${OUTPUT_FILE}")
file(WRITE ${OUTPUT_FILE} "${HEADER_CONTENT}")
