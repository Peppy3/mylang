

add_subdirectory(args)
add_subdirectory(tokenizer)
add_subdirectory(parser)
add_subdirectory(ast)

target_sources(${PROJECT_NAME} PRIVATE ParserFile.c)
target_sources(${PROJECT_NAME} PRIVATE Keywords.c)
target_sources(${PROJECT_NAME} PRIVATE util.c)
target_sources(${PROJECT_NAME} PRIVATE arena.c)
target_sources(${PROJECT_NAME} PRIVATE main.c)


