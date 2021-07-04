
find_package(Git QUIET)

macro(ADD_GIT_DEPENDENCY depLibraryDir depTarget depName githubURL branchName)
if(NOT TARGET ${depTarget})
	set(GIT_RESULT )
	execute_process(COMMAND 
		${GIT_EXECUTABLE} clone -b ${branchName} ${githubURL} ${depLibraryDir}/${depName}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE GIT_RESULT)	
	add_subdirectory("${depLibraryDir}/${depName}")	
endif()
endmacro()

macro(ADD_GIT_DEPENDENCY_NO_CMAKE depLibraryDir depTarget depName githubURL)
if(NOT TARGET ${depTarget})
	set(GIT_RESULT )
	execute_process(COMMAND 
		${GIT_EXECUTABLE} clone ${githubURL} ${depLibraryDir}/${depName}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE GIT_RESULT)
endif()
endmacro()


#
#	Adds a list of sources to a target, sourceList should be a list variable
#
macro(ADD_SOURCES_LIST targetName sourceList)
	list(TRANSFORM ${sourceList} PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")
	set(lfefiles )
	foreach(lfe IN LISTS ${sourceList})
		set(lfefiles ${lfefiles} ${lfe})
	endforeach()
	target_sources(${targetName} PRIVATE ${lfefiles})
endmacro(ADD_SOURCES_LIST)

#
#	Returns the child paths of a given directory
#
macro(SUBDIRLIST result curdir)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	set(dirlist "")
	foreach(child ${children})
		if(IS_DIRECTORY ${curdir}/${child})
			list(APPEND dirlist ${child})
		endif()
	endforeach()
	set(${result} ${dirlist})
endmacro()

#
#	Adds a list of subdirectories to the project, pathList should be a list variable
#
macro(ADD_SUBDIRS_LIST pathList)
	foreach(lfe IN LISTS ${pathList})
		add_subdirectory(${lfe})
	endforeach()
endmacro(ADD_SUBDIRS_LIST)

#
#	Includes all subdirectories from the current source path
#
macro(ADD_SUBDIRS_HERE)
	set(dirlist )
	SUBDIRLIST(dirlist ${CMAKE_CURRENT_SOURCE_DIR})
	foreach(lfe IN LISTS dirlist)
		set(lfename )		
		get_filename_component(lfename ${lfe} NAME)
		add_subdirectory(${lfename})
	endforeach()
endmacro()
