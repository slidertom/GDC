set(file_proj 			"${generation_path}/ALL_BUILD.vcxproj")
set(file_filter 		"${file_proj}.filters")
set(fileinstallcmake 	"${generation_path}/cmake_install.cmake")

file(GLOB sln_file "*.sln")
file(REMOVE ${file_proj})
file(REMOVE ${file_filter})
file(REMOVE ${fileinstallcmake})

if( "${sln_file}" STREQUAL "")
	message(STATUS "ERROR: *.sln file not found")
endif()

file(READ ${sln_file} 										sln_file_content)
string(FIND ${sln_file_content} "\nProject" 				all_build_start)
string(FIND ${sln_file_content} "EndProject\n" 				all_build_ending)
string(SUBSTRING ${sln_file_content} 0 ${all_build_start} 	str_file_content_first_part)

set(end_proj_str "EndProject\n") 										 					# Since we find the first letter we need to add the word length.
string(LENGTH ${end_proj_str} end_proj_str_length)						 					# Gets the string length
math(EXPR all_build_ending "${all_build_ending}+${end_proj_str_length}") 					# Required to add two numbers
string(SUBSTRING ${sln_file_content} ${all_build_ending} -1 str_file_content_second_part)

string(CONCAT file_result ${str_file_content_first_part} "\n" ${str_file_content_second_part})
file(WRITE ${sln_file} ${file_result})