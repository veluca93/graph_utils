#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include "CLI11.hpp"

class Commands {
public:
  using add_command_t = std::function<void(CLI::App *)>;

private:
  static auto &Cmds() {
    static std::vector<add_command_t> commands;
    return commands;
  }

public:
  static void Register(add_command_t cmd) { Cmds().push_back(cmd); }
  static void AddCommands(CLI::App *app) {
    for (auto cmd : Cmds()) {
      cmd(app);
    }
  }
};

class RegisterCommand {
public:
  RegisterCommand(Commands::add_command_t c) { Commands::Register(c); }
};

#endif
