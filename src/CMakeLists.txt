add_executable(minishell processus.c main.c builtin.c parser.c)
install (TARGETS minishell DESTINATION bin/)
